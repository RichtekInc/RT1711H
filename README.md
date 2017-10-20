# RT1711H

## Changelog

### v019
- [Linux] Fix Coding Style : remove typedef	
- [PE] Discover Cable only if VconnSource just for PD30


### v018
- [Linux] Remove dummy EXPORT_SYMBOL
- [TCPC] Fixed dr_swap failed after pr_swap
- [TYPEC] Ignore RpLvl Change during AttachWait state.	
- [PE] Send CableReset before DiscoverCable (DR_SWAP & PR_SWAP)
  - CONFIG_USB_PD_RESET_CABLE	
- [PE] Optimize SenderResponse Timer	
- [PE] Ensure VconnSource before DiscoverCable	
- [PE] Change RX_CAP first during dr_swap to UFP
- [PE] enter ping_test_mode if receive ping message
- [DPM] Dynamic disable Vconn if EMark absent
  - CONFIG_TCPC_VCONN_SUPPLY_MODE
- [TCPM] Support block version interface
  - CONFIG_USB_PD_BLOCK_TCPM
  - CONFIG_USB_PD_TCPM_CB_2ND 
- [PPS] Support Sink PPS based on PD20
  - CONFIG_USB_PD_REV30_PPS_SINK

### v017
- [TCPC] Fix tcpc_timer deadlock warning
- [TCPC] Add option for Attach Wake lock Timeout
  - CONFIG_TCPC_ATTACH_WAKE_LOCK_TOUT
- [TCPC] Add debug info pre-string
  - CONFIG_TCPC_DBG_PRESTR	
- [TYPEC] Alert TYPEC state change after pr_swap
- [PE] Fixed receive tx_failed during HardReset/ReadyState	
- [RT1711] Optimize CCNoise Counter
- [RT1715] Optimize RXDZ setting

### v016
- [CONFIG] Change force discharge config name
  - New: CONFIG_TYPEC_CAP_FORCE_DISCHARGE 
  - Old: CONFIG_USB_PD_SRC_FORCE_DISCHARGE 
- [TCPC] Postpone Enter Lower Power Mode
- [TCPC] Retry Enter Lower Power Mode	
  - CONFIG_TCPC_LPM_CONFIRM
  - CONFIG_TCPC_LPM_POSTPONE
- [TCPC] Handle Ra-detach independently
- [TCPC] pd_dbg_info_thread : Using wakeup instead of polling	
- [TYPEC] Still DRP Toggling after Rd detect if VBUS not vsafe0v
- [TYPEC] Add CustomHV flag for legacy cable
  - CONFIG_TYPEC_CAP_CUSTOM_HV
- [TYPEC] Add legacy cable solution for another type TA
  - CONFIG_TYPEC_CHECK_LEGACY_CABLE2
- [TYPEC] Fixed AttachWait_Sink notification
- [PE] Postpone reply ps_ready for Vconn Swap to On
- [PE] AMS flow : get_source_cap from sink
- [PE] Ignore hreset if type_c only (Against Spec)
  - CONFIG_USB_PD_SNK_IGNORE_HRESET_IF_TYPEC_ONLY
- [PE] Fixed Sink still wait hreset after 3 times retry
- [DPM] If device doesn't reply discoverID, doesn't send discoverSVID
- [DPM] Fixed PD_STATE notification
- [VDM] Dynamic allocate svid_data	
- [DUAL_ROLE] Using mode operation for TYPEC_ONLY

### v015
- [LINUX] Fix Coding Style
- [RT1711] Optimize I2C access
- [TYPEC] Modify TypeC Role Swap Rule (Compatibility)
- [PE] Ignore repeat pd_ready after pr_swap
  - CONFIG_USB_PD_IGNORE_PS_RDY_AFTER_PR_SWAP
- [DPM] Implement Charging Policy
- [VDM] Ignore VDM Attention if act as UFP
- [VDM] Stop reply or send VDM message if RX Busy
  - CONFIG_USB_PD_STOP_SEND_VDM_IF_RX_BUSY
  - CONFIG_USB_PD_STOP_REPLY_VDM_IF_RX_BUSY

