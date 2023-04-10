# [wb32-dfu-updater_cli](https://github.com/WestberryTech/wb32-dfu-updater)

[![Current Version](https://img.shields.io/github/tag/WestberryTech/wb32-dfu-updater.svg)](https://github.com/WestberryTech/wb32-dfu-updater/tags)
[![License](https://img.shields.io/github/license/WestberryTech/wb32-dfu-updater)](https://github.com/WestberryTech/wb32-dfu-updater/blob/master/LICENSE)
[![GitHub contributors](https://img.shields.io/github/contributors/WestberryTech/wb32-dfu-updater.svg)](https://github.com/WestberryTech/wb32-dfu-updater/pulse/monthly)
[![GitHub forks](https://img.shields.io/github/forks/WestberryTech/wb32-dfu-updater.svg?style=social&label=Fork)](https://github.com/WestberryTech/wb32-dfu-updater/)
[<img src="https://s1.ax1x.com/2022/05/18/OoUE79.png" width="2%" height="3%" />](https://formulae.brew.sh/formula/wb32-dfu-updater_cli)
[<img src="https://s1.ax1x.com/2022/05/18/OoawP1.png" width="2%" height="3%" />](https://packages.msys2.org/package/mingw-w64-x86_64-wb32-dfu-updater?repo=mingw64)  

`wb32-dfu-updater` is a host tool used to download and upload firmware to/from WB32 MCU via USB. (`wb32-dfu-updater_cli` is the command line version)

## How to build wb32-dfu-updater_cli:

**Windows system please run on MINGW64!!!**

### Prerequisites :  
- cmake-3.0.0 or more see http://www.cmake.org/cmake/resources/software.html
- libusb-1.0.24 or more see https://github.com/libusb/libusb/releases/download/v1.0.24/libusb-1.0.24.tar.bz2

### Install the wb32-dfu-updater_cli :
- ``` git clone https://github.com/WestberryTech/wb32-dfu-updater.git ```
- ``` cd wb32-dfu-updater ```
- ``` bash ./bootstrap.sh install ```
- If Permission denied is displayed, use the ``` sudo bash ./bootstrap.sh install ```

## Windows driver

You can found the Windows driver for wb32-dfu-updater_cli in the `driver` directory.

To install the Windows driver for wb32-dfu-updater_cli, you should unzip the package and run `winusb_install.bat`.

## Linux bash error

If you encounter either of the following errors

``` bash: ./bootstrap.sh: cannot execute: required file not found ```

or

``` ./bootstrap.sh: line 2: $'\r': command not found ```

then you will need to convert bootstrap.sh from DOS to Unix encoding before attempting to run the script.


The easiest way to do this is by using the CLI tool dos2unix.

``` sudo apt install dos2unix ```

``` dos2unix bootstrap.sh ```
