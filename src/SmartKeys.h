#ifndef __SmartKeys_h__
#define __SmartKeys_h__

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <string.h>

#include "usbdrv.h"
#include "scancode-ascii-table.h"

typedef uint8_t byte;

#define MOD_CONTROL_LEFT    (1<<0)
#define MOD_SHIFT_LEFT      (1<<1)
#define MOD_ALT_LEFT        (1<<2)
#define MOD_GUI_LEFT        (1<<3)
#define MOD_CONTROL_RIGHT   (1<<4)
#define MOD_SHIFT_RIGHT     (1<<5)
#define MOD_ALT_RIGHT       (1<<6)
#define MOD_GUI_RIGHT       (1<<7)

#define KEY_ENTER       40
#define KEY_ESCAPE      41
#define KEY_BACKSPACE   42
#define KEY_TAB         43
#define KEY_SPACE       44
#define KEY_CAPSLOCK    0x39
#define KEY_F1          58
#define KEY_F2          59
#define KEY_F3          60
#define KEY_F4          61
#define KEY_F5          62
#define KEY_F6          63
#define KEY_F7          64
#define KEY_F8          65
#define KEY_F9          66
#define KEY_F10         67
#define KEY_F11         68
#define KEY_F12         69
#define KEY_PRINTSCREEN 0x46
#define KEY_DELETE      0x4C
#define KEY_NUMLOCK     0x53
#define KEY_ARROW_RIGHT 0x4F
#define KEY_ARROW_LEFT  0x50
#define KEY_ARROW_DOWN  0x51
#define KEY_ARROW_UP    0x52

static uchar idleRate;
static uchar ledState;

/* HID report descriptor WITH LED output report (53 bytes) */
const PROGMEM char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
  0x05, 0x01, 0x09, 0x06, 0xa1, 0x01,
  0x05, 0x07, 0x19, 0xe0, 0x29, 0xe7,
  0x15, 0x00, 0x25, 0x01, 0x75, 0x01,
  0x95, 0x08, 0x81, 0x02,
  0x95, 0x01, 0x75, 0x08, 0x25, 0x65,
  0x19, 0x00, 0x29, 0x65, 0x81, 0x00,
  0x05, 0x08, 0x19, 0x01, 0x29, 0x05,
  0x95, 0x05, 0x75, 0x01, 0x91, 0x02,
  0x95, 0x01, 0x75, 0x03, 0x91, 0x03,
  0xc0
};

struct AltEntry {
  const char* code;
  char character;
};

class SmartKeysDevice {
public:
  SmartKeysDevice() {
    cli();
    usbDeviceDisconnect();
    _delay_ms(250);
    usbDeviceConnect();
    usbInit();
    sei();
    memset(reportBuffer, 0, sizeof(reportBuffer));
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
  }

  void update() { usbPoll(); }

  void stableDelay(uint16_t ms) {
    for (uint16_t i = 0; i < ms; i++) {
      usbPoll();
      ::delay(1);
    }
  }

  void sendKeyPress(byte keyPress, byte modifiers) {
    while (!usbInterruptIsReady()) {
      usbPoll();
      _delay_ms(5);
    }
    reportBuffer[0] = modifiers;
    reportBuffer[1] = keyPress;
    usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
  }

  void sendKeyPress(byte keyPress) { sendKeyPress(keyPress, 0); }

  void sendKeyStroke(byte keyStroke, byte modifiers) {
    sendKeyPress(keyStroke, modifiers);
    sendKeyPress(0, 0);
  }

  void sendKeyStroke(byte keyStroke) { sendKeyStroke(keyStroke, 0); }

  void typeChar(uint8_t chr) {
    uint8_t data = pgm_read_byte_near(ascii_to_scan_code_table + (chr - 8));
    sendKeyStroke(data & 0x7F, data >> 7 ? MOD_SHIFT_RIGHT : 0);
  }

  void print(const char* s) {
    while (*s) { typeChar(*s++); }
  }

  bool isNumLockOn() { return ledState & 1; }

  void ensureNumLock() {
    stableDelay(100);
    if (!isNumLockOn()) {
      sendKeyStroke(KEY_NUMLOCK);
      stableDelay(100);
    }
  }

  void sendAltCode(const char* digits, uint16_t delayMs = 10) {
    ensureNumLock();
    sendKeyPress(0, MOD_ALT_LEFT);
    stableDelay(delayMs);

    for (uint8_t i = 0; digits[i]; i++) {
      if (digits[i] >= '0' && digits[i] <= '9') {
        sendKeyPress(numpadKey(digits[i] - '0'), MOD_ALT_LEFT);
        stableDelay(delayMs);
        sendKeyPress(0, MOD_ALT_LEFT);
        stableDelay(delayMs);
      }
    }

    sendKeyPress(0);
    stableDelay(delayMs);
  }

  void smartPrint(const char* text, uint16_t delayMs = 10) {
    for (uint16_t i = 0; text[i]; i++) {
      bool found = false;
      for (uint8_t j = 0; j < _escapeCount; j++) {
        if (text[i] == _escapeList[j].character) {
          sendAltCode(_escapeList[j].code, delayMs);
          found = true;
          break;
        }
      }
      if (!found) {
        typeChar(text[i]);
      }
      stableDelay(delayMs);
    }
  }

  void setEscapeList(AltEntry* list, uint8_t count) {
    _escapeList = list;
    _escapeCount = count;
  }

  uchar reportBuffer[2];

private:
  static uint8_t numpadKey(uint8_t digit) {
    static const uint8_t keys[] = {
      0x62, 0x59, 0x5A, 0x5B, 0x5C,
      0x5D, 0x5E, 0x5F, 0x60, 0x61
    };
    return keys[digit];
  }

  AltEntry _defaultList[5] = {
    {"39", '\''},
    {"45", '-'},
    {"47", '/'},
    {"58", ':'},
    {"124", '|'}
  };

  AltEntry* _escapeList = _defaultList;
  uint8_t _escapeCount = 5;
};

SmartKeysDevice SmartKeys = SmartKeysDevice();

#ifdef __cplusplus
extern "C" {
#endif

uchar usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (usbRequest_t *)((void *)data);
  usbMsgPtr = SmartKeys.reportBuffer;
  if ((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS) {
    if (rq->bRequest == USBRQ_HID_GET_REPORT) {
      return sizeof(SmartKeys.reportBuffer);
    } else if (rq->bRequest == USBRQ_HID_SET_IDLE) {
      idleRate = rq->wValue.bytes[1];
    } else if (rq->bRequest == USBRQ_HID_SET_REPORT) {
      return USB_NO_MSG;
    }
  }
  return 0;
}

uchar usbFunctionWrite(uchar *data, uchar len) {
  if (len > 0) ledState = data[0];
  return 1;
}

#ifdef __cplusplus
}
#endif

#endif
