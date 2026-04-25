# Init Main Branch

clone this repo with the submodules

```
git clone --recursive https://github.com/bokumentation/STASRG-SulfurMonitoring-Hardware.git
```

# ghostl + ESP32-S3: The `-mtext-section-literals` Fix

## The Problem

Building ESP-IDF projects that use `ghostl` (via `espsoftwareserial`) for **ESP32-S3** fails with:

```
dangerous relocation: l32r: literal placed after use: .literal._ZNK8delegate...
```

This error **does not** occur on:
- **ESP32** (Xtensa LX6)
- **ESP32-C3** (RISC-V)

And it **does not** occur when building the same code with **Arduino IDE** (arduino-esp32).

## Root Cause

### The `l32r` instruction

The Xtensa architecture has a `l32r` (load literal from PC-relative) instruction used to load 32-bit constants. It loads a value from a **literal pool** — a table of constants placed at a known offset from the current PC. The range of `l32r` is approximately ±256 KB from the instruction.

### IRAM vs Flash on ESP32-S3

Functions marked with `IRAM_ATTR` (as many are in `Delegate.h` and `SoftwareSerial`) are placed in **IRAM** (internal RAM, address range 0x4037xxxx). The normal code (.text) is placed in **flash** (mapped to 0x4200xxxx).

Without `-mtext-section-literals`:
- The compiler places all literal pools in a **centralized .literal section** that lives alongside the flash code (0x4200xxxx)
- Functions in IRAM emit `l32r` instructions pointing to the .literal section in flash
- The distance between IRAM (0x4037xxxx) and the literal pool in flash (0x4200xxxx) exceeds the `l32r` addressing range
- The linker rejects this as a **dangerous relocation**

### Why ESP32 (LX6) is fine

ESP32 uses Xtensa LX6 which defaults to **`-mtext-section-literals` being enabled** in ESP-IDF. So the literal pool is placed inside each text section — no cross-section addressing issue.

### Why ESP32-C3 (RISC-V) is fine

ESP32-C3 is RISC-V, not Xtensa. It has no `l32r` instruction — it uses a different instruction encoding for loading constants. The issue is Xtensa-specific.

### Why Arduino IDE is fine

Arduino IDE (arduino-esp32) sets compile flags differently than ESP-IDF. In particular, it passes `-mtext-section-literals` by default in its platform configuration, avoiding the problem entirely. This is why the same source code that fails in ESP-IDF works in Arduino without any modifications.

## The Fix: `-mtext-section-literals`

Add this to the `CMakeLists.txt` of any component that depends on `ghostl` (e.g. `espsoftwareserial`):

```cmake
target_compile_options(${COMPONENT_LIB} PRIVATE -mtext-section-literals)
```

### How it works

The `-mtext-section-literals` flag tells the Xtensa compiler to place each function's literal pool **inside that function's own text section**, rather than in a centralized `.literal` section. Since the function is in IRAM, its literals go to IRAM too — well within `l32r` range.

### Is it safe?

**Yes, it is completely safe:**

| Concern | Assessment |
|---|---|
| Runtime behavior | No functional change. Literals are read-only constants — being in IRAM vs flash makes no difference to program correctness. |
| Performance | No impact. Constants are read the same way via cache at full speed. |
| IRAM / ISR usage | Preserved. All `IRAM_ATTR` functions continue to work correctly in interrupt context. |
| Binary size | Minor increase (typically < 1 KB). Literal pools may be duplicated across functions instead of shared, but this is negligible. |
| Espressif recommendation | Standard practice. Used by default on ESP32 and in Arduino-esp32. |

## The Previous Attempt: Removing `IRAM_ATTR`

### What we tried

We removed `IRAM_ATTR` from the `operator()` methods in `DelegatePImpl` and `DelegateImpl` classes (the `std::function`/`FUNC` path) in `Delegate.h`.

### Why it worked

Without `IRAM_ATTR`, those functions are placed in flash (0x4200xxxx), right next to the centralized `.literal` section. The `l32r` instruction can reach the literal pool fine — same section, same address space.

### Why we reverted it

Removing `IRAM_ATTR` means those functions **cannot be called from ISR context** — a fundamental requirement for software serial implementations where UART callbacks happen in interrupt context. The `FP` and `FPA` dispatch paths (raw function pointers) retained `IRAM_ATTR`, but the `std::function` path lost ISR safety.

### The better solution

Instead of modifying source code (and compromising ISR safety), we use `-mtext-section-literals` which requires **zero source changes**. All dispatch paths keep their `IRAM_ATTR`, all functions stay ISR-safe, and the linker error is resolved at the compiler flag level.

## Summary

| Build Environment | Default `-mtext-section-literals` | Builds? |
|---|---|---|
| ESP-IDF + ESP32 | ✅ Yes (default) | ✅ |
| ESP-IDF + ESP32-C3 | N/A (RISC-V, no l32r) | ✅ |
| ESP-IDF + ESP32-S3 | ❌ No (default) | ❌ Fails |
| Arduino IDE + ESP32-S3 | ✅ Yes (in platform config) | ✅ |

The fix: add `-mtext-section-literals` to the consumer component's CMakeLists.txt. No source code changes needed.