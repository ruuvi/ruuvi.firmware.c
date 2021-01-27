# ruuvi.firmware.c
Current git repository status:
[![Build Status](https://jenkins.ruuvi.com/buildStatus/icon?job=ruuvi.firmware.c+-+deploy)](https://jenkins.ruuvi.com/job/ruuvi.firmware.c%20-%20deploy/)
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=alert_status)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=bugs)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=code_smells)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Coverage](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=coverage)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=duplicated_lines_density)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Lines of Code](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=ncloc)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Maintainability Rating](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=sqale_rating)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Reliability Rating](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=reliability_rating)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)
[![Technical Debt](https://sonarcloud.io/api/project_badges/measure?project=ruuvi_ruuvi.firmware.c&metric=sqale_index)](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c)

Ruuvi Firmware version 3. Built on top of Nordic SDK 15, uses both Ruuvi and external repositories as submodules.
Under development, please follow [Ruuvi Blog](https://blog.ruuvi.com) for details. The project is in beta stage, no breaking changes are intented but will be done if absolutely necessary for some reason.  

# Setting up

## Prerequisites
* [gcc-arm-none-eabi-7-2018-q2-update](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads/7-2018-q2-update) Check that nRF5_SDK_15.3.0_59ac345/components/toolchain/gcc  Makefile.posix or Makefile.windows points to it.
* git
* python 3.7 or later and [pip package manager](https://pypi.org/project/pip/)
* [Nordic mergehex](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_nrfutil%2FUG%2Fnrfutil%2Fnrfutil_intro.html) merge 2 or 3 (not 4) .hex files into one.
* [Nordic nrfutil](https://infocenter.nordicsemi.com/index.jsp?topic=%2Fug_nrfutil%2FUG%2Fnrfutil%2Fnrfutil_intro.html) Creates DeviceFirmwareUpdate zip file from hex
* [Nordic Command Line Tools](https://www.nordicsemi.com/Software-and-tools/Development-Tools/nRF-Command-Line-Tools/Download)
* [Nordic nRFconnect](https://www.nordicsemi.com/Software-and-tools/Development-Tools/nRF-Connect-for-desktop) for your desktop or phone to upload DFU to the Ruuvi and a means to transfer the DFU zip file to you phone.
* A computer or phone with a bluetooth radio to receive advertisments from the ruuvi. See [Dealing with the data](https://github.com/ruuvi/ruuvitag_fw/wiki/Dealing-with-the-data)
* on Mac OS [XCode](https://wilsonmar.github.io/xcode/)

### Suggested 
* [Ruuvi Dev kit board](https://shop.ruuvi.com/product/devkit/) and a USB power & data cable.
* Or any other SWD programmer and a cable matching your target board. 
 
### To run makefiles locally, e.g. before making Pull Requests
* [Ceedling](http://www.throwtheswitch.org/ceedling)
* [PVS-Studio Analyzer](https://www.viva64.com/en/pvs-studio/) 
* [astyle](https://sourceforge.net/projects/astyle/files/)
* [doxygen](https://www.doxygen.nl/index.html)

## SDK 15.3
Download [Nordic SDK15.3](https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v15.x.x/) (.8GB)and install it at the project root.
If you're working on multiple nRF projects, use soft linking to have only one copy.

## Submodules
Run `git submodule sync --recursive` and `git submodule update --init --recursive` to update the modules from new remotes. 

## Toolchain
ARMGCC is used for [Jenkins builds](http://jenkins.ruuvi.com/job/ruuvi.firmware.c/), it's recommended that you use Segger Embedded Studio for developing. You can make the project and a single variant by runnning "make variantName" (for example "make ruuvitag_b" at top level of this repository
 
Segger Embedded Studio can be set up by installing [nRF Connect for Desktop](https://www.nordicsemi.com/?sc_itemid=%7BB935528E-8BFA-42D9-8BB5-83E2A5E1FF5C%7D) 
and following Getting Started plugin instructions.

Start SES and open `ruuvi_ruuvi.firmware.c.emProject` at root level. Each of the target boards is in their own project.

## Code style
Code is formatted with [Artistic Style](http://astyle.sourceforge.net). 
Run `make astyle`.

## Static analysis
The code can be checked with PVS Studio and Sonarcloud for some common errors, style issues and potential problems. [Here](https://ruuvi.github.io/ruuvi.firmware.c/fullhtml/index.html) is a link to generated report.


### PVS
Obtain license and software from [Viva64](https://www.viva64.com/en/pvs-studio/).

Make runs PVS Studio scan and outputs results under doxygen/html/fullhtml. 

This produces hundreds of warnings, you need to filter the warnings you're interested in. For example you probably want to filter out warnings related to 64-bit systems. 

### Sonar scan
Travis pushes the results to [SonarCloud.IO](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c).
SonarCloud uses access token which is private to Ruuvi, you'll need to fork the project and setup
the SonarCloud under your own account if you wish to run Sonar Scan on your own code.

# Running unit tests
## Ceedling
Unit tests are implemented with Ceedling. Run the tests with
`ceedling test:all`

### Gcov
Ceedling can also generate Gcov reports with `ceedling gcov:all utils:gcov`.
The report can be found under _build/artifacts/gcov_.

## Unit test continuous integration
Travis will fail the build if unit test fails and Gcov results will get pushed to SonarCloud.

# Usage
Compile and flash the project to your board using Segger Embedded Studio. 
Note: You should erase your board entirely in case there is a bootloader from a previous firmware.

If you're building binaries for distribution, use the provided make script to generate binaries.
This way you can be certain to have a repeatable build process. The makefile takes tag name of current git commit
and names the binaries with the tag. The version information also gets compiled into binaries. 
If you have tagged the version as `v3.99.1` the outputs will be named `$BOARD_armgcc_ruuvifw_$VARIANT_v3.99.1_$TYPE.extension`.
For example `ruuvitag_b_armgcc_ruuvifw_default_v3.29.3-rc1_full.hex`. 

Tags should be valid semantic versions, starting with `v` and possibly having pre-release information such as `-rc2`. Do not add build information such as `+TestFW`.

# How to contribute
Please let us know your thoughts on the direction and structure of the project. Does the project help you to understand how to build code for the RuuviTag?
Is the structure of the project sensible to you? 

If you want to assist in the project maintenance by fixing some issues _doxygen.error_ is
a good place to look for code which needs better commenting. Project badges at the top of the
readme point to issues which range from trivial clarifications to complex refactoring. 

If you want to add new features, please discuss the feature first, and then create ceedling
unit tests for the functionality. Once the functionality is agreed and testable in can be integrated
into project.

# Licensing
Ruuvi code is BSD-3 licensed. Submodules and external dependencies have their own licenses, which are BSD-compatible.

# Documentation
Document is generated with Doxygen. Run `make doxygen` to generate the docs locally, or
browse to [Travis built docs](https://ruuvi.github.io/ruuvi.firmware.c)

