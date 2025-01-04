#include <AccelStepper.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#define DRIVER 1 // stepper driver with Step and Direction pins
#define enablePinTop 6
#define enablePinBottom 7
#define servoLockPin 8
#define rs 9
#define en 13
#define d4 11
#define d5 12
#define d6 10
#define d7 14

Servo myServo;
AccelStepper bottomDrawer(DRIVER, 2, 3); // Stepper for Drawer 1 (Step: Pin 2, Direction: Pin 3)
AccelStepper topDrawer(DRIVER, 4, 5); // Stepper for Drawer 2 (Step: Pin 4, Direction: Pin 5)
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
bool bottomLocked = true;
bool bottomClosed = true;
bool topClosed = true;

void setup() {
  Serial.begin(9600);

  bottomDrawer.setMaxSpeed(1000);  // Max speed in steps per second
  bottomDrawer.setAcceleration(500); // Acceleration in steps/second^2
  topDrawer.setMaxSpeed(1000);
  topDrawer.setAcceleration(500);

  bottomDrawer.disableOutputs();
  topDrawer.disableOutputs();

  pinMode(enablePinTop, OUTPUT);
  pinMode(enablePinBottom, OUTPUT);
  digitalWrite(enablePinTop, HIGH); // Disables motor driver to save energy when not in use
  digitalWrite(enablePinBottom, HIGH);

  myServo.attach(servoLockPin);
  myServo.write(0);

  lcd.begin(16,2);
}

void loop() {
  // Continually check for serial input from Raspberry Pi
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n'); // Read command
    command.trim();
    processCommand(command);
  }
}


/* Finds the last space before end of LCD character display limit 
 * to allow for logical splitting of text between lines.
 */
int findLastSpace(String command, int maxLength) {
  int lastSpace = command.lastIndexOf(' ', maxLength - 1);
  if (lastSpace >= 0) {
    return lastSpace;
  }
  return maxLength;
}


/* Temporarily prints message to LCD module, splitting text 
 * logically to maintain readability. 
 */
void printLCDMessage(String command) {
  lcd.clear();

  lcd.setCursor(0, 0);
  int firstEnd = findLastSpace(command, 16);
  lcd.print(command.substring(0, firstEnd));

  if (command.length() > firstEnd) {
    String remaining = command.substring(firstEnd + 1);
    lcd.setCursor(0, 1);
    lcd.print(remaining.substring(0, 16));
  }
  delay(1200);
  lcd.clear();
}


/* Opens drawer specified by inputted stepped motor. 
 * Disables output after opening.
 */
void openDrawer(AccelStepper &stepper) {
  stepper.moveTo(450);
  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }
  stepper.disableOutputs();
}


/* Closes drawer specified by inputted stepped motor. 
 * Disables output after closing.
 */
void closeDrawer(AccelStepper &stepper) {
  stepper.moveTo(20);
  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }
  stepper.disableOutputs();
}


/* Processes and handles commands received from Raspberry Pi through serial
 * communication. Identifies command received and executes necessary actions
 * corresponding to command type. Ensures logical constraints on locking
 * and unlocking the bottom drawer based on current state. Prints essential 
 * commands on LCD and sends response back to the Pi. 
 *
 * Note: to save energy and prevent overheating, this function temporarily 
 * enables motor driver power (sets enablePin LOW) when necessary for drawer 
 * movement. Otherwise, disables driver power (sets enablePin HIGH).
 */
void processCommand(String command) {
  String message = command;

  // Commands for inital awakening and speech recognition
  if (command == "Fred Awakened!" || command == "Listening..." || 
      command == "Recognizing..." || command == "Unable to understand" || 
      command == "Unable to request results") {
    Serial.println(message);
    printLCDMessage(message);
    return;
  }
  
  // Commands for drawer actions
  if (command == "unlock_bottom") {
    myServo.write(90);
    bottomLocked = false;
    Serial.println("Bottom Drawer Unlocked!");
  } else if (command == "lock_bottom") {
    if (bottomClosed) {
      myServo.write(0);
      bottomLocked = true;
      Serial.println("Bottom Drawer Locked!");
    } else if (!bottomClosed) {  // Prevents locking without closed bottom drawer
      message = "Close Bottom to Lock!";
      Serial.println(message);
      printLCDMessage(message);
    }
  } else if (command == "open_bottom_drawer") {
    if (bottomLocked) {  // Prevents opening bottom drawer when locked
      message = "Bottom Locked. Access Denied.";
      Serial.println(message);
      printLCDMessage(message);
    } else if (!bottomLocked) {
      digitalWrite(enablePinBottom, LOW);
      openDrawer(bottomDrawer); 
      bottomClosed = false;
      digitalWrite(enablePinBottom, HIGH);
      Serial.println("Bottom Drawer Opened!");
    }
  } else if (command == "open_top_drawer") {
    digitalWrite(enablePinTop, LOW);
    openDrawer(topDrawer); 
    topClosed = false;
    digitalWrite(enablePinTop, HIGH);
    Serial.println("Top Drawer Opened!");
  } else if (command == "close_bottom_drawer") {
    digitalWrite(enablePinBottom, LOW);
    closeDrawer(bottomDrawer); 
    bottomClosed = true;
    digitalWrite(enablePinBottom, HIGH);
    Serial.println("Bottom Drawer Closed!");
  } else if (command == "close_top_drawer") {
    digitalWrite(enablePinTop, LOW);
    closeDrawer(topDrawer);
    topClosed = true;
    digitalWrite(enablePinTop, HIGH);
    Serial.println("Top Drawer Closed!");
  } else if (command == "open_any_drawer") {
      digitalWrite(enablePinTop, LOW);
      openDrawer(topDrawer); 
      topClosed = false;
      digitalWrite(enablePinTop, HIGH);
      if (bottomLocked) {
        bottomClosed = true;
      } else {
        digitalWrite(enablePinBottom, LOW);
        openDrawer(bottomDrawer); 
        bottomClosed = false;
        digitalWrite(enablePinBottom, HIGH);
      }
      if (!topClosed && !bottomClosed) {
        Serial.println("Both Drawers Opened!");
      } else {
        message = "Top Opened! Bottom Locked.";
        Serial.println(message);
        printLCDMessage(message);
      }
  } else if (command == "close_any_drawer") {
      digitalWrite(enablePinTop, LOW);
      closeDrawer(topDrawer); 
      topClosed = true;
      digitalWrite(enablePinTop, HIGH);
      digitalWrite(enablePinBottom, LOW);
      closeDrawer(bottomDrawer);
      bottomClosed = true;
      digitalWrite(enablePinBottom, HIGH);
      message = "Both Drawers Closed!";
      Serial.println(message);
  } else {
    message = "Unknown Command.";
    Serial.println(message);
  }
}