RWNX_VERS_NUM=6.4.5.0

#
# WAITING FOR KCONFIG {
#
CONFIG_RWNX_SOFTMAC ?= m
CONFIG_RWNX_FULLMAC ?= m
CONFIG_RWNX_FHOST ?= n

#
# DEBUG OPTIONS
CONFIG_RWNX_UM_HELPER_DFLT ?= "/dini/dini_bin/rwnx_umh.sh"

#
# FW ARCH:
CONFIG_RWNX_SDM ?= n
CONFIG_RWNX_TL4 ?= n

# IPC version
CONFIG_RWNX_OLD_IPC ?= n

# Support of P2P DebugFS for enabling/disabling NoA and OppPS
CONFIG_RWNX_P2P_DEBUGFS ?= y

#
# } // WAITING FOR KCONFIG
#

subdir-ccflags-$(CONFIG_DEBUG_FS) += -DCONFIG_RWNX_DEBUGFS
subdir-ccflags-$(CONFIG_DEBUG_FS) += -DCONFIG_RWNX_UM_HELPER_DFLT=\"$(CONFIG_RWNX_UM_HELPER_DFLT)\"
subdir-ccflags-$(CONFIG_RWNX_P2P_DEBUGFS) += -DCONFIG_RWNX_P2P_DEBUGFS

# FW VARS
subdir-ccflags-y += -DNX_VIRT_DEV_MAX=4
subdir-ccflags-y += -DNX_REMOTE_STA_MAX=10
subdir-ccflags-y += -DNX_MU_GROUP_MAX=62
subdir-ccflags-y += -DNX_TXDESC_CNT=64
subdir-ccflags-y += -DNX_TX_MAX_RATES=4
subdir-ccflags-y += -DNX_CHAN_CTXT_CNT=3

# FW ARCH:
subdir-ccflags-$(CONFIG_RWNX_SDM) += -DCONFIG_RWNX_SDM
subdir-ccflags-$(CONFIG_RWNX_TL4) += -DCONFIG_RWNX_TL4
subdir-ccflags-$(CONFIG_RWNX_OLD_IPC) += -DCONFIG_RWNX_OLD_IPC

obj-$(CONFIG_RWNX_SOFTMAC) += softmac/
obj-$(CONFIG_RWNX_FULLMAC) += fullmac/
obj-$(CONFIG_RWNX_FHOST)   += fhost/

all: modules

modules clean:
	@$(PWD)/mklink.sh
	$(MAKE) -C $(KERNELDIR) O=$(KBUILDDIR) M=$(PWD) $@
	@$(PWD)/mklink.sh clean

