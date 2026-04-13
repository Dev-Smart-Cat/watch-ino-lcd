# watch-ino-lcd

An Arduino-based digital watch with a 16x2 LCD display, featuring a real-time clock and a built-in stopwatch accessible via a scrollable second screen.

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

[Watch the demo video on Vimeo](https://vimeo.com/1182733890?share=copy&fl=sv&fe=ci)

---

## Button Wiring

All buttons are wired with `INPUT_PULLUP`, meaning:
- `HIGH` (1) = button **not pressed**
- `LOW` (0) = button **pressed**

Debounce is handled in software with a 50ms delay inside the `readButton()` function, which also tracks the previous state to detect only the moment the button transitions from HIGH to LOW, preventing multiple triggers from a single press.

---

## License

This project is licensed under the [MIT License](LICENSE).
