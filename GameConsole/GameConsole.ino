#include <LiquidCrystal.h>

// Constant for lcd size
#define LCDWIDTH 16
#define LCDHEIGHT 2

// Start by defining the relationship between
//       note, period, &  frequency.
#define c 3830 // 261 Hz
#define d 3400 // 294 Hz
#define e 3038 // 329 Hz
#define f 2864 // 349 Hz
#define g 2550 // 392 Hz
#define a 2272 // 440 Hz
#define b 2028 // 493 Hz
#define C 1912 // 523 Hz
// Define a special note, 'R', to represent a rest
#define R 0

enum State
{
  HOME,
  TRIVIA,
  CREDITS
};

// Constant pins
const int homeBtn = 8;
const int rightBtn = 13;
const int leftBtn = 7;
const int speakerPin = 9;

//const int marioMusic = [ e, e, e, c, e, g, g ];
int marioMusic[] = {  C,  b,  g,  C,  b,   e,  R,  C,  c,  g, a, C };
int marioBeats[]  = { 16, 16, 16,  8,  8,  16, 32, 16, 16, 16, 8, 8 }; 
int MAX_COUNT = sizeof(marioMusic) / 2; // Melody length, for looping.

// Which app is the console currently in
State consoleState = HOME;
// The app gets one int to store all of it's data. Use it wisely
int appData;
// Used by the displayText function to prevent displaying the same text
int lastTextId;
// Used by the button manager to figure out buttonPress and buttonRelease
int buttonState[3];
int lastButtonState[3];

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup()
{
  consoleState = HOME;
  Serial.begin(9600);
  Serial.println("Initializing Hardware...");

  // Buttons
  pinMode(homeBtn, INPUT);
  pinMode(rightBtn, INPUT);
  pinMode(leftBtn, INPUT);
  // Speaker
  pinMode(speakerPin, OUTPUT);

  // Setup LCD
  lcd.begin(LCDWIDTH, LCDHEIGHT);
  lcd.clear();
  displayText("Hello World!", "Starting...", 1);

  // Reset Variables
  lastTextId = 0;
  appData = 0;
  consoleState = HOME;

  // Finish everything
  delay(500);
  Serial.println("Done intializing.");
}

void loop()
{
  // Button Press logic
  buttonManager();

  switch (consoleState)
  {
  case HOME:
    homeLoop();
    break;
  case TRIVIA:
    homeButton();
    triviaLoop();
    break;
  default:
    homeLoop();
  }
}

#pragma region Button Manager &Press / Release System

void buttonManager()
{
  // Home Btn check
  int homeState = digitalRead(homeBtn);
  // If currently pressed and wasn't pressed earlier
  if (homeState && lastButtonState[0] == 0)
  {
    // Set it as pressed
    buttonState[0] = 1;
  }
  else if (homeState && lastButtonState[0] == 1)
  {
    // Set it as button hold
    buttonState[0] = 3;
  }
  else if (!homeState && lastButtonState[0] == 1)
  {
    // Set it as button release
    buttonState[0] = 2;
  }
  else
  {
    buttonState[0] = 0;
  }
  lastButtonState[0] = homeState;

  // Left Btn check
  int leftState = digitalRead(leftBtn);
  // If currently pressed and wasn't pressed earlier
  if (leftState && lastButtonState[1] == 0)
  {
    // Set it as pressed
    buttonState[1] = 1;
  }
  else if (leftState && lastButtonState[1] == 1)
  {
    // Set it as button hold
    buttonState[1] = 3;
  }
  else if (!leftState && lastButtonState[1] == 1)
  {
    // Set it as button release
    buttonState[1] = 2;
  }
  else
  {
    buttonState[1] = 0;
  }
  lastButtonState[1] = leftState;

  // Right Btn check
  int rightState = digitalRead(rightBtn);
  // If currently pressed and wasn't pressed earlier
  if (rightState && lastButtonState[2] == 0)
  {
    // Set it as pressed
    buttonState[2] = 1;
  }
  else if (rightState && lastButtonState[2] == 1)
  {
    // Set it as button hold
    buttonState[2] = 3;
  }
  else if (!rightState && lastButtonState[2] == 1)
  {
    // Set it as button release
    buttonState[2] = 2;
  }
  else
  {
    buttonState[2] = 0;
  }
  lastButtonState[2] = rightState;
}

// Currently pressed, last loop iteration was not pressed
int buttonPress(int btnId)
{
  if (btnId == homeBtn)
  {
    return buttonState[0] == 1;
  }
  else if (btnId == leftBtn)
  {
    return buttonState[1] == 1;
  }
  else if (btnId == rightBtn)
  {
    return buttonState[2] == 1;
  }
  else
  {
    Serial.print("No button found with id ");
    Serial.println(btnId);
  }
}

// Currently pressed, last loop iteration was pressed
int buttonHold(int btnId)
{
  if (btnId == homeBtn)
  {
    return buttonState[0] == 3;
  }
  else if (btnId == leftBtn)
  {
    return buttonState[1] == 3;
  }
  else if (btnId == rightBtn)
  {
    return buttonState[2] == 3;
  }
  else
  {
    Serial.print("No button found with id ");
    Serial.println(btnId);
  }
}

