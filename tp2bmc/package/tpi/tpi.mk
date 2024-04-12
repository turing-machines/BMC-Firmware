###########################################################
# turing pi CLI
###########################################################
TPI_VERSION:= b6772ed0516db04b14bd7ec50511fb16eeb4a641
TPI_SITE = $(call github,turing-machines,tpi,$(TPI_VERSION))
TPI_LICENSE = Apache-2.0
TPI_LICENSE_FILES = LICENSE
TPI_CARGO_INSTALL_OPTS = --features localhost,native-tls

$(eval $(cargo-package))
