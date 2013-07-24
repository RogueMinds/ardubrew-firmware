# Arduino Temperature Controller

## Requirements

* [Dallas Temperature](https://github.com/milesburton/Arduino-Temperature-Control-Library.git)
* [One Wire](http://www.pjrc.com/teensy/td_libs_OneWire.html)
* [Time](http://arduino.cc/playground/Code/Time)

If you cannot see the USB serial port corresponding to the Arduino device, you may also need to
install the [Virtual COM Port Drivers](http://www.ftdichip.com/Drivers/VCP.htm).

## Usage

Download the Arduino IDE from [arduino.cc](http://arduino.cc/en/Main/Software) and place
the required libraries into the following location:

    Mac OS X:  ~/Documents/Arduino/libraries
    Windows:   %USERPROFILE%\My Documents\Arduino\libraries

**Note:** The **Time** library archive may extract to a subdirectory called **Time**, which
contains another subdirectory called **Time**. The subdirectory is the one that must be placed
in the /path/to/libraries location.
