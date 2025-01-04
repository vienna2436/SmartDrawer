from dotenv import load_dotenv
import serial
import time
import os

# Initialize serial connection to Arduino
arduino = serial.Serial('/dev/ttyACM0', 9600, timeout = 2)

# Load environment variable from .env. Assign NFC tag ID and file path
load_dotenv()
nfc_tag = "Drawer NFC scanned"
file_path = os.getenv("NFC_FILE_PATH")


# Sends unlock command to Arduino and returns response
def arduino_unlock(command):
    arduino.write(command.encode())
    for i in range(5):
        time.sleep(1)
        if arduino.in_waiting > 0:
            return arduino.readline().decode()
    return "No response recieved in time from Arduino"


# Check if NFC tag scanned and unlock bottom drawer if tag matches
def main():
    try:
        while True:
            if os.path.exists(file_path):
                with open(file_path, 'r') as file:
                    if file.read().strip() == nfc_tag:
                        print("Drawer NFC scanned")
                        print("Sending command to unlock drawer")
                        arduinoResponse = arduino_unlock("unlock_bottom")
                        print("Arduino responded: ", arduinoResponse)
                        open(file_path, 'w').close()
    except KeyboardInterrupt:
        print("Keyboard Interrupt")    

    
if __name__ == '__main__':
    main()