.PHONY: all fw 

all: fw 

fw:
	@echo build FW
	git submodule update --init --recursive
	$(MAKE) -C targets/ruuvitag_b/armgcc


clean:
	@echo cleaning build files…
	$(MAKE) -C targets/ruuvitag_b/armgcc

