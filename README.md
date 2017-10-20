# RT1711H

## Changelog

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
