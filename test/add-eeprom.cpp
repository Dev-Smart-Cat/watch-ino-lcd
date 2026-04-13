/*

The circuit has a problem with the power supply. The power supply is low-cost, purchased only for learning circuit development.

The problem occurs when the circuit is moved (sometimes not so abruptly), causing the power supply to turn on and off. 

This happens because the power supply is low-cost as mentioned, and the batteries inside the case are a bit loose. 

I tried to fix it by tigthing the batteries inside the case, but the problem persisted.

The solution found was to use the Arduino's EEPROM to save the last time recorded on the LCD display at the moment the power is cut.

Immediately after restarting, the display shows the last time saved in the EEPROM memory.

This code was used to run tests and confirm the functionality of this approach, and whether it would be applicable to this project. 

The test results can be seen in the videos in the demos directory.

*/


#include <EEPROM.h>
#include <Arduino.h>
#include <LiquidCrystal.h>

// Setting up lcd pins (RS, EN, D4, D5, D6, D7)
//const = used for variable that will never change
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
// Create LCD objects with pins defined above
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// EEPROM addres stuck at 0 position
const int EEPROM_ADDR = 0;

unsigned long startSavedTime = 0;             // Stores the time value read from EEPROM on start up, used as the time offset. 
unsigned long lastSavedTime = 0;              // After starting the loop, the startSavedTime value becomes last time
const unsigned long interval = 1000;          // Value to compare the result between current time and last time, and increment time when the result is >= the interval 

void setup() {
    lcd.begin(16, 2);   // Initialize the lcd display
    lcd.clear();        // Make sure the lcd display is clear during start up

    /*

    The get() method allows reading data saved in the Arduino's EEPROM memory.

    In this specific case, this function retrieves from memory the saved time every time the Arduino starts up.
    
    */

    EEPROM.get(EEPROM_ADDR, startSavedTime);
}

void loop() {

    /*
    Condition to store the currentTime value in the EEPROM, how that works?

    currentTime 500 = startSavedTime 0 + millis() 500
    currentTime 500 - lastSavedTime 0 >= interval 1000
    500 - 0 = 500
    is 500 >= 1000 (interval)?
    false
    -------------------------

    currentTime 1000 = startSavedTime 0 + millis() 1000
    currentTime - lastSavedTime >= interval
    1000 - 0 = 1000
    is 1000 >= 1000 (interval)?
    true
    When true, save the currentTime value in the EEPROM memory
    currentTime = 1000
    Then, the 1000 becomes the lastSavedTime
    LCD displays => 1 second
    -------------------------

    Power supply moved at 1 second, Arduino reinitializes:
    get() the value saved in the address EEPROM_ADDR and assign to the variable startSavedTime = 1000
    millis() reinitialize = 0
    currentTime = startSavedTime + millis()
    1000 = 1000 + 0
    -------------------------

    currentTime - lastSavedTime >= interval
    1000 - 0 = 1000
    is 1000 >= 1000 (interval)
    true
    LCD displays => 1 second (where the power supply was cut off). 

    */

    unsigned long currentTime = startSavedTime + millis();

    if (currentTime - lastSavedTime >= interval) {
        EEPROM.put(EEPROM_ADDR, currentTime);
        lastSavedTime = currentTime;
    }

    unsigned long totalsecs = currentTime / 1000;       // Current time value used to calclate the total seconds
    unsigned long mins = totalsecs / 60;                // Calculate minutes
    unsigned secs = totalsecs % 60;                     // Remaining division value becomes the seconds
    

    // The displays shows the time
    lcd.setCursor(0, 0);
    if (mins < 10) lcd.print("0");
    lcd.print(mins);
    lcd.print(":");
    if (secs < 10) lcd.print("0");
    lcd.print(secs);
}

