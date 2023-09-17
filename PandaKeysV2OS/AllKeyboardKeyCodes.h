/*
      THIS FILE ACTS AS A REPOSITORY FOR ALL POSSIBLE KEYS THAT CAN BE CHOSEN TO ACT AS A HOTKEY!
      THESE KEYCODES ARE TAKEN FROM THE KEYBOARD LIBRARY REFERENCE ON ARDUINO'S WEBISTE!
      https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/

      If you would like to use any standard characters or numbers (excluding numpad numbers), you
      can directly type that character into a hotkey scheme. For example, if you want to type
      an "f" as your hotkey, you can directly use 'f' (in apostrophes, not quotations) when updating
      the hotkey scheme. Example below:
                                                                      |
                                                                      v
      hotkey_array[0][0].SetAllSettings("F", HOTKEYTYPE_KEYBOARD, 5, 'f', 0, 0);

      The following names are for keyboard modifier keys like shift and control. To use these, replace
      the character in the keycode spot with a library-define name. See example below:
                                                                                    |
                                                                                    v
      hotkey_array[0][0].SetAllSettings("Left Shift", HOTKEYTYPE_KEYBOARD, 5, KEY_LEFT_SHIFT, 0, 0);

      ---------------MODIFIERS-----------------
      KEY_LEFT_CTRL
      KEY_LEFT_SHIFT
      KEY_LEFT_ALT
      KEY_LEFT_GUI
      KEY_RIGHT_CTRL
      KEY_RIGHT_SHIFT
      KEY_RIGHT_ALT
      KEY_RIGHT_GUI

      The following names are for special keyboard keys like Enter, Home, Numpad numbers, and Function keys.
      You use these just like the keyboard modifiers. See list below:

      ---------------ALPHANUMERIC KEY CLUSTER-----------------
      KEY_TAB
      KEY_CAPS_LOCK
      KEY_BACKSPACE
      KEY_RETURN
      KEY_MENU

      ---------------NAVIGATION CLUSTER-----------------
      KEY_INSERT
      KEY_DELETE
      KEY_HOME
      KEY_END
      KEY_PAGE_UP
      KEY_PAGE_DOWN
      KEY_UP_ARROW
      KEY_DOWN_ARROW
      KEY_LEFT_ARROW
      KEY_RIGHT_ARROW

      ---------------NUMPAD KEYS-----------------
      KEY_NUM_LOCK
      KEY_KP_SLASH
      KEY_KP_ASTERISK
      KEY_KP_MINUS
      KEY_KP_PLUS
      KEY_KP_ENTER
      KEY_KP_1
      KEY_KP_2
      KEY_KP_3
      KEY_KP_4
      KEY_KP_5
      KEY_KP_6
      KEY_KP_7
      KEY_KP_8
      KEY_KP_9
      KEY_KP_0
      KEY_KP_DOT

      ---------------ESCAPE AND FUNCTION KEYS-----------------
      KEY_ESC
      KEY_F1
      KEY_F2
      KEY_F3
      KEY_F4
      KEY_F5
      KEY_F6
      KEY_F7
      KEY_F8
      KEY_F9
      KEY_F10
      KEY_F11
      KEY_F12
      KEY_F13
      KEY_F14
      KEY_F15
      KEY_F16
      KEY_F17
      KEY_F18
      KEY_F19
      KEY_F20
      KEY_F21
      KEY_F22
      KEY_F23
      KEY_F24
*/