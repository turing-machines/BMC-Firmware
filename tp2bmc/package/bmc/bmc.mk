###########################################################
# bmc
###########################################################
 
BMC_VERSION:= 1.1.0
BMC_SITE:= $(TOPDIR)/../app/bmc
BMC_SITE_METHOD:=local
BMC_INSTALL_TARGET:=YES
BMC_DEPENDENCIES = cjson tpi_rs 
 
define BMC_BUILD_CMDS
 $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef
 
define BMC_INSTALL_TARGET_CMDS
 $(INSTALL) -D -m 0755 $(@D)/bmc $(TARGET_DIR)/bin
endef
 
define BMC_PERMISSIONS
 /bin/bmc f 4755 0 0 - - - - - 
endef
 
$(eval $(generic-package))
