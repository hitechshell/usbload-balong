## About

usbload is an an emergency USB boot loader utility for Huawei LTE modems and routers with Balong V2R7, V7R11 and V7R22 chipsets.
It loads the "raminit" and "payload" through an emergency serial port, accessible if the firmware is corrupted or the boot pin (test point) is shorted to ground.

**This utility can make your device unbootable!**
Use it only if you fully understand all risks and consequences. In case of any issues, you're on your own. Do not expect any help.

## What is raminit?

Raminit, as the name suggests, is responsible for initializing RAM. This is a small (about 4kb) file that is loaded into small RAM, which does not require initialization.

Source code and build instructions can be found at https://github.com/hitechshell/balong-raminit

## What is payload?

Any binary file that is compiled to run on a suitable architecture. For example U-Boot or legacy bootloader from vendor.

## Thanks

Thanks to forth32 for the emergency serial port boot algorithm.
