<a id="top"></a>
Setup Arduino IDE for Pro Trinket
===================================
<br>**Install Driver for Pro Trinket, see [Using the USB bootloader](https://learn.adafruit.com/introducing-pro-trinket/starting-the-bootloader):**

- Download and install the [Adafruit Drivers](https://learn.adafruit.com/introducing-pro-trinket/windows-setup) for Windows.
- Connect Pro Trinket to USB port, specify location (toplevel directory) of driver.
- (If needed, manually Update Driver in Windows Device Manager.)
  
![](image/device_manager.png)

<br>**In Arduimo 1.8.5 open File > Preferences:**

- Add Board Manager URL for Adafruit boards (see [list](https://github.com/arduino/Arduino/wiki/Unofficial-list-of-3rd-party-boards-support-urls)):  
  https://adafruit.github.io/arduino-board-index/package_adafruit_index.json

![](image/arduino_preferences.png)

<br>**[List of additional Boards Manager URLs](https://github.com/arduino/Arduino/wiki/Unofficial-list-of-3rd-party-boards-support-urls) (click folder icon in above panel):**

![](image/arduino_additional_boards_managers_urls.png)

<br>**Select board and programmer:**

Open board manager, Tools > Board, Board Manager

- Select **Contributed**.
- Click **Adafruit AVR Boards** (Installing...).
- *Quit and restart Arduino IDE*.
- Select Tools > Board > **Pro Trinket 3V/12MHz (USB)**
- Select Tools > Programmer > **USBtinyISP**

<br>**Test if program can be uploaded to Pro Trinket:**

- Load blink example, File > Examples > 01.Basics > **Blink**
- Compile code: Sketch > **Verify/Compile** (Ctrl+R)
- **Activate bootloader** on Trinket: press button on Pro Trinket:
- Red <span style="color:red">LED</span> **glows** brighter/less bright
- Upload program: Sketch > **Upload** (Ctrl+U)::
- Red <span style="color:red">LED</span> **blinks** 1s on, 1s off

