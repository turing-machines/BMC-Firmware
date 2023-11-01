###########################################################
# turing pi CLI
###########################################################
TPI_VERSION:= d8c0bc6c1cd4f7054b8645b3398ba263662c8d0e
TPI_SITE = $(call github,turing-machines,tpi,$(TPI_VERSION))
TPI_LICENSE = Apache-2.0
TPI_LICENSE_FILES = LICENSE
TPI_CARGO_INSTALL_OPTS = --features localhost,native-tls

$(eval $(cargo-package))
