/*
 * SmartKeys - All Functions Example
 *
 * Demonstrates every available function in the SmartKeys library.
 * Open a text editor (e.g. Notepad) before plugging in the Digispark.
 */

#include <SmartKeys.h>

// Custom escape list (optional - there's a built-in default)
AltEntry myEscapeList[] = {
  {"39",  '\''},
  {"45",  '-'},
  {"47",  '/'},
  {"58",  ':'},
  {"124", '|'},
  {"64",  '@'},
  {"126", '~'}
};

void setup() {
  // --- stableDelay(ms) ---
  // USB-safe delay: calls usbPoll() every ms to keep connection alive.
  // Always use this instead of delay() in your sketches.
  SmartKeys.stableDelay(2000);

  // --- ensureNumLock() ---
  // Reads NumLock LED state from the host OS.
  // If NumLock is OFF, presses NumLock to turn it ON.
  // Required for reliable Alt code entry.
  SmartKeys.ensureNumLock();

  // --- isNumLockOn() ---
  // Returns true if NumLock is currently active.
  // (ensureNumLock calls this internally)

  // --- print(text) ---
  // Types text using US keyboard scancodes.
  // Works for basic ASCII (a-z, A-Z, 0-9, common symbols).
  // Layout-dependent: may produce wrong chars on non-US layouts.
  SmartKeys.print("Hello World");
  SmartKeys.sendKeyStroke(KEY_ENTER);

  // --- typeChar(ascii) ---
  // Types a single ASCII character (same as print, but one char).
  SmartKeys.typeChar('X');
  SmartKeys.sendKeyStroke(KEY_ENTER);

  // --- smartPrint(text) ---
  // Layout-independent printing: characters in the escape list
  // are typed via Alt codes (works on any keyboard layout).
  // All other characters use regular scancodes.
  SmartKeys.smartPrint("path: C:/users | grep -c 'admin'");
  SmartKeys.sendKeyStroke(KEY_ENTER);

  // --- sendAltCode(digits) ---
  // Sends a Windows Alt code directly.
  // Holds Alt, types digits on numpad, releases Alt.
  // Automatically ensures NumLock is ON before sending.
  SmartKeys.sendAltCode("124");  // |
  SmartKeys.sendAltCode("64");   // @
  SmartKeys.sendKeyStroke(KEY_ENTER);

  // --- sendKeyStroke(key) ---
  // Sends a single key press + release. Use KEY_* defines.
  SmartKeys.sendKeyStroke(KEY_ENTER);
  SmartKeys.sendKeyStroke(KEY_TAB);
  SmartKeys.sendKeyStroke(KEY_BACKSPACE);

  // --- sendKeyStroke(key, modifiers) ---
  // Key press + release with modifier(s).
  // Modifiers: MOD_CONTROL_LEFT, MOD_SHIFT_LEFT, MOD_ALT_LEFT, MOD_GUI_LEFT
  //            MOD_CONTROL_RIGHT, MOD_SHIFT_RIGHT, MOD_ALT_RIGHT, MOD_GUI_RIGHT
  // Combine with | (bitwise OR).
  SmartKeys.sendKeyStroke(KEY_ENTER, MOD_SHIFT_LEFT);          // Shift+Enter
  SmartKeys.sendKeyStroke(KEY_ARROW_LEFT, MOD_CONTROL_LEFT);   // Ctrl+Left (word jump)
  SmartKeys.sendKeyStroke(KEY_TAB, MOD_ALT_LEFT);              // Alt+Tab (switch window)

  // --- sendKeyPress(key, modifiers) / sendKeyPress(key) ---
  // Sends key DOWN only (no release). Use for holding keys.
  // Send sendKeyPress(0) to release all keys.
  SmartKeys.sendKeyPress(KEY_ARROW_DOWN);   // hold Down arrow
  SmartKeys.stableDelay(50);
  SmartKeys.sendKeyPress(0);                // release

  // --- Arrows ---
  SmartKeys.sendKeyStroke(KEY_ARROW_UP);
  SmartKeys.sendKeyStroke(KEY_ARROW_DOWN);
  SmartKeys.sendKeyStroke(KEY_ARROW_LEFT);
  SmartKeys.sendKeyStroke(KEY_ARROW_RIGHT);

  // --- Ctrl+A, Ctrl+C, Ctrl+V (select all, copy, paste) ---
  SmartKeys.print("Select me");
  SmartKeys.stableDelay(100);
  SmartKeys.sendKeyStroke(4, MOD_CONTROL_LEFT);   // Ctrl+A (KEY_A = 4)
  SmartKeys.stableDelay(100);
  SmartKeys.sendKeyStroke(6, MOD_CONTROL_LEFT);   // Ctrl+C (KEY_C = 6)
  SmartKeys.stableDelay(100);
  SmartKeys.sendKeyStroke(KEY_ARROW_RIGHT);        // deselect, move to end
  SmartKeys.sendKeyStroke(KEY_ENTER);
  SmartKeys.sendKeyStroke(25, MOD_CONTROL_LEFT);  // Ctrl+V (KEY_V = 25)

  // --- Win+R (Run dialog) ---
  // SmartKeys.sendKeyStroke(21, MOD_GUI_LEFT);    // Win+R (KEY_R = 21)

  // --- setEscapeList(list, count) ---
  // Replace the default escape list with your own.
  // Characters in this list will be typed via Alt codes in smartPrint().
  SmartKeys.setEscapeList(myEscapeList, 7);
  SmartKeys.smartPrint("now @ and ~ also work via alt codes");
  SmartKeys.sendKeyStroke(KEY_ENTER);

  // --- update() ---
  // Manually poll V-USB. stableDelay() calls this internally.
  // Use in tight loops to keep USB alive:
  // while (condition) { SmartKeys.update(); }
}

void loop() {
}
