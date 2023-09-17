#include "Arduino.h"
#include "Hotkey.h"

Hotkey::Hotkey()
{
  SetHotkeyName("unassigned");
  SetKeycode(0x00);
  SetLocation(255, 255);
  SetHotkeyType(0);
  SetHotkeyDelay(100);
  HotkeyOff();
}

void Hotkey::SetAllSettings(String newName, uint8_t newType, unsigned int newDelay, uint8_t newKeycode, uint8_t newRow, uint8_t newCol)
{
  SetHotkeyName(newName);
  SetKeycode(newKeycode);
  SetLocation(newRow, newCol);
  SetHotkeyType(newType);
  SetHotkeyDelay(newDelay);
  HotkeyOff();
}

void Hotkey::SetKeycode(uint8_t newKeycode)
{
  keyCode[0] = newKeycode;
}

void Hotkey::SetLocation(uint8_t row, uint8_t col)
{
  locRow = row;
  locCol = col;
}

void Hotkey::SetHotkeyName(String newName)
{
  hotkeyName = newName;
}

void Hotkey::SetHotkeyType(uint8_t newType)
{
  hotkeyType = newType;
}

void Hotkey::SetHotkeyDelay(unsigned int newDelay)
{
  hotkeyDelay = newDelay;
}

String Hotkey::GetHotkeyName()
{
  return hotkeyName;
}

uint8_t Hotkey::GetHotkeyType()
{
  return hotkeyType;
}

uint8_t Hotkey::GetKeycode()
{
  return keyCode[0];
}

uint8_t Hotkey::GetLocationRow()
{
  return locRow;
}

uint8_t Hotkey::GetLocationCol()
{
  return locCol;
}

bool Hotkey::GetHotkeyState()
{
  return isActive;
}

void Hotkey::ToggleHotkey()
{
  bool currentHotkeyStatus = isActive;
  isActive = !currentHotkeyStatus;

  if(isActive == true)
    HotkeyOn();
  else
    HotkeyOff();
}

void Hotkey::HotkeyOff()
{
  if(hotkeyType == HOTKEYTYPE_KEYBOARD)
  {
    Keyboard.release(keyCode[0]);
    isActive = false;
  }
  else if(hotkeyType == HOTKEYTYPE_REPEATHOLDING || hotkeyType == HOTKEYTYPE_REPEATTOGGLE)
  {
    isActive = false;
  }
  else
  {
    //do nothing
  }
}

void Hotkey::HotkeyOn()
{
  if(hotkeyType == HOTKEYTYPE_SINGLE)
  {
    Keyboard.write(keyCode[0]);
  }
  else if(hotkeyType == HOTKEYTYPE_KEYBOARD)
  {
    Keyboard.press(keyCode[0]);
    isActive = true;
  }
  else if(hotkeyType == HOTKEYTYPE_REPEATHOLDING || hotkeyType == HOTKEYTYPE_REPEATTOGGLE || hotkeyType == HOTKEYTYPE_COMBO)
  {
    isActive = true;
  }
  else
  {
    //do nothing
  }
}

void Hotkey::HotkeyAction()
{
  static unsigned int lastActionTime = 0;

  //Check if a hotkey is active before proceeding through the loop
  if (isActive == true)
  {
    //Check the current system time
    unsigned int currentTime = millis();

    //Check for if enough time has passed to repeat the hotkey. Delay set by the user.
    if((currentTime - lastActionTime) >= hotkeyDelay)
    {
      //Depending on the type of hotkey, perform different actions.
      if(hotkeyType == HOTKEYTYPE_REPEATHOLDING || hotkeyType == HOTKEYTYPE_REPEATTOGGLE)
      {
        //This will keep pressing and releasing the assigned hotkey at a specific interval.
        Keyboard.write(keyCode[0]);
      }
      else if(hotkeyType == HOTKEYTYPE_COMBO)
      {
        //This will activate all the assigned hotkeys in an assigned combo, then release them.
        for(int comboCounter = 0; comboCounter < 5; comboCounter++)
        {
          Keyboard.press(keyCode[comboCounter]);
        }
        Keyboard.releaseAll();
        isActive = false;
      }
      else
      {
        //do nothing
      }

      //Update last time the loop ran.
      lastActionTime = currentTime;
    }
  }
}