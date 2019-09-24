.PHONY: all sync ruuvitag_b kaarle keijo clean

all: sync clean ruuvitag_b kaarle keijo

sync:
	@echo Synchronizing GIT...
	# TODO: reject if repo is not clean
	git fetch && git submodule foreach --recursive git submodule update --init --recursive
	git submodule sync --recursive
	git submodule update --recursive

ruuvitag_b:
	@echo build FW
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc DEBUG=-DNDEBUG
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_default
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_longlife
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc DEBUG=-DDEBUG RUN_TESTS=-DRUUVI_RUN_TESTS
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_test

kaarle:
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc DEBUG=-DNDEBUG
	targets/kaarle/armgcc/package.sh -n ruuvifw_default
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG
	targets/kaarle/armgcc/package.sh -n ruuvifw_longlife
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc DEBUG=-DDEBUG RUN_TESTS=-DRUUVI_RUN_TESTS
	targets/kaarle/armgcc/package.sh -n ruuvifw_test

keijo: 
	$(MAKE) -C targets/keijo/armgcc clean
	$(MAKE) -C targets/keijo/armgcc DEBUG=-DNDEBUG
	targets/keijo/armgcc/package.sh -n ruuvifw_default
	$(MAKE) -C targets/keijo/armgcc clean
	$(MAKE) -C targets/keijo/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG
	targets/keijo/armgcc/package.sh -n ruuvifw_longlife


clean:
	@echo cleaning build files…
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/keijo/armgcc clean

