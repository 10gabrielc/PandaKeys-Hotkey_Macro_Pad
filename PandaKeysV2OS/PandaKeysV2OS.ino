/*
    PANDAKEYS OS V2.0

    Program to control a small user-customizable hotkey box using HID emulation through
    Arduino microcontrollers. V1.0 was based off the Raspberry Pi Pico, where this
    version is a complete rewrite to switch to C++ on an Arduino Pro Micro.

    Written by Gabriel Contreras, 2023

    =!HOW TO CREATE YOUR OWN HOTKEY!=
    To create your own hotkey, you will be giving the program multiple pieces of information at once.
    The information required for a hotkey is broken down below:

    hotkey_array[ExampleRow][ExampleColumn].SetAllSettings("Example Name", EXAMPLE_HOTKEYTYPE, ExampleDelayValue, ExampleKey, ExampleLocationRow, ExampleLocationColumn);
    
    -hotkey_array: This is the data container that stores all of the hotkey "objects" created when the system turns on.
                    By putting a row and column value in the [][], you address a specific hotkey within that data.
    -ExampleRow/ExampleColumn: This is the specific hotkey you want to update the information for. If you wanted to address
                                the hotkey in the 2nd row, 3rd column, you would write [1][2] (numbers start at 0, not 1).
    -.SetAllSettings: This is the hotkey function that when called, allows you to update a hotkey's information.
    -"Example Name": This is what you would like to name the hotkey. This can be used for debugging to check what a
                      hotkey is supposed to be. Will be used in the future when screen functionality is enabled
    -EXAMPLE_HOTKEYTYPE: This tells the system the behavior you want for the hotkey, and there are 5 options:
                          HOTKEYTYPE_SINGLE: When this key is pressed, it will send the keystroke once and not repeat
                          HOTKEYTYPE_KEYBOARD: This key will act just like a keyboard: hold it down and it will repeat at a keyboard's natural speed
                          HOTKEYTYPE_REPEATHOLDING: This key will repeat the hotkey action at a specific rate while the key is held down
                          HOTKEYTYPE_REPEATTOGGLE: This key will repeat the hotkey endlessly until the key is pressed again
                          HOTKEYTYPE_COMBO: This will be for multiple keys to be sent at once, but is currently unimplemented
    -ExampleDelayValue: This is the delay between sends when the hotkey type is something that repeats
    -ExampleKey: This is for the actual keycode for a key on the keyboard. Put an ASCII character in ' ' or check
                  the AllKeyboardKeyCodes.h file for information on special keys you can send
    -ExampleLocationRow/LocationColumn: This updates the hotkey's internal information with its current row/column location

    Check for the section of the code below that indicates where users can modify the code to edit/add new hotkeys.  

    =!HOW TO CREATE YOUR OWN HOTKEY PROFILE!=
    If you would like to create an entirely new profile to be loaded on startup, you can copy and paste one of the functions to start.
    Then, change the name of the function (Hotkey_SetDummyProfile to Hotkey_IJKLProfile). Use the information above to modify
    all the hotkeys you would like to use or change. Lastly, check the function in the user-modifiable section called Hotkey_SetActiveHotkeys().
    Take the name of your new profile's function and write it into one of the case statements. For example:
    case 3:                                     case 3:
      Hotkey_SetDummyProfile();     -->             Hotkey_IJKLProfile();
      break;                                        break;
    Now, when picking the fourth hotkey on the hotkey box, it will load your new profile!

    ***Only modify code in the indicated user-modifiable section! All other functions are not to be edited or errors will occur.***
*/

#define NUM_ROWS_KEYS 3                                           //Number of hotkey rows present on the box
#define NUM_COLS_KEYS 4                                           //Number of hotkey columns present on the box
#define NUM_HOTKEYS 12                                            //Total number of hotkeys for reference
#define NUM_LIGHTING_MODES 6                                      //How many possible programmed lighting modes there are for the LEDs
#define NUM_USER_PROFILES 8                                       //Number of total possible profiles loadable onto the hotkeys at one time

//PINS FOR KEYSWITCH MATRIX
const byte switchRow0 = 9;                                        
const byte switchRow1 = 8;
const byte switchRow2 = 7;
const byte switchCol0 = 10;
const byte switchCol1 = 16;
const byte switchCol2 = 14;
const byte switchCol3 = 15;

