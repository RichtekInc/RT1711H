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
#include <linux/usb/tcpci.h>
#include <linux/usb/tcpci_typec.h>

#ifdef CONFIG_USB_POWER_DELIVERY
#include <linux/usb/pd_core.h>
#include <linux/usb/pd_dpm_core.h>
#include <linux/usb/pd_policy_engine.h>
#endif	/* CONFIG_USB_POWER_DELIVERY */

/* Inquire TCPC status */

int tcpm_shutdown(struct tcpc_device *tcpc_dev)
{
#ifdef CONFIG_TCPC_SHUTDOWN_VBUS_DISABLE
	if (tcpc_dev->typec_power_ctrl)
		tcpci_disable_vbus_control(tcpc_dev);
#endif	/* CONFIG_TCPC_SHUTDOWN_VBUS_DISABLE */

	if (tcpc_dev->ops->deinit)
		tcpc_dev->ops->deinit(tcpc_dev);

	return 0;
}

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

int tcpm_typec_set_wake_lock(
	struct tcpc_device *tcpc, bool user_lock)
{
	int ret;

	mutex_lock(&tcpc->access_lock);
	ret = tcpci_set_wake_lock(
		tcpc, tcpc->wake_lock_pd, user_lock);
	tcpc->wake_lock_user = user_lock;
	mutex_unlock(&tcpc->access_lock);

	return ret;
}

int tcpm_typec_set_usb_sink_curr(
	struct tcpc_device *tcpc_dev, int curr)
{
	bool force_sink_vbus = true;

#ifdef CONFIG_USB_POWER_DELIVERY
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);

	if (pd_port->pd_prev_connected)
		force_sink_vbus = false;
#endif	/* CONFIG_USB_POWER_DELIVERY */

	tcpc_dev->typec_usb_sink_curr = curr;

	if (tcpc_dev->typec_remote_rp_level != TYPEC_CC_VOLT_SNK_DFT)
		force_sink_vbus = false;

	if (force_sink_vbus) {
		tcpci_sink_vbus(tcpc_dev,
			TCP_VBUS_CTRL_TYPEC, TCPC_VBUS_SINK_5V, -1);
	}

#ifdef CONFIG_USB_POWER_DELIVERY
	mutex_unlock(&pd_port->pd_lock);
#endif	/* CONFIG_USB_POWER_DELIVERY */

	return 0;
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

int tcpm_typec_set_custom_hv(struct tcpc_device *tcpc, bool en)
{
#ifdef CONFIG_TYPEC_CAP_CUSTOM_HV
	int ret = TCPM_SUCCESS;

	mutex_lock(&tcpc->access_lock);
	if (tcpc->typec_attach_old == TYPEC_UNATTACHED)
		ret = TCPM_ERROR_UNATTACHED;
	else
		tcpc->typec_during_custom_hv = en;
	mutex_unlock(&tcpc->access_lock);

	return ret;
#else
	return TCPM_ERROR_NO_SUPPORT;
#endif	/* CONFIG_TYPEC_CAP_CUSTOM_HV */
}

int tcpm_typec_role_swap(struct tcpc_device *tcpc_dev)
{
	if (tcpc_dev->typec_attach_old == TYPEC_UNATTACHED)
		return TCPM_ERROR_UNATTACHED;

#ifdef CONFIG_TYPEC_CAP_ROLE_SWAP
	return tcpc_typec_swap_role(tcpc_dev) == 0)
#else
	return TCPM_ERROR_NO_SUPPORT;
#endif /* CONFIG_TYPEC_CAP_ROLE_SWAP */
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

uint8_t tcpm_inquire_cable_current(
	struct tcpc_device *tcpc_dev)
{
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	if (pd_port->power_cable_present)
		return pd_get_cable_curr_lvl(pd_port)+1;

	return PD_CABLE_CURR_UNKNOWN;
}

uint32_t tcpm_inquire_dpm_flags(struct tcpc_device *tcpc_dev)
{
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	return pd_port->dpm_flags;
}

uint32_t tcpm_inquire_dpm_caps(struct tcpc_device *tcpc_dev)
{
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	return pd_port->dpm_caps;
}

void tcpm_set_dpm_flags(struct tcpc_device *tcpc_dev, uint32_t flags)
{
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);
	pd_port->dpm_flags = flags;
	mutex_unlock(&pd_port->pd_lock);
}

