.PHONY: all fw clean

all: fw 

fw:
	@echo build FW
	git submodule update --init --recursive
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc MODE=-DAPPLICATION_MODE_LONGLIFE
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_longlife
	$(MAKE) -C targets/ruuvitag_b/armgcc clean
	$(MAKE) -C targets/ruuvitag_b/armgcc
	targets/ruuvitag_b/armgcc/package.sh -n ruuvifw_default


clean:
	@echo cleaning build files…
	$(MAKE) -C targets/ruuvitag_b/armgcc clean

