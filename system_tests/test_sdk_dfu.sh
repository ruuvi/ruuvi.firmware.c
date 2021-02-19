#!/bin/bash
cd "$(dirname "$0")"
if [ ! `which adb` ]
then  echo " -- get  adb ( Android Debug Bridge )  https://developer.android.com/studio/releases/platform-tools          and try again " 
exit 9
fi
source .test_env
# Tag on this commit
TAG=$(git describe --tags --exact-match)
echo "Testing ${TAG}"
RELEASE_URL="https://github.com/ruuvi/ruuvi.firmware.c/releases/download"
RESULT_FILE="test_sdk_update_result.txt" 
TEST_FILE="test_sdk_update.xml"
BOARD="ruuvitag_b"
FW_NAME="armgcc_ruuvifw"
TEST_VARIANT="${BOARD}_${FW_NAME}_test_${TAG}_full.hex"
APP_VARIANT="${BOARD}_${FW_NAME}_default_${TAG}_dfu_app.zip"
SDK_VARIANT="${BOARD}_${FW_NAME}_default_${TAG}_sdk12.3_to_15.3_dfu.zip"

rm *dfu_app.zip*
rm *dfu.zip*
rm *full.hex*

wget ${RELEASE_URL}/${TAG}/${TEST_VARIANT}
wget ${RELEASE_URL}/${TAG}/${APP_VARIANT}
wget ${RELEASE_URL}/${TAG}/${SDK_VARIANT}

fetch_result () {
  adb pull "/sdcard/${FIRMWARE_PATH}/${RESULT_FILE}" ${RESULT_FILE} #> /dev/null
}

adb push ${SDK_VARIANT} "/sdcard/${FIRMWARE_PATH}/${SDK_VARIANT}"
adb push ${APP_VARIANT} "/sdcard/${FIRMWARE_PATH}/${APP_VARIANT}"
adb push ${TEST_FILE} "/sdcard/${FIRMWARE_PATH}/${TEST_FILE}" #> /dev/null
adb shell rm "/sdcard/${FIRMWARE_PATH}/${RESULT_FILE}" #> /dev/null
rm ${RESULT_FILE}
adb shell am start-foreground-service -a no.nordicsemi.android.action.START_TEST \
    -e FIRMWARE_PATH "/${FIRMWARE_PATH}" -e SDK_UPDATE ${SDK_VARIANT} \
    -e APP_UPDATE ${APP_VARIANT} \
    -e RUUVITAG_B_DEVKIT_APPNAME ${RUUVITAG_B_DEVKIT_APPNAME} \
    -e RUUVITAG_B_DEVKIT_BLNAME ${RUUVITAG_B_DEVKIT_BLNAME} \
    -e no.nordicsemi.android.test.extra.EXTRA_FILE_PATH "/sdcard/${FIRMWARE_PATH}/${TEST_FILE}" #> /dev/null
RESULT=${RESULT}+$? #> /dev/null

while [ ! -f ${RESULT_FILE} ]
do
  fetch_result
  sleep 10s
done
