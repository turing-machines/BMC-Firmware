###########################################################
# turing pi CLI
###########################################################
TPI_VERSION:= 0.1.0
TPI_SITE = $(call github,turing-machines,tpi,$(TPI_VERSION))
TPI_INSTALL_TARGET =YES
TPI_LICENSE = Apache-2.0
TPI_LICENSE_FILES = LICENSE
TPI_INSTALL_TARGET = YES

$(eval $(cargo-package))
