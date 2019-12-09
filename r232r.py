import binascii
import serial
import sys

def format_data(data):
    result = []

    for i in range(len(data)):
        result.append('{0:0>2x}'.format(ord(data[i])))

    return ' '.join(result)

if len(sys.argv) != 2:
    print('Uncorrent input format')

with serial.Serial(sys.argv[1]) as com_port:
    com_port.baudrate = 16000

    print(com_port)

    while True:
        data = com_port.read()
        print(data, end=' ')
