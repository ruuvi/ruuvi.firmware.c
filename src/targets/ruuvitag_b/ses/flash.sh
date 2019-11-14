#!/bin/bash
cd "$(dirname "$0")"
MODE="Debug"
NAME="dev"
VERSION="0.0.1"

while getopts "m:n:v:" option;
do
case "${option}"
in
m) MODE=${OPTARG};;
n) NAME=${OPTARG};;
v) VERSION=${OPTARG};;
esac
done

if [ -z "${MODE+x}" ]; 
  then echo "MODE is unset"; 
  else echo "MODE is set to '$MODE'"; 
fi

if [ -z "${NAME+x}" ]; 
  then echo "NAME is unset"; 
  else echo "NAME is set to '$NAME'"; 
fi

if [ -z "${VERSION+x}" ]; 
  then echo "VERSION is unset"; 
  else echo "VERSION is set to '$VERSION'"; 
fi

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

nrfutil settings generate --family NRF52 --application ../../../Output/${MODE}/Exe/ruuvitag_b.hex --application-version 1 --application-version-string "$VERSION" --bootloader-version 1 --bl-settings-version 1 settings.hex
mergehex -m ../../../../nRF5_SDK_15.3.0_59ac345/components/softdevice/s132/hex/s132_nrf52_6.1.1_softdevice.hex ruuvitag_b_bootloader_3.1.0_s132_6.1.1.hex settings.hex -o sbc.hex
mergehex -m sbc.hex ../../../Output/${MODE}/Exe/ruuvitag_b.hex -o packet.hex
nrfjprog --family nrf52 --eraseall
nrfjprog --family nrf52 --program packet.hex
nrfjprog --family nrf52 --reset

mv packet.hex ruuvitag_b\_ses\_$NAME\_$VERSION\_${MODE}\_full.hex
cp ../../../Output/${MODE}/Exe/ruuvitag_b.hex ruuvitag_b\_ses\_$NAME\_$VERSION\_${MODE}\_app.hex
nrfutil pkg generate --application ../../../Output/${MODE}/Exe/ruuvitag_b.hex --application-version 1 --application-version-string "$VERSION" --hw-version 0xB0 --sd-req 0xB7 --key-file ruuvi_open_private.pem ruuvitag_b\_ses\_$NAME\_$VERSION\_${MODE}\_dfu.zip
