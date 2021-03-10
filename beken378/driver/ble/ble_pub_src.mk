# -------------------------------------------------------------------
# BLE Pub Source file list
# -------------------------------------------------------------------

#ble pub
SRC_BLE_PUB_C += ./beken378/driver/ble/ble.c
SRC_BLE_PUB_C += ./beken378/driver/ble/modules/app/src/app_ble.c
SRC_BLE_PUB_C += ./beken378/driver/ble/modules/app/src/app_comm.c
SRC_BLE_PUB_C += ./beken378/driver/ble/modules/app/src/app_sdp.c
SRC_BLE_PUB_C += ./beken378/driver/ble/modules/app/src/app_sec.c
SRC_BLE_PUB_C += ./beken378/driver/ble/modules/app/src/app_task.c
SRC_BLE_PUB_C += ./beken378/driver/ble/plactform/driver/ble_icu/ble_icu.c
SRC_BLE_PUB_C += ./beken378/driver/ble/plactform/driver/uart/ble_uart.c
SRC_BLE_PUB_C += ./beken378/driver/ble/plactform/modules/arch/ble_arch_main.c
SRC_BLE_PUB_C += ./beken378/driver/ble/plactform/modules/common/RomCallFlash.c
SRC_BLE_PUB_C += ./beken378/driver/ble/plactform/modules/dbg/dbg.c
SRC_BLE_PUB_C += ./beken378/driver/ble/plactform/modules/dbg/dbg_mwsgen.c
SRC_BLE_PUB_C += ./beken378/driver/ble/plactform/modules/dbg/dbg_swdiag.c
SRC_BLE_PUB_C += ./beken378/driver/ble/plactform/modules/dbg/dbg_task.c
SRC_BLE_PUB_C += ./beken378/driver/ble/plactform/modules/rf/src/ble_rf_xvr.c
SRC_BLE_PUB_C += ./beken378/driver/ble/profiles/comm/src/comm.c
SRC_BLE_PUB_C += ./beken378/driver/ble/profiles/comm/src/comm_task.c
SRC_BLE_PUB_C += ./beken378/driver/ble/profiles/prf/src/prf.c
SRC_BLE_PUB_C += ./beken378/driver/ble/profiles/prf/src/prf_utils.c
SRC_BLE_PUB_C += ./beken378/driver/ble/profiles/sdp/src/sdp_service.c
SRC_BLE_PUB_C += ./beken378/driver/ble/profiles/sdp/src/sdp_service_task.c

#ble mesh pub
ifeq ($(CFG_SUPPORT_BLE_MESH),1)
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mesh_api.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mesh_api_msg.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mesh_param_int.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mm_api.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/mm_api_msg.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/m_api.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/mesh_api/m_api_msg.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_route.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_tb.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_tb_bind.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_tb_replay.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/common/mm_tb_state.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_bat.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_dtt.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_loc.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_lvl.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_oo.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_plvl.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_poo.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/gens/mm_gens_prop.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc_ctl.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc_hsl.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc_ln.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lightc/mm_lightc_xyl.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lights/mm_lights.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lights/mm_lights_ctl.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lights/mm_lights_hsl.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/lights/mm_lights_ln.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/Scenes/m_fnd_Scenes.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/transition_time/m_fnd_generic_transition_time.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/vendor/mm_vendors.c
SRC_BLE_PUB_C += ./beken378/driver/ble/beken_ble_sdk/mesh/src/models/vendor/mm_vendor_midea.c
SRC_BLE_PUB_C += ./beken378/driver/ble/modules/app/src/app_mesh.c
SRC_BLE_PUB_C += ./beken378/driver/ble/modules/app/src/app_mm_msg.c
SRC_BLE_PUB_C += ./beken378/driver/ble/modules/gernel_api/mesh_general_api.c
SRC_BLE_PUB_C += ./beken378/driver/ble/modules/mesh_model/ali/app_light_ali_server.c
endif
