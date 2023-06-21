# Official Turing Pi BMC firmware

![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/turing-machines/BMC-firmware/build.yml?style=for-the-badge)

Welcome to the new home of the Turing Pi firmware. The Turing Pi is a compact AI
& edge computing cluster purposed to run cloud stacks and AI inference at the
edge. Find out more on our [website](turingpi.com).

### Wenyi0421 Repository

The Wenyi0421 repository was formerly used for firmware development. We have
decided to branch off and commit our fresh inputs to this repository, which
implies that we will gradually phase out the Wenyi0421 repository. Currently, we
are working towards a first release, which will land here soon.

## Specs Turing Pi V2

 * CPU Allwinner T113-S3 (ARM Cortex-A7)
 * 128 MB DDR3 RAM
 * 1 GB SPI NAND flash (MX35LF1GE4AB)
 * EEPROM (24C02C)
 * 3 port Gigabit Ethernet Switch (RTL8370MB)
 * Ethernet PHYceiver (RTL8201F-VB-CG)
 * SD card slot
 * Buildroot SDK 2023.02.1
    * Linux 5.4
    * Linaro GCC 7.2
    * U-boot bootloader

## Install firmware

There are two ways to upgrade the firmware on your current board.
1. Using the PhoenixSuit. It allows you to write an entire new image to the board.
2. Using an OTA package. This package can be uploaded to the board via the web UI. (This method does not update the kernel).

The required images can be found in one of our release packages. get the latest
[here](https://github.com/turing-machines/BMC-firmware/releases).

On our
[website](https://help.turingpi.com/hc/en-us/articles/8686945524893-Baseboard-Management-Controller-BMC-)
you can find an elaborate manual on performing an update using both methods.

## Setup build environment

Currently, only X86 Linux build hosts are supported. They are required to have
the following packages installed:

```shell
sudo apt-get install build-essential subversion git-core
libncurses5-dev zlib1g-dev gawk flex quilt libssl-dev xsltproc
libxml-parser-perl mercurial bzr ecj cvs unzip lib32z1 lib32z1-dev lib32stdc++6
libstdc++6 libncurses-dev u-boot-tools mkbootimg -y
```

## Build

To build the firmware image manually, execute the following command:

```shell
cd buildroot
make BR2_EXTERNAL=../tp2bmc tp2bmc_defconfig
make
```

To package up a swu (for OTA update via webui):

```shell
cd output/images/
cp -a ../../../tp2bmc/swupdate/* .
./genSWU.sh 1.0.0
```
