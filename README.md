# RT1711H

## Changelog

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
