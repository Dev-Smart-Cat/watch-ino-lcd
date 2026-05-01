# watch-ino-lcd

An Arduino-based digital watch with a 16x2 LCD display, featuring a real-time clock and a built-in stopwatch accessible via a scrollable second screen. The project includes EEPROM persistence to recover the watch and stopwatch state after a power interruption.

---

## Hardware

- **Arduino Nano** (Uno or compatible)
- **16x2 LiquidCrystal LCD** — connected via pins RS=12, EN=11, D4=5, D5=4, D6=3, D7=2
- **Potentiometer** — connected to the LCD contrast pin (V0) to adjust display contrast
- **Jumper wires**
- **5 push buttons** with `INPUT_PULLUP`:

| Button | Pin | Function |
|--------|-----|----------|
| `btnSetMode` | 8 | Toggle set mode / cycle stopwatch states |
| `btnMin` | 7 | Increment minutes (set mode only) |
| `btnHour` | 6 | Increment hours (set mode only) |
| `btnHome` | 9 | Return to watch screen |
| `btnRight` | 10 | Navigate to stopwatch screen |

---

## Screens

**Watch screen (main)** — displays `HH:MM:SS` counting up to 23:59:59 then resetting.

**Stopwatch screen** — accessed by pressing `btnRight`, which scrolls the LCD to reveal `MM:SS:hs` (minutes, seconds, hundredths of seconds) with 10ms resolution.

---

## Features

### Set Mode
Pressing `btnSetMode` on the watch screen toggles set mode on and off:
- **On:** time stops counting, `SET UP` appears on the display, `btnMin` and `btnHour` become active to adjust the time
- **Off:** `SET UP` is cleared, time resumes counting

### Stopwatch
Pressing `btnSetMode` on the stopwatch screen cycles through 3 states:

```
idle (0) → running (1) → stopped (2) → reset back to idle (0)
```

### Seconds Progress Bar
The second row of the LCD display on the watch screen works as a visual progress bar that fills up as the seconds advance.

- Each of the 16 LCD columns represents a portion of 60 seconds
- A cell is filled completely using character `0xFF` (solid block from the HD44780 CGROM) based on a direct proportion:

```
filledCells = (seconds × 16) / 59
```

- At 0 seconds: the bar is empty
- At 59 seconds: the bar is completely filled (16 blocks)
- Each block represents approximately **3.68 seconds**
- When seconds reset to 0, the bar clears automatically

---

## The Battery Problem

This project uses a **low-cost battery holder** that came with a hardware defect: when the watch is moved abruptly, the battery briefly loses contact with the terminals, cutting the power supply and resetting the Arduino. As a result, the time and stopwatch state were lost on every accidental power interruption.

### How it was solved

The solution was to use the Arduino's built-in **EEPROM memory** to save the watch and stopwatch state periodically. This way, when the power is cut and the Arduino restarts, it reads the last saved state from EEPROM and resumes from where it left off — instead of resetting to zero.

To keep the time accurate even after a reboot, a time compensation technique is used: the saved time is combined with the internal Arduino timer, so the watch recovers not just the last saved minute, but also accounts for the small amount of time that passes during the restart itself.

The stopwatch follows the same approach — its elapsed time and state (running or stopped) are also saved, so after a power cut it either resumes counting or displays the paused time exactly as it was.

> The full technical details — including the variables, EEPROM addresses, and time compensation logic — are documented in the inline comments throughout the source code in `test/ee-w-sw.cpp` and `src/main.cpp`.

---

### EEPROM Persistence
The watch and stopwatch states are saved to the Arduino's internal EEPROM (1KB, addresses 0–10) and restored on every reboot.

The following states are persisted:

| State | EEPROM Address | Type |
|-------|---------------|------|
| Watch time (`wSavedTime`) | 0 | `unsigned long` (4 bytes) |
| Set mode (`setMode`) | 4 | `bool` (1 byte) |
| Stopwatch time (`swSavedTime`) | 5 | `unsigned long` (4 bytes) |
| Stopwatch screen (`swScreen`) | 9 | `bool` (1 byte) |
| Stopwatch state (`swState`) | 10 | `byte` (1 byte) |

To minimize EEPROM wear (rated at 100,000 write cycles per address), the watch time is saved only once per minute.


## Project Structure

```
src/main.cpp          — main source, uploaded to Arduino via Arduino IDE
include/watch.cpp     — same functions separated following good development practices
test/stopwatch.cpp    — prototype with stopwatch feature
test/add-eeprom.cpp   — prototype testing EEPROM persistence
```

> **Note:** PlatformIO has upload errors to the Arduino board, so the code is currently loaded via the Arduino IDE directly, but keeping the project structure best practices through PlatformIO.

---

## Demo

**Before the fix:**
[Watch the demo with the defect](https://vimeo.com/1188450962?share=copy&fl=sv&fe=ci)

**After the fix:**
[Watch the demo with the defect resolved](https://vimeo.com/1188451662?share=copy&fl=sv&fe=ci)

---

## Button Wiring

All buttons are wired with `INPUT_PULLUP`, meaning:
- `HIGH` (1) = button **not pressed**
- `LOW` (0) = button **pressed**

Debounce is handled in software with a 50ms delay inside the `readButton()` function, which also tracks the previous state to detect only the moment the button transitions from HIGH to LOW, preventing multiple triggers from a single press.

---

## License

This project is licensed under the [MIT License](LICENSE).
