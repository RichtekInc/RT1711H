/*
 * Copyright (C) 2016 Richtek Technology Corp.
 *
 * Power Delivery Managert Driver
 *
 * Author: TH <tsunghan_tsai@richtek.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include <linux/usb/tcpm.h>
#include <linux/usb/pd_core.h>
#include <linux/usb/tcpci.h>
#include <linux/usb/pd_policy_engine.h>
#include <linux/usb/pd_dpm_core.h>
#include <linux/usb/tcpci_typec.h>

/* Inquire TCPC status */

int tcpm_inquire_remote_cc(struct tcpc_device *tcpc_dev,
	uint8_t *cc1, uint8_t *cc2, bool from_ic)
{
	int rv = 0;

	if (from_ic) {
		rv = tcpci_get_cc(tcpc_dev);
		if (rv < 0)
			return rv;
	}

	*cc1 = tcpc_dev->typec_remote_cc[0];
	*cc2 = tcpc_dev->typec_remote_cc[1];
	return 0;
}

int tcpm_inquire_vbus_level(
	struct tcpc_device *tcpc_dev, bool from_ic)
{
	int rv = 0;
	uint16_t power_status = 0;

	if (from_ic) {
		rv = tcpci_get_power_status(tcpc_dev, &power_status);
		if (rv < 0)
			return rv;

		tcpci_vbus_level_init(tcpc_dev, power_status);
	}

	return tcpc_dev->vbus_level;
}

bool tcpm_inquire_cc_polarity(
	struct tcpc_device *tcpc_dev)
{
	return tcpc_dev->typec_polarity;
}

uint8_t tcpm_inquire_typec_attach_state(
	struct tcpc_device *tcpc_dev)
{
	return tcpc_dev->typec_attach_new;
}

uint8_t tcpm_inquire_typec_role(
	struct tcpc_device *tcpc_dev)
{
	return tcpc_dev->typec_role;
}

uint8_t tcpm_inquire_typec_local_rp(
	struct tcpc_device *tcpc_dev)
{
	uint8_t level;

	switch (tcpc_dev->typec_local_rp_level) {
	case TYPEC_CC_RP_1_5:
		level = 1;
		break;

	case TYPEC_CC_RP_3_0:
		level = 2;
		break;

	default:
	case TYPEC_CC_RP_DFT:
		level = 0;
		break;
	}

	return level;
}

int tcpm_typec_set_rp_level(
	struct tcpc_device *tcpc_dev, uint8_t level)
{
	uint8_t res;

	if (level == 2)
		res = TYPEC_CC_RP_3_0;
	else if (level == 1)
		res = TYPEC_CC_RP_1_5;
	else
		res = TYPEC_CC_RP_DFT;

	return tcpc_typec_set_rp_level(tcpc_dev, res);
}

int tcpm_typec_change_role(
	struct tcpc_device *tcpc_dev, uint8_t typec_role)
{
	return tcpc_typec_change_role(tcpc_dev, typec_role);
}

#ifdef CONFIG_USB_POWER_DELIVERY

bool tcpm_inquire_pd_connected(
	struct tcpc_device *tcpc_dev)
{
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	return pd_port->pd_connected;
}

bool tcpm_inquire_pd_prev_connected(
	struct tcpc_device *tcpc_dev)
{
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	return pd_port->pd_prev_connected;
}

uint8_t tcpm_inquire_pd_data_role(
	struct tcpc_device *tcpc_dev)
{
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	return pd_port->data_role;
}

uint8_t tcpm_inquire_pd_power_role(
	struct tcpc_device *tcpc_dev)
{
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	return pd_port->power_role;
}

uint8_t tcpm_inquire_pd_vconn_role(
	struct tcpc_device *tcpc_dev)
{
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	return pd_port->vconn_source;
}

#endif	/* CONFIG_USB_POWER_DELIVERY */

/* Request TCPC to send PD Request */

#ifdef CONFIG_USB_POWER_DELIVERY

int tcpm_power_role_swap(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_PR_SWAP);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_power_role_swap);

int tcpm_data_role_swap(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_DR_SWAP);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_data_role_swap);

int tcpm_vconn_swap(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_VCONN_SWAP);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_vconn_swap);

int tcpm_goto_min(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_GOTOMIN);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_goto_min);

int tcpm_soft_reset(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_SOFTRESET);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_soft_reset);

int tcpm_hard_reset(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_HARDRESET);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_hard_reset);

int tcpm_get_source_cap(
	struct tcpc_device *tcpc_dev, struct tcpm_power_cap *cap)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_GET_SOURCE_CAP);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	/* TODO: Finish it later */

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_get_source_cap);

