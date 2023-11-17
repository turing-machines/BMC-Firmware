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

## Manual builds

If you want to build the BMC firmware yourself, there is some preparation
needed, which depends on your working environment.

The recommended way is to build using Docker, for that is a `Dockerfile` in the
root directory of this repository, that can be used to build a development
container. It has all the dependencies needed to build the firmware.

> The build process needs approx. 5GB to 10GB disk space. On OSX you need that
> amount of space reserved and free in the the LIMA VM of Docker or Rancher
> Desktop.

The build process uses [Buildroot](https://buildroot.org/) further documentation
can be found [here](https://buildroot.org/downloads/manual/manual.html).

### Using Docker on Linux

We would recommend that you go through the official docker documentation for
further details. If you want to quickly build and run it, execute the following
commands in the root of your repository:

```shell
# On the host: build the docker image
docker build . -t tp_container

# On the host: enter the container
docker run -it --rm -v $PWD:/src -w /src/buildroot -u $(id -u):$(id -g) tp_container
# NOTE: the shell prompt might be a bit garbled, this is fine
#       the -u $(id -u):$(id -g) parameter ensures that the generated files
#       are owned by your user

# inside of the container: prepare buildroot
make BR2_EXTERNAL=../tp2bmc tp2bmc_defconfig

# build
make
```

### Using Docker on OSX

Build of the BMC firmware is also possible here by using [Rancher
Desktop](https://rancherdesktop.io) or [Docker
Desktop](https://www.docker.com/products/docker-desktop/). The way Docker and
directory forwarding into the is working on OSX we need some addition step in
the manual build process. Those might not be needed for Docker Desktop, but
strange issues might happen. So if unsure follow this guide, it also speeds up
building on OSX significantly.

```shell
# On OSX shell: create container
docker build . -t tp_container

# On OSX shell: create a docker volume for building volume
docker volume create turing-pi-bmc-firmware

# On OSX shell: check out the repository in the volume
docker run -it -v turing-pi-bmc-firmware:/src -w /src tp_container \
  git clone https://github.com/turing-machines/BMC-Firmware.git

# On OSX shell: enter the container
docker run -it --rm -v turing-pi-bmc-firmware:/src -w /src/buildroot tp_container

# inside of the container: prepare buildroot
make BR2_EXTERNAL=../tp2bmc tp2bmc_defconfig

# build
make
```

> NOTE: Due to the ways this works the generated firmware files are on the
> volume

> NOTE: this steps also work on native Linux with Docker.

> Q: Why do we need this on OSX? A: This is because on Docker Desktop and
> Rancher Desktop on OSX use a remote filesystem via some tunneling mechanisms.
> This does not resemble all needed POSIX capabilities, and leads to strange
> errors and most likely non-working builds.

### Build nativly on a Debian or Ubuntu-like host

Currently, only X86 Linux build hosts are supported. They are
required to have the following packages installed:

```shell
# install packages needed for build
sudo apt-get -y install \
  build-essential subversion git-core \
  libncurses5-dev zlib1g-dev gawk flex quilt libssl-dev xsltproc \
  libxml-parser-perl mercurial bzr ecj cvs unzip zlib1g-dev \
  libstdc++6 libncurses-dev u-boot-tools mkbootimg

# prepare buildroot
make BR2_EXTERNAL=../tp2bmc tp2bmc_defconfig

# build
make
```

> NOTE: This might work on WSL on Windows with Ubuntu, too.

Once the build completed, the SD card image and upgrade package
(`rootfs.erofs`)
can be found in the buildroot output folder, `buildroot/output/images/`.

Both UI and `tpi` can be used to write the upgrade package to your board. When
you try to upload the image via the firmware upgrade tab on the UI, You will
notice that the file extension is not matching the one the UI expects. You can
ignore this, a `.tpu` image is nothing more than a rename of `rootfs.erofs`
image. To smoothen the experience, you can decide to change the extension of the
file to `.tpu`.
