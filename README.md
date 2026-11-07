# Hostel Security System

An Arduino Mega 2560-based hostel security prototype that combines **PIR
motion detection**, **RFID card verification**, an **I2C LCD**, and an
**audible buzzer alarm**.

The system monitors a protected area. When motion is detected, it starts
a 10-second verification window. An authorised RFID card can be scanned
during this period to grant access and silence the buzzer. If no
authorised card is presented before the timer expires, the system
activates the alarm.

> **Project type:** Academic / educational prototype\
> **Controller:** Arduino Mega 2560\
> **Language:** Arduino C/C++

------------------------------------------------------------------------

## Table of contents

-   [Features](#features)
-   [How it works](#how-it-works)
-   [Hardware requirements](#hardware-requirements)
-   [Wiring and pin connections](#wiring-and-pin-connections)
-   [Software requirements](#software-requirements)
-   [Installation and setup](#installation-and-setup)
-   [Configuration](#configuration)
-   [Testing the system](#testing-the-system)
-   [Current implementation and
    limitations](#current-implementation-and-limitations)
-   [Possible future improvements](#possible-future-improvements)
-   [Repository structure](#repository-structure)
-   [References](#references)

## Features

-   Detects movement using a PIR motion sensor.
-   Starts a 10-second RFID verification period after a motion event.
-   Checks scanned RFID data against a configured authorised card ID.
-   Activates a buzzer if the verification period expires without
    successful authentication.
-   Deactivates the buzzer when an authorised card is scanned.
-   Displays system states and access messages on a 16×2 I2C LCD.
-   Prints sensor readings, RFID data, and state information to the
    Serial Monitor.
-   Returns to standby after successful access or the configured
    area-clear reset condition.

## How it works

1.  **Standby:** The LCD displays `System Active` and `Safe Mode`; the
    buzzer is off.
2.  **Motion detection:** The sketch detects the configured PIR input
    condition and starts tracking.
3.  **Verification:** The LCD asks the user to scan an ID card. The
    verification timer starts.
4.  **Alarm:** If 10 seconds elapse before successful authorisation, the
    buzzer turns on and the LCD displays an intruder alert.
5.  **RFID check:** The EM-18 reader sends the card ID to the Arduino
    Mega over the `Serial1` hardware serial interface.
6.  **Authorised card:** If the received ID matches the configured ID,
    access is granted, the buzzer is turned off, and a welcome message
    is displayed.
7.  **Reset:** The system returns to standby after the welcome message.

The main program uses the Boolean state variables `isTracking`,
`isAlarming`, and `accessGranted` to manage these states. The countdown
is measured with `millis()`.

## Hardware requirements

  -----------------------------------------------------------------------
  Component                           Purpose
  ----------------------------------- -----------------------------------
  Arduino Mega 2560                   Main controller

  PIR motion sensor                   Detects movement in the monitored
                                      area

  EM-18 RFID reader                   Reads RFID card/tag IDs and sends
                                      them serially

  RFID card or tag                    Provides the ID used for
                                      authorisation

  16×2 I2C LCD                        Displays system messages

  Buzzer                              Provides an audible alarm

  Breadboard and jumper wires         Circuit assembly and connections

  USB cable / suitable power supply   Programming and powering the
                                      prototype
  -----------------------------------------------------------------------

## Wiring and pin connections

The following table reflects the pin assignments in the supplied Arduino
sketch and project report.

  Component pin           Arduino Mega 2560 Notes
  --------------------- ------------------- -----------------------------------
  LCD VCC                                5V Power
  LCD GND                               GND Common ground
  LCD SDA                          20 (SDA) I2C data
  LCD SCL                          21 (SCL) I2C clock
  PIR VCC                                5V Power
  PIR GND                               GND Common ground
  PIR OUT                                 7 Must match `IR_PIN` in the sketch
  EM-18 VCC                              5V Power
  EM-18 GND                             GND Common ground
  EM-18 TX                         19 (RX1) RFID serial data input
  Buzzer positive (+)                    11 Controlled by `BUZZER_PIN`
  Buzzer negative (−)                   GND Common ground

**Important:** The Arduino Mega uses pin 19 as `RX1` for `Serial1`.
Connect the EM-18 reader's TX output to this pin. Ensure all modules
share a common ground.

The sketch currently defines:

``` cpp
#define IR_PIN 7
#define BUZZER_PIN 11
```

The PIR output must therefore be connected to digital pin 7 for this
sketch.

## Software requirements

-   [Arduino IDE](https://www.arduino.cc/en/software)
-   Arduino Mega 2560 board support in the Arduino IDE
-   `Wire.h` (included with the Arduino platform)
-   `LiquidCrystal_I2C.h` library
-   Serial Monitor (included with the Arduino IDE)

The Serial Monitor is configured for **9600 baud**.

## Installation and setup

1.  Clone or download this repository:

    ``` bash
    git clone <YOUR_GITHUB_REPOSITORY_URL>
    cd <YOUR_REPOSITORY_FOLDER>
    ```

2.  Open `sketch_securitysys.ino` in the Arduino IDE.

3.  Install the **LiquidCrystal_I2C** library using the Arduino IDE
    Library Manager if it is not already installed.

4.  Connect the Arduino Mega 2560 to your computer.

5.  In the Arduino IDE, select:

    -   **Board:** Arduino Mega or Mega 2560
    -   **Processor:** ATmega2560
    -   **Port:** The port corresponding to your connected board

6.  Check the wiring against the [pin connection
    table](#wiring-and-pin-connections).

7.  Update the authorised RFID ID and welcome name as described in
    [Configuration](#configuration).

8.  Upload the sketch.

9.  Open the Serial Monitor and set its baud rate to **9600**.

10. Observe the LCD and Serial Monitor while testing motion detection
    and RFID scans.

## Configuration

### Set the authorised RFID card ID

In `sketch_securitysys.ino`, find:

``` cpp
String knownRFID = "REPLACE_WITH_YOUR_12_CHARACTER_CARD_ID";
```

Replace the placeholder with the 12-character ID reported by your EM-18
reader. The comparison in the current sketch uses the first 12 received
characters.

**Do not publish a real access-card ID in a public repository.** Use a
placeholder in the version you upload to GitHub, and enter your actual
card ID only in your local copy.

### Set the welcome name

In `grantAccess()`, change the welcome text to the name you want
displayed:

``` cpp
lcd.print("Welcome User");
```

If you change the text, keep the LCD's 16-character row limit in mind.

### Change the motion and alarm timing

The current verification timeout is 10 seconds:

``` cpp
if (millis() - detectionTime >= 10000)
```

The area-clear reset condition currently checks for more than 15 seconds
since `detectionTime`:

``` cpp
if (millis() - detectionTime > 15000)
```

These are millisecond values. Modify them only if you also want to
change the documented system behaviour.

## Testing the system

Test the prototype in a controlled environment.

  -----------------------------------------------------------------------
  Test                                Expected result
  ----------------------------------- -----------------------------------
  Power on / reset                    LCD displays standby messages and
                                      buzzer remains off

  Trigger the PIR input               LCD displays motion detected and
                                      asks for an RFID scan

  Present the configured authorised   Access is granted and buzzer is off
  card during tracking                

  Allow the 10-second verification    Buzzer activates and intruder alert
  period to expire                    is displayed

  Present the authorised card while   Access is granted and the buzzer is
  the alarm is active                 deactivated

  Present an unrecognised card        Access-denied message is displayed;
                                      the current state is then restored

  Observe the Serial Monitor          Status, sensor, RFID, alarm, and
                                      access messages are printed
  -----------------------------------------------------------------------

The project report records a Serial Monitor test showing the timeout,
buzzer activation, RFID recognition, successful access, and return to
standby.

## Current implementation and limitations

This repository represents an **educational prototype**, not a
production-ready access-control or life-safety system.

-   **PIR logic:** The supplied sketch treats a `LOW` reading on
    `IR_PIN` as the motion-detection condition. Confirm that this
    matches the behaviour and output configuration of the PIR module
    being used.
-   **RFID authentication:** The sketch compares a received 12-character
    ID with one ID stored in the source code. This is a basic
    demonstration, not secure credential management.
-   **Fingerprint authentication:** Fingerprint verification is not
    implemented in the supplied `.ino` sketch. Although it is mentioned
    in parts of the project documentation/simulation, there is no
    fingerprint sensor library or verification routine in this code.
-   **Wokwi simulation differences:** The project report notes that the
    Wokwi diagram connects PIR OUT to pin 3, while the sketch uses
    pin 7. The diagram's RFID pushbutton on pin 22 also does not
    reproduce the EM-18 serial data expected on `Serial1` / pin 19.
    Reconcile the simulation with the actual sketch before relying on
    it.
-   **LED indicators:** LEDs shown in the circuit diagram are not
    configured or controlled by the supplied sketch.
-   **Reset timing:** The area-clear condition measures elapsed time
    from the original `detectionTime`; it does not measure 15 continuous
    seconds of no motion.
-   **Blocking delays:** The sketch uses `delay(2000)` after an invalid
    card and `delay(5000)` after successful access. During these delays,
    the main loop does not process other events.
-   **Deployment safety:** A real installation would need stronger
    authentication, tamper resistance, appropriate power backup,
    fail-safe design, and professional security review. Do not use this
    prototype as the sole means of securing a hostel or other occupied
    building.

## Possible future improvements

-   Add fingerprint authentication using a compatible fingerprint sensor
    and implement the complete verification flow.
-   Replace blocking delays with non-blocking `millis()`-based state
    handling.
-   Improve RFID frame validation and support multiple authorised cards.
-   Add event logging and an administrator-managed credential list.
-   Add LED status indicators and implement them in the sketch.
-   Align the Wokwi simulation, circuit diagram, wiring, and source
    code.
-   Add tamper detection, backup power, and suitable fail-safe behaviour
    before any real-world deployment.

## Repository structure

A simple repository layout could be:

``` text
hostel-security-system/
├── README.md
├── sketch_securitysys.ino
├── Hostel_Security_System_Project_Report.docx
└── docs/
    └── circuit-diagram.png
```

Add the report and circuit image only if you intend to publish them.
Update this structure if your actual filenames or folders differ.

## References

-   [Arduino Mega 2560 Rev3 -- Official
    documentation](https://docs.arduino.cc/hardware/mega-2560/)
-   [Arduino Serial -- Language
    Reference](https://docs.arduino.cc/language-reference/en/functions/communication/serial/)
-   [Arduino `millis()` -- Language
    Reference](https://docs.arduino.cc/language-reference/en/functions/time/millis/)
-   [Wokwi -- Hostel Security System circuit
    simulation](https://wokwi.com/projects/479311477214620673)
-   `LiquidCrystal_I2C` library documentation.
-   EM-18 RFID reader module documentation / datasheet.
-   PIR motion sensor module documentation.

------------------------------------------------------------------------

## License

No license has been specified. If you want others to reuse, modify, or
distribute this project, add a license file (for example, an MIT
License) after choosing the terms you want.
