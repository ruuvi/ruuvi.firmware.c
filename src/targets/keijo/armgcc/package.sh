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

BINNAME=keijo\_armgcc\_${NAME}\_${VERSION}

rm keijo_armgcc*${NAME}*

mergehex -m ../../../../nRF5_SDK_15.3.0_59ac345/components/softdevice/s140/hex/s140_nrf52_6.1.1_softdevice.hex _build/nrf52811_xxaa.hex -o packet.hex

mv packet.hex ${BINNAME}\_full.hex
mv _build/nrf52811_xxaa.map ${BINNAME}\_app.map
mv _build/nrf52811_xxaa.hex ${BINNAME}\_app.hex
