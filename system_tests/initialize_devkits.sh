#!/bin/bash
cd "$(dirname "$0")"
source .test_env
FACTORYFW="ruuvi_firmware_full_2.5.9.hex"  
if [ -f $FACTORYFW ]; then
   echo "Found factory fw." > /dev/null
else
   wget https://github.com/ruuvi/ruuvitag_fw/releases/download/v2.5.9/ruuvi_firmware_full_2.5.9.hex
fi

nrfjprog -s ${RUUVITAG_B_DEVKIT_SN} -f NRF52 --eraseall > /dev/null
RESULT=$? > /dev/null
nrfjprog -s ${RUUVITAG_B_DEVKIT_SN} -f NRF52 --program s132_nrf52_3.1.0_softdevice.hex > /dev/null
RESULT=${RESULT}+$? > /dev/null
nrfjprog -s ${RUUVITAG_B_DEVKIT_SN} -f NRF52 --program ruuvitag_b_bootloader.hex > /dev/null
RESULT=${RESULT}+$? > /dev/null
nrfjprog -s ${RUUVITAG_B_DEVKIT_SN} -f NRF52 --reset > /dev/null
RESULT=${RESULT}+$? > /dev/null
echo ${RESULT}