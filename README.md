# [wb32-dfu-updater_cli](https://github.com/WestberryTech/wb32-dfu-updater)

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
