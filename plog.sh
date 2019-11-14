#!/bin/bash
rm -rf issues/*
plog-converter -a 'GA:1,2;64:1;OP:1,2,3;CS:1;MISRA:1,2' -d V677,Renew -t fullhtml -o issues _build/nrf52832_xxaa/*.PVS-Studio.log
plog-converter -a 'GA:1,2;64:1;OP:1,2,3;CS:1;MISRA:1,2' -d V677,Renew -t tasklist -o issues/PVS-Studio.tasks _build/nrf52832_xxaa/*.PVS-Studio.log
plog-converter -a 'GA:1,2;64:1;OP:1,2,3;CS:1;MISRA:1,2' -d V677,Renew -t xml -o      issues/pvs.xml _build/nrf52832_xxaa/*.PVS-Studio.log
