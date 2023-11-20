###########################################################
#
# bmc-ui
###########################################################

BMC_UI_VERSION = v1.0.1
BMC_UI_SITE = $(call github,turing-machines,BMC-UI,$(BMC_UI_VERSION))
BMC_UI_LICENSE = GPL-2.0
BMC_UI_LICENSE_FILES = LICENSE
define BMC_UI_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/srv/bmcd/www/
	cp -r $(BMC_UI_SRCDIR)/dist/* $(TARGET_DIR)/srv/bmcd/www/
	chmod 755 $(TARGET_DIR)/srv/bmcd/www/
endef

$(eval $(generic-package))
