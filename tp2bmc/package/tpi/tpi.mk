###########################################################
# turing pi CLI
###########################################################
TPI_VERSION:= 6ef9484687f90a83b14660362decf76597f54fd6
TPI_SITE = $(call github,turing-machines,tpi,$(TPI_VERSION))
TPI_LICENSE = Apache-2.0
TPI_LICENSE_FILES = LICENSE
TPI_CARGO_INSTALL_OPTS = --features localhost,native-tls

$(eval $(cargo-package))