//PINS FOR OTHER FUNCTIONS
const byte functionBtn = 6;

//ALL FASTLED INITIALIZATIONS
#include <FastLED.h>                                              //Include FastLED library
#define LED_PIN A3                                                //The pin the LEDs should be connected to
#define NUM_LEDS 12                                               //The number of LEDs connected to the strand
#define CHIPSET WS2812B                                           //The chipset that drives the LEDs
#define COLOR_ORDER GRB                                           //Which order the LED data should be sent to the LEDs
#define MAX_BRIGHTNESS 200                                        //The maximum possible brightness the LEDs can shine out of 255
#define NUM_ROWS_LEDS NUM_ROWS_KEYS                               //The number of rows of LEDs in the box
#define NUM_COLS_LEDS NUM_COLS_KEYS                               //The number of columns of LEDs in the box
CRGB key_leds[NUM_LEDS];                                          //Container to store LED lighting data
byte led_vals[NUM_ROWS_LEDS][NUM_COLS_LEDS];                      //2D array to store row/column info of LED brightness
byte led_hues[NUM_ROWS_LEDS][NUM_COLS_LEDS];                      //2D array to store row/column info of LED hue
byte led_sats[NUM_ROWS_LEDS][NUM_COLS_LEDS];                      //2D array to store row/column info of LED saturation
byte globalBrightness = MAX_BRIGHTNESS;                           //The current brightness of the LEDs
byte ledMode = 2;                                                 //The current lighting mode of the LEDs


//ALL HOTKEY LIBRARY INITIALIZATIONS
#include "Hotkey.h"                                               //Include the hotkey library
Hotkey hotkey_array[NUM_ROWS_KEYS][NUM_COLS_KEYS];                //Create a 2D array of hotkeys to resemble the hotkey layout of the box

//ARRAYS FOR KEYSWITCH ACTIVATION
bool keyswitch_pressed[NUM_ROWS_KEYS][NUM_COLS_KEYS];             //The array to check if a hotkey was pressed
bool keyswitch_state[NUM_ROWS_KEYS][NUM_COLS_KEYS];               //The array to hold what the current state of the keyswitches are
bool keyswitch_last[NUM_ROWS_KEYS][NUM_COLS_KEYS];                //The array to hold what the last state of the keyswitches were



//=====================================================================================================================================================================================
//=============================================================  START OF USER MODIFIABLE PARTS OF THE CODE ===========================================================================
//=====================================================================================================================================================================================

/* ---------"VARIABLES" MODIFIABLE BY THE USER-------------*/
#define USB_POLL_RATE 1000                                         //Number of USB data refreshes per second
#define LED_REFRESH_RATE 20                                       //Number of LED refreshes per second

/*-------------------------------------------------------------=PUT ANY NEW HOTKEY PROFILES OR MODIFY PREVIOUSLY MADE ONES IN HERE=--------------------------------------------*/
void Hotkey_SetDummyProfile()
{
  hotkey_array[0][0].SetAllSettings("DUMMY1", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 0, 0);
  hotkey_array[0][1].SetAllSettings("DUMMY2", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 0, 1);
  hotkey_array[0][2].SetAllSettings("DUMMY3", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 0, 2);
  hotkey_array[0][3].SetAllSettings("DUMMY4", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 0, 3);
  hotkey_array[1][0].SetAllSettings("DUMMY5", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 1, 0);
  hotkey_array[1][1].SetAllSettings("DUMMY6", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 1, 1);
  hotkey_array[1][2].SetAllSettings("DUMMY7", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 1, 2);
  hotkey_array[1][3].SetAllSettings("DUMMY8", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 1, 3);
  hotkey_array[2][0].SetAllSettings("DUMMY9", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 2, 0);
  hotkey_array[2][1].SetAllSettings("DUMMY10", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 2, 1);
  hotkey_array[2][2].SetAllSettings("DUMMY11", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 2, 2);
  hotkey_array[2][3].SetAllSettings("DUMMY12", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 2, 3);
}

void Hotkey_SetGamingProfile()
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

