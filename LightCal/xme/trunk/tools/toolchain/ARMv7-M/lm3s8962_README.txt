Using OpenOCD to reflash the controller (LM3S8962)
---------------------------------------------------------
If you have any questions please contact Vladimir Rupanov
rupanov@in.tum.de
---------------------------------------------------------
-2) Extract this archive to OpenOCD directory, merging "interface" and 
"target" directories with existing ones.

-1) Connect the Luminary Board with USB debug connector.

0) Go to Device Manager. 
Replace the "Stellaris Evaluation Board A" driver with the driver from OpenOCD
 (unpack the archive first):
c:\Program Files\OpenOCD\0.4.0\drivers\libusb-win32_ft2232_driver-100223.zip
Make sure you have a group "LibUSB-Win32 Devices" with 
"Luminary Micro Stellaris LM3S811 Evaluation Kit (Channel A)".

1) 
Normally, OpenOCD is found 
Start "ocdconnect.bat". The window should be showing something like this:
______________________________________________________________________________
>openocd -f interface\luminary.cfg -f target\lm3s8962.cfg -f mylm.cfg
Open On-Chip Debugger 0.4.0 (2010-02-22-19:05)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.berlios.de/doc/doxygen/bugs.html
1000 kHz
jtag_nsrst_delay: 100
jtag_ntrst_delay: 100
Port mappings
init
Info : clock speed 1000 kHz
Info : JTAG tap: lm3s8962.cpu tap/device found: 0x3ba00477 (mfg: 0x23b, part: 0xba00, ver: 0x3)
Info : lm3s8962.cpu: hardware has 6 breakpoints, 4 watchpoints
reset
Info : JTAG tap: lm3s8962.cpu tap/device found: 0x3ba00477 (mfg: 0x23b, part: 0xba00, ver: 0x3)
______________________________________________________________________________

2) Start a telnet client (like PuTTY). Connect localhost:4444.
You should get a welcome string from Open On-Chip Debugger. All commands are 
put in from this console.

3) Commands available and tested:
halt                        
 Halt the target
 
reset
 Resets the target
 
stellaris mass_erase 0      
 Erase bank 0 of flash. You should always perform flash erase before 
 programming. Target should be halted.
 
flash write_bank 0 <filename_bin> <offset>
 Programs flash with data from <filename_bin> starting from <offset>.
 !!!NOTE!!!: Under Windows filenames should be given with double backslashes, 
            like: e:\\eCar\\bin\\frontAxle.bin
 
help
 lists all commands and allows getting more info on these.
 
4) For further needs you could have configured your own command sequences, 
writing these directly into .cfg files and feeding these to OpenOCD with -f 
option.