# Official Turing Pi BMC firmware

![GitHub Workflow
Status](https://img.shields.io/github/actions/workflow/status/turing-machines/BMC-firmware/build.yml?branch=master&style=for-the-badge)

The Turing Pi is a compact AI & edge computing cluster purposed to run cloud
stacks and AI inference at the edge. Find out more on our
[website](turingpi.com).

The firmware is based on a Linux 5.4 kernel and hosts a web interface with a
REST API to control and manage the board. The packages
[bmcd](https://www.github.com/turing-machines/bmcd),
[tpi](https://github.com/turing-machines/tpi) and
[bmc-ui](https://github.com/turing-machines/BMC-UI) are part of the firmware and
facilitate most of this functionality.

## Reporting issues & requesting features

It is recommended to use the issue tracker of the current BMC-Firmware repository
to request features or submit bug reports. We are open to all feedback and
improvements. We scan the dependent repositories regularly for activity, but for
visibility reasons, we will mainly use the issue tracker of this repository.

## BMC chip specs

 * CPU Allwinner T113-S3 (ARM Cortex-A7)
 * 128 MB DDR3 RAM
 * 128 MB SPI NAND flash (MX35LF1GE4AB)
 * EEPROM (24C02C)
 * 3 port Gigabit Ethernet Switch (RTL8370MB)
 * Ethernet PHYceiver (RTL8201F-VB-CG)
 * SD card slot

## Install firmware

>**Note: If you are running a firmware version lower than < v2.0.0, you must do
>a one-time-only SD card upgrade to version v2.0.0.**

>**Note 2: Prior to v2.0.0 a third-party tool 'PhoenixSuit' was required to
>flash firmware. This tool is obsoleted, and only the methods described on our
>website can be used to flash your board.**

The latest firmware images can be found on the [release page](https://github.com/turing-machines/BMC-firmware/releases).

On our
[website](https://help.turingpi.com/hc/en-us/articles/8686945524893-Baseboard-Management-Controller-BMC-)
you can find more information on installing firmware.

**We recently announced the v2 release of our firmware. The documentation pages will
be updated as soon as v2 goes public.**

## Build

### Using Docker

In the root of our repository there is a Dockerfile that can be used to build a
development container. It has all the dependencies needed to build the firmware.
We would recommend that you go through the official docker documentation. But if
you want to quickly build and run it, execute the following commands in the root
of your repository:

```shell
# build the docker image
docker build . -t tp_container

# open the docker container
docker run -it -v $PWD:$PWD --workdir $PWD tp_container
```

Run the following command inside the docker container:

```shell
cd buildroot
make BR2_EXTERNAL=../tp2bmc tp2bmc_defconfig && make
```

Once the build completed, both the SD card image and upgrade package
(`rootfs.erofs`)
can be found in the buildroot output folder, `buildroot/output/images/`.

Both UI and `tpi` can be used to write the upgrade package to your board. When
you try to upload the image via the firmware upgrade tab on the UI. You will
notice that the file extension is not matching the one the UI expects. You can
ignore this, a `.tpu` image is nothing more than a rename of `rootfs.erofs`
image. To smoothen the experience, you can decide to change the extension of the
file to `.tpu`.
