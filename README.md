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
Under development, please follow [Ruuvi Blog](https://ruuvi.com/blog/) for details. 
The project is in beta stage, no breaking changes are expected
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
* On Mac OS [XCode](https://wilsonmar.github.io/xcode/)

### Suggested 
* [Ruuvi Dev kit board](https://shop.ruuvi.com/product/devkit/) and a USB power & data cable.
* Or any other SWD programmer and a cable matching your target board. 
 
### To validate you changes, for example before making Pull Requests
* [Ceedling](http://www.throwtheswitch.org/ceedling)
* [PVS-Studio Analyzer](https://www.viva64.com/en/pvs-studio/) 
* [Artistic Style](https://sourceforge.net/projects/astyle/files/)
* [doxygen](https://www.doxygen.nl/index.html)

## SDK 15.3.
Download [Nordic SDK15.3](https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v15.x.x/) (.8GB)and install it at the project root.
If you're working on multiple nRF projects, use soft linking to have only one copy.

# Creating your fork
### Use git to include all Submodules ( ruuvi.drivers.c, ruuvi.endpoint.c, etc )
Run `git submodule sync --recursive` and `git submodule update --init --recursive` to update the modules from the master repository . 

## Coding style
Use coding style consistant with [BARR-C:2018](https://barrgroup.com/embedded-systems/books/embedded-c-coding-standard).
Coding style is enforced with [Artistic Style](http://astyle.sourceforge.net). 
Some source files were inserted into the master repository before this was established and running astyle will revise the source as necessary.
See .astylerc for non-default options (for example max-code-length=90).
Run `make astyle`.

# Compiling and Testing
## Segger Embedded Studio should be used for developing. 
Segger Embedded Studio is set up by installing [nRF Connect for Desktop](https://www.nordicsemi.com/?sc_itemid=%7BB935528E-8BFA-42D9-8BB5-83E2A5E1FF5C%7D) 
and following Getting Started instructions.

### Start SES 
`File -> Open Solution -> ruuvi.firmware.c.emProject`. 
Each of the target boards is in their own project.
Sess  'build' compiles and links the firmware. 
In the Project Explorer select the correct project according to the appropriate tag name.
Select the Build Configuration: `Debug`, `Long Life` or `Release`.   

### Build 
Navigate to `Build -> Build <project name>` or press the F7 key. 

### Connect you board with either:
* nRF52 DevKit and [RuuviTag Development Shield](https://lab.ruuvi.com/devshield/). 
* nRF52 DevKit and [TC2030-CTX-NL 6-pin Cable](https://www.tag-connect.com/product/tc2030-ctx-nl-6-pin-no-legs-cable-with-10-pin-micro-connector-for-cortex-processors)

### Debug 
Navigate to `Debug -> Go` or press the F5 key.
This will flash the firmware and start the debugger.

## ARMGCC is used for [Jenkins builds](http://jenkins.ruuvi.com/job/ruuvi.firmware.c/)
You can make the project and a single variant by runnning "make variantName" (for example "make ruuvitag_b" at top level of this repository

When building binaries for distribution, use the provided 'Makefile' script.
This way you can be certain to have a repeatable build process. 
The Makefile uses the tag name of current git commit for filenames and version number.
Tags should be valid semantic versions, starting with `v` and possibly having pre-release information such as `-rc2`. Do not add build information such as `+TestFW`. If you have tagged the version as `v3.99.1` the files will be named `$BOARD_armgcc_ruuvifw_$VARIANT_v3.99.1_$TYPE.extension`. For example `ruuvitag_b_armgcc_ruuvifw_default_v3.29.3-rc1_full.hex`. 

### Flashing
Connect your board with either:
* nRF52 DevKit and [RuuviTag Development Shield](https://lab.ruuvi.com/devshield/). 
* nRF52 DevKit and [TC2030-CTX-NL 6-pin Cable](https://www.tag-connect.com/product/tc2030-ctx-nl-6-pin-no-legs-cable-with-10-pin-micro-connector-for-cortex-processors)

### nRF Command Line Tools
Navigate to `ruuvi.firmware.c/src/targets/<board name>/armgcc`.<br>
Run `make` to compile the application.<br>
Run `./package.sh` to generate the complete firmware HEX and ZIP files.<br>
To flash the tag, run 
<pre>
nrfjprog  --eraseall   # including previous bootloader
nrfjprog  --program ruuvitag_b_armgcc_ruuvifw_v3.30.0-RC5_app.hex --verify --fast --reset 
</pre>       

# Finishing up
## Static analysis
The code can be checked with PVS Studio and Sonarcloud for some common errors, style issues and potential problems. [Here](https://ruuvi.github.io/ruuvi.firmware.c/fullhtml/index.html) is a link to generated report.

### PVS
Obtain license and software from [Viva64](https://www.viva64.com/en/pvs-studio/).
Installation process is described in [ruuvi.docs.com](https://docs.ruuvi.com/toolchain/pvs-studio)
Make runs PVS Studio scan and outputs results under doxygen/html/fullhtml. 
This produces many warnings, you need to filter the warnings you're interested in. For example you probably want to filter out warnings related to 64-bit systems. 

### Sonar scan
Travis pushes the results to [SonarCloud.IO](https://sonarcloud.io/dashboard?id=ruuvi_ruuvi.firmware.c).
SonarCloud uses access token which is private to Ruuvi.
You need to fork the project and setup the SonarCloud under your own account to run Sonar Scan on your own code.

## Running unit tests
### Ceedling
Unit tests are implemented with Ceedling. Run the tests with
`ceedling test:all`

### Gcov
Ceedling can also generate Gcov reports with `ceedling gcov:all utils:gcov`.
The report can be found under _build/artifacts/gcov_.

### Unit test continuous integration
Travis will fail the build if unit test fails and Gcov results will get pushed to SonarCloud.

# How to contribute
Please let us know your thoughts on the direction and structure of the project. Does the project help you to understand how to build code for the RuuviTag?
Is the structure of the project sensible to you? 

If you want to assist in the project maintenance by fixing some issues _doxygen.error_ is
a good place to look for code which needs better commenting. Project badges at the top of the
readme point to issues which range from trivial clarifications to complex refactoring. 

If you want to add new features, please discuss the feature first at the [Forum](https://f.ruuvi.com) and then create ceedling
unit tests for the functionality. Once the functionality is agreed and testable in can be integrated
into project.

# Licensing
Ruuvi code is BSD-3 licensed. Submodules and external dependencies have their own licenses, which are BSD-compatible.

# Documentation
Document is generated with Doxygen. Run `make doxygen` to generate the docs locally, or
browse to [Travis built docs](https://ruuvi.github.io/ruuvi.firmware.c)
