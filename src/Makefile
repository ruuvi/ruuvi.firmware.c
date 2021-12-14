# @file  Makefile
# @author Otso Jousimaa <otso@ojousima.net>
# @date 2021-03-30
#       2021-11-19 DG align options, MODE=APPLICATION_MODE_xxx
# @brief Setup compile of sources for BOARDS and Variants.
#        Set up directory names and call targets/ruuvitag_b/armgcc/Makefile
#          which sets up options and calls Makefile.common which does compile & link
#
#        There MUST be tab characters in this Makefile (Other wise error: missing separator)
#
# @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
#
# There is a seperate version of the for the
# Segger Embedded Studio at ruuvi.firmware.c.emProject.
# Changes to this should be made there as well.

#  For example v3.31.1
TAG := $(shell git describe --tags --exact-match)

# For example 24cbe77 (the hash)
COMMIT := $(shell git rev-parse --short HEAD)

# VERSION will be the TAG if it's good, other the ugly commit hash
VERSION := $(if $(TAG),\\\"$(TAG)\\\",\\\"$(COMMIT)\\\")

BOARDS = ruuvitag_b kaarle kalervo keijo
VARIANTS = default test

.PHONY: all sync ${BOARDS} analysis publish clean

all:  clean ${BOARDS}

sync:
	@echo Synchronizing GIT...
	# TODO: reject if repo is not clean
	git submodule update --init --recursive
	git submodule sync --recursive
	git submodule update --init --recursive


ruuvitag_b:
	@echo build FW ${VERSION}
	#   application_modes.h always includes application_default.h
    # DEBUG used by Ruuvi and Nordic

	@echo + Calling targets/ruuvitag_b/armgcc/Makefile
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
    #            The NAME=-Dxxx generate a #define xxx 
	$(MAKE) -C targets/ruuvitag_b/armgcc 	MODE=-DAPPLICATION_MODE_DEFAULT \
											DEBUG=-DNDEBUG \
											FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	@echo + package next
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_default

	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc 	MODE=-DAPPLICATION_MODE_LONGLIFE \
											DEBUG=-DNDEBUG \
											FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	@echo + package next
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_longlife

	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc 	MODE=-DAPPLICATION_MODE_LONGMEM \
											DEBUG=-DNDEBUG \
											FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	@echo + package next
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_longmem

	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc 	MODE=-DAPPLICATION_MODE_DEBUG \
											FW_VERSION=-DAPP_FW_VERSION=${VERSION} \
											DEBUG=-DDEBUG \
											RUN_TESTS=-DRUUVI_RUN_TESTS \
											OPT="-Og -g3"
	#                        DEBUG version with -Og and -g3 includes gdb information
	@echo + package next
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_test

kaarle:
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc MODE=-DAPPLICATION_MODE_DEFAULT \DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/kaarle/armgcc/package.sh -n ruuvifw_default

	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/kaarle/armgcc/package.sh -n ruuvifw_longlife

	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc DEBUG=-DDEBUG RUN_TESTS=-DRUUVI_RUN_TESTS FW_VERSION=-DAPP_FW_VERSION=${VERSION} OPT="-Og -g3"
	targets/kaarle/armgcc/package.sh -n ruuvifw_test

kalervo:
	$(MAKE) -C targets/kalervo/armgcc clean
	$(MAKE) -C targets/kalervo/armgcc MODE=-DAPPLICATION_MODE_DEFAULT \DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/kalervo/armgcc/package.sh -n ruuvifw_default

	$(MAKE) -C targets/kalervo/armgcc clean
	$(MAKE) -C targets/kalervo/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/kalervo/armgcc/package.sh -n ruuvifw_longlife

keijo:
	$(MAKE) -C targets/keijo/armgcc clean
	$(MAKE) -C targets/keijo/armgcc MODE=-DAPPLICATION_MODE_DEFAULT \DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/keijo/armgcc/package.sh -n ruuvifw_default

	$(MAKE) -C targets/keijo/armgcc clean
	$(MAKE) -C targets/keijo/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/keijo/armgcc/package.sh -n ruuvifw_longlife

analysis:
	@echo build FW ${VERSION}
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc 	DEBUG=-DNDEBUG \
											FW_VERSION=-DAPP_FW_VERSION=${VERSION} \
											OPT="-Og -g3" \
											VERBOSE=1 \
											ABSOLUTE_PATHS=1

# https://medium.com/@systemglitch/continuous-integration-with-jenkins-and-github-release-814904e20776
publish:
	@echo Publishing $(TAG)
	./release.sh

clean:
	@echo Cleaning build files
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kalervo/armgcc clean
	$(MAKE) -C targets/keijo/armgcc clean

# } The end!
