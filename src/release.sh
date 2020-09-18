#!/bin/bash
GH_USER="ruuvi"
GH_REPO="ruuvi.firmware.c"
#$(GH_TOKEN) Set this in environment.

BOARDS=(ruuvitag_b kaarle kalervo keijo)
VARIANTS=(default longlife longmem test)
TAG=$(git describe --tags --exact-match)
if [ -z "$TAG" ]; then echo "Tag is not set, exit"; exit; fi

message="$(git for-each-ref refs/tags/$TAG --format='%(contents)')"

# Get the title and the description as separated variables
name=$(echo "$message" | head -n 1)
description=$(echo "$message" | tail -n -1)
# Create a release
release=$(curl -XPOST -H "Authorization:token ${GH_TOKEN}" --data "{\"tag_name\": \"${TAG}\", \"target_commitish\": \"master\", \"name\": \"$name\", \"body\": \"${description}\", \"draft\": false, \"prerelease\": true}" https://api.github.com/repos/${GH_USER}/${GH_REPO}/releases)
echo "${release}"

# Extract the id of the release from the creation response
id=$( echo "${release}" | grep -m 1 "id.:" | grep -Eo '[0-9]+')
[ "$id" ] || { echo "Error: Failed to get release id for tag: $tag"; echo "$release" | awk 'length($0)<100' >&2; exit 1; }
echo "${id}"

for BOARD in "${BOARDS[@]}"
do
  for VARIANT in "${VARIANTS[@]}"
  do
    APPNAME=${BOARD}_armgcc_ruuvifw_${VARIANT}_${TAG}_app.hex
    FULLNAME=${BOARD}_armgcc_ruuvifw_${VARIANT}_${TAG}_full.hex
    DFUNAME=${BOARD}_armgcc_ruuvifw_${VARIANT}_${TAG}_dfu_app.zip
    SDKNAME=${BOARD}_armgcc_ruuvifw_${VARIANT}_${TAG}_sdk12.3_to_15.3_dfu.zip

    curl -XPOST -H "Authorization:token ${GH_TOKEN}" -H "Content-Type:application/octet-stream" --data-binary @targets/${BOARD}/armgcc/${APPNAME} https://uploads.github.com/repos/${GH_USER}/${GH_REPO}/releases/${id}/assets?name=${APPNAME}
    curl -XPOST -H "Authorization:token ${GH_TOKEN}" -H "Content-Type:application/octet-stream" --data-binary @targets/${BOARD}/armgcc/${FULLNAME} https://uploads.github.com/repos/${GH_USER}/${GH_REPO}/releases/${id}/assets?name=${FULLNAME}
    curl -XPOST -H "Authorization:token ${GH_TOKEN}" -H "Content-Type:application/octet-stream" --data-binary @targets/${BOARD}/armgcc/${DFUNAME} https://uploads.github.com/repos/${GH_USER}/${GH_REPO}/releases/${id}/assets?name=${DFUNAME}
    curl -XPOST -H "Authorization:token ${GH_TOKEN}" -H "Content-Type:application/octet-stream" --data-binary @targets/${BOARD}/armgcc/${SDKNAME} https://uploads.github.com/repos/${GH_USER}/${GH_REPO}/releases/${id}/assets?name=${SDKNAME}

  done
done

