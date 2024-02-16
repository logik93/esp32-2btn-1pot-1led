#include <Arduino.h>
#include <BleGamepad.h>

#define BUTTON_1_PIN 25
#define BUTTON_2_PIN 26
#define LED_PIN 13

BleGamepad bleGamepad("BLE Driving Controller", "lemmingDev", 100);

const byte buttonPins[] = {BUTTON_1_PIN, BUTTON_2_PIN};
const int potPin = 34;                
const int numberOfPotSamples = 5;     
const int delayBetweenHIDReports = 5; 

bool currentButtonStates[sizeof(buttonPins)];
bool previousButtonStates[sizeof(buttonPins)];

void setup()
{
    Serial.begin(115200);
    pinMode(buttonPins[0], INPUT_PULLUP);
    pinMode(buttonPins[1], INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);

    BleGamepadConfiguration bleGamepadConfig;
    bleGamepadConfig.setAutoReport(false);
    bleGamepadConfig.setButtonCount(sizeof(buttonPins));
    bleGamepad.begin(&bleGamepadConfig);
}

void loop()
{
    if (bleGamepad.isConnected())
    {
        digitalWrite(LED_PIN, LOW); // Turn on LED when connected
    }
    else
    {
        // Blink LED quickly if not connected
        digitalWrite(LED_PIN, millis() % 500 < 250 ? HIGH : LOW);
    }

    // Handling Potentiometer
    int potValue = 0;
    for (int i = 0; i < numberOfPotSamples; i++)
    {
        potValue += analogRead(potPin);
    }
    potValue /= numberOfPotSamples;
    int adjustedValue = map(potValue, 0, 4095, 32767, 0); // Adjusting the value range

    bleGamepad.setX(adjustedValue);
    bleGamepad.sendReport();

    // Handling Buttons
    for (byte currentIndex = 0; currentIndex < sizeof(buttonPins); currentIndex++)
    {
        currentButtonStates[currentIndex] = digitalRead(buttonPins[currentIndex]);

        if (currentButtonStates[currentIndex] != previousButtonStates[currentIndex])
        {
            if (currentButtonStates[currentIndex] == LOW)
            {
                bleGamepad.press(currentIndex + 1);
            }
            else
            {
                bleGamepad.release(currentIndex + 1);
            }
        }
    }

    bool buttonsChanged = false;
    for (byte currentIndex = 0; currentIndex < sizeof(buttonPins); currentIndex++)
    {
        if (currentButtonStates[currentIndex] != previousButtonStates[currentIndex])
        {
            buttonsChanged = true;
            break;
        }
    }

    if (buttonsChanged)
    {
        for (byte currentIndex = 0; currentIndex < sizeof(buttonPins); currentIndex++)
        {
            previousButtonStates[currentIndex] = currentButtonStates[currentIndex];
        }

        bleGamepad.sendReport();
    }
}
