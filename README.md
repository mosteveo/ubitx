uBitx_Teensy v0.01

This is a fork of the v1.061 code written by KD8CEC (kd8cec@gmail.com) (http://www.hamskey.com) for the uBitx hardware.


Major changes from KD8CEC's code include swapping out the tired and slow Arduino nano for 
a Teensy 3.5. This allows further expansion and vastly less memory restraints. Other changes include moving from a 4bit 2x16 LCD display to a 4x20 I2C LCD display. The screen saver (idle time) was disabled for now.


In progress: GPS, serial and manual time setting support


Planned expansions include SD card support (channels?, messaging?), GPS support (automatic QTH and time keeping (for WSPR etc..), SWR meter, antenna tuner, etc... fell free to add to the code with the expanded memory. 

My personal favorite modification to come is running the uBitx in headless mode (only a on/off switch) and controlled through a raspberry pi with 5" touchscreen. On the Pi will be loaded FLDIGI, WSJTx, etc... now the digital modes are not limited by a small LCD screen and minimal user interaction.



########################### Original Readme from KD8CEC ################################

#IMPORTANT INFORMATION
----------------------------------------------------------------------------
- Now Release Version 1.061 on my blog (http://www.hamskey.com)
- You can download and compiled hex file and uBITX Manager application on my blog (http://www.hamskey.com)

#NOTICE
----------------------------------------------------------------------------
I received uBITX a month ago and found that many features are required, and began coding with the idea of implementing minimal functionality as a general hf transceiver rather than an experimental device.

Most of the basic functions of the HF transceiver I thought were implemented.
The minimum basic specification for uBITX to operate as a radio, I think it is finished.
So I will release the 0.27 version and if I do not see the bug anymore, I will try to change the version name to 1.0.
Now uBITX is an HF radio and will be able to join you in your happy hams life.
Based on this source, you can use it by adding functions.

http://www.hamskey.com

DE KD8CEC
kd8cec@gmail.com

#uBITX
uBITX firmware, written for the Raduino/Arduino control of uBITX transceivers
This project is based on https://github.com/afarhan/ubitx and all copyright is inherited.
The copyright information of the original is below.

KD8CEC
----------------------------------------------------------------------------
Prepared or finished tasks for the next version
  - Reduce Program size
  - uBITX with RTL-SDR 
  - Direct control for Student
  
----------------------------------------------------------------------------
## REVISION RECORD
1.061
 - Added WSPR
   You only need uBITX to use WSPR. No external devices are required.
   Added Si5351 module for WSPR
 - Update uBITX Manager to Version 1.0
 - Reduce program size
   for WSPR
   for other Module
 - Fixed IF Shift Bug
   Disable IF Shift on TX
   IF shift available in USB mode
   Fixed cat routine in IF Shift setup
- Bugs fixed
   cw start delay option
   Auto key Bug
   (found bug : LZ1LDO)
   Message selection when Auto Key is used in RIT mode
   (found bug : gerald)
- Improve CW Keying (start TX)

1.04
  - Optimized from Version1.03
  - Reduce program size (97% -> 95%)
  
1.03
  - Change eBFO Calibration Step (50 to 5)
  - Change CW Frequency Display type
  
1.02
  - Applied CW Start Delay to New CW Key logic (This is my mistake when applying the new CW Key Logic.Since uBITX operations are not significantly affected, this does not create a separate Release, It will be reflected in the next release.) - complete
  - Modified CW Key Logic for Auto Key, (available AutoKey function by any cw keytype) - complete
  - reduce cpu use usage (working)
  - reduce (working)

1.01
  - Fixed Cat problem with (IAMBIC A or B Selected)
1.0 
  - rename 0.30 to 1.0
  
0.35
  - vfo to channel bug fixed (not saved mode -> fixed, channel has frequency and mode)
  - add Channel tag (ch.1 ~ 10) by uBITX Manager
  - add VFO to Channel, Channel To VFO
  
0.34
  - TX Status check in auto Keysend logic
  - optimize codes
  - change default tune step size, and fixed bug
  - change IF shift step (1Hz -> 50Hz)
  
0.33
  - Added CWL, CWU Mode, (dont complete test yet)
  - fixed VFO changed bug.
  - Added Additional BFO for CWL, CWL
  - Added IF Shift
  - Change confirmation key PTT -> function key (not critical menus)
  - Change CW Key Select type, (toggle -> select by dial)
  
0.32
  - Added function Scroll Frequencty on upper line
  - Added Example code for Draw meter and remarked (you can see and use this code in source codes)
  - Added Split function, just toggle VFOs when TX/RX

0.31
  - Fixed CW ADC Range error
  - Display Message on Upper Line (anothor VFO Frequency, Tune Step, Selected Key Type)

0.30
 - implemented the function to monitor the value of all analog inputs. This allows you to monitor the status of the CW keys connected to your uBITX. 
 - possible to set the ADC range for CW Keying. If no setting is made, it will have the same range as the original code. If you set the CW Keying ADC Values using uBITX Manager 0.3, you can reduce the key error.
 - Added the function to select Straight Key, IAMBICA, IAMBICB key from the menu.
 - default Band select is Ham Band mode, if you want common type, long press function key at band select menu, uBITX Manager can be used to modify frequencies to suit your country.

0.29
 - Remove the use of initialization values in BFO settings - using crruent value, if factory reset
 - Select Tune Step, default 0, 20, 50, 100, 200, Use the uBITX Manager to set the steps value you want. You can select Step by pressing and holding the Function Key (1sec ~ 2sec).
 - Modify Dial Lock Function, Press the Function key for more than 3 seconds to toggle dial lock.
 - created a new frequency tune method. remove original source codes, Threshold has been applied to reduce malfunction. checked the continuity of the user operating to make natural tune possible.
 - stabilize and remove many warning messages - by Pullrequest and merge
 - Changed cw keying method. removed the original code and applied Ron's code and Improved compatibility with original hardware and CAT commnication. It can be used without modification of hardware.
 
0.28
 - Fixed CAT problem with hamlib on Linux
 - restore Protocol autorecovery logic

0.27 
   (First alpha test version, This will be renamed to the major version 1.0)
 - Dual VFO Dial Lock (vfoA Dial lock)
 - Support Ham band on uBITX
   default Hamband is regeion1 but customize by uBITX Manager Software
 - Advanced ham band options (Tx control) for use in all countries. You can adjust it yourself.   
 - Convenience of band movement

0.26
  - only Beta tester released & source code share
  - find a bug on none initial eeprom uBITX - Fixed (Check -> initialized & compatible original source code)
  - change the version number 0.26 -> 0.27
  - Prevent overflow bugs
  - bug with linux based Hamlib (raspberry pi), It was perfect for the 0.224 version, but there was a problem for the 0.25 version.  
    On Windows, ham deluxe, wsjt-x, jt65-hf, and fldigi were successfully run. Problem with Raspberry pi.

0.25
  - Beta Version Released
    http://www.hamskey.com/2018/01/release-beta-version-of-cat-support.html
  - Added CAT Protocol for uBITX
  - Modified the default usb carrier value used when the setting is wrong.
  - Fixed a routine to repair when the CAT protocol was interrupted.

0.24
  - Program optimization
    reduce usage ram rate (string with M() optins)
  - Optimized CAT protocol for wsjt-x, fldigi

0.23
  - added delay_background() , replace almost delay() to delay_background for prevent timeout
  - cat library compatible with FT-817 Command 
    switch VFOA / VFOB, 
    Read Write CW Speed
    Read Write CW Delay Time
    Read Write CW Pitch (with sidetone)
    All of these can be controlled by Hamradio deluxe.

  - modified cat libray function for protocol for CAT communication is not broken in CW or TX mode
  - Ability to change CW Delay
  - Added Dial Lock function
  - Add functions CW Start dely (TX -> CW interval)
  - Automatic storage of VFO frequency
    It was implemented by storing it only once when the frequency stays 10 seconds or more after the change.
    (protect eeprom life)

  
0.22
  - fixed screen Update Problem
  - Frequency Display Problem - Problems occur below 1Mhz
  - added function Enhanced CAT communication
  - replace ubitx_cat.ino to cat_libs.ino
  - Save mode when switching to VFOA / VFOB


0.21
  - fixed the cw side tone configuration.
  - Fix the error that the frequency is over.
  - fixed frequency display (alignment, point) 


0.20 
  - original uBITX software (Ashhar Farhan)

## Original README.md
uBITX firmware, written for the Raduino/Arduino control of uBITX transceigers

Copyright (C) 2017,  Ashhar Farhan

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
