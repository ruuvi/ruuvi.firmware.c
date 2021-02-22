# Tag on this commit
TAG := $(shell git describe --tags --exact-match)
# Commit hash from git
COMMIT := $(shell git rev-parse --short HEAD)
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
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_default
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_longlife
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc MODE=-DAPPLICATION_MODE_LONGMEM DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_longmem
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc DEBUG=-DDEBUG RUN_TESTS=-DRUUVI_RUN_TESTS FW_VERSION=-DAPP_FW_VERSION=${VERSION} OPT="-Og -g3"
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_test

kaarle:
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/kaarle/armgcc/package.sh -n ruuvifw_default
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/kaarle/armgcc/package.sh -n ruuvifw_longlife
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc DEBUG=-DDEBUG RUN_TESTS=-DRUUVI_RUN_TESTS FW_VERSION=-DAPP_FW_VERSION=${VERSION} OPT="-Og -g3"
	targets/kaarle/armgcc/package.sh -n ruuvifw_test

kalervo: 
	$(MAKE) -C targets/kalervo/armgcc clean
	$(MAKE) -C targets/kalervo/armgcc DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/kalervo/armgcc/package.sh -n ruuvifw_default
	$(MAKE) -C targets/kalervo/armgcc clean
	$(MAKE) -C targets/kalervo/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/kalervo/armgcc/package.sh -n ruuvifw_longlife

keijo: 
	$(MAKE) -C targets/keijo/armgcc clean
	$(MAKE) -C targets/keijo/armgcc DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/keijo/armgcc/package.sh -n ruuvifw_default
	$(MAKE) -C targets/keijo/armgcc clean
	$(MAKE) -C targets/keijo/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION}
	targets/keijo/armgcc/package.sh -n ruuvifw_longlife

analysis:
	@echo build FW ${VERSION}
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc DEBUG=-DNDEBUG FW_VERSION=-DAPP_FW_VERSION=${VERSION} OPT="-Og -g3" VERBOSE=1 ABSOLUTE_PATHS=1

# https://medium.com/@systemglitch/continuous-integration-with-jenkins-and-github-release-814904e20776
publish:
	@echo Publishing $(TAG)
	./release.sh

clean:
	@echo cleaning build files…
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kalervo/armgcc clean
	$(MAKE) -C targets/keijo/armgcc clean