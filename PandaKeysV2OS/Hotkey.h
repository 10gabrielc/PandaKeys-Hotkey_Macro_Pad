#ifndef HOTKEY_H
#define HOTKEY_H

#define HOTKEYTYPE_SINGLE 0
#define HOTKEYTYPE_KEYBOARD 1
#define HOTKEYTYPE_REPEATHOLDING 2
#define HOTKEYTYPE_REPEATTOGGLE 3
#define HOTKEYTYPE_COMBO 4

#include "Arduino.h"
#include "Keyboard.h"
#include "string.h"

class Hotkey
{
  private:
    bool isActive;
    uint8_t hotkeyType;
    unsigned int hotkeyDelay;
    String hotkeyName;
    uint8_t keyCode[5];
    uint8_t locRow;
    uint8_t locCol;

  public:
    Hotkey();

    void SetAllSettings(String newName, uint8_t newType, unsigned int newDelay, uint8_t newKeycode, uint8_t newRow, uint8_t newCol);
    void SetKeycode(uint8_t newKeycode);
    void SetLocation(uint8_t row, uint8_t col);
    void SetHotkeyName(String newName);
    void SetHotkeyType(uint8_t newType);
    void SetHotkeyDelay(unsigned int newDelay);
    
    uint8_t GetKeycode();
    uint8_t GetLocationRow();
    uint8_t GetLocationCol();
    String GetHotkeyName();
    uint8_t GetHotkeyType();
    unsigned int GetHotkeyDelay();
    bool GetHotkeyState();

    void HotkeyAction();
    void ToggleHotkey();
    void HotkeyOff();
    void HotkeyOn();
};

#endif