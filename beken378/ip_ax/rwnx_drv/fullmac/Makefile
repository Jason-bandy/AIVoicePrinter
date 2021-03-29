# Enable A-MSDU support (need FW support)
## Select this if FW is compiled with AMSDU support
CONFIG_RWNX_SPLIT_TX_BUF ?= y
## Select this TO send AMSDU
CONFIG_RWNX_AMSDUS_TX ?= y

# Enable BFMER support (need FW support)
CONFIG_RWNX_BFMER ?= y

# Support of MU-MIMO transmission (need FW support)
ifeq ($(CONFIG_RWNX_BFMER), y)
CONFIG_RWNX_MUMIMO_TX ?= n
else
CONFIG_RWNX_MUMIMO_TX = n
endif

# Enable handling of radar event
CONFIG_RWNX_RADAR ?= y

# Enable HW queue for Broadcast/Multicast traffic (need FW support)
CONFIG_RWNX_BCMC ?= y

# Enable Monitor+Data interface support (need FW support)
CONFIG_RWNX_MON_DATA ?= n

# extra DEBUG config
CONFIG_RWNX_SW_PROFILING ?= n
CONFIG_RWNX_DBG ?= n

obj-m := rwnx_fdrv.o
rwnx_fdrv-y := rwnx_msg_tx.o          \
               rwnx_msg_rx.o          \
               rwnx_utils.o           \
               rwnx_cmds.o            \
               rwnx_irqs.o            \
               rwnx_cfgfile.o         \
               rwnx_strs.o            \
               rwnx_rx.o              \
               rwnx_tx.o              \
               rwnx_txq.o             \
               rwnx_main.o            \
               rwnx_mod_params.o      \
               rwnx_mesh.o            \
	       rwnx_platform.o	      \
	       rwnx_pci.o             \
               rwnx_dini.o            \
               rwnx_v7.o              \
               ipc_host.o             \
               rwnx_tdls.o

rwnx_fdrv-$(CONFIG_RWNX_RADAR)       += rwnx_radar.o
rwnx_fdrv-$(CONFIG_DEBUG_FS)         += rwnx_debugfs.o
rwnx_fdrv-$(CONFIG_DEBUG_FS)         += rwnx_fw_dump.o
rwnx_fdrv-$(CONFIG_DEBUG_FS)         += rwnx_fw_trace.o
rwnx_fdrv-$(CONFIG_NL80211_TESTMODE) += rwnx_testmode.o
rwnx_fdrv-$(CONFIG_RWNX_BFMER)       += rwnx_bfmer.o
rwnx_fdrv-$(CONFIG_RWNX_MUMIMO_TX)   += rwnx_mu_group.o

ccflags-y := -DCONFIG_RWNX_FULLMAC
ccflags-y += -I$(src)
ccflags-y += -I$(src)/..
ccflags-$(CONFIG_RWNX_RADAR) += -DCONFIG_RWNX_RADAR
ccflags-$(CONFIG_RWNX_MON_DATA) += -DCONFIG_RWNX_MON_DATA
ccflags-$(CONFIG_RWNX_BFMER) += -DCONFIG_RWNX_BFMER
ccflags-$(CONFIG_RWNX_SPLIT_TX_BUF) += -DCONFIG_RWNX_SPLIT_TX_BUF
ifeq ($(CONFIG_RWNX_SPLIT_TX_BUF), y)
ccflags-$(CONFIG_RWNX_AMSDUS_TX) += -DCONFIG_RWNX_AMSDUS_TX
endif
ccflags-$(CONFIG_RWNX_DBG) += -DCONFIG_RWNX_DBG
ccflags-$(CONFIG_RWNX_SW_PROFILING) += -DCONFIG_RWNX_SW_PROFILING
ccflags-$(CONFIG_RWNX_MUMIMO_TX) += -DCONFIG_RWNX_MUMIMO_TX

ifeq ($(CONFIG_RWNX_MUMIMO_TX), y)
ccflags-y += -DCONFIG_USER_MAX=2
else
ccflags-y += -DCONFIG_USER_MAX=1
endif

ifeq ($(CONFIG_RWNX_BCMC), y)
ccflags-y += -DNX_TXQ_CNT=5
else
ccflags-y += -DNX_TXQ_CNT=4
endif

# For old kernel (<=3.19)
ifeq ($(shell test $(VERSION) -lt 4 -a "$(CONFIG_VENDOR_RWNX)" = y ; echo $$?),0)
subdir-ccflags-y += -DCONFIG_VENDOR_RWNX_VHT_NO80
endif

quiet_cmd_genvers = GENVERSION $@
      cmd_genvers = ($(if $(KBUILD_EXTMOD),,$(srctree)/)$(src)/../mkvers.sh $@)

$(obj)/rwnx_main.o: $(obj)/rwnx_version_gen.h

$(obj)/rwnx_version_gen.h: FORCE
	$(call cmd,genvers)

clean-files := rwnx_version_gen.h
