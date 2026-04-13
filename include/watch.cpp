/*

This include was created with the goal of keeping the project following good development practices,

separating the functions into another folder. However, the functions are also included in the source code,

because the code is being uploaded to the Arduino via the Arduino IDE, since PlatformIO has errors uploading the code to the Arduino.

*/

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

/*

This function reads the button states to add hours, minutes and put the watch in set mode.

It includes debounce to prevent the button state from being updated multiple tomes.

The previous state (lastState) is tracked to detect only HIGH -> LOW transition,
meaning the exact moment the button is pressed.

*/
bool readButton(int pin, bool &lastState) {
    // Check the button state:
    // HIGH = true = 1 = not pressed
    // LOW = false = 0 = pressed
    bool currentState = digitalRead(pin);
    // Condition is true when the button is pressed:
    // currentState = false (button pressed)
    // lastState = true (button not pressed in the previous loop)
    // currentState = LOW (button pressed)
    if (currentState != lastState && currentState == LOW) {
        delay(50);                  // delay to debounce the button state (do not update multiple times)
        lastState = currentState;   // assign the current state value to the last state
        return true;                // return true when button is pressed 
    }
    lastState = currentState;       // assign the current state to the last state though the button was not pressed
    return false;                   // return false when button is not pressed
}

/*

This function handles the btnSetMode button behavior depending on the active screen.

When the watch screen is active, it puts the watch in and out of set mode,
allowing hours and minutes to be adjusted.

When the stopwatch screen is active, there is no set mode.
Instead, it cycles through the stopwatch states: idle -> running -> stopped -> reset.

*/

void handleButtons() {

    //SET MODE BUTTON: 
    if (readButton(btnSetMode, lastBtnMode)) {

        // Condition to put the watch into set mode state
        // when the LCD is not in the stopwatch screen.
        if (!swScreen) {
            // When not in the stopwatch screen, the setMode variable must be toggled (since it starts as false),
            // meaning it is inverted to true to put the watch in SET UP state and allow hours and minites
            // to be incremented to update the watch.
            setMode = !setMode;

            // Time does not run when in set mode
            if (setMode) {
                lcd.setCursor(0, 0);
                lcd.print("SET UP");
            } else {                         
                lcd.setCursor(0, 0);
                lcd.print("         ");     // Run time when not in set mode
            }  
        } else {                        // When the stopwatch is active, each btnSetMode press cycles through 3 states:                        
            lcd.setCursor(20, 0);       // set cursor to the stopwatch position on the LCD
            lcd.print("00:00:00");      // Put the stopwatch to 0 when switch to stopwatch screen
            if (swState == 0) {         // Idle state: not started yet or was reset
                swState = 1;            // set to rununing state, loop() starts counting 
                swLastTime = millis();  
            } else if (swState == 1) {  
                swState = 2;            // When in running state, invert the swState to 2 (stopped)
            } else {                    // When reset state (0), reset the stopwatch 
                swState = 0;
                swHundredsthSecs = 0;
                swSeconds = 0;
                swMinutes = 0;
                lcd.setCursor(20, 0);
                lcd.print("00:00:00");
            }
        }
    }

    // Process minutes/hours button only when SetMode == true
    if (setMode && !swScreen) {
        // INCREMENT MINUTES
        if (readButton(btnMin, lastBtnMin)) {
            minutes++;  
            if (minutes >= 60) {
                minutes = 0;        // Return to 0 when reaches 60
            }
            // Reset seconds when adjusting minutes
            seconds = 0;
        }
        // INCREMENT HOURS
        if (readButton(btnHour, lastBtnHour)) {
            hours++;
            if (hours >= 24) {
                hours = 0;          // Return to 0 when reaches 24
            }
        }
    }
}

void updateWatch() {
    // Set the seconds position on the display
    lcd.setCursor(13, 0);
    if (seconds < 10) {
        lcd.print("0");
    }
    lcd.print(seconds);

    lcd.setCursor(12, 0);   // Set the separator position between seconds and minutes
    lcd.print(":");

    // Set the minutes position on the display
    lcd.setCursor(10, 0);
    if (minutes < 10) {
        lcd.print("0");
    }
    lcd.print(minutes);

    lcd.setCursor(9, 0);    // Set the separator position between minutes and hours
    lcd.print(":");

    // Set the hours position on the display
    lcd.setCursor(7, 0);
    if (hours < 10) {
        lcd.print("0");
    }
    lcd.print(hours);
}

void updateStopWatch() {

    // Set stopwatch position
    lcd.setCursor(20, 0);
    if (swMinutes < 10) {
        lcd.print("0");
    }
    lcd.println(swMinutes);

    lcd.setCursor(22, 0);   // Set separator position between swMinutes and swSeconds
    lcd.print(":");

    // Set seconds position
    lcd.setCursor(23, 0);
    if (swSeconds < 10) {
        lcd.print("0");
    }
    lcd.print(swSeconds);

    lcd.setCursor(25, 0);
    lcd.print(":");         // Set separator between swSeconds and swHundredsthSecs

    // Set sw hundredsth of seconds position
    lcd.setCursor(26, 0);
    if (swHundredsthSecs < 10) {
        lcd.print("0");
    }
    lcd.print(swHundredsthSecs);
}

void scrollHome() {

    int readBtnLeft = digitalRead(btnHome);  
    
    // Condition to confirm when the button is pressed
    if (readBtnLeft == 0) {
        lcd.home();             // Return to watch screen
        swScreen = false;        // set to false to indicate the stopwatch was left and return to watch screen (main screen)
    }
}

void scrollRight() {

    int readBtnRight = digitalRead(btnRight);

    if (readBtnRight == 0) {
        for (int posit = 0; posit < 25; posit++) {
            lcd.scrollDisplayRight();
            delay(50);
        }
        swScreen = true;        // set to true to indicate the stopwatch screen is active
    }

    // StopWatch screen
    lcd.setCursor(16, 0);
    lcd.print(" SW ");
}