void Hotkey_SetNumpadProfile()
{
  hotkey_array[0][0].SetAllSettings("NUM7", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_7, 0, 0);
  hotkey_array[0][1].SetAllSettings("NUM8", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_8, 0, 1);
  hotkey_array[0][2].SetAllSettings("NUM9", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_9, 0, 2);
  hotkey_array[0][3].SetAllSettings("MINUS", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_MINUS, 0, 3);
  hotkey_array[1][0].SetAllSettings("NUM4", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_4, 1, 0);
  hotkey_array[1][1].SetAllSettings("NUM5", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_5, 1, 1);
  hotkey_array[1][2].SetAllSettings("NUM6", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_6, 1, 2);
  hotkey_array[1][3].SetAllSettings("PLUS", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_PLUS, 1, 3);
  hotkey_array[2][0].SetAllSettings("NUM1", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_1, 2, 0);
  hotkey_array[2][1].SetAllSettings("NUM2", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_2, 2, 1);
  hotkey_array[2][2].SetAllSettings("NUM3", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_3, 2, 2);
  hotkey_array[2][3].SetAllSettings("ENTER", HOTKEYTYPE_KEYBOARD, 4, KEY_KP_ENTER, 2, 3);
}

void Hotkey_SetArrowKeysProfile()
{
  hotkey_array[0][0].SetAllSettings("Empty Key", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 0, 0);
  hotkey_array[0][1].SetAllSettings("Up Arrow", HOTKEYTYPE_KEYBOARD, 4, KEY_UP_ARROW, 0, 1);
  hotkey_array[0][2].SetAllSettings("Empty Key", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 0, 2);
  hotkey_array[0][3].SetAllSettings("Empty Key", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 0, 3);
  hotkey_array[1][0].SetAllSettings("Left Arrow", HOTKEYTYPE_KEYBOARD, 4, KEY_LEFT_ARROW, 1, 0);
  hotkey_array[1][1].SetAllSettings("Down Arrow", HOTKEYTYPE_KEYBOARD, 4, KEY_DOWN_ARROW, 1, 1);
  hotkey_array[1][2].SetAllSettings("Right Arrow", HOTKEYTYPE_KEYBOARD, 4, KEY_RIGHT_ARROW, 1, 2);
  hotkey_array[1][3].SetAllSettings("Empty Key", HOTKEYTYPE_KEYBOARD, 4, KEY_PAUSE, 1, 3);
  hotkey_array[2][0].SetAllSettings("Left Control", HOTKEYTYPE_KEYBOARD, 4, KEY_LEFT_CTRL, 2, 0);
  hotkey_array[2][1].SetAllSettings("Left Shift", HOTKEYTYPE_KEYBOARD, 4, KEY_LEFT_SHIFT, 2, 1);
  hotkey_array[2][2].SetAllSettings("Left Alt", HOTKEYTYPE_KEYBOARD, 4, KEY_LEFT_ALT, 2, 2);
  hotkey_array[2][3].SetAllSettings("Spacebar", HOTKEYTYPE_KEYBOARD, 4, ' ', 2, 3);
}
/*--------------------------------------------------------------------=END OF HOTKEY PROFILES=------------------------------------------------------*/

/*ADD NEW PROFILES INTO THE PROFILE SELECTOR FUNCTION BELOW, REPLACING A PREVIOUSLY ENTERED ONE*/
void Hotkey_SetActiveHotkeys(byte profileNum)
{
  switch(profileNum)
  {
    case 0:
      Hotkey_SetGamingProfile();
      break;
    case 1:
      Hotkey_SetNumpadProfile();
      break;
    case 2:
      Hotkey_SetArrowKeysProfile();
      break;
    case 3:
      Hotkey_SetDummyProfile();
      break;
    case 4:
      Hotkey_SetDummyProfile();
      break;
    case 5:
      Hotkey_SetDummyProfile();
      break;
    case 6:
      Hotkey_SetDummyProfile();
      break;
    case 7:
      Hotkey_SetDummyProfile();
      break;
    default:
      Hotkey_SetDummyProfile();
  }
}

//=====================================================================================================================================================================================
//=============================================================  END OF USER MODIFIABLE PARTS OF THE CODE =============================================================================
//=====================================================================================================================================================================================

