###########################################################
# turing pi CLI
###########################################################
TPI_VERSION:= e4b1b17bcda85af40a80059819a1ae412d2495bf
TPI_SITE = $(call github,turing-machines,tpi,$(TPI_VERSION))
TPI_LICENSE = Apache-2.0
TPI_LICENSE_FILES = LICENSE
TPI_CARGO_INSTALL_OPTS = --features local-only

$(eval $(cargo-package))
