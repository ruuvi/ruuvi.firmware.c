PROJECT_NAME     := keijo_s140
TARGETS          := nrf52811_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../nRF5_SDK_15.3.0_59ac345
PROJ_DIR := ../../..

LDSCRIPT := $(if $(LDSCRIPT),$(LDSCRIPT),keijo.ld)

$(OUTPUT_DIRECTORY)/nrf52811_xxaa.out: \
  LINKER_SCRIPT  := $(LDSCRIPT)

include ${PROJ_DIR}/gcc_sources.make

# Source files common to all targets
SRC_FILES += ${COMMON_SOURCES} \
   $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52811.S \
   $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52811.c
   
# Include folders common to all targets
INC_FOLDERS += ${COMMON_INCLUDES}

# Libraries common to all targets
LIB_FILES += 

# Optimization flags
OPT ?= -Os
# Uncomment the line below to enable link time optimization
#OPT += -flto

CFLAGS += $(MODE)
CFLAGS += $(RUN_TESTS)
CFLAGS += $(DEBUG)
CFLAGS += $(FW_VERSION)
CFLAGS += $(OPT)
CFLAGS += $(COMMON_DEFINES)
CFLAGS += -DBL_SETTINGS_ACCESS_ONLY
CFLAGS += -DBOARD_KEIJO
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_SOFT
CFLAGS += -DNRF52811_XXAA
CFLAGS += -DNRF52_PAN_74
CFLAGS += -DNRF_DFU_SVCI_ENABLED
CFLAGS += -DNRF_DFU_TRANSPORT_BLE=1
CFLAGS += -DNRF_SD_BLE_API_VERSION=6
CFLAGS += -DS140
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DNRF_CRYPTO_MAX_INSTANCE_COUNT=1
CFLAGS += -DuECC_ENABLE_VLI_API=0
CFLAGS += -DuECC_OPTIMIZATION_LEVEL=3
CFLAGS += -DuECC_SQUARE_FUNC=0
CFLAGS += -DuECC_SUPPORT_COMPRESSED_POINT=0
CFLAGS += -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1
CFLAGS += -DSWI_DISABLE0
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
CFLAGS += -mfloat-abi=soft
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing -fstack-usage
CFLAGS += -fno-builtin -fshort-enums 

# C++ flags common to all targets
CXXFLAGS += $(OPT)


# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=soft
ASMFLAGS += -DBL_SETTINGS_ACCESS_ONLY
ASMFLAGS += -DBOARD_KEIJO
ASMFLAGS += -DBOARD_CUSTOM
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DNRF52811_XXAA
ASMFLAGS += -DNRF52_PAN_74
ASMFLAGS += -DNRF_DFU_SVCI_ENABLED
ASMFLAGS += -DNRF_DFU_TRANSPORT_BLE=1
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=6
ASMFLAGS += -DS140
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DNRF_CRYPTO_MAX_INSTANCE_COUNT=1
ASMFLAGS += -DuECC_ENABLE_VLI_API=0
ASMFLAGS += -DuECC_OPTIMIZATION_LEVEL=3
ASMFLAGS += -DuECC_SQUARE_FUNC=0
ASMFLAGS += -DuECC_SUPPORT_COMPRESSED_POINT=0
ASMFLAGS += -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1
ASMFLAGS += -DSWI_DISABLE0

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=soft
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

nrf52811_xxaa: CFLAGS += -D__HEAP_SIZE=512
nrf52811_xxaa: CFLAGS += -D__STACK_SIZE=8192
nrf52811_xxaa: ASMFLAGS += -D__HEAP_SIZE=512
nrf52811_xxaa: ASMFLAGS += -D__STACK_SIZE=8192

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default print_cflags

# Default target - first one defined
default: nrf52811_xxaa

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common
$(foreach target, $(TARGETS), $(call define_target, $(target)))

print_cflags:
	@echo "CFLAGS: " $(CFLAGS) "\n"

