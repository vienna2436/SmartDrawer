# Smart Drawer
#### aka F.R.E.D. (Friendly Robotic Embedded Drawer)
A voice-activated drawer system powered by a Raspberry Pi and Arduino. The system integrates wake word detection, speech 
recognition, NFC-triggered drawer security, an LCD display, and precise drawer movement to make desktop storage 
more convenient and accessible.

This was a personal project I made to learn about hardware/software integration, embedded systems, and basic mechatronics. 
My goals were to build a functioning smart drawer and explore microprocessor/microcontroller processing, NFC technology, 
real-time audio processing, and project management. I also wanted to gain more exposure to project design and management, 
including detailed planning, 3D modeling, and system debugging. 


## Design Overview
### Mechanical Design
To implement the automatic drawer functionality, I used a custom-built wooden frame with two drawers driven by a 3D-printed rack and pinion mechanism 
using stepper motors for their precise control and strength. To save time, I altered an existing online model of a rack, pinion, and servo mount in TinkerCAD and 
Bambu Studio to fit my project needs, including a D-cut shaft of the stepper motor, custom rack lengths, and a 
specific mount size. I also decided to use ball-bearing drawer slides (side-mounted) for easier drawer movement. As for the
locking mechanism, I used a servo and a 3D-printed holder, which I also modeled. Additional features I added include an LCD module to print critical communication commands and an NFC tag that my iPhone reads to send a command to the Raspberry 
Pi over SSH to unlock the bottom drawer. 
### System Architecture 
The Raspberry Pi manages the high-level logic, including wake-word detection using Picovoice, speech recognition with Google's Speech Recognition library, NFC command processing, and communication with the Arduino.
The Arduino is used to control the hardware and execute the commands sent by the Pi, interfacing directly with stepper motors, a servo motor, and the LCD display.


## Setup Requirements
### Hardware Components
* Raspberry Pi 4B
* Arduino Uno
* Stepper Motors (NEMA 17) and Drivers (TB6600)
* Servo Motor
* NFC Tag
* LCD Display
* USB Microphone
* Drawer Frame and Shelves
### Software Components
* Raspberry Pi Libraries (Python):
   * `SpeechRecognition`: Recognizing voice commands
   * `sounddevice`: Real-time audio input stream
   * `pvporcupine`: Customized wake-word detection
   * `serial`: Serial communication between devices
* Arduino Libraries (C/C++):
   * `AccelStepper`: Stepper motor control
   * `Servo`: Servo motor control
   * `LiquidCrystal`: LCD Display
 
## Potential Future Additions
* Advanced voice recognition: Implement NLP techniques to improve understanding of natural voice commands
* Voice feedback: System responds to commands with audible feedback
* Additional access: Develop a companion mobile app for alternative access
* Ultrasonic distance: Use physical proximity to open drawers