### v014
- [Config] Suggest disable WatchDog & INTRST
- [RT1711] Sync rt1711h device driver revision to 14
- [TCPC] New Event Queue for deferred_tcp_event
- [TCPC] Remove DPM event : PD request & VDM request
- [TCPC] Change dual role class behavior
- [TYPEC] Support Sink VBUS from Audio ACC device
  - CONFIG_TYPEC_CAP_AUDIO_ACC_SINK_VBUS
- [TYPEC] Notify AttachWait_Sink & AttachWait_SRC
  - CONFIG_TYPEC_NOTIFY_ATTACHWAIT_SNK
  - CONFIG_TYPEC_NOTIFY_ATTACHWAIT_SRC
- [TYPEC] Modify detach condition for different attached state
- [PE] Support Auto Discharge (Experimental)
  - CONFIG_TYPEC_CAP_AUTO_DISCHARGE
- [PE] Support Force Discharge (Experimental)
  - CONFIG_USB_PD_SRC_HIGHCAP_POWER
  - CONFIG_USB_PD_SRC_FORCE_DISCHARGE
- [PE] Support Partner request first during ready (Compatibility)
  - CONFIG_USB_PD_PARTNER_CTRL_MSG_FIRST
- [PE] Error Recovery if vbus invalid after cancel pr_swap
- [PE] Ignore unknown event during HReset
- [DPM] Support delay UFP flow after startup (Compatibility)
  - CONFIG_USB_PD_UFP_FLOW_DELAY
- [DPM] Enable delay DFP flow after HardReset (Compatibility)
  - CONFIG_USB_PD_DFP_FLOW_DELAY_RESET
- [DPM] Random flow delay (Compatibility)
  - CONFIG_USB_PD_RANDOM_FLOW_DELAY
- [DPM] Support try to pr_swap to Sink if bad power (Mobile)
  - CONFIG_USB_PD_SRC_TRY_PR_SWAP_IF_BAD_PW
- [DPM] Keep Partner ID & SVID for inquire 
  - CONFIG_USB_PD_KEEP_PARTNER_ID
  - CONFIG_USB_PD_KEEP_SVIDS
- [DPM] Remove VDM Header in CableVDO variable
- [DPM] Retry auto pr_swap & dr_swap if communication failed 
- [DPM] Change attemp get flags rule (get_sink_cap)
- [TCPM] Change to new TCPM I/F, Old I/F can enable by:
  - CONFIG_USB_PD_LEGACY_TCPM 
- Fixed Compiler error for Type-C Only Case

### v013
- [SNK] Implement Power Transitions (pSnkStdby)
  - CONFIG_USB_PD_SNK_STANDBY_POWER
- [SNK] Modify HardReset Transitions (iSafe0mA)
  - CONFIG_USB_PD_SNK_HRESET_KEEP_DRAW
- [PE] Provide shutdown flow
  - CONFIG_TCPC_SHUTDOWN_CC_DETACH
  - CONFIG_TCPC_SHUTDOWN_VBUS_DISABLE
- [TYPEC] Improve Legacy Cable Support
- [TYPEC] Modify attach_wake_lock timing.
- [DPM] Provide set attach_wake_lock function
- [DPM] Provide set usb sink current function
- [RT1711] Provide I2C low reset function 
  - CONFIG_TCPC_I2CRST_EN
- [RT1711] Change init_cc_params for Noise margin
- [DTS] Suggest enable NoSuspend flag

### v012
- [PE] Fixed Cable VDO index wrong.
- [PE] Using PD_BUG_ON instead of BUG_ON()
- [PE] Fixed HResetAlert issue during unattach 
- [DPM] Improve auto dr_swap flow
- [TypeC] Support TypeC Role Swap
- [TypeC] Improve Legacy Cable Support
- [TCPM] Add error code
- [RT1711] Fixed kzalloc bug
- [RT1711] Disable AutoIdle if Vconn On

