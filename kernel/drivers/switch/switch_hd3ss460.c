/* drivers/switch/switch_hd3ss460.c
 * TI HD3SS460 Switch Driver (example)
 *
 * Copyright (C) 2014 Richtek Technology Corp.
 * Author: Patrick Chang <patrick_chang@richtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/switch.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/workqueue.h>
#include <linux/gpio.h>
#include <linux/notifier.h>
#include <linux/usb/tcpci.h>
#include <linux/usb/tcpm.h>


enum {
	SW_EN = 0,
	SW_AMSEL,
	SW_POL,
	SW_NR,
};

static const char *of_gpio_props[] = {
	"gpio,enable",
	"gpio,amsel",
	"gpio,polarity",
};

static const char *of_sw_name_props[] = {
	"name,enable",
	"name,amsel",
	"name,polarity",
};

static const char *hd3ss460_sw_default_names[] = {
	"hd3ss460_enable",
	"hd3ss460_amsel",
	"hd3ss460_polarity",
};

typedef struct hd3ss460_switch_data {
	struct switch_dev sdevs[SW_NR];
	unsigned int gpios[SW_NR];
	struct notifier_block nb;
	struct tcpc_device *tcpc;
	const char *tcpc_name;
	struct mutex io_lock;
} hd3ss460_switch_data_t;

typedef struct {
	int pin_assignment;
	const char *config_name;
	uint8_t gpio_states[SW_NR];
} sw_config_t;



static const sw_config_t configs[] = {
	{
		-1,
		"pin_assignment_default",
		{0, 0, 0},
	},
	{
		3,
		"pin_assignment_c",
		{1, 0, 0},
	},
	{
		4,
		"pin_assignment_d",
		{1, 0, 0},
	},
};

#define SW_EN_DELAY_US	500

int hd3ss460_set_switch(
	struct hd3ss460_switch_data *switch_data, uint8_t enable,
	uint8_t  amsel, uint8_t polarity)
{
	mutex_lock(&switch_data->io_lock);
	if (enable) {
		/* Make switch open before change to the other mode */
		if (switch_data->sdevs[SW_EN].state) {
			gpio_set_value(switch_data->gpios[SW_EN], 0);
			udelay(SW_EN_DELAY_US);
		}
		gpio_set_value(switch_data->gpios[SW_AMSEL], amsel);
		gpio_set_value(switch_data->gpios[SW_POL], polarity);
		udelay(SW_EN_DELAY_US);
		gpio_set_value(switch_data->gpios[SW_EN], 1);
	} else {
		gpio_set_value(switch_data->gpios[SW_EN], 0);
		gpio_set_value(switch_data->gpios[SW_AMSEL], amsel);
		gpio_set_value(switch_data->gpios[SW_POL], polarity);
	}
	switch_set_state(switch_data->sdevs + SW_EN, enable);
	switch_set_state(switch_data->sdevs + SW_AMSEL, amsel);
	switch_set_state(switch_data->sdevs + SW_POL, polarity);
	mutex_unlock(&switch_data->io_lock);
	return 0;
}

/* Please handle the notification in notifier call function,
 * handle SWITCH control event
 */
static int hd3ss460_tcp_notifer_call(struct notifier_block *nb,
				unsigned long event, void *data)
{
	hd3ss460_switch_data_t* switch_data = container_of(nb, hd3ss460_switch_data_t, nb);
	struct tcp_notify *tcp_noti = data;

	switch (event) {
	case TCP_NOTIFY_AMA_DP_STATE:
		// TODO
		hd3ss460_set_switch(switch_data, tcp_noti->ama_dp_state.active,
			0, tcp_noti->ama_dp_state.polarity);
		break;
	case TCP_NOTIFY_TYPEC_STATE:
		switch (tcp_noti->typec_state.new_state) {
			case TYPEC_ATTACHED_SNK:
			case TYPEC_ATTACHED_SRC:
			hd3ss460_set_switch(switch_data, 1, 0,
				tcp_noti->typec_state.polarity);
			break;
			case TYPEC_UNATTACHED:
			hd3ss460_set_switch(switch_data, 0, 0, 0);
			break;
		}
		break;
	default:
		break;
	};

	return NOTIFY_OK;
}


static int hd3ss460_parse_dt(struct hd3ss460_switch_data *switch_data,
	struct device *dev)
{
	struct device_node *np = dev->of_node;
	int i;

	if (!np)
		return -EINVAL;
	for (i = 0; i < SW_NR; i++) {
		switch_data->gpios[i] = of_get_named_gpio(np, of_gpio_props[i], 0);
		if (of_property_read_string(np, of_sw_name_props[i],
			&switch_data->sdevs[i].name) < 0)
			switch_data->sdevs[i].name = hd3ss460_sw_default_names[i];
	}
	if (of_property_read_string(np, "tcp_name",
		&switch_data->tcpc_name) < 0)
		switch_data->tcpc_name = "tcp_dp_port";

	return 0;
}

static int hd3ss460_switch_set_state(struct switch_dev *sdev, int new_state)
{
	struct hd3ss460_switch_data *switch_data =  switch_dev_get_drvdata(sdev);
	int index = sdev - switch_data->sdevs;

	dev_dbg(sdev->dev, "%s : switch number = %d, name = %s\n",
		__FUNCTION__, index, hd3ss460_sw_default_names[index]);
	if (index >= SW_NR)
		return -EFAULT;
	gpio_set_value(switch_data->gpios[index], new_state);
	return 0;
}

