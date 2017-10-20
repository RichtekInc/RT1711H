# RT1711H

## Changelog

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
