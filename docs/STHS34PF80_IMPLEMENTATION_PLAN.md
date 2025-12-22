# STHS34PF80 Thermal Infrared Presence Sensor Implementation Plan

**Status:** In Progress  
**Date:** 2024-12-22 (Updated)  
**Author:** Ruuvi Development Team  

---

## Implementation Progress

| Phase | Description | Status | PR/Branch |
|-------|-------------|--------|-----------|
| **1** | ruuvi.drivers.c integration | ✅ Complete | `implement-270` - Pending Review |
| **2** | ruuvi.boards.c configuration | ✅ Complete | `feature/sths34pf80-support` |
| **3** | app_sensor integration | ✅ Complete | `feature/sths34pf80-support` (firmware) |
| **4** | Data format 7/87 | ⬜ Not Started | - |
| **5** | Testing | ⬜ Not Started | - |

---

## Table of Contents

1. [Overview](#overview)
2. [Sensor Capabilities](#sensor-capabilities)
3. [Work Phases](#work-phases)
4. [Phase 1: ruuvi.drivers.c Integration](#phase-1-ruuvidriversc-integration)
5. [Phase 2: ruuvi.boards.c Configuration](#phase-2-ruuviboardsc-configuration)
6. [Phase 3: app_sensor Integration](#phase-3-app_sensor-integration)
7. [Phase 4: Data Format 7/87 Implementation](#phase-4-data-format-787-implementation)
8. [Phase 5: Testing](#phase-5-testing)
9. [Dependencies & Prerequisites](#dependencies--prerequisites)
10. [Open Questions](#open-questions)
11. [Estimated Timeline](#estimated-timeline)

---

## Overview

The STHS34PF80 is a thermal infrared (IR) presence and motion detection sensor from STMicroelectronics. It provides:
- Object temperature measurement
- Ambient temperature measurement  
- Presence detection (human presence in a space)
- Motion detection (movement within field of view)
- Ambient shock detection (rapid temperature changes)

### Key Use Cases
- Room occupancy detection
- Seat/desk occupancy monitoring
- Movement detection in spaces
- Pomodoro-style activity reminders
- Smart building automation

---

## Sensor Capabilities

### Hardware Specifications
| Parameter | Value |
|-----------|-------|
| **I2C Address** | 0x5A (7-bit: 0x5A or 8-bit: 0xB5) |
| **WHO_AM_I** | 0xD3 |
| **Output Data Rate** | 0.25 Hz to 30 Hz |
| **Operating Voltage** | 1.7V - 3.6V |
| **Current (Continuous)** | ~10 µA @ 1 Hz (depends on oversamppling) |

### Data Outputs
| Output | Type | Description |
|--------|------|-------------|
| `TOBJECT_L/H` | int16_t | Object temperature (infrared) |
| `TAMBIENT_L/H` | int16_t | Ambient temperature |
| `TPRESENCE_L/H` | int16_t | Presence detection value |
| `TMOTION_L/H` | int16_t | Motion detection value |
| `TAMB_SHOCK_L/H` | int16_t | Ambient temperature shock |
| `TOBJ_COMP_L/H` | int16_t | Compensated object temperature |
| `FUNC_STATUS` | uint8_t | Flags: PRES_FLAG, MOT_FLAG, TAMB_SHOCK_FLAG |

---

## Work Phases

```
┌─────────────────────────────────────────────────────────────────┐
│  Phase 1: ruuvi.drivers.c Integration                          │
│  ├── ruuvi_interface_sths34pf80.h                              │
│  ├── ruuvi_interface_sths34pf80.c                              │
│  └── Platform I2C wrapper (ri_i2c_sths34pf80.c)                │
├─────────────────────────────────────────────────────────────────┤
│  Phase 2: ruuvi.boards.c Configuration                         │
│  └── Board definitions for STHS34PF80-enabled hardware         │
├─────────────────────────────────────────────────────────────────┤
│  Phase 3: app_sensor Integration                               │
│  ├── app_config.h: Enable/config macros                        │
│  ├── app_sensor.c: Init, configure, data retrieval             │
│  └── app_sensor.h: Sensor context and defaults                 │
├─────────────────────────────────────────────────────────────────┤
│  Phase 4: Data Format 7/87 (ruuvi.endpoints.c)                 │
│  ├── ruuvi_endpoint_7.h                                        │
│  ├── ruuvi_endpoint_7.c                                        │
│  └── Encryption support for format 0x87                        │
├─────────────────────────────────────────────────────────────────┤
│  Phase 5: Testing                                               │
│  ├── Unit tests (Ceedling/CMock)                               │
│  └── Integration tests on hardware                             │
└─────────────────────────────────────────────────────────────────┘
```

---

## Phase 1: ruuvi.drivers.c Integration ✅ COMPLETE

### 1.0 Completion Summary

**Branch:** `implement-270` (pending review)  
**Files Created:**
- `src/interfaces/environmental/ruuvi_interface_sths34pf80.h` - Driver interface header
- `src/interfaces/environmental/ruuvi_interface_sths34pf80.c` - Driver implementation (~590 lines)
- `src/interfaces/i2c/ruuvi_interface_i2c_sths34pf80.h` - I2C adapter header
- `src/interfaces/i2c/ruuvi_interface_i2c_sths34pf80.c` - I2C adapter with precise error passthrough
- `test/interfaces/environmental/test_ruuvi_interface_sths34pf80.c` - 31 Ceedling unit tests

**Files Modified:**
- `src/ruuvi_driver_sensor.h` - Added `presence:1`, `motion:1`, `ir_object:1` to bitfield
- `src/ruuvi_driver_enabled_modules.h` - Added `RI_STHS34PF80_ENABLED`
- `gcc_sources.make` - Added source files and include paths
- `project.yml` - Added STHS34PF80 ST driver paths for Ceedling

**Key Implementation Details:**
- Singleton pattern following SHTCX/DPS310 reference implementations
- Uses ST's official `sths34pf80_reg.h/c` driver via `stmdev_ctx_t`
- Precise error handling: I2C adapter passes through `rd_status_t` codes
- Data fields: `temperature_c` (ambient), `presence`, `motion`, `ir_object`
- All 31 unit tests passing

### 1.1 New Files to Create

#### `src/interfaces/environmental/ruuvi_interface_sths34pf80.h`

```c
#ifndef RUUVI_INTERFACE_STHS34PF80_H
#define RUUVI_INTERFACE_STHS34PF80_H

#include "ruuvi_driver_error.h"
#include "ruuvi_driver_sensor.h"

/**
 * @defgroup STHS34PF80 STHS34PF80 Interface
 * @brief Implement @ref rd_sensor_t functions on STHS34PF80
 *
 * The STHS34PF80 is a thermal infrared presence/motion sensor.
 * This implementation supports:
 * - Object temperature (infrared)
 * - Ambient temperature
 * - Presence detection
 * - Motion detection
 */

/** @brief @ref rd_sensor_init_fp */
rd_status_t ri_sths34pf80_init(rd_sensor_t *p_sensor, rd_bus_t bus, uint8_t handle);
/** @brief @ref rd_sensor_init_fp */
rd_status_t ri_sths34pf80_uninit(rd_sensor_t *p_sensor, rd_bus_t bus, uint8_t handle);
/** @brief @ref rd_sensor_setup_fp */
rd_status_t ri_sths34pf80_samplerate_set(uint8_t *samplerate);
rd_status_t ri_sths34pf80_samplerate_get(uint8_t *samplerate);
/** @brief @ref rd_sensor_setup_fp */
rd_status_t ri_sths34pf80_resolution_set(uint8_t *resolution);
rd_status_t ri_sths34pf80_resolution_get(uint8_t *resolution);
/** @brief @ref rd_sensor_setup_fp */
rd_status_t ri_sths34pf80_scale_set(uint8_t *scale);
rd_status_t ri_sths34pf80_scale_get(uint8_t *scale);
/** @brief @ref rd_sensor_dsp_fp */
rd_status_t ri_sths34pf80_dsp_set(uint8_t *dsp, uint8_t *parameter);
rd_status_t ri_sths34pf80_dsp_get(uint8_t *dsp, uint8_t *parameter);
/** @brief @ref rd_sensor_setup_fp */
rd_status_t ri_sths34pf80_mode_set(uint8_t *mode);
rd_status_t ri_sths34pf80_mode_get(uint8_t *mode);
/** @brief @ref rd_sensor_data_fp */
rd_status_t ri_sths34pf80_data_get(rd_sensor_data_t *const data);

#endif // RUUVI_INTERFACE_STHS34PF80_H
```

### 1.2 Data Fields Extension (ruuvi_driver_sensor.h)

The `rd_sensor_data_bitfield_t` needs extension for presence/motion data:

```c
typedef struct
{
    // ... existing fields ...
    unsigned int presence : 1;        //!< Presence detected (boolean-like)
    unsigned int motion : 1;          //!< Motion detected (boolean-like)
    unsigned int ir_object : 1;       //!< IR object signal (dimensionless, relative)
    unsigned int reserved: 7;         //!< Reserved bits
} rd_sensor_data_bitfield_t;
```

**Decision:** Add 3 new fields: `presence`, `motion`, `ir_object`.

| Field | Rationale |
|-------|-----------|
| `presence` | Core STHS34PF80 capability - stationary human detection |
| `motion` | Core STHS34PF80 capability - movement detection |
| `ir_object` | Dimensionless IR object signal from STHS34PF80. Not calibrated °C, but useful for relative comparisons (e.g., "stove is on" detection). Future: add calibrated `object_temp_c` field if contactless thermometer (MLX90614) is implemented. |

**Note:** `ambient_shock` omitted - too sensor-specific. Can be derived from ambient temp delta if needed.

### 1.3 Platform Abstraction Layer

#### `src/nrf5_sdk15_platform/environmental/ruuvi_interface_i2c_sths34pf80.c`

Wrapper connecting ST's driver to nRF5 I2C:

```c
// Adapter for ST's stmdev_ctx_t to Ruuvi I2C interface
int32_t ri_i2c_sths34pf80_write(void *handle, uint8_t reg, const uint8_t *data, uint16_t len);
int32_t ri_i2c_sths34pf80_read(void *handle, uint8_t reg, uint8_t *data, uint16_t len);
```

### 1.4 Implementation Details

#### Init Sequence
1. Check WHO_AM_I (expect 0xD3)
2. Perform soft reset
3. Configure averaging from `APP_SENSOR_STHS34PF80_AVG_TMOS` / `APP_SENSOR_STHS34PF80_AVG_TAMB`
4. Set gain mode from `APP_SENSOR_STHS34PF80_GAIN`
5. Configure low-pass filters from `APP_SENSOR_STHS34PF80_LPF_*`
6. Set ODR to power-down (sleep mode)
7. Configure interrupts (optional)

**Note:** All sensor-specific settings (averaging, gain, LPF) must be configurable via `app_config.h`, not hardcoded.

#### Data Retrieval
```c
rd_status_t ri_sths34pf80_data_get(rd_sensor_data_t *const data)
{
    // 1. Read FUNC_STATUS for presence/motion flags
    // 2. Read TAMBIENT_L/H for ambient temperature
    // 3. Read TOBJECT_L/H or TOBJ_COMP_L/H for object temperature
    // 4. Read TPRESENCE_L/H for presence intensity
    // 5. Read TMOTION_L/H for motion intensity
    // 6. Populate data fields
}
```

### 1.5 Files Modified

| File | Changes |
|------|---------|
| `src/interfaces/environmental/` | Add new .c/.h files |
| `src/ruuvi_driver_sensor.h` | Add presence/motion fields |
| `src/nrf5_sdk15_platform/` | Add I2C platform adapter |
| `gcc_sources.make` | Add new source files |
| `ruuvi_driver_enabled_modules.h` | Add `RI_STHS34PF80_ENABLED` |

---

## Phase 2: ruuvi.boards.c Configuration ✅ COMPLETE

### 2.0 Completion Summary

**Branch:** `feature/sths34pf80-support`  
**Files Modified:**
- `ruuvi_board_defaults.h` - Added default macros (sensor not present)
- `ruuvi_board_ruuvitag_b.h` - Optional support (disabled by default, set `RB_ENVIRONMENTAL_STHS34PF80_PRESENT=1` to enable)
- `ruuvi_board_pca10040.h` - Enabled for development/testing with external sensor

**New Board Macros:**
| Macro | Default | Description |
|-------|---------|-------------|
| `RB_ENVIRONMENTAL_STHS34PF80_PRESENT` | 0 | Set to 1 if sensor populated |
| `RB_STHS34PF80_I2C_ADDRESS` | 0x5A | Fixed 7-bit I2C address |
| `RB_STHS34PF80_I2C_MAX_SPD` | 400k | Max I2C speed |
| `RB_STHS34PF80_SENSOR_POWER_PIN` | UNUSED | Power control pin |
| `RB_INT_STHS34PF80_PIN` | UNUSED | Optional interrupt pin |

### 2.1 New Board Definition (or extend existing)

For a new board with STHS34PF80 (e.g., `ruuvi_board_motion.h`):

```c
// STHS34PF80 Infrared Presence Sensor
#define RB_ENVIRONMENTAL_STHS34PF80_PRESENT   1
#define RB_STHS34PF80_I2C_ADDRESS             0x5AU  // 7-bit address
#define RB_STHS34PF80_I2C_MAX_SPD             RB_I2C_FREQUENCY_400k
#define RB_STHS34PF80_SENSOR_POWER_PIN        RB_I2C_BUS_POWER_PIN  // Shared I2C bus power

// Interrupt pin (optional, for data ready / presence detection)
#define RB_INT_STHS34PF80_PIN                 RB_PORT_PIN_MAP(0, 11)  // Example
```

### 2.2 I2C Configuration

Ensure I2C pins are defined:
```c
#define RB_I2C_SDA_PIN               RB_PORT_PIN_MAP(0, 4)
#define RB_I2C_SCL_PIN               RB_PORT_PIN_MAP(0, 5)
```

### 2.3 Files to Modify/Create

| File | Changes |
|------|---------|
| `ruuvi_board_<new>.h` | New board with STHS34PF80 |
| `ruuvi_board_defaults.h` | Add default values for STHS34PF80 configs |

---

## Phase 3: app_sensor Integration ✅ COMPLETE

### 3.0 Completion Summary

**Branch:** `feature/sths34pf80-support` (in ruuvi.firmware.c)  
**Files Modified:**
- `src/application_config/app_config.h` - Sensor enable/config macros
- `src/app_sensor.h` - Sensor index, include, default config macro
- `src/app_sensor.c` - Static context and registration

**Configuration Macros Added:**
| Macro | Default | Description |
|-------|---------|-------------|
| `APP_SENSOR_STHS34PF80_ENABLED` | `RB_ENVIRONMENTAL_STHS34PF80_PRESENT` | Enable sensor |
| `APP_SENSOR_STHS34PF80_DSP_FUNC` | `RD_SENSOR_DSP_LAST` | DSP function |
| `APP_SENSOR_STHS34PF80_DSP_PARAM` | 1 | DSP parameter |
| `APP_SENSOR_STHS34PF80_MODE` | `RD_SENSOR_CFG_CONTINUOUS` | Operating mode |
| `APP_SENSOR_STHS34PF80_RESOLUTION` | `RD_SENSOR_CFG_DEFAULT` | Resolution |
| `APP_SENSOR_STHS34PF80_SAMPLERATE` | 1 | 1 Hz sample rate |
| `APP_SENSOR_STHS34PF80_SCALE` | `RD_SENSOR_CFG_DEFAULT` | Scale |
| `RI_STHS34PF80_ENABLED` | `APP_SENSOR_STHS34PF80_ENABLED` | Driver enable |
| `APP_FLASH_SENSOR_STHS34PF80_RECORD` | 0xC4 | NVM record ID |

### 3.1 app_config.h Additions

```c
/** @brief Enable STHS34PF80 sensor */
#ifndef APP_SENSOR_STHS34PF80_ENABLED
#   define APP_SENSOR_STHS34PF80_ENABLED RB_ENVIRONMENTAL_STHS34PF80_PRESENT
#endif

#ifndef APP_SENSOR_STHS34PF80_DSP_FUNC
#   define APP_SENSOR_STHS34PF80_DSP_FUNC RD_SENSOR_DSP_LAST
#endif
#ifndef APP_SENSOR_STHS34PF80_DSP_PARAM
#   define APP_SENSOR_STHS34PF80_DSP_PARAM 1
#endif
#ifndef APP_SENSOR_STHS34PF80_MODE
#   define APP_SENSOR_STHS34PF80_MODE RD_SENSOR_CFG_CONTINUOUS
#endif
#ifndef APP_SENSOR_STHS34PF80_RESOLUTION
#   define APP_SENSOR_STHS34PF80_RESOLUTION RD_SENSOR_CFG_DEFAULT
#endif
#ifndef APP_SENSOR_STHS34PF80_SAMPLERATE
#   define APP_SENSOR_STHS34PF80_SAMPLERATE (1U)  // 1 Hz default
#endif
#ifndef APP_SENSOR_STHS34PF80_SCALE
#   define APP_SENSOR_STHS34PF80_SCALE RD_SENSOR_CFG_DEFAULT
#endif

/** @brief STHS34PF80-specific configuration */
#ifndef APP_SENSOR_STHS34PF80_AVG_TMOS
#   define APP_SENSOR_STHS34PF80_AVG_TMOS (32U)  // Averaging for object temp: 2,8,32,128,256,512,1024,2048
#endif
#ifndef APP_SENSOR_STHS34PF80_AVG_TAMB
#   define APP_SENSOR_STHS34PF80_AVG_TAMB (8U)   // Averaging for ambient temp: 1,2,4,8
#endif
#ifndef APP_SENSOR_STHS34PF80_GAIN
#   define APP_SENSOR_STHS34PF80_GAIN (0U)       // 0=wide mode, 7=default mode
#endif
#ifndef APP_SENSOR_STHS34PF80_LPF_M
#   define APP_SENSOR_STHS34PF80_LPF_M (0U)      // Motion LPF: ODR/9,20,50,100,200,400,800
#endif
#ifndef APP_SENSOR_STHS34PF80_LPF_P
#   define APP_SENSOR_STHS34PF80_LPF_P (0U)      // Presence LPF
#endif
#ifndef APP_SENSOR_STHS34PF80_LPF_P_M
#   define APP_SENSOR_STHS34PF80_LPF_P_M (0U)    // Presence+Motion LPF
#endif
#ifndef APP_SENSOR_STHS34PF80_LPF_A_T
#   define APP_SENSOR_STHS34PF80_LPF_A_T (0U)    // Ambient temp LPF
#endif

/** @brief Enable STHS34PF80 driver */
#ifndef RI_STHS34PF80_ENABLED
#   define RI_STHS34PF80_ENABLED APP_SENSOR_STHS34PF80_ENABLED
#endif
```

### 3.2 app_sensor.h Additions

Add sensor index:
```c
enum
{
    // ... existing indices ...
#if APP_SENSOR_STHS34PF80_ENABLED
    STHS34PF80_INDEX,
#endif
    SENSOR_COUNT
};
```

Add default configuration macro:
```c
#if APP_SENSOR_STHS34PF80_ENABLED
#define APP_SENSOR_STHS34PF80_DEFAULT_CFG                     \
  {                                                            \
    .sensor = {0},                                            \
    .init = &ri_sths34pf80_init,                              \
    .configuration =                                          \
        {                                                     \
            .dsp_function = APP_SENSOR_STHS34PF80_DSP_FUNC,   \
            .dsp_parameter = APP_SENSOR_STHS34PF80_DSP_PARAM, \
            .mode = APP_SENSOR_STHS34PF80_MODE,               \
            .resolution = APP_SENSOR_STHS34PF80_RESOLUTION,   \
            .samplerate = APP_SENSOR_STHS34PF80_SAMPLERATE,   \
            .scale = APP_SENSOR_STHS34PF80_SCALE},            \
    .nvm_file = APP_FLASH_SENSOR_FILE,                        \
    .nvm_record = APP_FLASH_SENSOR_STHS34PF80_RECORD,         \
    .bus = RD_BUS_I2C,                                        \
    .handle = RB_STHS34PF80_I2C_ADDRESS,                      \
    .pwr_pin = RB_STHS34PF80_SENSOR_POWER_PIN,                \
    .pwr_on = RI_GPIO_HIGH,                                   \
    .fifo_pin = RI_GPIO_ID_UNUSED,                            \
    .level_pin = RB_INT_STHS34PF80_PIN,                       \
    .i2c_max_speed = RB_STHS34PF80_I2C_MAX_SPD                \
  }
#endif
```

### 3.3 app_sensor.c Modifications

Add sensor context:
```c
#if APP_SENSOR_STHS34PF80_ENABLED
static rt_sensor_ctx_t sths34pf80 = APP_SENSOR_STHS34PF80_DEFAULT_CFG;
#endif
```

Update `m_sensors_init()`:
```c
#if APP_SENSOR_STHS34PF80_ENABLED
    m_sensors[STHS34PF80_INDEX] = &sths34pf80;
#endif
```

---

## Phase 4: Data Format 7/87 Implementation

### 4.1 Overview

Based on [ADR-dataformat-motion-sensor.md](src/ruuvi.endpoints.c/docs/ADR-dataformat-motion-sensor.md):

- **Format 0x07**: Unencrypted (DIY / Home Assistant)
- **Format 0x87**: AES-128 encrypted (EN 18031-2 compliant)

### 4.2 Payload Structure (Option A - Recommended)

| Offset | Size | Field | Encoding |
|--------|------|-------|----------|
| 0 | 1 | Message Counter | uint8 (0-254) |
| 1 | 1 | State Flags | bitfield |
| 2-3 | 2 | Temperature | int16 (0.005°C resolution) |
| 4-5 | 2 | Humidity | uint16 (0.0025% resolution) |
| 6-7 | 2 | Pressure | uint16 (50000-115534 Pa) |
| 8 | 1 | Tilt X (Pitch) | int8 (±90°) |
| 9 | 1 | Tilt Y (Roll) | int8 (±90°) |
| 10-11 | 2 | Luminosity | uint16 (0-65534 lux) |
| 12 | 1 | Color Temperature | uint8 (1000-7650K) |
| 13 | 1 | Battery + Motion | packed byte |
| 14 | 1 | Motion Count | uint8 (0-254) |
| 15 | 1 | CRC8 | CRC8-CCITT |

### 4.3 State Flags Byte

| Bit | Name | Description |
|-----|------|-------------|
| 0 | MOTION_NOW | Motion currently detected |
| 1 | PRESENCE | Presence/occupancy detected |
| 2-7 | Reserved | Future use |

### 4.4 New Files

#### `src/ruuvi_endpoint_7.h`

```c
#ifndef RUUVI_ENDPOINT_7_H
#define RUUVI_ENDPOINT_7_H

#include "ruuvi_endpoints.h"

#define RE_7_DESTINATION          (0x07U)
#define RE_87_DESTINATION         (0x87U)  // Encrypted variant
#define RE_7_DATA_LENGTH          (22U)    // Manufacturer data length
#define RE_7_PAYLOAD_LENGTH       (16U)    // Payload (encrypted block)

// State flags
#define RE_7_FLAG_MOTION_NOW      (1U << 0)
#define RE_7_FLAG_PRESENCE        (1U << 1)

/** @brief All data required for Ruuvi dataformat 7 package. */
typedef struct
{
    re_float humidity_rh;
    re_float pressure_pa;
    re_float temperature_c;
    re_float tilt_x_deg;
    re_float tilt_y_deg;
    re_float luminosity_lux;
    re_float color_temp_k;
    re_float battery_v;
    uint8_t message_counter;
    uint8_t motion_count;
    uint8_t motion_intensity;  // 0-15
    bool motion_now;
    bool presence;
    uint64_t address;
} re_7_data_t;

re_status_t re_7_encode(uint8_t *const buffer, const re_7_data_t *data);
re_status_t re_7_decode(const uint8_t *const buffer, re_7_data_t *data);

// Encrypted variant (0x87)
re_status_t re_87_encode(uint8_t *const buffer, const re_7_data_t *data, 
                         const uint8_t *key);
re_status_t re_87_decode(const uint8_t *const buffer, re_7_data_t *data,
                         const uint8_t *key);

#endif // RUUVI_ENDPOINT_7_H
```

### 4.5 Encryption Considerations

- **Algorithm:** AES-128-ECB
- **Key Management:** Device-specific shared secret
- **Block Size:** 16 bytes (exact payload size)
- **Message Counter:** For replay detection

### 4.6 Files to Create/Modify

| File | Changes |
|------|---------|
| `src/ruuvi_endpoint_7.h` | New header |
| `src/ruuvi_endpoint_7.c` | New implementation |
| `src/ruuvi_endpoints.h` | Add RE_7_ENABLED macro |

---

## Phase 5: Testing

### 5.1 Unit Tests (Ceedling)

#### `test/interfaces/environmental/test_ruuvi_interface_sths34pf80.c`

```c
void test_ri_sths34pf80_init_ok(void);
void test_ri_sths34pf80_init_null_sensor(void);
void test_ri_sths34pf80_init_wrong_id(void);
void test_ri_sths34pf80_data_get_presence(void);
void test_ri_sths34pf80_data_get_motion(void);
void test_ri_sths34pf80_samplerate_set(void);
void test_ri_sths34pf80_mode_set_continuous(void);
void test_ri_sths34pf80_mode_set_sleep(void);
```

#### `test/test_ruuvi_endpoint_7.c`

```c
void test_re_7_encode_valid_data(void);
void test_re_7_encode_invalid_values(void);
void test_re_7_decode_valid_buffer(void);
void test_re_7_state_flags(void);
void test_re_87_encrypt_decrypt(void);
void test_re_7_crc8(void);
```

### 5.2 Integration Tests

- Hardware presence detection accuracy
- Power consumption measurement
- I2C communication stability
- Interrupt-driven detection
- Multi-sensor operation (with BME280, LIS2DH12, etc.)

### 5.3 System Tests

- BLE advertisement with format 7/87
- Gateway reception and parsing
- iOS/Android app parsing
- Encryption/decryption in cloud

---

## Dependencies & Prerequisites

### Software
- [ ] STMems STHS34PF80 driver already in `STMems_Standard_C_drivers/sths34pf80_STdC/`
- [ ] Ruuvi I2C interface (`ruuvi_interface_i2c.h`)
- [ ] AES-128 crypto library (for format 0x87)

### Hardware
- [ ] Hardware board with STHS34PF80 mounted
- [ ] I2C connection verified
- [ ] Power supply and enable pin verified

### Documentation
- [ ] STHS34PF80 datasheet
- [ ] Application notes (AN5867, AN5868)
- [ ] ADR for dataformat 7/87

---

## Decisions (Resolved)

### Q1: Data Field Extension Strategy ✅
**Decision:** Add 3 new fields to `rd_sensor_data_bitfield_t`:
- `presence` - Stationary human detection
- `motion` - Movement detection  
- `ir_object` - Dimensionless IR object signal (not calibrated °C, but useful for "stove is on" type detection)

---

### Q2: Interrupt-Driven vs Polled Mode ✅
**Decision:** MVP uses **polling** for consistency with other sensors.

**Note:** Interrupt-driven mode may be requested by marketing for lower power / faster response. Keep architecture flexible for future enhancement.

---

### Q3: Multiple STHS34PF80 Sensors ✅
**Decision:** Single instance only for MVP.

---

### Q4: Threshold Configuration ✅
**Decision:** All thresholds configurable via `app_config.h` (compile-time).

Runtime GATT configuration is out of scope for MVP.

---

### Q5: Encryption Key Management ✅
**Decision:** AES-128 key derived from:
- **64-bit DeviceID** (from nRF FICR registers)
- **8-character password** (ASCII, provides remaining 64 bits)

**Default password:** `"RuuviCom"` (configurable as firmware constant in `app_config.h`)

```c
// app_config.h
#ifndef APP_SENSOR_ENCRYPTION_PASSWORD
#   define APP_SENSOR_ENCRYPTION_PASSWORD "RuuviCom"  // 8 chars = 64 bits
#endif
```

**Key construction:**
```
AES-128 Key = DeviceID[63:0] || PASSWORD[63:0]
            = 0xXXXXXXXXXXXXXXXX || "RuuviCom"
```

---

## Estimated Timeline

| Phase | Description | Duration | Dependencies |
|-------|-------------|----------|--------------|
| **1** | ruuvi.drivers.c integration | 3-5 days | ST driver available |
| **2** | ruuvi.boards.c configuration | 0.5 days | Hardware defined |
| **3** | app_sensor integration | 1-2 days | Phase 1 complete |
| **4** | Data format 7/87 | 2-3 days | ADR finalized |
| **5** | Testing | 2-3 days | All phases complete |
| | **Total** | **8-13 days** | |

---

## Next Steps

1. [ ] Review and finalize this plan
2. [ ] Resolve open questions
3. [ ] Create feature branch: `feature/sths34pf80-support`
4. [ ] Begin Phase 1 implementation
5. [ ] Hardware bring-up in parallel

---

## References

- [STHS34PF80 Datasheet](https://www.st.com/resource/en/datasheet/sths34pf80.pdf)
- [ST Driver Source](src/ruuvi.drivers.c/STMems_Standard_C_drivers/sths34pf80_STdC/)
- [ADR: Motion Sensor Data Format](src/ruuvi.endpoints.c/docs/ADR-dataformat-motion-sensor.md)
- [DPS310 Implementation Reference](src/ruuvi.drivers.c/src/interfaces/environmental/ruuvi_interface_dps310.c)
- [Ruuvi Sensor Interface](src/ruuvi.drivers.c/src/ruuvi_driver_sensor.h)
