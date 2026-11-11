# Programming a PIC32CK — practical guide

How to build, flash, verify and debug the PIC32CK firmware in this repository.

The PIC32CK SG01 is a **Cortex-M33 with TrustZone-M**, and that single fact
changes almost everything about how it is built and programmed compared with the
SAM E51. If you read only one section, read
[The one thing you must understand](#the-one-thing-you-must-understand) and
[Getting the unified hex](#getting-the-unified-hex).

Target device: **PIC32CK2051SG01064**
Board: **PIC32CK SG01 Curiosity Nano** (EV33V87A), usually on a
**Curiosity Nano Base for Click boards** (AC164162)

---

## Contents

1. [The one thing you must understand](#the-one-thing-you-must-understand)
2. [Memory map](#memory-map)
3. [The projects in this repository](#the-projects-in-this-repository)
4. [Prerequisites](#prerequisites)
5. [Building](#building)
6. [Getting the unified hex](#getting-the-unified-hex)
7. [Flashing](#flashing)
8. [Verifying it works](#verifying-it-works)
9. [Debugging](#debugging)
10. [Traps — read before you lose a day](#traps--read-before-you-lose-a-day)
11. [Board pin reference](#board-pin-reference)
12. [Known limitations](#known-limitations)

---

## The one thing you must understand

**A working PIC32CK image is two programs, not one, and they must be flashed
together in a single operation.**

Because of TrustZone, the device runs a *secure* application first, which then
hands control to the *non-secure* application. This repository therefore has:

| Image | Project | What it contains |
|---|---|---|
| **Secure** | `pic32ck_secure` | BootROM vectors, all configuration/fuse pages, the secure trampoline, the NSC veneers |
| **Non-secure** | `demo_pic32ck`, `smart_sensor_pic32ck`, `smart_io_pic32ck`, `smart_hmi_pic32ck` | The actual application — everything you normally think of as "the firmware" |

Boot order:

```
BootROM  →  BOOTCFG / BLDRCFG fuses  →  Boot Flash 0x08000000
         →  secure application 0x0c000000
         →  non-secure application 0x0c100000   (TZ_START_NS)
```

### Why they must be flashed together

**Every `Program` operation performs a full chip erase first.** So if you flash
the secure image and then the non-secure image, the second operation erases the
first. You end up with:

- no valid boot configuration (the fuse pages read back as zero after erase), and
- a device that does nothing at all on reset — no LED, no UART, no sign of life.

This is not a subtle failure. It looks exactly like "my firmware is broken", and
it has cost real debugging time on this project.

**The rule: only ever program `<project>-unified.hex`.** Never program
`demo_pic32ck.hex` or `pic32ck_secure.hex` on their own.

The secure image is *shared*: all four application projects declare
`pic32ck_secure` as a loadable project and merge against the same secure hex.
You do not need a different secure build per application.

---

## Memory map

| Region | Origin | Size | Contents |
|---|---|---|---|
| `boot_rom` | `0x08000000` | 128 KiB | BootROM vectors |
| Configuration / fuses | `0x0a000000` | — | `IDAU_ANS`, `IDAU_RNS`, `BROM_BSEQ` sequence numbers, `USERCFG1`/`USERCFG2` panels |
| CAL OTP | `0x0a007000` | — | Factory calibration. **Secure-only, always** — see [the ADC trap](#3-the-adc-cannot-be-used-from-non-secure-code) |
| `rom` (secure) | `0x0c000000` | 1 MiB | Secure application |
| `rom_nsc` | `0x0c0fffc0` | 64 B | Non-secure-callable veneers |
| `rom` (non-secure) | `0x0c100000` | 1 MiB | Application — `TZ_START_NS` |
| `ram` (non-secure) | `0x20040000` | 256 KiB | Application RAM |

Linker sizes come from `-DANS_SIZE=0x100000` (non-secure flash) and
`-DRNS_SIZE=0x40000` (non-secure RAM).

A successful programming run reports exactly these ranges — use it as a sanity
check:

```
program memory: start address = 0x8000000,  end address = 0x8000fff
program memory: start address = 0xc000000,  end address = 0xc000fff
program memory: start address = 0xc0ff000,  end address = 0xc121fff
configuration memory
Programming complete
Program succeeded.
```

If you do **not** see the `0x8000000` and `0xc000000` ranges, you are programming
the application hex alone, not the unified image.

---

## The projects in this repository

| MPLAB project | PIC32CK configuration | Board variant | IP on the T1S network |
|---|---|---|---|
| `AIoT_PIC32CK_Secure` | `pic32ck_secure` | shared secure trampoline | — |
| `AIoT_Demo` | `demo_pic32ck` | Demo | 192.168.0.104 |
| `AIoT_Smart_Sensor` | `smart_sensor_pic32ck` | Smart Sensor | 192.168.0.100 |
| `AIoT_Smart_HMI` | `smart_hmi_pic32ck` | Smart HMI | 192.168.0.101 |
| `AIoT_Smart_IO` | `smart_io_pic32ck` | Smart I/O | 192.168.0.102 |

**Each application project holds two configurations, one per device.** Switching
a board between PIC32CK and SAM E51 is a configuration change, not a different
project: `AIoT_Demo` offers `demo_pic32ck` and `demo_same51`, `AIoT_Smart_HMI`
offers `smart_hmi_pic32ck` and `smart_hmi_same51`, and so on. The PIC32CK
configuration is listed first in each manifest, so it is the one selected by
default.

The two configurations share nothing but the project name. Each has its own
device, its own file set (so the PIC32CK configuration never sees
`harmony/AIoT_SAME51/` and vice versa), its own toolchain property group, and
its own `cmake/<project>/<configuration>/` directory — which is also why only
the PIC32CK configurations have a `user.cmake`. The secure project stays
separate, because all four applications share it.

Only the PIC32CK configuration declares the loadable secure project, so only it
produces a `-unified.hex`. A SAM E51 configuration produces a plain
`<configuration>.hex`, which is the correct and complete image for that device —
the unified-hex rule below is a TrustZone matter and does not apply to it.

All four applications share **one** Harmony/MCC configuration tree at
`harmony/AIoT_PIC32CK/`, with `secure/` and `non_secure/` subtrees. Board
differences live in `firmware/src/hal/<board>_pic32ck/`, and code common to all
PIC32CK boards lives in `firmware/src/hal/common_pic32ck/`.

The IP address is derived from `BOARD_TYPE` in the board's `hal.h`, via the table
in `firmware/src/board_types.h`.

---

## Prerequisites

| Tool | Version used | Notes |
|---|---|---|
| MPLAB X | v6.30 | Needed for `mdb` and IPE even if you work in VS Code |
| XC32 compiler | v5.00 | |
| MPLAB extension for VS Code | current | Owns all CMake generation |
| `PIC32CK-SG_DFP` device pack | 1.8.265 | |
| CMSIS pack | 6.3.0 | |
| CMake | 3.x | |
| Ninja | 1.13.2 | Ships with the MPLAB extension |
| `hexmate` | 3.00 | Ships with the MPLAB extension — **not on your PATH** |
| Debugger | PKOB nano (on-board) | |

The MPLAB extension keeps its own tool copies under:

```
%USERPROFILE%/.mplab/app-finder/apps/<tool>/<version>/...
```

---

## Building

### Order matters the first time

The secure project must have been **configured and built at least once** before
an application can be built, because the application consumes two files the
secure build writes to disk — the hex that the merge step reads, and the CMSE
import library that the application's link line references.

1. Build `AIoT_PIC32CK_Secure : pic32ck_secure`
2. Build your application, e.g. `AIoT_Demo : demo_pic32ck`

After that, `user.cmake` in each application makes the merge depend on a fresh
secure build, so step 1 happens automatically on subsequent builds.

### The CMSE import library

The secure build publishes two artifacts, not one:

| Artifact | Consumed by | How |
|---|---|---|
| `out/AIoT_PIC32CK_Secure/pic32ck_secure.hex` | the merge step | read from disk by `hexmate` |
| `out/AIoT_PIC32CK_Secure/pic32ck_secure_sg_veneer.lib` | the application **link** | put on the link line by `user.cmake` |

Despite the extension the `.lib` is not an archive — it is a small
`elf32-littlearm` object carrying one absolute symbol per non-secure-callable
entry point, at that entry's veneer address in `rom_nsc`. Today there is one:

```
0c0fffc0 A secure_add
```

Compiling the secure image with `-mcmse` is what places the veneer in
`.gnu.sgstubs` and renames the real body to `__acle_se_secure_add`. The veneer
itself is 8 bytes — a secure gateway followed by a branch:

```
0c0fffc0 <secure_add>:
 c0fffc0:  e97f e97f   sg
 c0fffc4:  f700 baa9   b.w   c00051a <__acle_se_secure_add>
```

But `-mcmse` alone does not write the import library, and without it the
non-secure side has nothing to resolve the call against. Both halves of that
wiring live in `user.cmake`:

- **producing side** — `cmake/AIoT_PIC32CK_Secure/pic32ck_secure/user.cmake`
  adds `--cmse-implib --out-implib=<abs path>` to the secure link
- **consuming side** — each application's `user.cmake` puts the resulting `.lib`
  on its own link line, by absolute path

Both derive that path from the repository root, so neither depends on the
linker's working directory. See [trap 2](#2-mcc-regeneration-silently-breaks-the-build)
for why this is not done in the manifest.

To add a veneer, define the function `__attribute__((cmse_nonsecure_entry))` in
`secure/src/trustZone/nonsecure_entry.c`, prototype it on the non-secure side,
and rebuild the secure project first. Nothing else needs changing:
`ANSC_SIZE=0x40` gives 64 B of `rom_nsc`, which is eight 8-byte slots, and one
is in use.

### In VS Code / MPLAB

Use the MPLAB build task for the project. This is the path of least resistance,
because the extension puts `hexmate` on the PATH of builds it launches.

### From a terminal

```bash
cmake --build _build/AIoT_Demo/demo_pic32ck
```

This will compile and link successfully and then **fail on the last step** with:

```
'hexmate' is not recognized as an internal or external command
```

See the next section.

---

## Getting the unified hex

This is the question everyone hits. If your build produced only
`demo_pic32ck.elf` and `demo_pic32ck.hex` but no `demo_pic32ck-unified.hex`,
one of the following is true.

### Cause 1 — `hexmate` is not on the PATH (most common)

The generated merge target invokes `hexmate` **by bare name**. The MPLAB
extension resolves that from its own tool directory; a plain terminal cannot.
Everything compiles and links, then the merge fails — and if you used
`--clean-first`, the previous unified image has already been deleted.

**Fix A — put hexmate on the PATH for the build:**

```bash
export PATH="$HOME/.mplab/app-finder/apps/hexmate/v3.00/dist/win/bin:$PATH"
cmake --build _build/AIoT_Demo/demo_pic32ck
```

`user.cmake` prints this exact command, with the correct path for your machine,
as a warning at configure time.

**Fix B — merge by hand.** The merge is just a concatenation of two hex files:

```bash
<hexmate> out/AIoT_PIC32CK_Secure/pic32ck_secure.hex \
          out/AIoT_Demo/demo_pic32ck.hex \
          -Oout/AIoT_Demo/demo_pic32ck-unified.hex
```

where `<hexmate>` is:

```
%USERPROFILE%/.mplab/app-finder/apps/hexmate/v3.00/dist/win/bin/hexmate.exe
```

Argument order does not matter for correctness (the regions do not overlap), but
`-O` must be attached to the output path with no space.

**Fix C — build through the MPLAB task**, which sets the PATH for you.

### Cause 2 — the secure hex does not exist yet

The merge reads `out/AIoT_PIC32CK_Secure/pic32ck_secure.hex` from disk. If the
secure project has never been built, there is nothing to merge. `user.cmake`
warns about this too. Build the secure project once.

### Cause 3 — the merge target was never generated

The merge target only exists because the application's `.mplab.json` declares
`pic32ck_secure` as a **loadable project**:

```json
"include": ".vscode/AIoT_PIC32CK_Secure.mplab.json",
"type": "project",
"projectType": "loadable",
"dependentConfigurationName": "pic32ck_secure"
```

If you create a new PIC32CK application project, you must add this, or you will
silently get an application-only hex that bricks the board when flashed.

### Verifying you have the right file

**Do not judge by file size.** Counter-intuitively the unified hex is *smaller*
than the application-only hex (~397 KB versus ~408 KB), because `hexmate`
recompacts the records while merging.

The reliable offline check is which memory regions the hex actually covers. Intel
hex marks these with extended-linear-address records, so:

```bash
grep -oE "^:02000004[0-9A-Fa-f]{4}" out/AIoT_Demo/demo_pic32ck-unified.hex | sort -u
```

A correct **unified** image lists the boot, configuration and secure regions as
well as the application:

```
:020000040800     <- 0x08000000  boot ROM
:020000040A00     <- 0x0A000000  configuration / fuses
:020000040C00     <- 0x0C000000  secure application
:020000040C0F     <- 0x0C0FFFC0  NSC veneers
:020000040C10 ... <- 0x0C100000+ non-secure application
```

An **application-only** hex lists only `0000`, `0c10`, `0c11`, `0c12` — no
`0800`, no `0A00`, no `0C00`. If you see that, you are about to brick the board.

The same thing is visible when programming: the output must list the `0x8000000`
and `0xc000000` ranges (see [Memory map](#memory-map)).

---

## Flashing

### From the command line with `mdb`

`mdb` is MPLAB's command-line debugger. Drive it on stdin:

```bash
{
  echo "device PIC32CK2051SG01064"; sleep 2
  echo "hwtool pkobnano -p 0";      sleep 10
  echo "program out/AIoT_Demo/demo_pic32ck-unified.hex"; sleep 100
  echo "quit";                      sleep 4
} | "<MPLABX>/mplab_platform/bin/mdb.bat"
```

where `<MPLABX>` is e.g. `C:/Program Files/Microchip/MPLABX/v6.30`.

Always confirm `Program succeeded.` before drawing any conclusion about firmware
behaviour.

**After programming, the CPU is halted.** To run the firmware, either power-cycle
the board (unplug/replug USB) or issue `Reset` then `continue` in the same `mdb`
session. Note that `quit` halts the CPU again, so a session ending in `quit`
leaves the board stopped.

**Better: end the session by closing stdin instead of sending `quit`.** A session
that finishes `Reset` / `continue` and then simply reaches end-of-input exits
without halting — verified, no `Halting...` / `Target Halted` in the log — so the
board is left running. That gives a flash-and-run from one command with no
power-cycle:

```bash
{
  echo "device PIC32CK2051SG01064"; sleep 4
  echo "hwtool pkobnano -p 0";      sleep 16
  echo "program out/AIoT_Demo/demo_pic32ck-unified.hex"; sleep 125
  echo "Reset";                     sleep 6
  echo "continue";                  sleep 10
} | "<MPLABX>/mplab_platform/bin/mdb.bat"
```

`ipecmd` is not an alternative here: it has a `-OL` "release from reset" option,
but its tool list (`RICE`, `ICD3/4/5`, `PK3/4/5`, `SNAP`, `PKOB`, `PKOB4`, `J32`)
has no code for the Curiosity Nano's on-board PKOB nano / nEDBG.

### From MPLAB IPE or the IDE

Program the file `out/<project>/<config>-unified.hex`. Do not let the IDE
"program the project output" if that resolves to the application hex alone.

### Sanity check

The generous ~2 minute case: on a **bare** Curiosity Nano with no Click boards,
boot legitimately takes 110–140 s (see
[trap 7](#7-a-bare-board-boots-very-slowly)). With Click boards fitted it is
1–2 seconds.

---

## Verifying it works

The firmware's sign of life is **the terminal, not the LED.**

Connect to the Curiosity Nano's USB CDC port at **115200 / 8 / N / 1**.

> **Open the terminal *before* resetting the board.** The banner is printed once
> at startup and never again. A capture started after boot shows nothing, which
> looks identical to a dead board.

If you script the capture rather than using a terminal emulator, **assert DTR and
RTS on the port**. A raw port opened without them captured nothing across a full
boot; the same script with `DtrEnable`/`RtsEnable` set captured the complete
banner. A terminal emulator does this for you, so the failure only shows up in
scripts — and looks exactly like a dead board.

The newline requirement below applies to scripted input too: sending `w` or
`w\r` produces **no reply at all**. It must be `w\n`.

Expected output:

```
Sensor Demo with Demo Board
Version <date> - <time>
Curiosity Nano Board > 1[COMM Module] 2[Weather Click/OLED] 3[HVAC Click]

Initializing COMM module...
LwIP-Init [MAC=..., IP='192.168.0.104', ChipRev=2, PLCA-NodeId=...]
T1S Module found!
T1S LWIP Demo with Demo Board
...
Program Running:
Total registered topics: <n>
MQTT Connected
```

Interactive commands — **each must be terminated with a newline**, because
`terminal.c` only dispatches a command when it sees `'\n'`:

| Key | Effect |
|---|---|
| `w` | Print sensor readings |
| `v` | Toggle CSV output — prints exactly what is published over MQTT. The best way to check topic values without a broker |
| `c` | Redraw the header |
| `d` | Debug mode |
| `b` | Bridge mode to the COMM module |

### Why the LED stays dark

There is **no boot heartbeat**. The only two places that drive LED0 are the MQTT
`curiosity/led0` subscription callback and `blink_online_led()`, and the latter
returns immediately unless MQTT is connected. A dark LED0 is correct behaviour on
both PIC32CK and SAM E51 until the T1S link is up and a broker is reachable.

### If T1S / MQTT is not available

`T1S LWIP Demo` in the banner becomes `Stand-alone Sensor Demo`, and
`Unsupported MAC-PHY hardware found` is printed. That is the correct message when
no 10BASE-T1S Click board is fitted, not a defect.

---

## Debugging

`mdb` on this device has several sharp edges. These are all confirmed
behaviours, not guesses.

| Behaviour | What to do |
|---|---|
| `run`/`halt`/`continue` report nothing, `x` says "target is not ready for debugging" | **Issue `Reset` before `halt`/`stepi`/`continue`.** Stop reports only appear after a reset. |
| Multiple breakpoints silently do not report | **Use exactly one breakpoint per `mdb` session.** With several armed, stops go unreported even though execution demonstrably passes them. This produced two false "the firmware is hung" diagnoses on this project. |
| `x` returns `00000000` for RAM *and* peripheral space | Unusable on this target, even after a clean breakpoint halt. Read state out over the UART instead. |
| `print <symbol>` fails for file-static variables | Expected — they have no external linkage. |
| `break *addr <passCount>` returns "Breakpoint -1" | The pass-count form does not register. Use a plain breakpoint and repeated `continue`. |
| `quit` while running | Forces a halt and prints the PC. Useful as a crude sampling profiler. |

Symbol addresses change on every build — always re-derive them from
`out/<project>/<config>.lss` rather than reusing noted values.

Occasionally the PKOB nano drops off USB mid-programming
(`RDDI_COMMUNICATION_ERROR2`), which leaves the device **erased**. It
re-enumerates by itself; simply reprogram, and confirm `Program succeeded.`

---

## Traps — read before you lose a day

### 1. Programming either image alone bricks the board

Covered above. It is the single most expensive mistake available here.

### 2. MCC regeneration silently breaks the build

The MPLAB CMake generator re-emits three things for a TrustZone non-secure
project **every time it runs, regardless of what the `.mplab.json` manifest
says**:

1. Harmony's generated `non_secure/src/main.c` stub is added to the sources,
   colliding with `firmware/src/main.c` → `multiple definition of 'main'`
2. an `sg_veneer.lib` entry is added to the link libraries — a phantom
   `-l:<workspace>_sg_veneer.lib` that nothing produces, so the link fails with
   "cannot find"
3. `--defsym=_min_heap_size=512` overrides the 32456 the manifest asks for

It also passes `VECTOR_REGION` twice with conflicting values.

All four are corrected in each project's **`user.cmake`**, which is the only
hand-authored file under `cmake/` and is included *after* the generated targets
exist. Each correction logs what it did and is a no-op if the generator stops
emitting the problem.

Correction 2 also **adds the real import library back**, by absolute path — see
[the CMSE import library](#the-cmse-import-library).

**Do not try to fix these in the manifest — it does not hold.** Restoring the
manifest to a known-good state and regenerating reproduces all three. Two
further manifest failure modes, both observed while wiring up the import
library, are worth knowing before you try:

- `linker.input-libraries` expanded `.a` entries to a path relative to the
  project source directory, but passed a `.lib` entry through **verbatim** as a
  bare relative path, which `ld` then resolved against the ninja build
  directory. The link fails on a path that looks perfectly correct in the
  manifest.
- `linker.appendMe` never reached `.generated/rule.cmake` for the secure project
  at all. Manifest → CMake generation is done by the **VS Code extension**, not
  by CMake, so `cmake --build` rebuilds without regenerating — and the extension
  was seen to regenerate the four application projects within seconds of each
  manifest edit, yet never the secure one, even across an explicit build of it.
  The flags silently never appeared on the link line while everything looked
  configured.

The second is the more dangerous of the two, because nothing fails: you get a
correct-looking manifest and a link line that ignores it.

### 3. The ADC cannot be used from non-secure code

`ADC_Initialize()` copies the factory calibration word `FUSES_FCCFG65` from CAL
OTP at `0x0a007184`. The whole `0x0A000000` configuration/OTP space is
**secure-only by IDAU attribution, and there is no `NONSEC*` fuse that opens
it.** A non-secure read raises a BusFault which — with `AIRCR.BFHFNMINS = 0`, the
Cortex-M33 reset default — escalates into the **secure** `HardFault_Handler` and
spins.

Symptom: the board programs fine and then shows no sign of life whatsoever,
because the hang happens before the application's superloop.

The ADC component is therefore **removed** from the PIC32CK configuration, and
`ADC1_ENABLE()` is a documented no-op on every board. To use the ADC you must
bring it up in the **secure** application and expose it across the NSC veneer —
the plumbing for that already works, see
[the CMSE import library](#the-cmse-import-library).
One veneer entry point covering channel-select / start / poll / result is enough,
and keeping the poll on the secure side is also where Harmony's two *unbounded*
spin-waits (`ADC_SYNCBUSY`, `CTLINTFLAG.CRDY`) can be bounded.

### 4. Fault handling defaults to the secure world

With `AIRCR.BFHFNMINS = 0`, HardFault / BusFault / NMI target **Secure** state.
So a fault in your non-secure application lands in the *secure* image's handler.
When hunting a hang, set breakpoints on **both** fault handlers — the
non-secure one may never be reached.

### 5. TrustZone peripheral attribution is fuse-controlled

Peripherals are granted to the non-secure world by `#pragma config` in
`secure/src/config/default/initialization.c`
(`FUSES_USERCFG{1,2}_FUCFG1x_NONSEC*`; `SET` = non-secure, `CLEAR` = secure).
PORT and EIC stay `CLEAR` and are granted **per pin / per channel** from secure
code instead:

- `PORT_SEC_REGS->GROUP[n].PORT_NONSEC`
- `EIC_SEC_REGS->EIC_NONSEC`
- interrupts via `NVIC_SetTargetState()`

**All PORT configuration lives in the secure image.** The non-secure
`plib_port.c` writes no PORT registers at all. If a pin does not behave, look in
`secure/.../peripheral/port/plib_port.c`, not in the application.

### 6. Editing generated files — the paired-edit rule

MCC-generated files are regenerated from the store in
`harmony/AIoT_PIC32CK/mcc/`. A change made **only** in a generated file is lost
on the next regeneration; a change made **only** in the store does not affect the
current build.

So apply both: edit the `.yml` (usually `components/core.yml`) *and* the
generated `.c`/`.h`. This has been verified to survive regeneration —
a subsequent MCC run reproduced hand-applied values byte-for-byte.

Note the store keys pins by **pin number**, not name (`PIN_36_*` is PC03).

### 7. A bare board boots very slowly

On a Curiosity Nano with no Click boards, `init_modules()` takes **110–140 s**.
This is not a defect. Each I²C probe to an absent device takes 2–5 s and returns
`SERCOM_I2C_ERROR_BUS` — a *bus* error, not a NAK — because with no Click board
there are no SDA/SCL pull-ups, so the lines never behave like a valid bus. Five
sensor probes account for the whole delay. Fit any Click board and boot drops to
1–2 s.

Related: every sensor HAL busy-waits with an unbounded `while(..._IsBusy());` and
no transfer timeout is configured (`CTRLA.LOWTOUT`/`INACTOUT` unset). A genuinely
stuck bus will hang the superloop rather than degrade. This is shared behaviour,
identical on SAM E51.

### 8. Generated CMake is machine-specific

The generated merge target embeds an **absolute path** to the secure hex. Files
under `cmake/**/.generated/` are `.gitignore`d for this reason — every developer
regenerates them locally. Only `user.cmake` is version-controlled.

### 9. Pins are a shared, finite resource across the four boards

All four applications share one pin configuration, so a pin cannot be an input
for one board and an output for another. Two known conflicts:

- **PA09 / `AN3`** — Smart I/O's relay 2 needs it as a GPIO **output**; the
  RE46C42X smoke detector wants it as an ADC input. Currently a GPIO output.
- **PA00 / `INT3`** — Smart Sensor's RE46C42X chip select wants an **output**;
  Smart HMI's CapSense alert needs a readable **input**. Currently an output,
  which means CapSense touch is inactive (see
  [Known limitations](#known-limitations)).

A subtlety: PIC32CK emits the **full GPIO macro set regardless of direction**, so
these conflicts never show up as compile errors — only as silently dead
functionality. And `<PIN>_InputEnable()` does **not** help: it only clears `DIR`,
it does not set `PINCFG.INEN`, so a pin configured as an output cannot be read
back at run time. Only the pin configuration can fix that.

---

## Board pin reference

PIC32CK SG01 Curiosity Nano, as configured in this repository.

### On-board

| Function | Pin | Notes |
|---|---|---|
| LED0 | PD05 | **Active HIGH** (active LOW on SAM E51 — the board HALs invert `LED_Set`/`LED_Clear` to compensate) |
| SW0 | PD02 | **No EXTINT function** — its function A is NMI, so the button is *polled* from the superloop via `HAL_PollButton()`, not interrupt-driven |
| Debugger CDC UART | PA11 (TX) / PA12 (RX) | SERCOM5 PAD0/PAD1, 115200 8N1 |

### Buses

| Bus | Instance | Pins |
|---|---|---|
| I²C | SERCOM0 | PD06 (SDA) / PD07 (SCL) — these are the mikroBUS I²C pins |
| SPI | SERCOM4 | PB09 (MOSI) / PB10 (SCK) / PC01 (MISO), 10 MHz |
| Terminal USART | SERCOM5 | PA11 / PA12 |
| Click USART | SERCOM1 | PD08 / PD09 |

### Instance renumbering versus SAM E51

| Function | SAM E51 | PIC32CK |
|---|---|---|
| I²C | SERCOM2 | **SERCOM0** |
| SPI | SERCOM1 | **SERCOM4** |
| Click UART (sockets 1, 3) | SERCOM0 | **SERCOM1** |
| Click UART (socket 2) | SERCOM4 | **SERCOM2** |
| Terminal | SERCOM5 | SERCOM5 (unchanged) |

Application code never names a SERCOM directly — it goes through the
`HAL_I2C_*` / `HAL_SPI_*` / `HAL_CLICK_UART_*` aliases in
`firmware/src/hal/common_pic32ck/bus_hal.h`. Use those in new code.

### mikroBUS socket signals

| Signal | Socket 1 | Socket 2 | Socket 3 |
|---|---|---|---|
| CS | PC00 | PB08 | PB06 |
| RST | PB00 | PC02 | PA01 |
| INT | PA07 (EXTINT7) | PC03 | PA00 |
| AN | — | PA05 | PA09 |
| PWM | — | PB01 | PA06 |

> Socket 2's **INT** position is named `EN2` in the code, because the OLED C
> Click uses it as its panel-enable input. That is not a mistake — the click
> drives a TPS61041 boost converter from that pin to generate the display's 15 V
> rail. Get it wrong and the display stays black while SPI works perfectly.

Clock: PLL0 → 120 MHz CPU (GCLK0), 60 MHz peripheral clock (GCLK1). SysTick is
1 ms.

---

## Known limitations

Things that are deliberately incomplete or unverified, so nobody rediscovers them
as bugs.

| Item | Status |
|---|---|
| ADC / RE46C42X smoke detector | **Not available on PIC32CK.** Needs secure-side ADC plus an NSC veneer — see [trap 3](#3-the-adc-cannot-be-used-from-non-secure-code) |
| CapSense touch on `smart_hmi_pic32ck` | **Inactive.** The alert line gates all touch handling and PA00 is an output — see [trap 9](#9-pins-are-a-shared-finite-resource-across-the-four-boards). Fixable by making PA00 a digital input |
| Smart I/O fan I²C, multimeter SPI, relay outputs | Built and linked, **not verified on hardware** — the Click boards were unavailable |
| Smart HMI RNBD/BLE data path | Verified only so far as `RNBD_init()` not blocking; no module was attached |
| Boot time on a bare board | 110–140 s by design — see [trap 7](#7-a-bare-board-boots-very-slowly) |
| CMSE import library | **Verified at build level only.** The veneer, its address, the exported symbol and both link lines all check out, but no code calls a `cmse_nonsecure_entry` function yet, so a secure gateway has never actually been executed on silicon |
| DFP version skew between the two images | The secure project's generated `rule.cmake` pins `PIC32CK-SG_DFP/1.6.199`; the applications build against `1.8.265`. Both link and run, and the secure image only touches SysTick, PM and EVSYS — but it is an inconsistency between two halves of one device image, and worth aligning |

### Verified on hardware

For confidence, the following *are* confirmed on real silicon: boot to the
application superloop with no faults; the terminal in both directions; T1S link
and MQTT connection on all four board identities with their correct IPs; the OLED
C display; BME280 and APC1 sensor readings; and clean rebuilds of all five
projects with zero warnings.

---

## Where to look next

| Topic | Location |
|---|---|
| Full port history and design rationale | Git history of PR #56 "Adding PIC32CK" |
| Build corrections and why they exist | `cmake/<project>/<config>/user.cmake` |
| TrustZone fuse configuration | `harmony/AIoT_PIC32CK/secure/src/config/default/initialization.c` |
| All PORT configuration | `harmony/AIoT_PIC32CK/secure/src/config/default/peripheral/port/plib_port.c` |
| Board-specific differences and their reasons | `firmware/src/hal/<board>_pic32ck/hal.h` |