bool CheckKeyswitch(int rowToCheck, int colToCheck)
{
  //Use conditional statements to determine which row to Set HIGH or LOW to conduct reading
  bool row0status = (rowToCheck == 0 ? LOW : HIGH);
  bool row1status = (rowToCheck == 1 ? LOW : HIGH);
  bool row2status = (rowToCheck == 2 ? LOW : HIGH);

  //Set the rows to the correct states
  digitalWrite(switchRow0, row0status);
  digitalWrite(switchRow1, row1status);
  digitalWrite(switchRow2, row2status);

  //Read the state of the desired column to determine if a keyswitch was pressed
  bool columnStatus = false;
  if(colToCheck == 0)
    columnStatus = digitalRead(switchCol0);
  else if(colToCheck == 1)
    columnStatus = digitalRead(switchCol1);
  else if(colToCheck == 2)
    columnStatus = digitalRead(switchCol2);
  else if(colToCheck == 3)
    columnStatus = digitalRead(switchCol3);

  //Return the state of the desired keyswitch
  return columnStatus;
}
void CheckAllKeyswitches()
{
  //Iterate through all of the keyswitchs to check their states
  for(int rowCounter = 0; rowCounter < NUM_ROWS_KEYS; rowCounter++)
  {
    for(int colCounter = 0; colCounter < NUM_COLS_KEYS; colCounter++)
    {
      keyswitch_last[rowCounter][colCounter] = keyswitch_state[rowCounter][colCounter];                           //Store the last state the current keyswitch was in
      keyswitch_state[rowCounter][colCounter] = CheckKeyswitch(rowCounter, colCounter);                           //Get the current state of the keyswitch

      if(keyswitch_state[rowCounter][colCounter] == LOW && keyswitch_last[rowCounter][colCounter] == HIGH)        //Checking for when a keyswitch has just been pressed
        keyswitch_pressed[rowCounter][colCounter] = true;
      else if(keyswitch_state[rowCounter][colCounter] == HIGH && keyswitch_last[rowCounter][colCounter] == LOW)   //Checking for when a keyswitch has just been released
        keyswitch_pressed[rowCounter][colCounter] = false;
      else{}                                                                                                      //Do nothing if the keyswitch is not in the above states
    }
  }
}

void Function_MainHandler(byte userChoice)
{
  //Based off user's selection, perform a different function
  if(userChoice >= 0 && userChoice <= 7)                                //If choice is from top 2 rows, update the active hotkey scheme
  {
    Hotkey_SetActiveHotkeys(userChoice);
  }
  else if(userChoice == 8)                                              //If 9th key pressed, increase the brightness
  {
    //brightness increase
    globalBrightness+=10;
    if(globalBrightness > MAX_BRIGHTNESS)
      globalBrightness = 0;
  }
  else if(userChoice == 9)                                              //If 10th key pressed, begin color selection process
  {
    bool choiceMade = false;
    byte choiceRow = 0;
    byte choiceCol = 0;
    byte currentHue = led_hues[0][0];                                   //Use the current hue of the first LED as a base
    for(int row = 0; row < NUM_ROWS_KEYS; row++)                        //Fill LEDs with possible colors to choose
    {
      for(int col = 0; col < NUM_COLS_KEYS; col++)
      {
        led_hues[row][col] = currentHue;                                  //Set each LED to an incrementing hue color as it progresses down the strand
        currentHue+=5;
        LED_RowCol2Matrix(row, col, CHSV(led_hues[row][col], 255, 255));  //Send that data to the LED data array to be displayed
      }
    }
    LED_RowCol2Matrix(2, 3, CHSV(0, 0, 255));                           //Set final LED to white to indicate regenerating palette
    FastLED.show();                                                     //Push updates to LEDs
    while(choiceMade == false)                                          //Loop the following until a key is pressed
    {
      CheckAllKeyswitches();                                            //Check all keys for a press
      for(int row = 0; row < NUM_ROWS_KEYS; row++)
      {
        for(int col = 0; col < NUM_COLS_KEYS; col++)
        {
          if(keyswitch_pressed[row][col] == true)                       //If a key is pressed, record its row/column and update choice flag
          {
            keyswitch_pressed[row][col] = false;                        //Clear the key pressed flag
            choiceMade = true;                                          //Update while loop variable to be able to exit the loop
            choiceRow = row;                                            //Store the row and column number for the color location desired
            choiceCol = col;
          }
        }
      }

      if(choiceMade == true)                                            //If a choice was made, handle it accordingly
      {
        if(choiceRow == 2 && choiceCol == 3)                            //Check if regenerate palette key was pressed
        {
          for(int row = 0; row < NUM_ROWS_KEYS; row++)
          {
            for(int col = 0; col < NUM_COLS_KEYS; col++)
            {
              led_hues[row][col] = currentHue;                          //Regerating palette with new colors
              currentHue+=5;
              LED_RowCol2Matrix(row, col, CHSV(led_hues[row][col], 255, 255));
            }
          }
          LED_RowCol2Matrix(2, 3, CHSV(0, 0, 255));                     //set final LED to white to indicate regenerating palette
          FastLED.show();
          choiceMade = false;                                           //Clear choice flag to allow loop to repeat and wait for another press
        }
        else
        {
          currentHue = led_hues[choiceRow][choiceCol];                  //Color was chosen, store this color in first hue spot in hue array
          led_hues[0][0] = currentHue;
        }
      }
      else
      {
        //Do nothing, no key was pressed
      }
    }
  }
  else if(userChoice == 10)                                             //Change the current lighting mode
  {
    if(ledMode >= (NUM_LIGHTING_MODES-1))                               //Loop LED mode back to 0 when final mode reached
      ledMode = 0;
    else
      ledMode++;                                                        //Otherwise change to next LED mode
  }
  else if(userChoice == 11)
  {
    //save this for OLED control
  }
  else
  {
    //Do nothing
  }
}

