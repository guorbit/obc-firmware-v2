#!/usr/bin/env bash

# Stop echo from echoing back and creating an endless loop
stty -F /dev/ttyACM0 raw -echo

echo -n ' ' > /dev/ttyACM0
echo -n ' ' > /dev/ttyACM0
echo -n 'f' > /dev/ttyACM0
echo -n 'y' > /dev/ttyACM0
cat /dev/ttyACM0 | tee obc_flash_dump.txt