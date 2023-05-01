###########################################################
# helloworld
###########################################################
 
HELLOWORLD_VERSION:= 1.0.0
HELLOWORLD_SITE:= $(TOPDIR)/../app/helloworld
HELLOWORLD_SITE_METHOD:=local
HELLOWORLD_INSTALL_TARGET:=YES
 
define HELLOWORLD_BUILD_CMDS
 $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef
 
define HELLOWORLD_INSTALL_TARGET_CMDS
 $(INSTALL) -D -m 0755 $(@D)/helloworld $(TARGET_DIR)/bin
endef
 
define HELLOWORLD_PERMISSIONS
 /bin/helloworld f 4755 0 0 - - - - - 
endef
 
$(eval $(generic-package))
