#!/bin/bash
./build-wrapper-macosx-x86 --out-dir bw-output make clean analysis
source sonar_password.sh
sonar-scanner \
  -Dsonar.projectKey=ojousima_ruuvi.firmware.c \
  -Dsonar.organization=ojousima \
  -Dsonar.sources=. \
  -Dsonar.cfamily.build-wrapper-output=bw-output \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.login=$SONAR_PASSWORD