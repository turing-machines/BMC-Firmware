###########################################################
#
# bmc-ui
###########################################################

BMC_UI_VERSION = v3.1.0
BMC_UI_SITE = https://github.com/turing-machines/BMC-UI/releases/download/$(BMC_UI_VERSION)
BMC_UI_LICENSE = GPL-2.0
BMC_UI_LICENSE_FILES = LICENSE
define BMC_UI_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/srv/bmcd/www/
	cp -r $(BMC_UI_SRCDIR)* $(TARGET_DIR)/srv/bmcd/www/
	chmod 755 $(TARGET_DIR)/srv/bmcd/www/
endef

$(eval $(generic-package))
