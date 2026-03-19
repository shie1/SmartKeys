# SmartKeys

> **Note:** Alt code features (`smartPrint`, `sendAltCode`, `ensureNumLock`) are **Windows only**. Basic keyboard functions (`print`, `sendKeyStroke`, `sendKeyPress`) work on any OS.

Layout-independent USB keyboard library for **Digispark** (ATtiny85).  
A standalone replacement for DigiKeyboard with Windows Alt code support and automatic NumLock detection.

## Features

- **Layout-independent typing** — special characters are sent via Windows Alt codes, so they work on any keyboard layout
- **NumLock auto-detection** — reads LED state from the host OS and activates NumLock if needed (via HID LED output report)
- **Full keyboard control** — key press/release, shortcuts (Ctrl+C, Alt+Tab, Win+R...), arrows, function keys
- **Lightweight** — ~2.8 KB flash, 162 bytes RAM on ATtiny85
- **No dependencies** — includes its own V-USB stack, does not depend on DigiKeyboard

## Installation

Copy the `SmartKeys` folder to your Arduino libraries directory:

```
~/Arduino/libraries/SmartKeys/
```

Or clone this repo directly:

```bash
git clone https://github.com/shie1/SmartKeys.git ~/Arduino/libraries/SmartKeys
```

## Quick Start

```cpp
#include <SmartKeys.h>

void setup() {
  SmartKeys.stableDelay(2000);
  SmartKeys.smartPrint("echo 'hello' | grep 'h'");
  SmartKeys.sendKeyStroke(KEY_ENTER);
}

void loop() {}
```

## API Reference

### Typing

| Function | Description |
|---|---|
| `SmartKeys.print("text")` | Type text using US keyboard scancodes |
| `SmartKeys.typeChar('A')` | Type a single ASCII character |
| `SmartKeys.smartPrint("text")` | Layout-independent typing (Alt codes for special chars) |

### Alt Codes

| Function | Description |
|---|---|
| `SmartKeys.sendAltCode("124")` | Send a Windows Alt code (e.g. 124 = `\|`) |
| `SmartKeys.setEscapeList(list, count)` | Set custom Alt code escape list |

Default escape list: `'` `-` `/` `:` `|`

### Keys & Shortcuts

| Function | Description |
|---|---|
| `SmartKeys.sendKeyStroke(KEY_ENTER)` | Press and release a key |
| `SmartKeys.sendKeyStroke(key, MOD_CONTROL_LEFT)` | Shortcut (e.g. Ctrl+C) |
| `SmartKeys.sendKeyPress(key)` | Hold a key down |
| `SmartKeys.sendKeyPress(0)` | Release all keys |

### Modifiers

`MOD_CONTROL_LEFT` `MOD_SHIFT_LEFT` `MOD_ALT_LEFT` `MOD_GUI_LEFT`  
`MOD_CONTROL_RIGHT` `MOD_SHIFT_RIGHT` `MOD_ALT_RIGHT` `MOD_GUI_RIGHT`

Combine with `|`: `SmartKeys.sendKeyStroke(key, MOD_CONTROL_LEFT | MOD_SHIFT_LEFT)`

### Key Constants

`KEY_ENTER` `KEY_ESCAPE` `KEY_BACKSPACE` `KEY_TAB` `KEY_SPACE` `KEY_DELETE`  
`KEY_ARROW_UP` `KEY_ARROW_DOWN` `KEY_ARROW_LEFT` `KEY_ARROW_RIGHT`  
`KEY_F1`–`KEY_F12` `KEY_CAPSLOCK` `KEY_NUMLOCK` `KEY_PRINTSCREEN`

### NumLock & Utility

| Function | Description |
|---|---|
| `SmartKeys.ensureNumLock()` | Turn on NumLock if it's off |
| `SmartKeys.isNumLockOn()` | Check NumLock state |
| `SmartKeys.stableDelay(ms)` | USB-safe delay (keeps connection alive) |
| `SmartKeys.update()` | Manual USB poll |

## Custom Escape List

```cpp
AltEntry myList[] = {
  {"124", '|'},
  {"64",  '@'},
  {"126", '~'}
};

SmartKeys.setEscapeList(myList, 3);
SmartKeys.smartPrint("user@host ~/path");
```

## License

V-USB components are licensed under GNU GPL v2. See [License.txt](https://github.com/obdev/v-usb/blob/master/License.txt) for details.
