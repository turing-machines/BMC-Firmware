###########################################################
# turing pi CLI
###########################################################
TPI_VERSION:= da0aed1cb673cbab83bd6d2138c807230ff895e2
TPI_SITE = $(call github,turing-machines,tpi,$(TPI_VERSION))
TPI_LICENSE = Apache-2.0
TPI_LICENSE_FILES = LICENSE
TPI_CARGO_INSTALL_OPTS = --features localhost,native-tls

$(eval $(cargo-package))