// Currently not pressed, last loop iteration was pressed
int buttonRelease(int btnId)
{
  if (btnId == homeBtn)
  {
    return buttonState[0] == 2;
  }
  else if (btnId == leftBtn)
  {
    return buttonState[1] == 2;
  }
  else if (btnId == rightBtn)
  {
    return buttonState[2] == 2;
  }
  else
  {
    Serial.print("No button found with id ");
    Serial.println(btnId);
  }
}

#pragma endregion

// Checks if the home btn was pressed and goes to home if so
void homeButton()
{
  if (buttonPress(homeBtn))
  {
    delay(50);
    appData = 0;
    consoleState = HOME;
  }
}

// Home are id 0-99
void homeLoop()
{
  // 0 = immediately move to Trivia
  if (appData == 0)
  {
    displayText("< Game Console >", "", 1);
    delay(100);
    // If left/right buttons are clicked, switch between apps
    if (buttonPress(leftBtn))
    {
      delay(50);
      appData = 2;
    }
    if (buttonPress(rightBtn))
    {
      delay(50);
      appData = 1;
    }
  }
  // 1 = Trivia
  else if (appData == 1)
  {
    displayText("<    Trivia    >", "Test your mind", 1);
    delay(100);
    // If left/right buttons are clicked, switch between apps
    if (buttonPress(leftBtn))
    {
      delay(50);
      appData = 0;
    }
    if (buttonPress(rightBtn))
    {
      delay(50);
      appData = 2;
    }
    // If home button is clicked, launch the app
    else if (buttonPress(homeBtn))
    {
      delay(50);
      appData = 0;
      consoleState = TRIVIA;
    }
  }
  // 2 = Credits
  else if (appData == 2)
  {
    displayText("Made by Reva", "Miguel, Sohail", 2);
    delay(100);
    // If left/right buttons are clicked, switch between apps
    if (buttonPress(leftBtn))
    {
      delay(50);
      appData = 1;
    }
    if (buttonPress(rightBtn))
    {
      delay(50);
      appData = 0;
    }
  }
  // If something went wrong...
  else
  {
    displayText("Oops...", "Wrong Screen!", 3);
    appData = 1;
  }
}

// Trivia are id 100-199
void triviaLoop()
{
  if (appData == 0)
  {
    displayText("Welcome 2 Trivia", "Press [L] or [R]", 100);
    if (buttonPress(leftBtn) || buttonPress(rightBtn))
    {
      delay(50);
      appData = 1;
    }
  }
  else if (appData == 1)
  {
    displayText("Queen's age?", "[92]      [73]", 105);
    delay(100);
    if (buttonPress(leftBtn)) {
      delay(50);
      appData = 2;
    }
    if (buttonPress(rightBtn))
    {
      delay(50);
      appData = 3;
    }
  }
  else if (appData == 2)
  {
    displayText("Correct!", "", 101);
    delay(100);
    if (buttonPress(leftBtn) || buttonPress(rightBtn))
    {
      delay(50);
      appData = 4;
    }
  }
  else if (appData == 3)
  {
    displayText("Wrong!", "", 102);
    delay(100);
    if (buttonPress(leftBtn) || buttonPress(rightBtn))
    {
      delay(50);
      appData = 4;
    }
  }
  else if (appData == 4)
  {
    displayText("# of US States?", "[1]  [Home]  [2]", 106);
    delay(100);
    if (buttonPress(leftBtn)) {
      delay(50);
      appData = 5;
    }
    if (buttonPress(rightBtn))
    {
      delay(50);
      appData = 6;
    }
  }
  else if (appData == 5)
  {
    displayText("Wrong!", "", 103);
    delay(100);
    if (buttonPress(leftBtn) || buttonPress(rightBtn))
    {
      delay(50);
      appData = 7;
    }
  }
  else if (appData == 6)
  {
    displayText("Correct!", "", 104);
    delay(100);
    if (buttonPress(leftBtn) || buttonPress(rightBtn))
    {
      delay(50);
      appData = 7;
    }
  }
   else if (appData == 7)
  {
    displayText("Sohail's fav #", "[1]  [Home]  [2]", 116);
    delay(100);
    if (buttonPress(leftBtn)) {
      delay(50);
      appData = 8;
    }
    if (buttonPress(rightBtn)) {
      delay(50);
      appData = 9;
    }
  }
  else if (appData == 8)
  {
    displayText("Correct!", "", 110);
    delay(100);
    if (buttonPress(leftBtn) || buttonPress(rightBtn))
    {
      delay(50);
      appData = 10;
    }
  }
  else if (appData == 9)
  {
    displayText("Wrong!", "", 115);
    delay(100);
    if (buttonPress(leftBtn) || buttonPress(rightBtn))
    {
      delay(50);
      appData = 10;
    }
  }
}

#pragma region Display Utilities
// Display Text, use textId to prevent the same text from refreshing multiple times
void displayText(char topLine[], char botLine[], int textId)
{
  if (textId != lastTextId)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(topLine);
    lcd.setCursor(0, 1);
    lcd.print(botLine);
    lastTextId = textId;
  }
}

#pragma endregion