void Hotkey_MainHandler()
{
  //Check through the keypress array to see if any changes need to be made
  for(int rowCounter = 0; rowCounter < NUM_ROWS_KEYS; rowCounter++)
  {
    for(int colCounter = 0; colCounter < NUM_COLS_KEYS; colCounter++)
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
          break;
      }

      //If the hotkey needs to perform any actions per cycle, run this function to perform them.
      hotkey_array[rowCounter][colCounter].HotkeyAction();
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

void LED_MainHandler()
{

  //Based off the current lighting mode, update the backlighting LEDs accordingly
  switch(ledMode)
  {
    case 0:
      LED_AnimSolidStatic();
      break;
    case 1:
      LED_AnimSolidRainbow();
      break;
    case 2:
      LED_AnimSolidChase();
      break;
    case 3:
      LED_AnimPressFade();
      break;
    case 4:
      LED_AnimPressSplash();
      break;
    case 5:
      LED_AnimSparkle();
      break;
    case 6:
      LED_AnimFunction();
      break;
    default:
      LED_AnimSolidStatic();
  }
}
void LED_AnimSolidStatic()
{
  //Persistent variables needed across function calls
  static byte lastHue = 0;

  //Variables needed for this function
  byte currentHue = led_hues[0][0];

  if(lastHue != currentHue)                                                                 //If there was a color update, push that update to the LEDs
  {
    fill_solid(key_leds, NUM_LEDS, CHSV(currentHue, led_sats[0][0], led_vals[0][0]));       //Fill all LEDs with the same data using prebuilt function
    lastHue = currentHue;                                                                   //Update the previous hue
  }
}
void LED_AnimSolidRainbow()
{
  //Constants needed for this function
  const byte hueDelta = 5;                                                                  //How much the hues will change per cycle
  
  byte lastHue = led_hues[0][0];                                                            //Use the first led's hue value as the value for all others
  led_hues[0][0] = lastHue + hueDelta;                                                      //Shift the hue to create the rainbow over time effect
  fill_solid(key_leds, NUM_LEDS, CHSV(led_hues[0][0], led_sats[0][0], led_vals[0][0]));     //Use easier method of filling entire LED data array with same color
}
void LED_AnimSolidChase()
{
  //We cannot use the fill_rainbow for this since it needs to progress column by column
  //Constants needed for this function
  const byte hueDelta = 10;                                                                 //How much the rainbow will shift across the keys

  byte currentHue = led_hues[0][0];                                                         //Use the first led's hue value as the value for all others
  for(int col = 0; col < NUM_COLS_LEDS; col++)                                              //Iterate through all LEDs as we create the chasing rainbow effect
  {
    currentHue+=hueDelta;                                                                   //Shift the hue of the LEDs for every new column
    for(int row = 0; row < NUM_ROWS_LEDS; row++)
    {
      led_hues[row][col] = currentHue;                                                                //Store change in hue in global array
      LED_RowCol2Matrix(row, col, CHSV(led_hues[row][col], led_sats[row][col], led_vals[row][col]));  //Push updates to the LED data
    }
  }
}
void LED_AnimPressFade()
{
  //Constants used in this function
  const byte brightnessMax = 255;                                                           //The max value that the LEDs will shine at when set to full bright
  const byte brightnessMin = 30;                                                            //The minimum that the LEDs will dim to
  const byte delta = 5;                                                                     //How much the LEDs will dim by per cycle

  //Assign lighting based off key status. If a key is pressed, illuminate it to max, then dim down over time
  for (byte row = 0; row < NUM_ROWS_LEDS; row++)                                            //Iterate through all LEDs
  {
    for (byte col = 0; col < NUM_COLS_LEDS; col++)
    {
      //Check if a key is pressed
      bool keycheck = keyswitch_pressed[row][col];                                          //Check if current key above LED is pressed
      if(keycheck == true)
      {
        led_vals[row][col] = brightnessMax;                                                 //If pressed, set LED to max brightness
      }
      else
      {
        byte currentBrightness = led_vals[row][col];                                        //If LED is not at minimum brightness, decrease its brightness
        if(currentBrightness > brightnessMin)
          led_vals[row][col] = currentBrightness - delta;                                   //Store change in brightness in global array
      }
      LED_RowCol2Matrix(row, col, CHSV(100, 255, led_vals[row][col]));                      //Push updates to LED data
    }
  }
}
void LED_AnimPressSplash()
{
  //Constants needed for this function
  const byte minVal = 30;                                                                   //Minimum brightness LEDs will dim to
  const byte delta = 5;                                                                     //How much the LEDs will dim by per cycle
  const byte maxVal = 255;                                                                  //How bright the LEDs will be set to when relit
  const byte spreadVal = maxVal - delta;                                                    //The value that will be looked for when spreading a color
  const byte overrideThreshold = 150;                                                       //The value that will allow a new splash to overwrite another

  //First step is to decrement each LED's brightness by the delta value
  for (byte row = 0; row < NUM_ROWS_LEDS; row++)
  {
    for (byte col = 0; col < NUM_COLS_LEDS; col++)
    {
      byte tempVal = led_vals[row][col];
      if(tempVal > minVal)
        led_vals[row][col] = tempVal - delta;
    }
  }

  //Second step is to check for any key presses and illuminate those keys accordingly
  for (byte row = 0; row < NUM_ROWS_LEDS; row++)
  {
    for (byte col = 0; col < NUM_COLS_LEDS; col++)
    {
      //Check if a key is pressed
      bool keycheck = keyswitch_pressed[row][col];
      if(keycheck == true)
      {
        led_vals[row][col] = maxVal;
        led_hues[row][col] = random(256);
      }

      LED_RowCol2Matrix(row, col, CHSV(100, 255, led_vals[row][col]));
    }
  }

  //Third step is to propagate any LEDs from a previous keypress
  for (byte row = 0; row < NUM_ROWS_LEDS; row++)                                        //Check through all LEDs in the matrix
  {
    for (byte col = 0; col < NUM_COLS_LEDS; col++)
    {
      if (led_vals[row][col] == spreadVal)                                              //Check if the current brightness of an LED matches the value for color spread
      {
        //Need to check 4 LEDs around this LEDs to see if a spread is available
        int spreadUp = col + 1;
        int spreadDown = col - 1;
        int spreadLeft = row - 1;
        int spreadRight = row + 1;
        
        if(spreadUp < NUM_COLS_LEDS)                                                    //Make sure that the LED actually exists in the matrix
        {
          if(led_vals[row][spreadUp] <= overrideThreshold || led_vals[row][spreadUp] <= minVal)       //Check that current LED brightness is below threshol to overrite
          {
            led_vals[row][spreadUp] = maxVal;                                                         //Set brightness to set max brightness
            led_hues[row][spreadUp] = led_hues[row][col];                                             //set the color to the color that is spreading
          }
        }

        if(spreadDown >= 0)
        {
          if(led_vals[row][spreadDown] <= overrideThreshold || led_vals[row][spreadDown] <= minVal)
          {
            led_vals[row][spreadDown] = maxVal;
            led_hues[row][spreadDown] = led_hues[row][col];
          }
        }

        if(spreadLeft >= 0)
        {
          if(led_vals[spreadLeft][col] <= overrideThreshold || led_vals[spreadLeft][col] <= minVal)
          {
            led_vals[spreadLeft][col] = maxVal;
            led_hues[spreadLeft][col] = led_hues[row][col];
          }
        }

        if(spreadRight < NUM_ROWS_LEDS)
        {
          if(led_vals[spreadRight][col] <= overrideThreshold || led_vals[spreadRight][col] <= minVal)
          {
            led_vals[spreadRight][col] = maxVal;
            led_hues[spreadRight][col] = led_hues[row][col];
          }
        }
      }
      
    }
  }

  //Push all updates to the LED data arrays
  for (byte row = 0; row < NUM_ROWS_LEDS; row++)
  {
    for (byte col = 0; col < NUM_COLS_LEDS; col++)
    {
      LED_RowCol2Matrix(row, col, CHSV(led_hues[row][col], led_sats[row][col], led_vals[row][col]));
    }
  }
}
void LED_AnimSparkle()
{
  //Constants used in this function
  const int sparkleChance = 25;                                                                 //Chance that a new LED is lit up
  const byte minVal = 30;                                                                       //Minimum brightness for each LED
  const byte maxVal = 255;                                                                      //Maximum brightness for each LED
  const byte delta = 5;                                                                         //How much each LED is dimmed per cycle

  //Iterate through each LED to check whether it will be dimmed or a new light is generated
  for (byte row = 0; row < NUM_ROWS_LEDS; row++)
  {
    for (byte col = 0; col < NUM_COLS_LEDS; col++)
    {
      //Check for light level of the LED
      byte currentVal = led_vals[row][col];
      if(currentVal <= minVal)                                                                  //If LED brightness is at minimum, roll for new sparkle
      {
        byte lightChance = random(sparkleChance);                                               //Random roll for if LED will be relit
        if(lightChance == 0)                                                                    //If 0 generated, relight LED
        {
          led_hues[row][col] = random(256);                                                     //Generate a random hue for the new LED
          led_vals[row][col] = maxVal;                                                          //Set its brightness to max
        }
      }
      else                                                                                      //If LED is not at dimmest state, decrement its brightness
      {
        led_vals[row][col] = currentVal - delta;
      }

      LED_RowCol2Matrix(row, col, CHSV(led_hues[row][col], led_sats[row][col], led_vals[row][col]));  //Push changes to the LED data matrix
    }
  }
}
void LED_AnimFunction()
{
  //Variables needed for this function
  static byte valAdjBrightness = 0;                                                             //Persistent variable that stores the brightness adjust indicator LED data
  static byte colorAdjHue = 0;                                                                  //Persistent variable to store color change indicator LED data
  static byte colorModeAdjHue = 0;                                                              //Persistent variable to store lighting mode change LED data
  static byte colorModeAdjVal = 0;                                                              //Persistent variable to store lighting mode change LED data
  const byte delta = 5;                                                                         //How much the animated LEDs will change per cycle

  //Fill the first 8 LEDs with white for the hotkey profiles
  for (byte row = 0; row < 2; row++)
  {
    for (byte col = 0; col < NUM_COLS_LEDS; col++)
    {
      LED_RowCol2Matrix(row, col, CHSV(0, 0, 255));
    }
  }

  //Fill the botttom left key light with rising brightness to show it is brightness adjust
  valAdjBrightness+=delta;
  LED_RowCol2Matrix(2, 0, CHSV(60, 255, valAdjBrightness));

  //Fill second bottom key light next with a rainbow to show how to change color
  colorAdjHue+=delta;
  LED_RowCol2Matrix(2, 1, CHSV(colorAdjHue, 255, 255));

  //Fill third bottom key light with a changing color fade, to show change lighting mode
  if(colorModeAdjVal == 0)
  {
    colorModeAdjHue = random(256);                                                            //Every time the LED is at its dimmest, generate a new color
    colorModeAdjVal = 255;                                                                    //And set its brightness to max
  }
  else
  {
    colorModeAdjVal-=delta;
  }
  LED_RowCol2Matrix(2, 2, CHSV(colorModeAdjHue, 255, colorModeAdjVal));

  //Fill the fourth bottom key light with a blinking red light to enter the menu
  if(colorAdjHue >= 127)
    LED_RowCol2Matrix(2, 3, (CHSV(0, 255, 255)));
  else
    LED_RowCol2Matrix(2, 3, (CHSV(0, 255, 0)));
  
  delay(15);
  
}

void LED_RowCol2Matrix(byte row, byte col, CHSV newColor)
{
  //Convert row and column scheme to a sequential set of LEDs
  //Change the algorithm whether we are on an even or odd row, since LEDs snake back and forth
  int ledNum = 0;
  if((row % 2) != 0)                                                //Check for odd numbered rows
    ledNum = ((NUM_COLS_LEDS * (row + 1)) - 1) - col;               //Use algorithm to determine LED number
  else                                                              //If not odd, even row
    ledNum = (NUM_COLS_LEDS * row) + col;                           //Use adjusted algorithm
  key_leds[ledNum] = newColor;                                      //Store the color in the LED data array
}

void setup() 
{
  //Initialize all pins needed on the Pro Micro
  pinMode(switchRow0, OUTPUT);
  pinMode(switchRow1, OUTPUT);
  pinMode(switchRow2, OUTPUT);
  pinMode(switchCol0, INPUT_PULLUP);
  pinMode(switchCol1, INPUT_PULLUP);
  pinMode(switchCol2, INPUT_PULLUP);
  pinMode(switchCol3, INPUT_PULLUP);
  pinMode(functionBtn, INPUT_PULLUP);

  //Initialize FastLED library and key backlight LEDs
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(key_leds, NUM_LEDS).setCorrection(TypicalSMD5050);   //Main initialization
  for(int row = 0; row < NUM_ROWS_LEDS; row++)                                                        //Fill all led color data arrays with a test color
    for(int col = 0; col < NUM_COLS_LEDS; col++)
    {
      led_hues[row][col] = 120;
      led_sats[row][col] = 255;
      led_vals[row][col] = 255;
    }
  fill_solid(key_leds, NUM_LEDS, CHSV(led_hues[0][0], led_sats[0][0], led_vals[0][0]));               //Set user color data into LED data array
  FastLED.setBrightness(MAX_BRIGHTNESS);                                                              //Set brightness of LEDs to max 
  FastLED.show();                                                                                     //Push color data to LEDs
  delay(2000);                                                                                        //Hold for 2 seconds

  //Initialize keyboard/macro related functionality
  Hotkey_SetGamingProfile();                                                                          //Load a default hotkey scheme at startup
  Keyboard.begin();                                                                                   //Initialize the HID emulation

}

void loop() 
{
  //Define variables needed throughout the loop function
  static unsigned int lastUSBUpdate = 0;                          //Storage for last time USB update was run
  static unsigned int lastLEDUpdate = 0;                          //Storege for last time LEDs were updated
  unsigned int currentTime = millis();                            //Storage of the current system time

  //Check for if user wants to activate special functions of the device by pressing function button
  while(digitalRead(functionBtn) == LOW)
  {
    LED_AnimFunction();                                           //Display custom lighting scheme for when function button is pressed
    FastLED.setBrightness(globalBrightness);                      //Push new LED data to leds
    FastLED.show();
    CheckAllKeyswitches();                                        //Check keys for any pressed
    byte hotkeyCounter = 0;                                       //Create a counter to store which key was pressed
    for(int row = 0; row < NUM_ROWS_KEYS; row++)                 //Iterate through keys pressed array to check for presses
    {
      for(int col = 0; col < NUM_COLS_KEYS; col++)
      {
        if(keyswitch_pressed[row][col] == true)                   //If any key is pressed, handle the key's function accordingly
        {
          keyswitch_pressed[row][col] = false;                    //Clear key pressed flag
          Function_MainHandler(hotkeyCounter);                    //Offload to function handler
        }
        hotkeyCounter++;                                          //Increment the hotkey number each loop
      }
    }

    delay(50);                                                    //Utilize a blocking time delay when holding down the function button
  }

  //If enough time has elapsed, check and send any hotkey press updates to the PC
  if((currentTime - lastUSBUpdate) >= (1000 / USB_POLL_RATE))
  {
    //Check all the keyswitches for activation or deactivation
    CheckAllKeyswitches();
    //Based off current status of the keyswitches, perform hotkey operations
    Hotkey_MainHandler();
    //Update the last time this loop ran
    lastUSBUpdate = currentTime;
  }

  //If enough time has elapsed, push an LED lighting scheme update to the LEDs
  if((currentTime - lastLEDUpdate) >= (1000 / LED_REFRESH_RATE))
  {
    LED_MainHandler();
    FastLED.setBrightness(globalBrightness);
    FastLED.show();
    lastLEDUpdate = currentTime;
  }
}
