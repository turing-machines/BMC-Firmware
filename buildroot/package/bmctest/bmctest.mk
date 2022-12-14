###########################################################
# bmctest
###########################################################
 
BMCTEST_VERSION:= 1.0.0
BMCTEST_SITE:= $(TOPDIR)/../app/bmctest
BMCTEST_SITE_METHOD:=local
BMCTEST_INSTALL_TARGET:=YES
 
define BMCTEST_BUILD_CMDS
 $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef
 
define BMCTEST_INSTALL_TARGET_CMDS
 $(INSTALL) -D -m 0755 $(@D)/bmctest $(TARGET_DIR)/bin
endef
 
define BMCTEST_PERMISSIONS
 /bin/bmctest f 4755 0 0 - - - - - 
endef
 
$(eval $(generic-package))