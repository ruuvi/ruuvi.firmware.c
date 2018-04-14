# ruuvi.firmware.c
Ruuvi Firmware version 3. Built on top of Nordic SDK 15, uses both Ruuvi and external repositories as submodules.
Under development, not recommended for anything.

# Setting up

## SDK 15.0
Download from Nordic and unpack here, or unpack someplace else and update makefile `SDK_ROOT` to point at right folder.

## Submodules
`git submodule update --init --recursive`

## Toolchain
Follow this [Ruuvi blog post](https://blog.ruuvi.com/ruuvitag-dev-environment-e4b16e2d59e9).

## Usage
Run make in targets/board/armgcc

Flash to your board using nrfjprog or similar.
