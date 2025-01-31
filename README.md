# Official Turing Pi BMC firmware

![GitHub Workflow
Status](https://img.shields.io/github/actions/workflow/status/turing-machines/BMC-firmware/build.yml?branch=master&style=for-the-badge)

The Turing Pi is a compact AI & edge computing cluster purposed to run cloud
stacks and AI inference at the edge. Find out more on our
[website](https://turingpi.com).

The firmware is based on a Linux 5.4 kernel and hosts a web interface with a
REST API to control and manage the board. The packages
[bmcd](https://www.github.com/turing-machines/bmcd),
[tpi](https://github.com/turing-machines/tpi) and
[bmc-ui](https://github.com/turing-machines/BMC-UI) are part of the firmware and
facilitate most of this functionality.

## Table of Contents

- [Table of Contents](#table-of-contents)
- [Reporting issues \& requesting features](#reporting-issues--requesting-features)
- [BMC chip specs](#bmc-chip-specs)
- [Install firmware](#install-firmware)
- [Build / Development](#build--development)
- [Quickstart](#quickstart)
- [Start DevContainer](#start-devcontainer)
- [macOS / Darwin](#macos--darwin)
  - [macOS / Darwin Build Performance](#macos--darwin-build-performance)
- [Windows](#windows)
  - [Build Performance](#build-performance)
- [Scripts](#scripts)
- [Commands](#commands)
- [Building](#building)
  - [Linux / Windows](#linux--windows)
  - [OSX](#osx)
  - [Manual](#manual)
  - [Native](#native)
- [Output](#output)
- [Development](#development)

## Reporting issues & requesting features

It is recommended to use the issue tracker of the current BMC-Firmware repository
to request features or submit bug reports. We are open to all feedback and
improvements. We scan the dependent repositories regularly for activity, but for
visibility reasons, we will mainly use the issue tracker of this repository.

## BMC chip specs

- CPU Allwinner T113-S3 (ARM Cortex-A7)
- 128 MB DDR3 RAM
- 128 MB SPI NAND flash (MX35LF1GE4AB)
- EEPROM (24C02C)
- 3 port Gigabit Ethernet Switch (RTL8370MB)
- Ethernet PHYceiver (RTL8201F-VB-CG)
- SD card slot

## Install firmware

>**Note: If you are running a firmware version lower than < v2.0.0, you must do
>a one-time-only SD card upgrade to version v2.0.0.**
>
>**Note 2: Prior to v2.0.0 a third-party tool 'PhoenixSuit' was required to
>flash firmware. This tool is obsoleted, and only the methods described on our
>website can be used to flash your board.**

The latest firmware images can be found on the [release page](https://github.com/turing-machines/BMC-firmware/releases).

On our
[website](https://docs.turingpi.com/docs/turing-pi2-bmc-firmware-upgrade)
you can find more information on installing firmware.

## Build / Development

If you want to build the BMC firmware yourself, there is some preparation
needed, which depends on your working environment.

The build process uses [Buildroot](https://buildroot.org/) for further documentation
can be found [here](https://buildroot.org/downloads/manual/manual.html).
Buildroot is not included in this repository and needs to be downloaded once
before building.

This repository uses a `devcontainer` for a uniform development environment. The
devcontainer is available in a linux and darwin version. Windows users are recommended
to use WSL or Docker-Desktop.

There are several scripts available within the `scripts` directory to facilitate
easy development and building of the firmware. See the section [Scripts](#scripts) for
more information. Furthermore, there are several special `git` commands available to
ease development and the building process. More information about the commands are
available in the [Commands](#commands) section.

> **IMPORTANT**
>
> Before starting a build the `configure.sh` script must be run, this script must also be
> rerun everything buildroot is updated.

## Quickstart

1. Clone repository
2. Open in VSCode or any other editor that natively supports devcontainers
3. Start devcontainer for your platform, `Windows` users should use the `Linux` devcontainer
4. The `devcontainer` will auto-configure the repository `git` commands
5. Run `git configure`
6. Run `git build`
7. Firmware artifacts will appear in the `dist` directory

## Start DevContainer

If no popup appears to notify you of starting the devcontainer you can so so by searching for the
`devcontainer` commands in the VSCode `Command Pallete` which can be opened on Windows and Linux with
`CTRL + Shift + P` and on macOS with `Shift + Command + P`.

Choose `Rebuild and Reopen in Container` to start the devcontainer, after which you can select which OS
version you want to start.

![devcontainer Rebuild and Reopen in Container](docs/devc-rebuild-and-open.png?raw=true "Rebuild and Reopen in Container")
![devcontainer select OS](docs/devc-select-os.png?raw=true "Select OS")

## macOS / Darwin

Builds on OSX are different, and there is a devcontainer configuration specifically
available for darwin. The difference is that for builds on darwin the building process
takes place within a volume. The reason is APFS, the default APFS of macOS is
case-insensitive. Only on a darwin machine that uses the special APFS+Case-Sensitive
the default linux devcontainer can be used.

### macOS / Darwin Build Performance

For the best performance on macOS Docker-Desktop is recommended with the use of
the currently `BETA` feature of `Docker VMM` as Virtual Machine.
The `Apple Virtualization Framework` can cause Docker-Desktop to crash during a build.

## Windows

For building on Windows, both devcontainers can be used either the linux or darwin container.
Before starting the container under Windows, you might have an issue regarding the End-Of-Line
of the files.

The best way to handle this is to normalize the repository to `LF` line endings.
Run the following command before starting the devcontainer.

```shell
git config --local include.path ../.gitconfig
git config --global core.eol lf
git config --global core.autocrlf input
```

Now after this you can normalize the repository.

```shell
git rm -rf --cached .
git reset --hard HEAD
```

This will force all the files to have the correct line endings.

### Build Performance

When building on Windows with Anti-Virus software present, it is important to understand that
this can severly impact build speed as each file will be scanned during the build process.
Furthermore, the on-access scanner of Anti-Virus software can cause build compiliation corruption.
In order to bypass this, Windows users can use the macOS / darwin devcontainer, this will build
according to the same build process in a volume and the developer can use the `git sync` command
or `./scripts/sync.sh` to sync files between the host and the devcontainer.

## Scripts

The repository provides several scripts to facilitate easy development. All scripts are located in
the scripts directory.

| Script       | Description                                                                                                                     |
| ------------ | ------------------------------------------------------------------------------------------------------------------------------- |
| build.sh     | Script to build the firmware, firmware artifacts are placed in the `dist` directory, configure.sh must be run first             |
| clean.sh     | Cleanup repository removes the `buildroot` and `dist` directories                                                               |
| configure.sh | Configure the repository with the buildroot, this script must be run everytime is buildroot is updated or on a clean repository |
| init.sh      | This is the devcontainer initialization script, only used by the devcontainer on startup                                        |
| sync.sh      | Only for macOS / darwin, synchronize changes to the host                                                                        |

> **NOTE**
>
> If an additional script is added then some extra steps are required. The provided gitconfig turns off `filemode` this is due to the fact that development
> takes place on multiple platforms. However, we want to be able to execute the scripts after we have committed them, so when writing the script giving
> it `chmod +x` and then commiting is not sufficient. In order to commit the execute bit to the repsoitory the following command must be given
> to commit the execute bit to the git repository.
>
> `git update-index --chmod=+x <FILE>`
>
> This will stage the execute bit to the git staging, after which it can be commited with a message `chore: update file permissions`.

## Commands

When using the devcontainer the git repository is automatically configured to extend
the git commands to include the additional aliases for development.

If you are building on a native host you need to configure this manually.
This can be done by running the following command.

```shell
git config --local include.path ../.gitconfig
```

This command will extend the git config of the repository with the .gitconfig from the
repository.

All these commands are added as subcommands of the `git` command.
Example to run the `root` command you run `git root`.

If you want to build the firmware, run `git build`.

| Git Command | Description                                                                                  |
| ----------- | -------------------------------------------------------------------------------------------- |
| root        | Display root path of repository                                                              |
| sha1        | This will print the 8 char short sha of the current commit                                   |
| configure   | Configure the repository by setting up buildroot, must be run everytime buildroot is updated |
| build       | Build the firmware                                                                           |
| cleanup     | Cleanup the repository                                                                       |
| sync        | Used by macOS / darwin users to sync the changes between the container and the host          |

## Building

The recommended way is to build using the provided devcontainer, manual builds are
also possible. All devcontainers or manual builds use the same `Dockerfile` in the
root directory of this repository. It has all the dependencies needed to build the firmware.

> The build process needs approx. 5GB to 16GB disk space. On OSX you need that
> amount of space reserved and free in the the Virutal Machine of Docker or Rancher
> Desktop.

### Linux / Windows

Start the devcontainer, after the workspace is up and running you can start your development or
build the firmware using either the scripts from the script directory or the provided `git`
aliases. Because Linux and Windows have a case-sensitive filesystem the build can actually take
place on the host filesystem through the devcontainer mounted repository host directory.

The workspace directory `/work` in the devcontainer is the repository directory on the host.

### OSX

Start the devcontainer, after the workspace is up and running you can start your development or
build the firmware using either the scripts from the script directory or the provided `git`
aliases. MacOS uses APFS which is a case-insensitive filesystem, this causes build problems.
For this reason the workspace directory `/work` of the devcontainer is a docker volume which
bypasses the filesystem restriction. The repository on the host is mounted in the devcontainer
in the `/mnt` directory.

In order to sync changes back and forth between the host and container if needed the command
`git sync` can be used. However the devcontainer `/work` workspace is a full working git
repository. The log of the syncing between host and container can be viewed in the container
log file `/tmp/sync.log`.

> **IMPORTANT**
>
> The .git directory is **NOT** synced between the host and the devcontainer when a sync is initiated.
> This is to avoid corruption of the git repository.

### Manual

We would recommend that you go through the official docker documentation for
further details. If you want to quickly build and run it, execute the following
commands in the root of your repository:

```shell
# On the host: build the docker image
docker build . -t bmc-firmware

# On the host: enter the container
docker run -it --rm -v $PWD:/src -w /src -u $(id -u):$(id -g) bmc-firmware
# NOTE: the shell prompt might be a bit garbled, this is fine
#       the -u $(id -u):$(id -g) parameter ensures that the generated files
#       are owned by your user

# inside of the container: prepare buildroot
./scripts/configure.sh

# Build the firmware
./scripts/build.sh
```

### Native

Currently, only X86 Linux build hosts are supported. They are
required to have the following packages installed:

> **Commands**
>
> If you want the `git` aliases to work check the section [Commands](#commands) and run the
> command to activate the repository gitconfig.

```shell
# install packages needed for build
sudo apt-get -y install \
  build-essential subversion git-core \
  libncurses5-dev zlib1g-dev gawk flex quilt libssl-dev xsltproc \
  libxml-parser-perl mercurial bzr ecj cvs unzip zlib1g-dev \
  libstdc++6 libncurses-dev u-boot-tools mkbootimg

# prepare buildroot
./scripts/configure.sh

# build
./scripts/build.sh
```

## Output

After the build is completed the OTA image and the SDCard image are
copied to the `dist` directory, and SHA256 hashes are generated.

The RAW images are located in the `buildroot/output/images` directory.

- `rootfs.erofs`: OTA image
- `tp2-bmc-firmware-sdcard.img`: SDCard image

The SHA256 checksums are generated in using the `binary` format of the
`sha256sum` command. This can be identified by opening the `*.sha256` file,
when there is an asterisks `*` in front of the filename, this identifies that
the checksum requires binary validation. Using the default `sha256sum` text mode
as is default for the `sha256sum` command the generated sha is identical however
we do not want to generate a text sha of a binary file.

The build script automatically corrects the filename when copying the images to
the `dist` directory. However, this is for aesthetics only, and to ensure that
the Web UI accepts the generated OTA image.

## Development

If you require an additional buildroot directory you can run the configure script and
set the target directory or install dir where to put the buildroot.

For configure run:

```shell
git configure --help
```

The build script also provides additional arguments.

```shell
git build --help
```

The .gitignore allows for two workign directories while developing.

- tmp
- wip
