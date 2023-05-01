Package/xr829-firmware = $(call Package/firmware-default,Xradio xr829 firmware)

define Package/xr829-firmware/install
	$(INSTALL_DIR) $(1)/$(FIRMWARE_PATH)
	$(INSTALL_DATA) $(TOPDIR)/package/firmware/linux-firmware/xr829/boot_xr829.bin $(1)/$(FIRMWARE_PATH)/boot_xr829.bin
	$(INSTALL_DATA) $(TOPDIR)/package/firmware/linux-firmware/xr829/etf_xr829.bin $(1)/$(FIRMWARE_PATH)/etf_xr829.bin
	$(INSTALL_DATA) $(TOPDIR)/package/firmware/linux-firmware/xr829/fw_xr829.bin $(1)/$(FIRMWARE_PATH)/fw_xr829.bin
ifeq ($(CONFIG_XR829_USE_40M_SDD), y)
	$(INSTALL_DATA) $(TOPDIR)/package/firmware/linux-firmware/xr829/sdd_xr829_40M.bin $(1)/$(FIRMWARE_PATH)/sdd_xr829.bin
	$(INSTALL_DATA) $(TOPDIR)/package/firmware/linux-firmware/xr829/fw_xr829_bt_40M.bin $(1)/$(FIRMWARE_PATH)/fw_xr829_bt.bin
else
	$(INSTALL_DATA) $(TOPDIR)/package/firmware/linux-firmware/xr829/sdd_xr829.bin $(1)/$(FIRMWARE_PATH)/sdd_xr829.bin
	$(INSTALL_DATA) $(TOPDIR)/package/firmware/linux-firmware/xr829/fw_xr829_bt.bin $(1)/$(FIRMWARE_PATH)/fw_xr829_bt.bin
endif
endef
$(eval $(call BuildPackage,xr829-firmware))
