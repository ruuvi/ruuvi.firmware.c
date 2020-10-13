#!/bin/bash
cd "$(dirname "$0")"
source .test_env

nrfjprog -s ${RUUVITAG_B_DEVKIT_SN} -f NRF52 --eraseall > /dev/null
nrfjprog -s ${RUUVITAG_B_DEVKIT_SN} -f NRF52 --program s132_nrf52_3.1.0_softdevice.hex > /dev/null
nrfjprog -s ${RUUVITAG_B_DEVKIT_SN} -f NRF52 --program ruuvitag_b_bootloader.hex > /dev/null
nrfjprog -s ${RUUVITAG_B_DEVKIT_SN} -f NRF52 --reset > /dev/null

nrfjprog -s ${RUUVITAG_B_PPK_SN} -f NRF52 --eraseall > /dev/null
nrfjprog -s ${RUUVITAG_B_PPK_SN} -f NRF52 --program nus_throughput_sdapp.hex > /dev/null
nrfjprog -s ${RUUVITAG_B_PPK_SN} -f NRF52 --reset > /dev/null