void tcpm_set_dpm_caps(struct tcpc_device *tcpc_dev, uint32_t caps)
{
	pd_port_t *pd_port = &tcpc_dev->pd_port;

	mutex_lock(&pd_port->pd_lock);
	pd_port->dpm_caps = caps;
	mutex_unlock(&pd_port->pd_lock);
}

/* Inquire TCPC to get PD Information */

int tcpm_inquire_pd_contract(
	struct tcpc_device *tcpc, int *mv, int *ma)
{
	int ret = TCPM_SUCCESS;
	pd_port_t *pd_port = &tcpc->pd_port;

	if (tcpc->typec_attach_old == TYPEC_UNATTACHED)
		return TCPM_ERROR_UNATTACHED;

	if (!pd_port->pd_prev_connected)
		return TCPM_ERROR_NO_PD_CONNECTED;

	if ((mv == NULL) || (ma == NULL))
		return TCPM_ERROR_PARAMETER;

	mutex_lock(&pd_port->pd_lock);
	if (pd_port->explicit_contract) {
		*mv = pd_port->request_v;
		*ma = pd_port->request_i;
	} else
		ret = TCPM_ERROR_NO_EXPLICIT_CONTRACT;
	mutex_unlock(&pd_port->pd_lock);

	return ret;

}
EXPORT_SYMBOL(tcpm_inquire_pd_contract);

int tcpm_inquire_cable_inform(
	struct tcpc_device *tcpc, uint32_t *vdos)
{
	int ret = TCPM_SUCCESS;
	pd_port_t *pd_port = &tcpc->pd_port;

	if (tcpc->typec_attach_old == TYPEC_UNATTACHED)
		return TCPM_ERROR_UNATTACHED;

	if (!pd_port->pd_prev_connected)
		return TCPM_ERROR_NO_PD_CONNECTED;

	if (vdos == NULL)
		return TCPM_ERROR_PARAMETER;

	mutex_lock(&pd_port->pd_lock);
	if (pd_port->power_cable_present) {
		memcpy(vdos, pd_port->cable_vdos,
			sizeof(uint32_t) * VDO_MAX_NR);
	} else
		ret = TCPM_ERROR_NO_POWER_CABLE;
	mutex_unlock(&pd_port->pd_lock);

	return ret;
}
EXPORT_SYMBOL(tcpm_inquire_cable_inform);

int tcpm_inquire_pd_partner_inform(
	struct tcpc_device *tcpc, uint32_t *vdos)
{
#ifdef CONFIG_USB_PD_KEEP_PARTNER_ID
	int ret = TCPM_SUCCESS;
	pd_port_t *pd_port = &tcpc->pd_port;

	if (tcpc->typec_attach_old == TYPEC_UNATTACHED)
		return TCPM_ERROR_UNATTACHED;

	if (!pd_port->pd_prev_connected)
		return TCPM_ERROR_NO_PD_CONNECTED;

	if (vdos == NULL)
		return TCPM_ERROR_PARAMETER;

	mutex_lock(&pd_port->pd_lock);
	if (pd_port->partner_id_present) {
		memcpy(vdos, pd_port->partner_vdos,
			sizeof(uint32_t) * VDO_MAX_NR);
	} else
		ret = TCPM_ERROR_NO_PARTNER_INFORM;
	mutex_unlock(&pd_port->pd_lock);

	return ret;
#else
	return TCPM_ERROR_NO_SUPPORT;
#endif	/* CONFIG_USB_PD_KEEP_PARTNER_ID */
}
EXPORT_SYMBOL(tcpm_inquire_pd_partner_inform);

int tcpm_inquire_pd_source_cap(
	struct tcpc_device *tcpc, struct tcpm_power_cap *cap)
{
	int ret = TCPM_SUCCESS;
	pd_port_t *pd_port = &tcpc->pd_port;

	if (tcpc->typec_attach_old == TYPEC_UNATTACHED)
		return TCPM_ERROR_UNATTACHED;

	if (!pd_port->pd_prev_connected)
		return TCPM_ERROR_NO_PD_CONNECTED;

	if (cap == NULL)
		return TCPM_ERROR_PARAMETER;

