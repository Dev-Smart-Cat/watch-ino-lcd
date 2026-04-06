#include <Arduino.h>
#include <LiquidCrystal.h>

// Setting up lcd pins (RS, EN, D4, D5, D6, D7)
// const (key word) => used for variable cannot be changed once they are initialized.
// const cannot be assigned a new value later
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// Create LCD objects with pins defined above
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Watch variables
const int btnSetMode = 8;
const int btnMin = 7;
const int btnHour = 6;
const int btnHome = 9;
const int btnRight = 10;

// Button state variables
bool lastBtnMode = false;
bool lastBtnMin = false;
bool lastBtnHour = false;
bool setMode = false;       // Initialize set mode as false state, so that true is reverted (using !setMode) when set mode button is pressed.

//Stopwatch variables
// unsigned => indicate that the variable can only store non-negative values. 
// A regular int goes from -2147483648 to 2147483647. 
// A unsigned int goes from 0 to 4294967295.
unsigned long swLastTime = 0;       
int swHundredsthSecs = 0;          
int swSeconds = 0;
int swMinutes = 0;
bool swScreen = false;
int swState = 0;            // 0 = sw idle; 1 = sw running; 2 = sw stopped

// Set up time starting in 0
int seconds = 0;
int minutes = 0;
int hours = 0;

// long => 32 bits, allows millis() to reach 4,294,967,293 ms = 49 days.
// int => 16 bits, allows only from 0 to 65,535 ms. 
const unsigned long interval = 1000;    // Value to compare the result between current time and last time and increment time when the result is >= this value
unsigned long lastTime = 0;             // Value that initializes the time