### v011
- [TypeC] Notify Rp-Level Change during Attached.SNK
- [SRC] Add PD VSafe0V timeout for Compatibility
- [VDM] Using Enter/Exit Mode & UVDM timer
- [DPM] Implement auto pr_swap & dr_swap
- [DPM] Fixed ateemp discover ID bug
- [DPM] Option for DFP flow delay
- [PE] Support Custom DBGACC
- [PE] Only enable INTRST & WatchDog in Source Mode 	
- [RT1711] Modify clock gating flow.

### v010
- Change License & fix coding style
- [PE] Add retry DFP flow
- [PE] Modify Discover Cable flow
- [PE] Miss process VDM GoodCRC

### v009
- [TYPEC] Support DebugACC-SNK
- [TYPEC] Support Custom-SRC
- [TYPEC] Support PowerOff Charge (don't reset Charger)
- [TYPEC] Compatibility with abnormal cable or device
	- Legacy AtoC Cable
	- Hub with vbus > vsafe0v
- [PE] Fix GotoMin flow	
- [PE] Support UVDM
- [PE] Support Custom DebugACC
- [DP] Fix UFP_U issue.

### v008
- [TCPCI] Add control type for VBUS Control.
- [TCPCI] Add new VBUS Control : disable power path for Sink/Source 
- [RT1711] Remove unused register.
- [RT1711] Current Limit if Vconn OC
- [RT1711] Init cc param for different case 
- [PE] Add new event : VBUS Present 
- [TYPEC] Update Legacy Cable detect flow 
- [TCPCM] Add more i/f for custom

### v007
- [PE] Remove busy wait for tx finish
- [PE] Add a option for Receive HardReset over limit. <ESD?>
- [TYPEC] Support TypeC spec 1.2
- [TYPEC] Change solution for legacy CtoA cable
- [TYPEC] Support local Rp level as default to pass Ellisys test
- [PR_SWAP] Fix compatibility test with Apple MACBook (act as Sink)
  - source may turn off power & change CC before we got GoodCRC or Accept
- [PR_SWAP] Always do good power check for local_ext & local_src case
- [TCPCI] Change source vbus & sink vbus default value (0 -> -1)

### v006
- [LINUX] Fix Coding Style

### v005
- [TCPC] Add Wakeup Watchdog timer.
- [SNK] Add a new config for fast response type-c only source.
- [DFP] Add a new flag for DFP's DiscoverCableID.
- [SNK] Handle goto_min command.
- [SWAP] Reject swap request if DPM_CAP declare no support.
- [TYPEC] Support legacy CtoA cable.
- [Power] Add clock gating function if PD isn't enabled.
- [TYPEC] Fix SinkOnly or SourceOnly init issue

### v004
- [DP] Change dfp_d configuration rule
- [DPM] No support modal operation, still response ACK for DiscoverSVID Mode
- [SNK] Send HardReset if protocol error occurs during PE_SNK_TRANSITION_SINK state

### v003
- Fix the tcpc_init sequence
  - Add new tcpm_xxx function : discover_cable_id;
- [DP] Change DPSTS_DFP_D and DPSTS_UFP_D definition
- [SNK] Fix the incorrectness of internal request voltage state
        when match source_caps case  not found
- [SWAP] Add dpm_ack_immediately flag
		 Fix double PD message issue for vconn_swap & dr_swap flow
- [DFP] Fix no sop' capability after enter ready state.
- [SNK] Fix no hard_reset in PE_SNK_SEND_SOFT_RESET state.
		Sender Response triggers, but we don't do anything.
- [SNK] Fix the issue that it cannot return to ready state in PE_SRC_GET_SINK_CAP state.
		Sender Response triggers, but we don't do anything.
- [SRC] Fix only sending 4 times SourceCap after HardReset
- [DP] Change DP flow to notify related drivers dp_status.
- [SW-Reset] Igrone unknown event for send-soft-reset case. (send Hard-reset after timeout)

### v002
- Change init flow to avoid invalid INT
- Setup a timeout for register_notifier
- Add debug flag

### v001
- Initial release
- Support both rt1711h and rt1711