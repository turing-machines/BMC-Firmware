################################################################################
#
# rtl8189es
#
################################################################################

RTL8189ES_VERSION = e58bd86c9d9408c648b1246a0dd76b16856ec172
RTL8189ES_SITE = $(call github,jwrdegoede,rtl8189ES_linux,$(RTL8189ES_VERSION))
RTL8189ES_LICENSE = GPL-2.0

RTL8189ES_MODULE_MAKE_OPTS = \
	CONFIG_RTL8189ES=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR) \
	USER_EXTRA_CFLAGS="-Wno-error"

define RTL8189ES_LINUX_CONFIG_FIXUPS
	$(call KCONFIG_ENABLE_OPT,CONFIG_NET)
	$(call KCONFIG_ENABLE_OPT,CONFIG_WIRELESS)
	$(call KCONFIG_ENABLE_OPT,CONFIG_CFG80211)
	$(call KCONFIG_ENABLE_OPT,CONFIG_MMC)
endef

$(eval $(kernel-module))
$(eval $(generic-package))
