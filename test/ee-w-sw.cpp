#include <Arduino.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

// Setting up lcd pins (RS, EN, D4, D5, D6, D7)
//const = used for variable that will never change
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// Create LCD objects with pins defined above
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Watch variables
// const: 
const int btnSetMode = 8;
const int btnMin = 7;
const int btnHour = 6;
const int btnHome = 9;
const int btnRight = 10;

// Button state variables
bool lastBtnMode = false;
bool lastBtnMin = false;
bool lastBtnHour = false;
bool setMode = false;           // Initialize set mode as false state, so that true is reverted (using !setMode) when set mode button is pressed.

// Stopwatch variables
// Arduino memory capability:
// int => 16 bits = -32.768 to 32.767
// unsigned long => 32 bits = 0 to 4.294.967.295
// unsigned:
// long:
unsigned long swLastTime = 0;
int swHundredthSecs = 0;
int swSeconds = 0;
int swMinutes = 0;
bool swScreen = false;
int swState = 0;                // 0 = idle; 1 = running; 2 = stopped
unsigned long totalHundredthSecs;

// Set up time starting in 0
// unsigned:
// long:
unsigned long seconds = 0;
unsigned long minutes = 0;
unsigned long hours = 0;
unsigned long totalSecs;

// EEPROM variables
unsigned long wSavedTime = 0;       // Store the watch time value from EEPROM on start up, used as the time offset.
unsigned long swSavedTime = 0;      // Store the stopwatch value from EEPROM on start up 
unsigned long wLastTime = 0;        // After starting the loop, the watchSavedTime value becomes past = last time.
const int eeaddress = 0;            // Arduino Name address sixe is 1KB = 0 - 1023
const int eeSetMode = 4;            // EEPROM address to save the set mode state
const int eeswAddr = 5;             // EEPROM address to save stopwatch saved time, unsigned long 4 bytes
const int eeswScreen = 9;           // EEPROM address to save the stopwatch screen state -> bool (1 byte)
const int eeswState = 10;           // EEPROM address to sabe the stopwatch state: 0 = idle, 1 = running, 2 = stopped
bool savedSetMode;                  // Variable to restore the set mode bool value state FROM the EEPROM memory in the setup()
bool savedSwScreen;                 // Variable to return the stopwatch bool value saved in the EEPROM
// byte: memory usage 1 byte - 0 - 255
// int: memory usage 16 bits - -32.768 to 32.767  
byte savedSwState;                  // Variable to store the stopwatch state: 0 = idle, 1 = running, 2 = stopped

const unsigned long interval = 60000;    // Value to compare how many time passed (currentTime - wLastTime) and add seconds.
unsigned long lastTime = 0;

// Function to return bool value to set the button state
bool readButton(int pin, bool &lastState) {
    
    // Check the button state:
    // HIGH = true = 1 = not pressed
    // LOW = false = 0 = pressed
    bool currentState = digitalRead(pin);
    // Enters in this if when the following conditions are applied:
    if (currentState != lastState && currentState == LOW) {
        delay(50);                  // delay to debounce the button state (do not update multiple times)
        lastState = currentState;   // last state becomes the current state
        return true;                // Return true when the set mode button is pressed
    }
    lastState = currentState;       // last state becomes the current even when button is not pressed
    return false;                          // Return false when set mode button is not pressed
}

