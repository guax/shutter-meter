/* Shutter speed meter for analog cameras
 * 
 * Assumes you're using a 16x2 display and standard phototransistor
 * uses one extra button for reset of averages.
 * 
 * Author: Henrique Grolli Bassotto
 * Credits:
 * ema_xiety for inspiration and example code/hardware
 *           https://www.instructables.com/id/Arduino-Film-Camera-Shutter-Checker/
 * Ingrid Cezario for constructive criticism and great ideas.
 */
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"

// RS, E, D4, D5, D6, D7
Adafruit_LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

int photoTransistorPin = 6;
int resetButtonPin = 5;


bool sensorSignal; // a bolean used to read the state of the sensor
bool resetButonPressed;
unsigned long shutterStart, shutterEnd;

double measuredValue;
bool displayed;

int measurementCount;
double measurementSum;

void setup() {
    lcd.begin(16, 2);
    pinMode(photoTransistorPin, INPUT_PULLUP);
    pinMode(resetButtonPin, INPUT_PULLUP);
    delay(200); // Allow some time for sensor to clearup

    measurementCount = 0;
    measurementSum = 0;


    lcd.clear();
    lcd.print("Shutter Meter v1");
    lcd.setCursor(0, 1);
    lcd.print("Henrique Grolli");
    delay(1000);

    lcd.clear();
    lcd.print("Waiting");
    lcd.setCursor(0, 1);
    lcd.print("measurement");
    displayed = true;
}

void loop() {
    resetButonPressed = digitalRead(resetButtonPin);
    if (!resetButonPressed) {
        delay(100); // Waiting 100 millis to debounce
        while(!resetButonPressed) {
            // Wait for it to bounce back up in case user is holding it
            resetButonPressed = digitalRead(resetButtonPin);
            delay(100);
        }
        lcd.clear();
        lcd.print("Waiting");
        lcd.setCursor(0, 1);
        lcd.print("measurement");
        displayed = true;
    }
    sensorSignal = digitalRead(photoTransistorPin);
    if (!sensorSignal){
        // We only care when the signal goes to LOW
        shutterStart = micros();
        while (!sensorSignal) {
            // Do absolutely nothing until the signal goes back up
            shutterEnd = micros();
            sensorSignal = digitalRead(photoTransistorPin);
        }
        displayed = false;
        measuredValue = (shutterEnd - shutterStart) / 1000.0; // Need to divide by a float to avoid int casting
        measurementSum += measuredValue;
        measurementCount++;
    }

    if (measuredValue && !displayed) {
        displayed = true;
        lcd.clear();
        lcd.setCursor(0, 0);
        double avg_measurement = measurementSum / float(measurementCount);
        String avg_measurement_fraction = String(1000.0/avg_measurement, 1);
        lcd.print("1/" + avg_measurement_fraction);
        lcd.setCursor(14-avg_measurement_fraction.length(), 0);
        if (avg_measurement < 100.0) {
            lcd.setCursor(14-String(avg_measurement, 2).length(), 0);
            lcd.print(String(avg_measurement, 2) + "ms");
        } else {
            lcd.setCursor(14-String(int(avg_measurement), DEC).length(), 0);
            lcd.print(String(int(avg_measurement), DEC) + "ms");
        }
        lcd.setCursor(0, 1);
        lcd.print("1/" + String(1000.0/measuredValue, 1));
        if (measuredValue < 100.0) {
            lcd.setCursor(14-String(measuredValue, 2).length(), 1);
            lcd.print(String(measuredValue, 2) + "ms");
        } else {
            lcd.setCursor(14-String(int(measuredValue), DEC).length(), 1);
            lcd.print(String(int(measuredValue), DEC) + "ms");
        }
    }
}
