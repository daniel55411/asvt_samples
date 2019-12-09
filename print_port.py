#!/usr/bin/env python3
import sys, serial
port = serial.Serial(sys.argv[1])
print(port)
port.close()