	mutex_lock(&pd_port->pd_lock);
	if (pd_port->remote_src_cap.nr) {
		cap->cnt = pd_port->remote_src_cap.nr;
		memcpy(cap->pdos, pd_port->remote_src_cap.pdos,
			sizeof(uint32_t) * cap->cnt);
	} else
		ret = TCPM_ERROR_NO_SOURCE_CAP;
	mutex_unlock(&pd_port->pd_lock);

	return ret;
}
EXPORT_SYMBOL(tcpm_inquire_pd_source_cap);

int tcpm_inquire_pd_sink_cap(
	struct tcpc_device *tcpc, struct tcpm_power_cap *cap)
{
	int ret = TCPM_SUCCESS;
	pd_port_t *pd_port = &tcpc->pd_port;

	if (tcpc->typec_attach_old == TYPEC_UNATTACHED)
		return TCPM_ERROR_UNATTACHED;

	if (!pd_port->pd_prev_connected)
		return TCPM_ERROR_NO_PD_CONNECTED;

	if (cap == NULL)
		return TCPM_ERROR_PARAMETER;

	mutex_lock(&pd_port->pd_lock);
	if (pd_port->remote_snk_cap.nr) {
		cap->cnt = pd_port->remote_snk_cap.nr;
		memcpy(cap->pdos, pd_port->remote_snk_cap.pdos,
			sizeof(uint32_t) * cap->cnt);
	} else
		ret = TCPM_ERROR_NO_SINK_CAP;
	mutex_unlock(&pd_port->pd_lock);

	return ret;
}
EXPORT_SYMBOL(tcpm_inquire_pd_sink_cap);

void tcpm_extract_power_cap_val(
	uint32_t pdo, struct tcpm_power_cap_val *cap)
{
	switch (pdo & PDO_TYPE_MASK) {
	case PDO_TYPE_FIXED:
		cap->type = TCPM_POWER_CAP_VAL_TYPE_FIXED;
		cap->ma = PDO_FIXED_EXTRACT_CURR(pdo);
		cap->min_mv = cap->max_mv = PDO_FIXED_EXTRACT_VOLT(pdo);
		break;
	case PDO_TYPE_VARIABLE:
		cap->type = TCPM_POWER_CAP_VAL_TYPE_VARIABLE;
		cap->ma = PDO_VAR_EXTRACT_CURR(pdo);
		cap->min_mv = PDO_VAR_EXTRACT_MIN_VOLT(pdo);
		cap->max_mv = PDO_VAR_EXTRACT_MAX_VOLT(pdo);
		break;
	case PDO_TYPE_BATTERY:
		cap->type = TCPM_POWER_CAP_VAL_TYPE_BATTERY;
		cap->uw = PDO_BATT_EXTRACT_OP_POWER(pdo);
		cap->min_mv = PDO_BATT_EXTRACT_MIN_VOLT(pdo);
		cap->max_mv = PDO_BATT_EXTRACT_MAX_VOLT(pdo);
		break;
	default:
		cap->type = TCPM_POWER_CAP_VAL_TYPE_UNKNOWN;
		cap->min_mv = 0;
		cap->max_mv = 0;
		cap->ma = 0;
		break;
	}
}
EXPORT_SYMBOL(tcpm_extract_power_cap_val);

/* Request TCPC to send PD Request */

int tcpm_dpm_pd_power_swap(
	struct tcpc_device *tcpc, uint8_t role, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_PR_SWAP_AS_SNK + role,
		.event_cb = event_cb,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_pd_power_swap);

int tcpm_dpm_pd_data_swap(
	struct tcpc_device *tcpc, uint8_t role, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_DR_SWAP_AS_UFP + role,
		.event_cb = event_cb,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_pd_data_swap);

int tcpm_dpm_pd_vconn_swap(
	struct tcpc_device *tcpc, uint8_t role, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_VCONN_SWAP_OFF + role,
		.event_cb = event_cb,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_pd_vconn_swap);

int tcpm_dpm_pd_goto_min(
	struct tcpc_device *tcpc, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_GOTOMIN,
		.event_cb = event_cb,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_pd_goto_min);

int tcpm_dpm_pd_soft_reset(
	struct tcpc_device *tcpc, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_SOFTRESET,
		.event_cb = event_cb,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_pd_soft_reset);

