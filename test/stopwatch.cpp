#include <Arduino.h>
#include <LiquidCrystal.h>

// Setting up lcd pns (RS, EN, D4, D5, D6, D7)
//const = used for variable that will never change
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// Create LCD objects with pins defined above
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int btnSetMode = 6; 
const int btnMin = 7;      
const int btnHour = 8;
const int btnLeft = 9;
const int btnRight = 10;

bool lastBtnMode = false; 
bool lastBtnMin = false;
bool lastBtnHour = false;    
bool setMode = false;       // Initialize set mode as false state, so that true is reverted (using !setMode) when the set mode button mode is pressed. 

// Set up time starting in 0
int seconds = 0;
int minutes = 0;
int hours = 0;

const unsigned long interval = 1000;
unsigned long lastTime = 0;

// Function to return bool value to set the button stated
bool readButton(int pin, bool &lastState) {

    // Check the button state:
    // HIGH = true = 1 = not pressed
    // LOW = false = 0 = pressed
    bool currentState = digitalRead(pin);
    // Enters in this if when the following conditions are applied 
    if (currentState != lastState && currentState == LOW) {
        // delay to debounce the button state (do not update multiple times).
        delay(50);
        // last state  becomes the current state
        lastState = currentState;
        // Return true when the set mode button is pressed.
        return true;
    }
    // last state becomes the current even when the button is not pressed.
    lastState = currentState;
    // Return false when set mode button is not pressed.
    return false;
}

void handleButtons() {

    // SET MODE BUTTON:
    if (readButton(btnSetMode, lastBtnMode)) {

        setMode = !setMode;

        if (setMode) {
            lcd.setCursor(0, 0);
            lcd.print("SET UP");
        } else {
            lcd.setCursor(0, 0);
            lcd.print("      ");
        }
    }

    // Process minutes/hours button only when SetMode == true
    if (setMode) {
        // MINUTES BUTTON
        if (readButton(btnMin, lastBtnMin)) {
            minutes++;
            if (minutes >= 60) {
                minutes = 0;
            }
            // Reset seconds when adjusting minutes
            seconds = 0;
        }
        // HOURS BUTTON
        if (readButton(btnHour, lastBtnHour)) {
            hours++;
            if (hours >= 24) {
                hours = 0;
            }
        }
    }
}

void updateDisplay() {
    // Set the SECONDS position on the display
    lcd.setCursor(13, 0);
    if (seconds < 10) {
        lcd.print("0");
    }
    lcd.print(seconds);

    // Set the separator position between seconds and minutes
    lcd.setCursor(12, 0);
    lcd.print(":");

    // Set the MINUTES position on the display
    lcd.setCursor(10, 0);
    if (minutes < 10) {
        lcd.print("0");
    }
    lcd.print(minutes);

    // Set the separator position between the minutes and hours
    lcd.setCursor(9, 0);
    lcd.print(":");

    // Set the HOURS position on the display
    lcd.setCursor(7, 0);
    if (hours < 10) {
        lcd.print("0");
    }
    lcd.print(hours);
}

void setup() {

  // col, line
  lcd.begin(16, 2);
  lcd.clear();
  // INPUT_PULLUP
  // HIGH = true = 1 = not pressed
  // LOW = false = 0 = pressed
  pinMode(btnMin, INPUT_PULLUP);
  pinMode(btnHour, INPUT_PULLUP);
  pinMode(btnSetMode, INPUT_PULLUP);
  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);

}

void loop() {

    unsigned long currentTime = millis();

    if (currentTime - lastTime >= interval) {

        lastTime = currentTime;

        handleButtons();

        // Run time when not in set mode

        if (!setMode) {
            seconds++;

            if (seconds >= 60) {
                seconds = 0;
                minutes++;
            }

            if (minutes >= 60) {
                minutes = 0;
                hours++;
            }

            if (hours >= 24) {
                hours = 0;
            }
        }
        updateDisplay();   
    }

    int readBtnRight = digitalRead(btnRight);

    if (readBtnRight == 0) {
        for (int posCounter = 0; posCounter < 25; posCounter++) {
            lcd.scrollDisplayRight();
            delay(50);
        }
    }

    lcd.setCursor(16, 0);
    lcd.print(" StopWatch: ");
    lcd.setCursor(28, 0);
    lcd.print("28");

    int readBtnLeft = digitalRead(btnLeft);

    if (readBtnLeft == 0) {
        lcd.home();
        }
}