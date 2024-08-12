# The code to be ran on the rasberry pi
# Encodes a message given in the command line, repeating it a set number of times
# usage python <num> <message>

import RPi.GPIO as GPIO
import time
import sys

# Our encoding of morse code
morse_code = {
    'A': '.-', 'B': '-...', 'C': '-.-.', 'D': '-..', 'E': '.', 'F':'..-.', 'G':'--.', 'H':'....',
    'I': '..', 'J': '.---', 'K': '-.-', 'L': '.-..', 'M': '--', 'N': '-.', 'O': '---', 'P': '.--.',
    'Q': '--.-', 'R': '.-.', 'S': '...', 'T': '-', 'U': '..-', 'V': '..-', 'W': '.--', 'X': '-..-',
    'Y': '-.--', 'Z': '--..', '0': '-----', '1': '.----', '2': '..---', '3': '...--', '4': '....-',
    '5': '.....', '6': '-....', '7': '--...', '8': '---..', '9': '----.', ' ': '.-.-'
    }

end = '--.--'

# Times for space, long pulse, shot 
space = 1.5
long = .9
short = .3
delay = .5

#Set the gpio
GPIO.setmode(GPIO.BCM)
led_pin = 23 # where the light bulb is on the 

GPIO.setup(led_pin, GPIO.OUT)

# Turns on the light bulb for t seconds
def send_sig(t):    
    GPIO.output(led_pin, GPIO.HIGH)
    time.sleep(t)
    GPIO.output(led_pin, GPIO.LOW)

# Sends a particular character's more code encoding
def send(code):
    for symbol in code:
        if symbol == '.':
            print("Short")
            send_sig(short)
        elif symbol == "-":
            print("LONG")
            send_sig(long)
        time.sleep(delay)
    send_sig(space)
    print("SPACE")
    time.sleep(delay)

# Sends the full message 
def send_morse_code(message):
    for char in message:
        if char.upper() in morse_code:
            send(morse_code[char.upper()])
        else:
            print(f"Ignoring: {char}")
    send(end)

# MAIN, check we get the right args
if len(sys.argv) != 3:
    print("Usage: python3 morse.py <num> <msg")
    sys.exit(1)

# Sends the message num times
rep = int(sys.argv[1])
msg = sys.argv[2]

for _ in range(rep):
    send_morse_code(msg)
    time.sleep(space)
GPIO.cleanup()