int tcpm_dpm_pd_get_source_cap(
	struct tcpc_device *tcpc, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_GET_SOURCE_CAP,
		.event_cb = event_cb,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_pd_get_source_cap);

int tcpm_dpm_pd_get_sink_cap(
	struct tcpc_device *tcpc, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_GET_SINK_CAP,
		.event_cb = event_cb,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_pd_get_sink_cap);

int tcpm_dpm_pd_request(struct tcpc_device *tcpc,
	int mv, int ma, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_REQUEST,
		.event_cb = event_cb,

		.tcp_dpm_data.pd_req.mv = mv,
		.tcp_dpm_data.pd_req.ma = ma,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_pd_request);

int tcpm_dpm_pd_request_ex(struct tcpc_device *tcpc,
	uint8_t pos, uint32_t max, uint32_t oper, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_REQUEST_EX,
		.event_cb = event_cb,

		.tcp_dpm_data.pd_req_ex.pos = pos,
	};

	if (oper > max)
		return TCPM_ERROR_PARAMETER;

	tcp_event.tcp_dpm_data.pd_req_ex.max = max;
	tcp_event.tcp_dpm_data.pd_req_ex.oper = oper;
	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_pd_request_ex);

int tcpm_dpm_pd_bist_cm2(
	struct tcpc_device *tcpc, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_BIST_CM2,
		.event_cb = event_cb,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_pd_bist_cm2);

int tcpm_dpm_pd_hard_reset(struct tcpc_device *tcpc)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_HARD_RESET,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_pd_hard_reset);

int tcpm_dpm_pd_error_recovery(struct tcpc_device *tcpc)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_ERROR_RECOVERY,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_pd_error_recovery);

int tcpm_dpm_vdm_discover_cable(
	struct tcpc_device *tcpc, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_DISCOVER_CABLE,
		.event_cb = event_cb,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);

}
EXPORT_SYMBOL(tcpm_dpm_vdm_discover_cable);

int tcpm_dpm_vdm_discover_id(
	struct tcpc_device *tcpc, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_DISCOVER_ID,
		.event_cb = event_cb,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_vdm_discover_id);

#ifdef CONFIG_USB_PD_ALT_MODE

int tcpm_inquire_dp_ufp_u_state(
	struct tcpc_device *tcpc, uint8_t *state)
{
	int ret = TCPM_SUCCESS;
	pd_port_t *pd_port = &tcpc->pd_port;

	if (tcpc->typec_attach_old == TYPEC_UNATTACHED)
		return TCPM_ERROR_UNATTACHED;

	if (!pd_port->pd_prev_connected)
		return TCPM_ERROR_NO_PD_CONNECTED;

	if (state == NULL)
		return TCPM_ERROR_PARAMETER;

	mutex_lock(&pd_port->pd_lock);
	*state = pd_port->dp_ufp_u_state;
	mutex_unlock(&pd_port->pd_lock);

	return ret;
}
EXPORT_SYMBOL(tcpm_inquire_dp_ufp_u_state);

int tcpm_dpm_dp_attention(struct tcpc_device *tcpc,
	uint32_t dp_status, uint32_t mask, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_DP_ATTENTION,
		.event_cb = event_cb,

		.tcp_dpm_data.dp_data.val = dp_status,
		.tcp_dpm_data.dp_data.mask = mask,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_dp_attention);

#ifdef CONFIG_USB_PD_ALT_MODE_DFP

int tcpm_inquire_dp_dfp_u_state(
	struct tcpc_device *tcpc, uint8_t *state)
{
	int ret = TCPM_SUCCESS;
	pd_port_t *pd_port = &tcpc->pd_port;

	if (tcpc->typec_attach_old == TYPEC_UNATTACHED)
		return TCPM_ERROR_UNATTACHED;

	if (!pd_port->pd_prev_connected)
		return TCPM_ERROR_NO_PD_CONNECTED;

	if (state == NULL)
		return TCPM_ERROR_PARAMETER;

	mutex_lock(&pd_port->pd_lock);
	*state = pd_port->dp_dfp_u_state;
	mutex_unlock(&pd_port->pd_lock);

	return ret;
}
EXPORT_SYMBOL(tcpm_inquire_dp_dfp_u_state);

