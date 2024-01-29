###########################################################
# turing pi CLI
###########################################################
TPI_VERSION:= 1e08bc82f6e635e91d2e370ca5db7beed5238ee4
TPI_SITE = $(call github,turing-machines,tpi,$(TPI_VERSION))
TPI_LICENSE = Apache-2.0
TPI_LICENSE_FILES = LICENSE
TPI_CARGO_INSTALL_OPTS = --features localhost,native-tls

$(eval $(cargo-package))
