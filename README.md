# ruuvi.firmware.c
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
## SDK 15.3
Download [Nordic SDK15.3](https://developer.nordicsemi.com/nRF5_SDK/nRF5_SDK_v15.x.x/) and install it at the project root.
If you're working on multiple nRF projects, you can (and should) use softlinking instead.

## Submodules
Run `git submodule update --init --recursive`. This will search for and install the other git repositories referenced by this project. If any of the submodules has a changed remote, you'll need to run `git submodule sync --recursive` and again `git submodule update --init --recursive` to update the modules from new remotes. 

## Toolchain
ARMGCC is used for [Jenkins builds](http://jenkins.ruuvi.com/job/ruuvi.firmware.c/), it's recommended to use SES for developing. You can make the project and all variants by runnning "make" at top level of this repository. 
 
Segger Embedded Studio can be set up by installing [nRF Connect for Desktop](https://www.nordicsemi.com/?sc_itemid=%7BB935528E-8BFA-42D9-8BB5-83E2A5E1FF5C%7D) 
and following Getting Started plugin instructions.

Start SES and open `ruuvi_ruuvi.firmware.c.emProject` at root level, each of the target boards is in their own project.

## Code style
Code is formatted with [Artistic Style](http://astyle.sourceforge.net). 
Run `astyle --project=.astylerc ./target_file`. To format the entire project,
```
astyle --project=.astylerc "src/main.c"
astyle --project=.astylerc --recursive "src/application_config/*.h"
astyle --project=.astylerc --recursive "src/tasks/*.c"
astyle --project=.astylerc --recursive "src/tasks/*.h"
astyle --project=.astylerc --recursive "src/tests/*.c"
astyle --project=.astylerc --recursive "src/tests/*.h"
```

## Static analysis
The code can be checked with PVS Studio and Sonarcloud for some common errors, style issues and potential problems. [Here](https://ruuvi.github.io/ruuvi.firmware.c/fullhtml/index.html) is a link to generated report which gets pushed to GitHub.


### PVS
Obtain license and software from [Viva64](https://www.viva64.com/en/pvs-studio/).

Make runs PVS Studio scan and outputs results under doxygen/html/fullhtml. 

This results into hundreds of warnings, it is up to you to filter the data you're interested in. For example you probably want to filter out warnings related to 64-bit systems. 

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
Compile and flash the project to your board using SES. Instructions on how to use a bootloader will be added later on.
The project is not yet in a useful state for any practical purpose other than learning. 

Note: You should erase your board entirely in case there is a bootloader from a previous firmware.

# How to contribute
Please let us know your thoughts on the direction and structure of the project. Does the project help you to understand how to build code on RuuviTag?
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

