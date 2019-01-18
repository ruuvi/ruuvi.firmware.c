#!/bin/bash
NAME="dev"
VERSION="0.0.1"
while getopts "n:v:" option;
do
case "${option}"
in
n) NAME=${OPTARG};;
v) VERSION=${OPTARG};;
esac
done

bootloader="ruuvitag_b_bootloader_3.0.1_s132_6.1.0_debug.hex"     
if [ -f $bootloader ]; then
   echo "Found bootloader."
else
   wget https://github.com/ruuvi/ruuvi.nrf5_sdk15_bootloader.c/releases/download/3.0.0/ruuvitag_b_bootloader_3.0.1_s132_6.1.0_debug.hex
fi

key="ruuvi_open_private.pem"     
if [ -f $key ]; then
   echo "Found key"
else
   wget https://github.com/ruuvi/ruuvi.nrf5_sdk15_bootloader.c/releases/download/3.0.0/ruuvi_open_private.pem
fi

nrfutil settings generate --family NRF52 --application Output/Debug/Exe/ruuvi.firmware.c.hex --application-version 1 --application-version-string "$VERSION" --bootloader-version 1 --bl-settings-version 1 settings.hex
mergehex -m ../../../../nRF5_SDK_15.2.0_9412b96/components/softdevice/s132/hex/s132_nrf52_6.1.0_softdevice.hex ruuvitag_b_bootloader_3.0.0_s132_6.1.0_debug.hex settings.hex -o sbc.hex
mergehex -m sbc.hex Output/Debug/Exe/ruuvi.firmware.c.hex -o packet.hex
nrfjprog --family nrf52 --eraseall
nrfjprog --family nrf52 --program packet.hex
nrfjprog --family nrf52 --reset

mv packet.hex ruuvitag_b\_ses\_$NAME\_$VERSION\_full.hex
cp Output/Debug/Exe/ruuvi.firmware.c.hex ruuvitag_b\_ses\_$NAME\_$VERSION\_app.hex
nrfutil pkg generate --application Output/Debug/Exe/ruuvi.firmware.c.hex --application-version 1 --application-version-string "$VERSION" --hw-version 0x0b --sd-req 0xAF --key-file ruuvi_open_private.pem ruuvitag_b\_ses\_$NAME\_$VERSION\_dfu.zip