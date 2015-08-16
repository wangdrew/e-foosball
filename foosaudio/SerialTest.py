__author__ = 'andrewwang'

import serial

ARDUINO_SERIAL_ADDR = "/dev/ttyUSB0"

def main():
    arduino = serial.Serial(ARDUINO_SERIAL_ADDR,
                            baudrate=9600,
                            bytesize=serial.EIGHTBITS,
                            parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE)

    while True:
        ascii_val = arduino.readline()
        print(ascii_val)


if "__name__" == "__main__":
    main()