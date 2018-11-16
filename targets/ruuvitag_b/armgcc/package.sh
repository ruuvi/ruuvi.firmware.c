#!/bin/bash
cd "$(dirname "$0")"
NAME="ruuvifw"
VERSION=$(git rev-parse --short HEAD)
while getopts "n:v:" option;
do
case "${option}"
in
n) NAME=${OPTARG};;
v) VERSION=${OPTARG};;
esac
done

BOOTLOADER="ruuvitag_b_bootloader_3.0.1_s132_6.1.0_debug.hex"     
if [ -f $BOOTLOADER ]; then
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

nrfutil settings generate --family NRF52 --application _build/nrf52832_xxaa.hex --application-version 1  --bootloader-version 1 --bl-settings-version 1 settings.hex 
mergehex -m ../../../../nRF5_SDK_15.2.0_9412b96/components/softdevice/s132/hex/s132_nrf52_6.1.0_softdevice.hex $BOOTLOADER settings.hex -o sbc.hex
mergehex -m sbc.hex _build/nrf52832_xxaa.hex -o packet.hex

rm ruuvitag_b_armgcc*$NAME*.hex
rm ruuvitag_b_armgcc*$NAME*.zip

mv packet.hex ruuvitag_b\_armgcc\_$NAME\_$VERSION\_full.hex
cp _build/nrf52832_xxaa.hex ruuvitag_b\_armgcc\_$NAME\_$VERSION\_app.hex
nrfutil pkg generate --application _build/nrf52832_xxaa.hex --application-version 1 --hw-version 0x0b --sd-req 0xAF --key-file ruuvi_open_private.pem ruuvitag_b\_armgcc\_$NAME\_$VERSION\_dfu.zip