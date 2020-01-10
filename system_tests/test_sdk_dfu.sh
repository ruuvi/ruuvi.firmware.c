#!/bin/bash
cd "$(dirname "$0")"
source .test_env
SDK_UPDATE="ruuvitag_b_armgcc_ruuvifw_test_v3.28.13_sdk12.3_to_15.3_dfu.zip"
APP_UPDATE="ruuvitag_b_armgcc_ruuvifw_test_v3.28.13_dfu_app.zip"
RESULT_FILE="test_sdk_update_result.txt" 
TEST_FILE="test_sdk_update.xml"

fetch_result () {
  adb pull "/sdcard/${FIRMWARE_PATH}/${RESULT_FILE}" ${RESULT_FILE} #> /dev/null
}

if [ -f $SDK_UPDATE ]; then
   echo "Found sdk update package." > /dev/null
else
   wget https://github.com/ruuvi/ruuvi.firmware.c/releases/download/v3.28.13/ruuvitag_b_armgcc_ruuvifw_test_v3.28.13_sdk12.3_to_15.3_dfu.zip
fi

if [ -f $APP_UPDATE ]; then
   echo "Found app update fw." > /dev/null
else
   wget https://github.com/ruuvi/ruuvi.firmware.c/releases/download/v3.28.13/ruuvitag_b_armgcc_ruuvifw_test_v3.28.13_dfu_app.zip
fi

adb push ${SDK_UPDATE} "/sdcard/${FIRMWARE_PATH}/${SDK_UPDATE}"
adb push ${APP_UPDATE} "/sdcard/${FIRMWARE_PATH}/${APP_UPDATE}"
adb push ${TEST_FILE} "/sdcard/${FIRMWARE_PATH}/${TEST_FILE}" #> /dev/null
adb shell rm "/sdcard/${FIRMWARE_PATH}/${RESULT_FILE}" #> /dev/null
rm ${RESULT_FILE}
adb shell am start-foreground-service -a no.nordicsemi.android.action.START_TEST \
    -e FIRMWARE_PATH "/${FIRMWARE_PATH}" -e SDK_UPDATE ${SDK_UPDATE} \
    -e APP_UPDATE ${APP_UPDATE} \
    -e RUUVITAG_B_DEVKIT_APPNAME ${RUUVITAG_B_DEVKIT_APPNAME} \
    -e RUUVITAG_B_DEVKIT_BLNAME ${RUUVITAG_B_DEVKIT_BLNAME} \
    -e no.nordicsemi.android.test.extra.EXTRA_FILE_PATH "/sdcard/${FIRMWARE_PATH}/${TEST_FILE}" #> /dev/null
RESULT=${RESULT}+$? #> /dev/null

while [ ! -f ${RESULT_FILE} ]
do
  fetch_result
  sleep 10s
done