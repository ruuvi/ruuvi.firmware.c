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
# The -Dxxx generates a #define xxx in C preprocessor.
#
# @copyright Ruuvi Innovations Ltd, license BSD-3-Clause.
#
# Default behavior (no arguments):
#   Builds ruuvitag_b and pca10040 boards with all variants:
#   default, test, longmem, longlife, uuid
#
# Usage examples:
#   make                                              # Build default boards/variants
#   make BOARDS="pca10040"                            # Build only pca10040, all variants
#   make VARIANTS="default test"                      # Build default boards, only default and test variants
#   make BOARDS="pca10040 ruuvitag_b" VARIANTS="default test"  # Specific boards and variants
#   make ruuvitag_b                                   # Build single board with all variants
#   make BOARDS="kaarle" VARIANTS="longlife"          # Build kaarle with longlife only
#
# Available boards: ruuvitag_b, pca10040, kaarle, kalervo, keijo
# Available variants: default, test, longmem, longlife, uuid

#  For example v3.31.1
TAG := $(shell git describe --tags --exact-match)

# For example 24cbe77 (the hash)
COMMIT := $(shell git rev-parse --short HEAD)

# VERSION will be the tag if one exists for current commit, otherwise the commit hash.
VERSION := $(if $(TAG),\\\"$(TAG)\\\",\\\"$(COMMIT)\\\")

# Default boards and variants - can be overridden from command line
# Usage: make BOARDS="pca10040 ruuvitag_b" VARIANTS="default test"
BOARDS ?= ruuvitag_b pca10040
VARIANTS ?= default test longmem longlife uuid

# Variant to mode mapping
MODE_default   = -DAPPLICATION_MODE_DEFAULT
MODE_test      = -DAPPLICATION_MODE_DEBUG
MODE_longmem   = -DAPPLICATION_MODE_LONGMEM
MODE_longlife  = -DAPPLICATION_MODE_LONGLIFE
MODE_uuid      = -DAPPLICATION_MODE_UUID

# Variant to debug flag mapping
DEBUG_default  = -DNDEBUG
DEBUG_test     = -DDEBUG
DEBUG_longmem  = -DNDEBUG
DEBUG_longlife = -DNDEBUG
DEBUG_uuid     = -DNDEBUG

# Variant to optimization flag mapping (release builds optimized, debug builds with debug info)
OPT_default    = -Os
OPT_test       = -Og -g3
OPT_longmem    = -Os
OPT_longlife   = -Os
OPT_uuid       = -Os

# Variant to extra flags mapping (test variant runs tests)
EXTRA_test     = RUN_TESTS=-DRUUVI_RUN_TESTS

.PHONY: all sync ${BOARDS} analysis publish clean build_variant

all: clean $(BOARDS)

sync:
	@echo Synchronizing GIT...
	# TODO: reject if repo is not clean
	git submodule update --init --recursive
	git submodule sync --recursive
	git submodule update --init --recursive

# Generic board target - builds all variants for a board
define BUILD_BOARD
$(1):
	@echo "Building FW $${VERSION} for board $(1)"
	@for variant in $(VARIANTS); do \
		echo "  Building variant: $$$$variant"; \
		case $$$$variant in \
			default)  debug="$(DEBUG_default)";  mode="$(MODE_default)";  opt="$(OPT_default)";  extra="" ;; \
			test)     debug="$(DEBUG_test)";     mode="$(MODE_test)";     opt="$(OPT_test)";     extra="$(EXTRA_test)" ;; \
			longmem)  debug="$(DEBUG_longmem)";  mode="$(MODE_longmem)";  opt="$(OPT_longmem)";  extra="" ;; \
			longlife) debug="$(DEBUG_longlife)"; mode="$(MODE_longlife)"; opt="$(OPT_longlife)"; extra="" ;; \
			uuid)     debug="$(DEBUG_uuid)";     mode="$(MODE_uuid)";     opt="$(OPT_uuid)";     extra="" ;; \
			*) echo "Unknown variant: $$$$variant"; exit 1 ;; \
		esac; \
		$$(MAKE) -C targets/$(1)/armgcc clean 2>/dev/null || true; \
		echo "  make -C targets/$(1)/armgcc FW_VERSION=-DAPP_FW_VERSION=$${VERSION} DEBUG=$$$$debug MODE=$$$$mode OPT=\"$$$$opt\" $$$$extra"; \
		$$(MAKE) -C targets/$(1)/armgcc \
			FW_VERSION=-DAPP_FW_VERSION=$${VERSION} \
			DEBUG=$$$$debug \
			MODE=$$$$mode \
			OPT="$$$$opt" \
			$$$$extra; \
		targets/$(1)/armgcc/package.sh -n ruuvifw_$$$$variant; \
	done
endef

# Generate targets for all boards
$(foreach board,ruuvitag_b pca10040 kaarle kalervo keijo,$(eval $(call BUILD_BOARD,$(board))))

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
	$(MAKE) -C targets/pca10040/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kalervo/armgcc clean
	$(MAKE) -C targets/keijo/armgcc clean
