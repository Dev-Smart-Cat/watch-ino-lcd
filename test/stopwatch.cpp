#include <Arduino.h>
#include <LiquidCrystal.h>

// Setting up lcd pins (RS, EN, D4, D5, D6, D7)
//const = used for variable that will never change
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// Create LCD objects with pins defined above
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Watch variables
const int btnSetMode = 8; 
const int btnMin = 7;      
const int btnHour = 6;
const int btnLeft = 9;
const int btnRight = 10;

// Button state variables
bool lastBtnMode = false; 
bool lastBtnMin = false;
bool lastBtnHour = false;    
bool setMode = false;       // Initialize set mode as false state, so that true is reverted (using !setMode) when the set mode button mode is pressed. 

// Stopwath variables
unsigned long swLastTime = 0;
int swHundredsthSecs = 0;
int swSeconds = 0;
int swMinutes = 0;
bool swScreen = false;
int swState = 0;        // 0 = idle; 1 = running; 2 = stopped

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

        if (!swScreen) {
            setMode = !setMode;
            
            // Time does not run when in set mode
            if (setMode) {
                lcd.setCursor(0, 0);
                lcd.print("SET UP");
            } else {
                lcd.setCursor(0, 0);
                lcd.print("      ");    // Run time when not in set mode
            }
        } else {
            lcd.setCursor(20, 0);
            lcd.print("00:00:00");
            if (swState == 0) {
                swState = 1;
                swLastTime = millis();
            } else if (swState == 1) {
                swState = 2;
            } else {
                swState = 0;
                swHundredsthSecs= 0;
                swSeconds = 0;
                swMinutes = 0;
                lcd.setCursor(20, 0);
                lcd.print("00:00:00");
                
            }
        }
    }

    // Process minutes/hours button only when SetMode == true
    if (setMode && !swScreen) {
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

void updateWatch() {
    // Set the SECONDS position on the display
    lcd.setCursor(13, 0);
    if (seconds < 10) {
        lcd.print("0");
    }
    lcd.print(seconds);

    lcd.setCursor(12, 0); // Set the separator position between seconds and minutes
    lcd.print(":");

    // Set the MINUTES position on the display
    lcd.setCursor(10, 0);
    if (minutes < 10) {
        lcd.print("0");
    }
    lcd.print(minutes);

    lcd.setCursor(9, 0); // Set the separator position between the minutes and hours
    lcd.print(":");

    // Set the HOURS position on the display
    lcd.setCursor(7, 0);
    if (hours < 10) {
        lcd.print("0");
    }
    lcd.print(hours);
}

void updateStopWatch() {
    // Set SW MINUTES position
    lcd.setCursor(20, 0);
    if (swMinutes < 10) {
        lcd.print("0");
    }
    lcd.print(swMinutes);

    lcd.setCursor(22, 0); // Set SEPARATOR position between swMinutes and swSeconds
    lcd.print(":");

    // Set SW SECONDS position
    lcd.setCursor(23, 0);
    if (swSeconds < 10) {
        lcd.print("0");
    }
    lcd.print(swSeconds);

    lcd.setCursor(25, 0);
    lcd.print(":");     // Set SEPARATOR between swSeconds and swHundredsthSecs

    // Set SW HUNDREDSTH of Secs position
    lcd.setCursor(26, 0);
    if (swHundredsthSecs < 10) {
        lcd.print("0");
    }
    lcd.print(swHundredsthSecs);

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
        updateWatch();   
    }

    int readBtnRight = digitalRead(btnRight);

    if (readBtnRight == 0) {
        for (int posCounter = 0; posCounter < 25; posCounter++) {
            lcd.scrollDisplayRight();
            delay(50);
        }
        swScreen = true;        // LCD is in the stop watch screen
    }

    // StopWatch
    lcd.setCursor(16, 0);
    lcd.print(" SW ");

    // Starts counter only when swState == 1 (running)
    if (swState == 1) {
        unsigned long swCurrrentTime = millis();
        // Stop watch shows MIN:SECS:HUNDSECS: 
        // 1 sec = 100 hundreth of secs
        // 100 hundreth of secs = 10 miliseconds
        // 1 sec = 1000 miliseconds
        // 10 milisecs - 0 last time >= 10: false
        // 11 milisecs - 0 last time >= 10: true, then enters in this condition
        // 21 milisecs - 11 last time >= 10: true, then enters in this condition
        if (swCurrrentTime - swLastTime >= 10) {
            swLastTime = swCurrrentTime;        // 0 becomes 1100
            
            swHundredsthSecs++;                 // 1 hudredth of seconds
            
            if (swHundredsthSecs >= 100) {
                swSeconds++;            // Add 1 sec when swHundredth of secs reaches 100 hundredth of secs
                swHundredsthSecs = 0;   // Return swHundredth of secs to 0
            }

            if (swSeconds >= 60 ) {
                swMinutes++;            // Add 1 min when swSeconds reaches 60 secs
                swSeconds = 0;          // Return swSeconds to 0
            }

            if (swMinutes >= 60) {
                swMinutes = 0;          // Return minutes to 0 when swMinutes reaches 0  
            }
            
            updateStopWatch();
        }
    }

    int readBtnLeft = digitalRead(btnLeft);

    if (readBtnLeft == 0) {
        lcd.home();
        swScreen = false;       // LCD left stopwatch screen and return to watch screen
    }
}
