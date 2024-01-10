#!/bin/bash

# Check if avrdude is installed
if ! command -v avrdude &> /dev/null; then
    echo "Error: avrdude not found. Please install avrdude."
    exit 1
fi

# Check if the correct number of arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <device> <hex_path>"
    exit 1
fi

device="$1"
hex_path="$2"

# Прошивка через avrdude
avrdude -v -p atmega328p -c arduino -P "$device" -b 57600 -D -U flash:w:"$hex_path":i


