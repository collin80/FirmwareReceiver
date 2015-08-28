# FirmwareReceiver
Interacts with due_can to allow for a remote unit to upgrade the current sketch. 

### Features
- CANBus bootloader without actually needing a bootloader (arduino sketches work with no funny business!)

Note: The flash storage is reset every time you upload a new sketch to your Arduino. So, do not upload
sketches via the IDE except the very first time! Thereafter use the special tools. But, otherwise
sketches run without modification.