int tcpm_dpm_dp_status_update(struct tcpc_device *tcpc,
	uint32_t dp_status, uint32_t mask, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_DP_STATUS_UPDATE,
		.event_cb = event_cb,

		.tcp_dpm_data.dp_data.val = dp_status,
		.tcp_dpm_data.dp_data.mask = mask,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_dp_status_update);

int tcpm_dpm_dp_config(struct tcpc_device *tcpc,
	uint32_t dp_config, uint32_t mask, tcp_dpm_event_cb event_cb)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_DP_CONFIG,
		.event_cb = event_cb,

		.tcp_dpm_data.dp_data.val = dp_config,
		.tcp_dpm_data.dp_data.mask = mask,
	};

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
EXPORT_SYMBOL(tcpm_dpm_dp_config);

#endif	/* CONFIG_USB_PD_ALT_MODE_DFP */
#endif	/* CONFIG_USB_PD_ALT_MODE */

#ifdef CONFIG_USB_PD_UVDM
int tcpm_dpm_send_uvdm(struct tcpc_device *tcpc,
	uint8_t cnt, uint32_t *data, bool wait_resp, tcp_dpm_event_cb event_cb)
{
	struct tcp_dpm_uvdm_data *uvdm_data;

	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_UVDM,
		.event_cb = event_cb,
	};

	if (cnt > PD_DATA_OBJ_SIZE)
		return TCPM_ERROR_PARAMETER;

	uvdm_data = &tcp_event.tcp_dpm_data.uvdm_data;

	uvdm_data->cnt = cnt;
	uvdm_data->wait_resp = wait_resp;
	memcpy(uvdm_data->vdos, data, sizeof(uint32_t) * cnt);

	return tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
}
#endif	/* CONFIG_USB_PD_UVDM */

int tcpm_put_tcp_dpm_event(
	struct tcpc_device *tcpc, struct tcp_dpm_event *event)
{
	bool ret;
	pd_port_t *pd_port = &tcpc->pd_port;

	if (tcpc->typec_attach_old == TYPEC_UNATTACHED)
		return TCPM_ERROR_UNATTACHED;

	if (!pd_port->pd_prev_connected)
		return TCPM_ERROR_NO_PD_CONNECTED;

	if (event->event_id >= TCP_DPM_EVT_IMMEDIATELY)
		ret = pd_put_tcp_pd_event(pd_port, event->event_id);
	else
		ret = pd_put_deferred_tcp_event(tcpc, event);

	if (!ret)
		return TCPM_ERROR_PUT_EVENT;

	return TCPM_SUCCESS;
}
EXPORT_SYMBOL(tcpm_put_tcp_dpm_event);

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

uint8_t tcpm_inquire_pd_charging_policy(struct tcpc_device *tcpc)
{
	pd_port_t *pd_port = &tcpc->pd_port;

	return pd_port->dpm_charging_policy;
}
EXPORT_SYMBOL(tcpm_inquire_pd_charging_policy);

int tcpm_set_pd_charging_policy(struct tcpc_device *tcpc, uint8_t policy)
{
	tcp_dpm_event_t tcp_event = {
		.event_id = TCP_DPM_EVT_REQUEST_AGAIN,
	};

	pd_port_t *pd_port = &tcpc->pd_port;

	mutex_lock(&pd_port->pd_lock);
	pd_port->dpm_charging_policy = policy;
	mutex_unlock(&pd_port->pd_lock);

	tcpm_put_tcp_dpm_event(tcpc, &tcp_event);
	return 0;
}
EXPORT_SYMBOL(tcpm_set_pd_charging_policy);

#ifdef CONFIG_USB_PD_ALT_MODE_RTDC
int tcpm_set_direct_charge_en(struct tcpc_device *tcpc, bool en)
{
	pd_port_t *pd_port = &tcpc->pd_port;

	mutex_lock(&pd_port->pd_lock);
	tcpc->pd_during_direct_charge = en;
	mutex_unlock(&pd_port->pd_lock);

	return 0;
}

bool tcpm_inquire_during_direct_charge(struct tcpc_device *tcpc)
{
	return tcpc->pd_during_direct_charge;
}
#endif	/* CONFIG_USB_PD_ALT_MODE_RTDC */

#endif /* CONFIG_USB_POWER_DELIVERY */
