#include "Hotkey.h"
//#include "Arduino_LED_Matrix.h"

#define KEYS_NUMOFROWS 3
#define KEYS_NUMOFCOLS 4
#define NUM_HOTKEYS 12

//PINS FOR KEYSWITCH MATRIX
const byte switchRow0 = 9;
const byte switchRow1 = 8;
const byte switchRow2 = 7;
const byte switchCol0 = 10;
const byte switchCol1 = 16;
const byte switchCol2 = 14;
const byte switchCol3 = 15;

//ALL FASTLED INITIALIZATIONS
#include <FastLED.h>
#define LED_PIN A3
#define NUM_LEDS 12
#define CHIPSET WS2812B
#define COLOR_ORDER GRB
#define MAX_BRIGHTNESS 200
#define LED_NUMOFROWS KEYS_NUMOFROWS
#define LED_NUMOFCOLS KEYS_NUMOFCOLS
CRGB key_leds[NUM_LEDS];
byte led_vals[LED_NUMOFROWS][LED_NUMOFCOLS];
byte led_hues[LED_NUMOFROWS][LED_NUMOFCOLS];
byte led_sats[LED_NUMOFROWS][LED_NUMOFCOLS];


//ALL HOTKEY LIBRARY INITIALIZATIONS
Hotkey hotkey_array[KEYS_NUMOFROWS][KEYS_NUMOFCOLS];

//ARRAYS FOR KEYSWITCH ACTIVATION
bool keyswitch_pressed[KEYS_NUMOFROWS][KEYS_NUMOFCOLS];
bool keyswitch_state[KEYS_NUMOFROWS][KEYS_NUMOFCOLS];
bool keyswitch_last[KEYS_NUMOFROWS][KEYS_NUMOFCOLS];

//ArduinoLEDMatrix matrix;
unsigned long frame[3];

