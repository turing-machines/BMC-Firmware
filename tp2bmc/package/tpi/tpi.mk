###########################################################
# turing pi CLI
###########################################################
TPI_VERSION:= 913d5fd3d134253042787d7a404f3a03e6b83772
TPI_SITE = $(call github,turing-machines,tpi,$(TPI_VERSION))
TPI_LICENSE = Apache-2.0
TPI_LICENSE_FILES = LICENSE
TPI_CARGO_INSTALL_OPTS = --features local-only

$(eval $(cargo-package))