void handleButtons() {
    
    // SET MODE BUTTON:
    if (readButton(btnSetMode, lastBtnMode)) {

        if (!swScreen) {                        // When not in stopwatch screen
            setMode = !setMode;                 // setMode initialized as false is inverted using ! and becomes true
            EEPROM.put(eeSetMode, setMode);     // Save the current set mode state in the eeprom 

            // Time does not run when is set mode
            if (setMode) {              // Condition true (thanks to the !setMode above) to enter in set mode state
                lcd.setCursor(0, 0);
                lcd.print("SET UP");
            } else {
                lcd.setCursor(0, 0);
                lcd.print("      ");    // Run time when not in set mode
            }
        } else {
            lcd.setCursor(20, 0);
            lcd.print("00:00:00");
            if (swState == 0) {                    // 0 swState = idle, resets the time/saved time to calculate time      
                swState = 1;                       // 1 swState = running, becomes 1 = running to satisfy the next condition
                // Save the stopwatch state in the EEPROM (idle, running, stopped), 
                // so the display shows the stopwatch was stopped when started/restarted
                EEPROM.put(eeswState, swState);     
                // all millis() start when Arduino starts up, 
                // therefore this line guarantees the unsigned long swCurrentTime = 0ms, 
                // so that, the stopwatch resets
                // How? If time passed 300.000 millisenconds?
                // unsigned long swCurrentTime = swSavedTime + millis()
                //                             (0UL - 300.000) + 300.000
                // 0UL - 300.000 = -300.000 + 300.000 = 0
                swSavedTime = 0UL - millis();       
                swLastTime = 0;
            } else if (swState == 1) {
                swState = 2;
                // Save the stopwatch state in the EEPROM (idle, running, stopped), 
                // so the display shows the stopwatch was running when started/restarted
                EEPROM.put(eeswState, swState);
            } else {
                // Reset all stopwatch variables as soon as the set mode button changes to 0 = idle 
                swState = 0;
                // Save the stopwatch state in the EEPROM (idle, running, stopped), 
                // so the display shows the stopwatch was stopped when started/restarted
                EEPROM.put(eeswState, swState);                                          
                swHundredthSecs = 0;
                swSeconds = 0;
                swMinutes = 0;
                swSavedTime = 0;
                swLastTime = 0;
                EEPROM.put(eeswAddr, 0UL);         // Reset the EEPROM memory where the stopwatch time is stored as soon as the set mode button to 0  
                lcd.setCursor(20, 0);
                lcd.print("00:00:00");
            }
        }
    }

    // Process minutes/hours button only when setMode == true
    if (setMode && !swScreen) {
        //MINUTES BUTTON
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

    /* SECONDS PROGRESS BAR */

    // Considering the LCD has 16 coluns, the variable filledCells
    // is a direct proportion to find how much seconds can be used to fill a block fully
    int filledCells = (seconds * 16) / 59;
    
    // Set up the lcd to starting filling the block fully
    // at column 0 (1st column), line 1 (2nd line)  
    lcd.setCursor(0, 1);
    // Iterable to go over all cells, from 1 - 16
    for (int cell = 0; cell < 16; cell++) {
        /*

        Condition to determine when the cell will be filled.

        This happens ONLY when reaching 3,75 secs or direct proportion > cell number:
        
        0,27 (1 sec * 16 cells) / 59 secs > i = 1 (cell number)?
            No = write(' ')

        0,54 (2 secs * 16 cells) / 59 secs > i = 1 (cell number)?
            No = write(' ')
        
        1,08 (4 secs * 16 cells) / 59 secs = 1 (cell number)?
            Yes = write(0xFF) fill cell 1 fully

        IMPORTANT NOTE: this example uses float as the filledCells values, but the script uses int
        to round the filledCells values.

        */ 
        if (filledCells > cell) {
            // Datasheet HD44780 — Hitachi says position 255 (decimal and FF hexidecimal) of the LCD
            // is the block fully filled
            // 0x: prefix to indicate the number is Hexidecimal
            lcd.write(0xFF);
        } else {
            lcd.write(' ');
        }
    }
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
    if (swHundredthSecs < 10) {
        lcd.print("0");
    }
    lcd.print(swHundredthSecs);

}

void scrollHome() {

    int readBtnHome = digitalRead(btnHome);

    // Condition to confirm when the button is pressed
    if (readBtnHome == 0) {
        lcd.home();             // Return to watch screen
        // when leaving the stopwatch screen, save the swScreen state:
        // save swScreen state: 
        // false = not on stopwatch screen
        // true = stopwatch screen
        swScreen = false;
        EEPROM.put(eeswScreen, swScreen);             // save the swScreen state in the EEPROM, display IS NOT showing the stopwatch
    }
}

void scrollRight() { 

    int readBtnRight = digitalRead(btnRight);       // Read the button set to scroll the screen to the right

    if (readBtnRight == 0) {                        // button INPUT_PULLUP: 0 = pressed, 1 - not pressed
        for (int posit = 0; posit < 25; posit++) {
            lcd.scrollDisplayRight();               // scroll the display to right until the stopwatch section
            delay(50);
        }
        // when scroll to the stop watch screen, 
        // because this variable starts the script as false,
        // save swScreen state: 
        // false = not on stopwatch screen
        // true = stopwatch screen
        swScreen = true;
        EEPROM.put(eeswScreen, swScreen);               // save the swScreen state the EEPROM memory = display is showing the stopwatch
    }
    // Stopwatch screen
    lcd.setCursor(16, 0);
    lcd.print(" SW ");
}

void setup() {

    // col, line
    lcd.begin(16, 2);
    lcd.clear();
    // Push button pins set up
    // INPUT_PULLUP for push button work in the inversed way:
    // HIGH = true = 1 = not pressed
    // LOW = false = 0 = pressed
    pinMode(btnMin, INPUT_PULLUP);
    pinMode(btnHour, INPUT_PULLUP);
    pinMode(btnSetMode, INPUT_PULLUP);
    pinMode(btnHome, INPUT_PULLUP);
    pinMode(btnRight, INPUT_PULLUP);

    EEPROM.get(eeswAddr, swSavedTime);      // Acess the EEPROM adress to get the stopwatch saved time

    EEPROM.get(eeaddress, wSavedTime);      // Access the eeprom address to get the saved time to start the loop

    /*
    
    SET MODE RETRIEVE FROM EEPROM STATUS:

    Retrieves from EEPROM the last saved setMode state, along with the last saved time.

    If true, sets the setMode variable to true, since it is initialized as false at the beginning of the code.
    
    setMode = true keeps the variable in set mode when entering the loop, because if (!setMode) = false, 
    
    indicates the watch is not set mode, and in that case time runs normally. 

    Then calculates the time saved in EEPROM and shows the "SET UP" message on the display.

    In summary, this block in setup() returns the display to set mode on reboot,
    
    in case the Arduino was powered off while set mode was active.

    Otherwise, if (savedSetMode) is not true, the loop starts normally.
    
    */
   
    EEPROM.get(eeSetMode, savedSetMode);    // Access the setmode address to get the set mode state
    if (savedSetMode) {
        setMode = true;                     
        totalSecs = wSavedTime / 1000;
        hours = (totalSecs / 3600) % 24;
        minutes = (totalSecs / 60) % 60;
        lcd.setCursor(0, 0);
        lcd.print("SET UP");
    }

    // STOPWATCH SCREEN STATE RETRIEVE FROM EEPROM
    EEPROM.get(eeswScreen, savedSwScreen);          // Retrieve the last stopwatch screen state
    if (savedSwScreen) {
        // if the stopwatch state was saved as true, reverts from false 
        // (initialized at the beginning of the script)
        // to true, so that the handleButtons() can be used as the screen is in the stopwatch screen
        swScreen = true;
        for (int posit = 0; posit < 25; posit++) {
            lcd.scrollDisplayRight();               // directly shows stopwatch screen
        }
    }

    /*
    
    STOPWATCH COUNTER TIME STATE RETRIEVE FROM EEPROM

    1. Retrive the stopwatch state (idle, running, stopped) from the EEPROM memory

    2. Condition confirm either the saved sw state was running, or the sw state was stopped:
        - If running, retrieve the time saved and continue to running.
        - If stopped, retrieve the time saved and shows the time stopped

    3. Assign the sw saved state to the original sw state variable.

    4. Calculate how many the hundredth of seconds using the stopwatch saved time
    
    5. Calculate each STOPWATCH time unit

    6. Assign the saved value to last time because it is the time passed until the power supply restarts.
    
    7. Use millis() to guarantee the saved time can be < 1 second becoming more precise,
    including the time passed until starts the loop()
    
    */  
    
    EEPROM.get(eeswState, savedSwState);            
    
    if (savedSwState == 1 || savedSwState == 2) {

        swState = savedSwState;                     

        unsigned long savedTotalHundredthSecs = swSavedTime / 10;   
 
        swMinutes = (savedTotalHundredthSecs / 6000) % 60;
        swSeconds = (savedTotalHundredthSecs / 100) % 60;
        swHundredthSecs = savedTotalHundredthSecs % 100;

        swLastTime = swSavedTime + millis();
    }

}

void loop() {
    
    // When loop starts millis() = 0,    
    unsigned long currentTime = wSavedTime + millis();  // Start the loop with the saved time retrieved above using EEPROM.get() to have the currentTime

    // Condition to save the currentTime value in the EEPROM memory
    if (currentTime - wLastTime >= interval) {
        EEPROM.put(eeaddress, currentTime);             // Save the current time in the EEPROM each 1 minute
        // CurrentTime becomes the past and is assigned as last time to be subtracted with currentTime value, 
        // and to confirm whether or not if 1 minute passed
        wLastTime = currentTime;                     
    }

    handleButtons();

    // Calculate the time when the watch is not in set mode
    if (!setMode) {
        totalSecs = currentTime / 1000;   // currentTime / 1000 because 1 second = 1000 milliseconds
        // totalsecs / 3600 segundos por hora % 24, 
        // hours is limited to 0 - 23
        // So when totalces / 3600 = 24 % 24 = 0 (restarting the hours counting), and 25 % 24 = 1,
        hours   = (totalSecs / 3600) % 24;
        // totalsecs / 60  because 1 minute = 60 seconds
        // and (totalsecs / 60) % 60 because % 60 returns the round result without ,00 
        minutes = (totalSecs / 60) % 60;
        // returns the remainder of the division per 60:
        // 3300 miliseconds / 1000 (1 second = 1000 miliseconds) = 3,3 % 60 = 3 seconds
        seconds = totalSecs % 60;
    } else {
        /*

        Unsigned long because the result of the time calculation can be > 16 bits (32.767)
        3600UL: UL unsigned long => 32 bits 0 to 4.294.967.295
        unsigned long hours * unsigned long = unsigned long
        but if not used UL => unsigned long * int = int
        
        */
        unsigned long newTimeMs = (hours * 3600UL + minutes * 60UL) * 1000UL;
        // newTimeMs - millis(): compensates the time because millis() will be added to wSavedTime to have currentTime
        // Set up time 5 - 2 (time passed since Arduino started) = 3
        // currentTime = 3 + 2 (time passed since Arduino started)
        // currentTime = 5 set up time  
        wSavedTime = newTimeMs - millis();
        EEPROM.put(eeaddress, newTimeMs);       // Save the newTime to the eeprom
        wLastTime = newTimeMs;                  // The new time becomes the last time 
    }

    updateWatch();      // Function to update the lcd display

    scrollRight();      // Function to scroll the lcd to right and access the stopwatch screen

    if (swState == 1) {

        unsigned long swCurrentTime = swSavedTime + millis();

        // Conditin to save the stopwatch time each 1 second
        if (swCurrentTime - swLastTime >= 1000) {
            EEPROM.put(eeswAddr, swCurrentTime);
            swLastTime = swCurrentTime;
        }

        totalHundredthSecs = swCurrentTime / 10;        // Calculate how many hundredth of secons passed

        // Calculate minutes, seconds, and hundredth of seconds:
        // MIN:SECS:HUNDREDTHSECS
        swMinutes = (totalHundredthSecs / 6000) % 60;   
        swSeconds = (totalHundredthSecs / 100) % 60;
        swHundredthSecs = totalHundredthSecs % 100;

    }

    updateStopWatch();

    scrollHome();       // Function to return the lcd to the watch screen 
}