void setup() 
{
  // put your setup code here, to run once:
  pinMode(switchRow0, OUTPUT);
  pinMode(switchRow1, OUTPUT);
  pinMode(switchRow2, OUTPUT);
  pinMode(switchCol0, INPUT_PULLUP);
  pinMode(switchCol1, INPUT_PULLUP);
  pinMode(switchCol2, INPUT_PULLUP);
  pinMode(switchCol3, INPUT_PULLUP);

  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(key_leds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setBrightness(MAX_BRIGHTNESS);
  fill_solid(key_leds, NUM_LEDS, CHSV(70, 255, 255));
  FastLED.show();

  Hotkey_SetDefaultNumpad();

  Keyboard.begin();

  //matrix.begin();
  Serial.begin(9600);
}

void loop() 
{
  //Check all the keyswitches for activation or deactivation
  CheckAllKeyswitches();

  //Print the results of keyswitch checking for debugging
  PrintKeyswitchResult(true);

  //Base off current state of system, display LED lighting under keys
  LED_MainHandler();
  FastLED.setBrightness(MAX_BRIGHTNESS);
  FastLED.show();

  //Based off current status of the keyswitches, perform hotkey operations
  Hotkey_MainHandler();

  Serial.println(hotkey_array[2][2].GetHotkeyName());

  /*ClearR4Matrix();
  for(int i = 0; i<8;i++)
  {
    for(int j = 0;j<12;j++)
    {
      MatrixToR4LEDs(i,j);
      matrix.loadFrame(frame);
      delay(30);
    }
  }
  //MatrixToR4LEDs(7,10);
  //matrix.loadFrame(frame);*/
  delay(50);

}


bool CheckKeyswitch(int rowToCheck, int colToCheck)
{
  //Set row pins to match current row checked
  bool row0status = (rowToCheck == 0 ? LOW : HIGH);
  bool row1status = (rowToCheck == 1 ? LOW : HIGH);
  bool row2status = (rowToCheck == 2 ? LOW : HIGH);
  digitalWrite(switchRow0, row0status);
  digitalWrite(switchRow1, row1status);
  digitalWrite(switchRow2, row2status);

  bool columnStatus = false;
  if(colToCheck == 0)
    columnStatus = digitalRead(switchCol0);
  else if(colToCheck == 1)
    columnStatus = digitalRead(switchCol1);
  else if(colToCheck == 2)
    columnStatus = digitalRead(switchCol2);
  else if(colToCheck == 3)
    columnStatus = digitalRead(switchCol3);

  return columnStatus;
}

void CheckAllKeyswitches()
{
  for(int rowCounter = 0; rowCounter < KEYS_NUMOFROWS; rowCounter++)
  {
    for(int colCounter = 0; colCounter < KEYS_NUMOFCOLS; colCounter++)
    {
      keyswitch_last[rowCounter][colCounter] = keyswitch_state[rowCounter][colCounter];
      keyswitch_state[rowCounter][colCounter] = CheckKeyswitch(rowCounter, colCounter);

      if(keyswitch_state[rowCounter][colCounter] == LOW && keyswitch_last[rowCounter][colCounter] == HIGH)
      {
        keyswitch_pressed[rowCounter][colCounter] = true;
      }
      else if(keyswitch_state[rowCounter][colCounter] == HIGH && keyswitch_last[rowCounter][colCounter] == LOW)
      {
        keyswitch_pressed[rowCounter][colCounter] = false;
      }
      else
      {
        //keyswitch_pressed[rowCounter][colCounter] = false;
      }
    }
  }
}

void PrintKeyswitchResult(bool checkPressedStatus)
{
  if(checkPressedStatus == true)
  {
    for(int rowCounter = 0; rowCounter < KEYS_NUMOFROWS; rowCounter++)
    {
      for(int colCounter = 0; colCounter < KEYS_NUMOFCOLS; colCounter++)
      {
        Serial.print(keyswitch_pressed[rowCounter][colCounter]);
        Serial.print(" ");
      }
      Serial.println("");
    }
  }
  else
  {
    for(int rowCounter = 0; rowCounter < KEYS_NUMOFROWS; rowCounter++)
    {
      for(int colCounter = 0; colCounter < KEYS_NUMOFCOLS; colCounter++)
      {
        Serial.print(keyswitch_state[rowCounter][colCounter]);
        Serial.print(" ");
      }
      Serial.println("");
    }
  }
}

void Hotkey_SingleFunction(byte hotkeyRow, byte hotkeyCol)
{
  if(keyswitch_pressed[hotkeyRow][hotkeyCol] == true)
  {
    hotkey_array[hotkeyRow][hotkeyCol].HotkeyOn();
    keyswitch_pressed[hotkeyRow][hotkeyCol] = false;
  }
}
void Hotkey_KeyboardFunction(byte hotkeyRow, byte hotkeyCol)
{
  //Get the state of what the hotkey is doing
  bool currentHotkeyState = hotkey_array[hotkeyRow][hotkeyCol].GetHotkeyState();

  //For simple keyboard key control, press or release the virtual key based off current conditions.
  if(currentHotkeyState == false && keyswitch_pressed[hotkeyRow][hotkeyCol] == true)
    hotkey_array[hotkeyRow][hotkeyCol].HotkeyOn();
  else if(currentHotkeyState == true && keyswitch_pressed[hotkeyRow][hotkeyCol] == false)
    hotkey_array[hotkeyRow][hotkeyCol].HotkeyOff();
  else{}
   //do nothing

}
void Hotkey_RepeatHoldFunction(byte hotkeyRow, byte hotkeyCol)
{
  //Get the current state of the hotkey.
  bool currentHotkeyState = hotkey_array[hotkeyRow][hotkeyCol].GetHotkeyState();

  //Depending on current conditions, repeat the hotkey function while the key is held down.
  if(keyswitch_pressed[hotkeyRow][hotkeyCol] == true && currentHotkeyState == false)
    hotkey_array[hotkeyRow][hotkeyCol].HotkeyOn();
  else if(keyswitch_pressed[hotkeyRow][hotkeyCol] == false && currentHotkeyState == true)
    hotkey_array[hotkeyRow][hotkeyCol].HotkeyOff();
  else{}
    //do nothing
}
void Hotkey_RepeatToggleFunction(byte hotkeyRow, byte hotkeyCol)
{
  //Get the current state of the hotkey
  bool currentHotkeyState = hotkey_array[hotkeyRow][hotkeyCol].GetHotkeyState();

  //Depending on current conditions, repeat the hotkey function until the same key is pressed again.
  if(keyswitch_pressed[hotkeyRow][hotkeyCol] == true && currentHotkeyState == false)
  {
    hotkey_array[hotkeyRow][hotkeyCol].HotkeyOn();
    keyswitch_pressed[hotkeyRow][hotkeyCol] = false;
  }
  else if(keyswitch_pressed[hotkeyRow][hotkeyCol] == true && currentHotkeyState == true)
  {
    hotkey_array[hotkeyRow][hotkeyCol].HotkeyOff();
    keyswitch_pressed[hotkeyRow][hotkeyCol] = false;
  }
  else{}
    //do nothing
}
void Hotkey_MainHandler()
{
  //Check through the keypress array to see if any changes need to be made
  for(int rowCounter = 0; rowCounter < KEYS_NUMOFROWS; rowCounter++)
  {
    for(int colCounter = 0; colCounter < KEYS_NUMOFCOLS; colCounter++)
    {
      //Depending on the type of hotkey, we do something different
      uint8_t currentHotkeyType = hotkey_array[rowCounter][colCounter].GetHotkeyType();

      switch(currentHotkeyType)
      {
        case HOTKEYTYPE_SINGLE:
          Hotkey_SingleFunction(rowCounter, colCounter);
          break;
        case HOTKEYTYPE_KEYBOARD:
          Hotkey_KeyboardFunction(rowCounter, colCounter);
          break;
        case HOTKEYTYPE_REPEATHOLDING:
          Hotkey_RepeatHoldFunction(rowCounter, colCounter);
          break;
        case HOTKEYTYPE_REPEATTOGGLE:
          Hotkey_RepeatToggleFunction(rowCounter, colCounter);
          break;
        default:
          Serial.println("Error: Hotkeytype");
      }

      //If the hotkey needs to perform any actions per cycle, run this function to perform them.
      hotkey_array[rowCounter][colCounter].HotkeyAction();
    }
  }
}

void Hotkey_SetDefaultGaming()
{
  hotkey_array[0][0].SetAllSettings("Q", HOTKEYTYPE_KEYBOARD, 4, 'q', 0, 0);
  hotkey_array[0][1].SetAllSettings("W", HOTKEYTYPE_KEYBOARD, 4, 'w', 0, 1);
  hotkey_array[0][2].SetAllSettings("E", HOTKEYTYPE_KEYBOARD, 4, 'e', 0, 2);
  hotkey_array[0][3].SetAllSettings("R", HOTKEYTYPE_KEYBOARD, 4, 'r', 0, 3);
  hotkey_array[1][0].SetAllSettings("A", HOTKEYTYPE_KEYBOARD, 4, 'a', 1, 0);
  hotkey_array[1][1].SetAllSettings("S", HOTKEYTYPE_KEYBOARD, 4, 's', 1, 1);
  hotkey_array[1][2].SetAllSettings("D", HOTKEYTYPE_KEYBOARD, 4, 'd', 1, 2);
  hotkey_array[1][3].SetAllSettings("F", HOTKEYTYPE_KEYBOARD, 4, 'f', 1, 3);
  hotkey_array[2][0].SetAllSettings("Z", HOTKEYTYPE_KEYBOARD, 4, 'z', 2, 0);
  hotkey_array[2][1].SetAllSettings("X", HOTKEYTYPE_KEYBOARD, 4, 'x', 2, 1);
  hotkey_array[2][2].SetAllSettings("C", HOTKEYTYPE_KEYBOARD, 4, 'c', 2, 2);
  hotkey_array[2][3].SetAllSettings("V", HOTKEYTYPE_KEYBOARD, 4, 'v', 2, 3);
}

void Hotkey_SetDefaultNumpad()
{
  hotkey_array[0][0].SetAllSettings("NUM7", HOTKEYTYPE_SINGLE, 4, KEY_KP_7, 0, 0);
  hotkey_array[0][1].SetAllSettings("NUM8", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_8, 0, 1);
  hotkey_array[0][2].SetAllSettings("NUM9", HOTKEYTYPE_REPEATHOLDING, 4, KEY_KP_9, 0, 2);
  hotkey_array[0][3].SetAllSettings("MINUS", HOTKEYTYPE_REPEATTOGGLE, 4, KEY_KP_MINUS, 0, 3);
  hotkey_array[1][0].SetAllSettings("NUM4", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_4, 1, 0);
  hotkey_array[1][1].SetAllSettings("NUM5", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_5, 1, 1);
  hotkey_array[1][2].SetAllSettings("NUM6", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_6, 1, 2);
  hotkey_array[1][3].SetAllSettings("PLUS", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_PLUS, 1, 3);
  hotkey_array[2][0].SetAllSettings("NUM1", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_1, 2, 0);
  hotkey_array[2][1].SetAllSettings("NUM2", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_2, 2, 1);
  hotkey_array[2][2].SetAllSettings("NUM3", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_3, 2, 2);
  hotkey_array[2][3].SetAllSettings("ENTER", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_ENTER, 2, 3);
}

void LED_MainHandler()
{
  const byte brightnessMin = 40;
  const byte brightnessMax = 255;

  //Assign lighting based off key status. If a key is pressed, illuminate it to max, then dim down over time
  for (byte rowCounter = 0; rowCounter < LED_NUMOFROWS; rowCounter++)
  {
    for (byte colCounter = 0; colCounter < LED_NUMOFCOLS; colCounter++)
    {
      //Check if a key is pressed
      bool keycheck = keyswitch_pressed[rowCounter][colCounter];
      if(keycheck == true)
      {
        led_vals[rowCounter][colCounter] = brightnessMax;
      }
      else
      {
        byte currentBrightness = led_vals[rowCounter][colCounter];
        if(currentBrightness > brightnessMin)
          led_vals[rowCounter][colCounter] = currentBrightness - 5;
      }

      LED_RowCol2Matrix(rowCounter, colCounter, CHSV(100, 255, led_vals[rowCounter][colCounter]));
    }
  }
}

void LED_RowCol2Matrix(byte row, byte col, CHSV newColor)
{
  //Convert row and column scheme to a sequential set of LEDs
  int ledNum = (LED_NUMOFCOLS * row) + col;
  key_leds[ledNum] = newColor;
}

void ClearR4Matrix()
{
  frame[0] = 0;
  frame[1] = 0;
  frame[2] = 0;
}
void MatrixToR4LEDs(int dataRow, int dataCol, bool screenFlip)
{
  //Calculate which LED of 96 should be lit up
  int ledToLight = (dataRow * 12) + dataCol;

  //Do not allow setting of LED if the number is too big
  if (ledToLight < 96)
  {
    //Determine which unsigned long variable to store the bit data in
    int uLongNum = 2 - (ledToLight / 32);

    //Determine which bit within the unsigned long variable to set
    int bitToSet = ledToLight - (32*(2 - uLongNum));

    //Set the specific bit of a specific unsigned long from the frame array
    bitSet(frame[uLongNum], bitToSet);
  }
}
