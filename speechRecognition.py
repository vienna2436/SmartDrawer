from dotenv import load_dotenv
import os
import pvporcupine
import speech_recognition as sr
import sounddevice as sd
import serial
import struct
import time

# Load environment variables from .env and assign to variables
load_dotenv()
ACCESS_KEY = os.getenv("ACCESS_KEY")
KEYWORD_PATH = os.getenv("KEYWORD_PATH")

# Initialize speech recognizer
rec = sr.Recognizer()
rec.energy_threshold = 300
rec.dynamic_energy_threshold = False

# Initialize serial connection
arduino = serial.Serial('/dev/ttyACM0', baudrate=9600, timeout=10)

# Create instance of porcupine (wake-word detection model)
# with keyword "Hey Fred"
handle = pvporcupine.create(
    access_key=ACCESS_KEY,
    keyword_paths=[KEYWORD_PATH]
)

# Initialize audio input stream
stream = sd.InputStream(
    samplerate=handle.sample_rate,
    blocksize=handle.frame_length,
    channels=1,
    dtype='int16'
)

# Recognizes speech from microphone input and prints 
# speech captured.
def identify_speech():
    with sr.Microphone() as source:
        # Adjust for ambient noise
        rec.adjust_for_ambient_noise(source, duration=1)
        command_arduino("Listening...")
        audio = rec.listen(source, timeout=15, phrase_time_limit=5)
        try:
            command_arduino("Recognizing...")
            text = rec.recognize_google(audio, language='en-US')
            print("You said: ", text)
            return text
        except sr.UnknownValueError:
            command_arduino("Unable to understand audio")
            return None
        except sr.RequestError as e:
            command_arduino("Unable to request results; {0}".format(e))
            return None


# Sends command to Arduino and returns response
def command_arduino(command):
    arduino.write(command.encode())
    for i in range(10):
        time.sleep(1)
        if arduino.in_waiting > 0:
            return arduino.readline().decode()
    return "No response recieved in time from Arduino"


# Processes voice commands and sends corresponding command
# to Arduino. Allows for open/close of the top/bottom drawer,
# and lock/unlock of the bottom drawer.
def process_voice_command():
    while True:
        voiceCommand = identify_speech()

        if voiceCommand is None:
            return

        if "open" in voiceCommand:
            if "top" in voiceCommand or "one" in voiceCommand or "upper" in voiceCommand:
                print("Sending command to open top drawer")
                arduinoResponse = command_arduino("open_top_drawer")
                print("Arduino responded: ", arduinoResponse)
            elif "bottom" in voiceCommand or "two" in voiceCommand or "lower" in voiceCommand: 
                print("Sending command to open bottom drawer")
                arduinoResponse = command_arduino("open_bottom_drawer")
                print("Arduino responded: ", arduinoResponse)
            else:
                print("Opening any closed drawer")
                arduinoResponse = command_arduino("open_any_drawer")
                print("Arduino responded: ", arduinoResponse)
        elif "close" in voiceCommand:
            if "top" in voiceCommand or "one" in voiceCommand or "upper" in voiceCommand:
                print("Sending command to close top drawer")
                arduinoResponse = command_arduino("close_top_drawer")
                print("Arduino responded:  ", arduinoResponse)
            elif "bottom" in voiceCommand or "two" in voiceCommand or "lower" in voiceCommand:
                print("Sending command to close bottom drawer")
                arduinoResponse1 = command_arduino("close_bottom_drawer")
                arduinoResponse2 = command_arduino("lock_bottom")
                print("Arduino responded: ", arduinoResponse1 + " and " + arduinoResponse2)
            else:
                print("Closing any open drawer")
                arduinoResponse1 = command_arduino("close_any_drawer")
                arduinoResponse2 = command_arduino("lock_bottom")
                print("Arduino responded: ", arduinoResponse1 + " and " + arduinoResponse2)
        elif "lock" in voiceCommand:
            print("Sending command to lock drawer")
            arduinoResponse = command_arduino("lock_bottom")
            print("Arduino responded: ", arduinoResponse)
        else: 
            print("Invalid command. Please try again.")


# Continually listens for wake word and notifies Arduino if 
# detected (to print awakened message on LCD). Then calls 
# function to process voice commands heard after awakening.
def main():
    try:
        with stream:
            while True:
                audio, status = stream.read(handle.frame_length)  # Contains raw audio data captured
                audio = struct.unpack_from("h" * handle.frame_length, audio)  # Unpacks raw data
                wake_word_index = handle.process(audio)
                if wake_word_index >= 0:
                    command_arduino("Fred Awakened!")
                    process_voice_command()
                
    except Exception as e:
        print("Error: ", e)        

    finally:
        if stream:
            stream.close()
        if handle:
            handle.delete()
        print("Resources used cleaned up")


if __name__ == "__main__":
    main()