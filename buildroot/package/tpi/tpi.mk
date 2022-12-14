###########################################################
# turing pi CLI
###########################################################
TPI_VERSION:= 1.0.0
TPI_SITE:= $(TOPDIR)/../app/tpi    #这里是应用程序的位置。
TPI_SITE_METHOD:=local
TPI_INSTALL_TARGET:=YES
define TPI_BUILD_CMDS
$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef
define TPI_INSTALL_TARGET_CMDS
$(INSTALL) -D -m 0755 $(@D)/tpi $(TARGET_DIR)/bin
endef
define TPI_PERMISSIONS
/bin/tpi f 4755 0 0 - - - - -
endef
$(eval $(generic-package))