int tcpm_get_sink_cap(
	struct tcpc_device *tcpc_dev, struct tcpm_power_cap *cap)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_GET_SINK_CAP);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	/* TODO: Finish it later */

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_get_sink_cap);

int tcpm_bist_cm2(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_pd_request_event(pd_port,
				PD_DPM_PD_REQUEST_BIST_CM2);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	/* TODO: Finish it later */

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_bist_cm2);

int tcpm_request(struct tcpc_device *tcpc_dev, int mv, int ma)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);
	ret = pd_dpm_send_request(pd_port, mv, ma);
	mutex_unlock(&pd_port->pd_lock);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_request);

int tcpm_error_recovery(struct tcpc_device *tcpc_dev)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	ret = pd_put_dpm_event(pd_port, PD_DPM_ERROR_RECOVERY);
	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}

int tcpm_discover_cable(struct tcpc_device *tcpc_dev, uint32_t *vdos)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);
	pd_port->dpm_flags |= DPM_FLAGS_CHECK_CABLE_ID;
	ret = vdm_put_dpm_discover_cable_event(pd_port);
	mutex_unlock(&pd_port->pd_lock);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}

int tcpm_vdm_request_id(struct tcpc_device *tcpc_dev,
				uint8_t *cnt, uint8_t *payload)
{
	bool ret;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);
	ret = vdm_put_dpm_vdm_request_event(
		pd_port, PD_DPM_VDM_REQUEST_DISCOVER_ID);
	mutex_unlock(&pd_port->pd_lock);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}

#ifdef CONFIG_USB_PD_ALT_MODE

int tcpm_dp_attention(
	struct tcpc_device *tcpc_dev, uint32_t dp_status)
{
	bool ret = false;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);

	ret = vdm_put_dpm_vdm_request_event(
		pd_port, PD_DPM_VDM_REQUEST_ATTENTION);

	if (ret) {
		pd_port->dp_status = dp_status;
		pd_port->mode_svid = USB_SID_DISPLAYPORT;
	}

	mutex_unlock(&pd_port->pd_lock);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_dp_attention);

#ifdef CONFIG_USB_PD_ALT_MODE_DFP

int tcpm_dp_status_update(
	struct tcpc_device *tcpc_dev, uint32_t dp_status)
{
	bool ret = false;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);

	ret = vdm_put_dpm_vdm_request_event(
		pd_port, PD_DPM_VDM_REQUEST_DP_STATUS_UPDATE);

	if (ret) {
		pd_port->dp_status = dp_status;
		pd_port->mode_svid = USB_SID_DISPLAYPORT;
	}

	mutex_unlock(&pd_port->pd_lock);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_dp_status_update);

int tcpm_dp_configuration(
	struct tcpc_device *tcpc_dev, uint32_t dp_config)
{
	bool ret = false;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);

	ret = vdm_put_dpm_vdm_request_event(
		pd_port, PD_DPM_VDM_REQUEST_DP_CONFIG);

	if (ret) {
		pd_port->local_dp_config = dp_config;
		pd_port->mode_svid = USB_SID_DISPLAYPORT;
	}

	mutex_unlock(&pd_port->pd_lock);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_dp_configuration);

#endif	/* CONFIG_USB_PD_ALT_MODE_DFP */
#endif	/* CONFIG_USB_PD_ALT_MODE */

#ifdef CONFIG_USB_PD_UVDM
int tcpm_send_uvdm(struct tcpc_device *tcpc_dev,
	uint8_t cnt, uint32_t *data, bool wait_resp)
{
	bool ret = false;
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	if (tcpc_dev->typec_attach_old == TYPEC_UNATTACHED)
		return TCPM_ERROR_UNATTACHED;

	if (cnt > VDO_MAX_SIZE)
		return TCPM_ERROR_PARAMETER;

	mutex_lock(&pd_port->pd_lock);

	pd_port->uvdm_cnt = cnt;
	pd_port->uvdm_wait_resp = wait_resp;
	memcpy(pd_port->uvdm_data, data, sizeof(uint32_t) * cnt);

	ret = vdm_put_dpm_vdm_request_event(
		pd_port, PD_DPM_VDM_REQUEST_UVDM);

	mutex_unlock(&pd_port->pd_lock);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return 0;
}
#endif	/* CONFIG_USB_PD_UVDM */

int tcpm_notify_vbus_stable(
	struct tcpc_device *tcpc_dev)
{
#if CONFIG_USB_PD_VBUS_STABLE_TOUT
	tcpc_disable_timer(tcpc_dev, PD_TIMER_VBUS_STABLE);
#endif

	pd_put_vbus_stable_event(tcpc_dev);
	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_notify_vbus_stable);


#endif /* CONFIG_USB_POWER_DELIVERY */
