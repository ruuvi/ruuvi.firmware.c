# @file  Makefile
# @author Otso Jousimaa <otso@ojousima.net>
# @date 2021-03-30
# @brief Setup compile of sources for BOARDS and Variants.
#        Set up directory names and call targets/ruuvitag_b/armgcc/Makefile
#        which sets up options and calls Makefile.common which does compile & link
#
# There is a seperate version of the project configuration for the
# Segger Embedded Studio at ruuvi.firmware.c.emProject.
# Changes to this makefile should be made there as well.
#
#	The -Dxxx generates a #define xxx in C preprocessor.
#
# @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.

#  For example v3.31.1
TAG := $(shell git describe --tags --exact-match)

# For example 24cbe77 (the hash)
COMMIT := $(shell git rev-parse --short HEAD)

# VERSION will be the tag if one exists for current commit, otherwise the commit hash.
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
	@echo build FW ${VERSION} for board $@

	@echo + Calling targets/ruuvitag_b/armgcc/Makefile
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc 	FW_VERSION=-DAPP_FW_VERSION=${VERSION} DEBUG=-DNDEBUG  MODE=-DAPPLICATION_MODE_DEFAULT   
	@echo + package next
	targets/$@/armgcc/package.sh -n ruuvifw_default

	$(MAKE) -C targets/$@/armgcc clean
	$(MAKE) -C targets/$@/armgcc 	FW_VERSION=-DAPP_FW_VERSION=${VERSION} DEBUG=-DNDEBUG  MODE=-DAPPLICATION_MODE_LONGLIFE   
	@echo + package next
	targets/$@/armgcc/package.sh -n ruuvifw_longlife

	$(MAKE) -C targets/$@/armgcc clean
	$(MAKE) -C targets/$@/armgcc 	FW_VERSION=-DAPP_FW_VERSION=${VERSION} DEBUG=-DNDEBUG  MODE=-DAPPLICATION_MODE_LONGMEM 
	@echo + package next
	targets/$@/armgcc/package.sh -n ruuvifw_longmem

	$(MAKE) -C targets/$@/armgcc clean
	$(MAKE) -C targets/$@/armgcc 	FW_VERSION=-DAPP_FW_VERSION=${VERSION} DEBUG=-DDEBUG   MODE=-DAPPLICATION_MODE_DEBUG \
											RUN_TESTS=-DRUUVI_RUN_TESTS OPT="-Og -g3" # gdb information included 
	@echo + package next
	targets/$@/armgcc/package.sh -n ruuvifw_test

kaarle:
	@echo build FW ${VERSION} for board $@
	$(MAKE) -C targets/$@/armgcc clean
	$(MAKE) -C targets/$@/armgcc 		FW_VERSION=-DAPP_FW_VERSION=${VERSION} DEBUG=-DNDEBUG  MODE=-DAPPLICATION_MODE_DEFAULT 
	targets/$@/armgcc/package.sh -n ruuvifw_default

	$(MAKE) -C targets/$@/armgcc clean
	$(MAKE) -C targets/$@/armgcc 		FW_VERSION=-DAPP_FW_VERSION=${VERSION} DEBUG=-DNDEBUG MODE=-DAPPLICATION_MODE_LONGLIFE 
	targets/$@/armgcc/package.sh -n ruuvifw_longlife

	$(MAKE) -C targets/$@/armgcc clean
	$(MAKE) -C targets/$@/armgcc 		FW_VERSION=-DAPP_FW_VERSION=${VERSION} DEBUG=-DDEBUG   MODE=-DAPPLICATION_MODE_DEBUG \
											RUN_TESTS=-DRUUVI_RUN_TESTS OPT="-Og -g3"
	targets/$@/armgcc/package.sh -n ruuvifw_test

kalervo:
	@echo build FW ${VERSION} for board $@
	$(MAKE) -C targets/$@/armgcc clean
	$(MAKE) -C targets/$@/armgcc 		FW_VERSION=-DAPP_FW_VERSION=${VERSION}  DEBUG=-DNDEBUG  MODE=-DAPPLICATION_MODE_DEFAULT 
	targets/$@/armgcc/package.sh -n ruuvifw_default

	$(MAKE) -C targets/$@/armgcc clean
	$(MAKE) -C targets/$@/armgcc		FW_VERSION=-DAPP_FW_VERSION=${VERSION}  DEBUG=-DNDEBUG  MODE=-DAPPLICATION_MODE_LONGLIFE 
	targets/$@/armgcc/package.sh -n ruuvifw_longlife

keijo:
	@echo build FW ${VERSION} for board $@
	$(MAKE) -C targets/$@/armgcc clean
	$(MAKE) -C targets/$@/armgcc			FW_VERSION=-DAPP_FW_VERSION=${VERSION}  DEBUG=-DNDEBUG  MODE=-DAPPLICATION_MODE_DEFAULT
	targets/$@/armgcc/package.sh -n ruuvifw_default

	$(MAKE) -C targets/$@/armgcc clean
	$(MAKE) -C targets/$@/armgcc 		FW_VERSION=-DAPP_FW_VERSION=${VERSION}  DEBUG=-DNDEBUG  MODE=-DAPPLICATION_MODE_LONGLIFE 
	targets/$@/armgcc/package.sh -n ruuvifw_longlife

analysis:
	@echo analysis FW ${VERSION}
	$(MAKE) -C targets/ruuvitag_b/armgcc clean FW_VERSION=-DAPP_FW_VERSION=${VERSION} 
	$(MAKE) -C targets/ruuvitag_b/armgcc 	OPT="-Og -g3" VERBOSE=1 ABSOLUTE_PATHS=1 DEBUG=-DNDEBUG 

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
