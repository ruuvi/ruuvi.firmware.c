#!/bin/bash
cd "$(dirname "$0")"
NAME="ruuvifw"
VERSION=$(git describe --exact-match --tags HEAD)
if [ -z "$VERSION" ]; then
  VERSION=$(git rev-parse --short HEAD)
fi

while getopts "n:v:" option;
do
case "${option}"
in
n) NAME=${OPTARG};;
v) VERSION=${OPTARG};;
esac
done
BINNAME=kaarle\_armgcc\_${NAME}\_${VERSION}
BOOTLOADER="kaarle_s132_6.1.1_bootloader_3.1.0.hex"     
if [ -f $BOOTLOADER ]; then
   echo "Found bootloader."
else
   wget https://github.com/ruuvi/ruuvi.nrf5_sdk15_bootloader.c/releases/download/3.1.0-beta/kaarle_s132_6.1.1_bootloader_3.1.0.hex
fi

key="ruuvi_open_private.pem"     
if [ -f $key ]; then
   echo "Found key"
else
   wget https://github.com/ruuvi/ruuvi.nrf5_sdk15_bootloader.c/releases/download/3.0.0/ruuvi_open_private.pem
fi

rm kaarle_armgcc*${NAME}*

nrfutil settings generate --family NRF52 --application _build/nrf52832_xxaa.hex --application-version 1  --bootloader-version 1 --bl-settings-version 1 settings.hex 
mergehex -m ../../../../nRF5_SDK_15.3.0_59ac345/components/softdevice/s132/hex/s132_nrf52_6.1.1_softdevice.hex ${BOOTLOADER} settings.hex -o sbc.hex
mergehex -m sbc.hex _build/nrf52832_xxaa.hex -o packet.hex

nrfutil pkg generate --application _build/nrf52832_xxaa.hex --application-version 1 --hw-version 0xCA --sd-req 0xB7 --key-file ruuvi_open_private.pem ${BINNAME}\_dfu_app.zip
mv packet.hex ${BINNAME}\_full.hex
mv _build/nrf52832_xxaa.map ${BINNAME}\_app.map
mv _build/nrf52832_xxaa.hex ${BINNAME}\_app.hex
