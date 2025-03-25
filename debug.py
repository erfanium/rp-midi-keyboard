#!/usr/bin/python3

import string
import re
import serial
from dataclasses import dataclass


# Define Key class
@dataclass
class Key:
    a: bool = False
    b: bool = False


# Initialize keys
keys: list[Key] = [Key() for _ in range(37)]


# Function to convert int to Base64 character
def int_to_base64_char(n: int):
    base64_chars: str = (
        string.ascii_uppercase + string.ascii_lowercase + string.digits + "+/"
    )
    if 0 <= n < len(base64_chars):
        return base64_chars[n]
    else:
        raise ValueError("Number must be between 0 and 63")


# Create header
header = "".join([int_to_base64_char(i) for i in range(37)])


# Function to print state
def flush_state():
    print("\033[H\033[J", end="")  # Clear screen
    print(header)
    print("".join(["A" if key.a else " " for key in keys]))
    print("".join(["B" if key.b else " " for key in keys]))


# Open the serial port
try:
    ser = serial.Serial("/dev/ttyACM0", 9600, timeout=1)  # Adjust baud rate as needed
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    exit(1)

# Read from serial
while True:
    try:
        line = ser.readline().decode("utf-8").strip()  # Read line and decode
        match = re.match(r"(kbd_\w+): (\d+)", line)
        if match:
            verb, index_str = match.groups()
            index = int(index_str)
            if index < 0 or index > 36:
                print(f"out of range index: {index}")
                pass
            key = keys[index]
            match verb:
                case "kbd_start_on":
                    key.a = True
                case "kbd_end_on":
                    key.b = True
                case "kbd_start_off":
                    key.a = False
                case "kbd_end_off":
                    key.b = False
                case _:
                    pass
            flush_state()
    except KeyboardInterrupt:
        print("Exiting...")
        ser.close()
        break
    except Exception as e:
        print(f"Error: {e}")
        ser.close()
        break