static int hd3ss460_switch_probe(struct platform_device *pdev)
{
	struct hd3ss460_switch_data *switch_data;
	int ret = 0, i, registered_sw_devs_nr;

	switch_data = devm_kzalloc(&pdev->dev,
		sizeof(*switch_data), GFP_KERNEL);
	if (!switch_data)
		return -ENOMEM;
	mutex_init(&switch_data->io_lock);
	platform_set_drvdata(pdev, switch_data);
	ret = hd3ss460_parse_dt(switch_data, &pdev->dev);
	if (ret < 0)
		goto err_parse_dt;

	for (i = 0, registered_sw_devs_nr = 0; i < SW_NR; i++) {
		ret = gpio_request(switch_data->gpios[i],
			switch_data->sdevs[i].name);
		if (ret < 0)
			goto err_request_gpio;
		registered_sw_devs_nr = i + 1;
	}
	for (i = 0, registered_sw_devs_nr = 0; i < SW_NR; i++) {
		ret = gpio_direction_output(switch_data->gpios[i], 0);
		if (ret < 0)
			goto err_set_gpio_output;
		registered_sw_devs_nr = i + 1;
	}
	for (i = 0, registered_sw_devs_nr = 0; i < SW_NR; i++) {
		switch_data->sdevs[i].set_state = hd3ss460_switch_set_state;
		ret = switch_dev_register(switch_data->sdevs + i);
		if (ret < 0)
			goto err_register_switch;
		switch_dev_set_drvdata(switch_data->sdevs + i, switch_data);
		registered_sw_devs_nr = i + 1;
	}

	switch_data->tcpc = tcpc_dev_get_by_name(switch_data->tcpc_name);
	if (switch_data->tcpc) {
		switch_data->nb.notifier_call = hd3ss460_tcp_notifer_call;
		register_tcp_dev_notifier(switch_data->tcpc,
			&switch_data->nb);
	} else
		pr_info("%s : Warning *** can't find tcpc device [%s]\n",
			__FUNCTION__, switch_data->tcpc_name);

	return 0;
err_register_switch:
	for (i = registered_sw_devs_nr; i > 0; i--)
		switch_dev_unregister(switch_data->sdevs + (i -1));
err_set_gpio_output:
	registered_sw_devs_nr = SW_NR;
err_request_gpio:
	for (i = registered_sw_devs_nr; i > 0; i--)
		gpio_free(switch_data->gpios[i]);

err_parse_dt:
	mutex_destroy(&switch_data->io_lock);
	return ret;
}

static int __devexit hd3ss460_switch_remove(struct platform_device *pdev)
{
	struct hd3ss460_switch_data *switch_data = platform_get_drvdata(pdev);
	int i;

	mutex_destroy(&switch_data->io_lock);
	for (i = 0; i < SW_NR; i++) {
		gpio_free(switch_data->gpios[i]);
		switch_dev_unregister(switch_data->sdevs + i);
	}
	return 0;
}

#ifdef CONFIG_PM
static int hd3ss460_suspend(struct device *dev)
{
	dev_dbg(dev, "hd3ss460 system suspending...\n");
	return 0;
}

static int hd3ss460_resume(struct device *dev)
{
	dev_dbg(dev, "hd3ss460 system resuming...\n");
	return 0;
}



static int hd3ss460_pm_suspend_runtime(struct device *dev)
{
	dev_dbg(dev, "hd3ss460 pm_runtime: suspending...\n");
	return 0;
}

static int hd3ss460_pm_resume_runtime(struct device *dev)
{
	dev_dbg(dev, "hd3ss460 pm_runtime: resuming...\n");
	return 0;
}
static void hd3ss460_shutdown(struct platform_device *pdev)
{
	dev_dbg(&pdev->dev, "hd3ss460 shutdown...\n");
}

static const struct dev_pm_ops hd3ss460_pm_ops = {
    SET_SYSTEM_SLEEP_PM_OPS(
    hd3ss460_suspend,
    hd3ss460_resume)
	SET_RUNTIME_PM_OPS(
		hd3ss460_pm_suspend_runtime,
		hd3ss460_pm_resume_runtime,
		NULL
	)
};
#define HD3SS460_PM_OPS	(&hd3ss460_pm_ops)
#else
#define HD3SS460_PM_OPS	(NULL)
#endif /* CONFIG_PM */


static const struct of_device_id hd3ss460_match_table[] = {
	{.compatible = "ti,hd3ss460",},
	{},
};

static struct platform_driver hd3ss460_switch_driver = {
	.probe		= hd3ss460_switch_probe,
	.remove		= __devexit_p(hd3ss460_switch_remove),
	.driver		= {
		.name	= "switch-hd3ss460",
		.owner	= THIS_MODULE,
		.of_match_table = hd3ss460_match_table,
		.pm = HD3SS460_PM_OPS,
	},
	.shutdown = hd3ss460_shutdown,
};

static int __init hd3ss460_switch_init(void)
{
	return platform_driver_register(&hd3ss460_switch_driver);
}

static void __exit hd3ss460_switch_exit(void)
{
	platform_driver_unregister(&hd3ss460_switch_driver);
}

device_initcall_sync(hd3ss460_switch_init);
module_exit(hd3ss460_switch_exit);

MODULE_AUTHOR("Patrick Chang <patrick_chang@richtek.com>");
MODULE_DESCRIPTION("HD3SS460 Switch driver");
MODULE_LICENSE("GPL");
