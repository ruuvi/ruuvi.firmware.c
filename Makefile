.PHONY: all fw clean

all: fw 

fw:
	@echo build FW
	# TODO: Check that repo is clean
	git submodule update --init --recursive
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_longlife

	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc DEBUG=-DNDEBUG
	$(MAKE) -C targets/kaarle/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE DEBUG=-DNDEBUG
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_default

	$(MAKE) -C targets/ruuvitag_b/armgcc clean	
	$(MAKE) -C targets/ruuvitag_b/armgcc DEBUG=-DDEBUG RUN_TESTS=-DRUUVI_RUN_TESTS
	$(MAKE) -C targets/kaarle/armgcc clean	
	$(MAKE) -C targets/kaarle/armgcc DEBUG=-DDEBUG RUN_TESTS=-DRUUVI_RUN_TESTS
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_test


clean:
	@echo cleaning build files…
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/kaarle/armgcc clean

