// FastLED
#include <FastLED.h>

// Define the number of LEDs in your strip
#define NUM_LEDS 180

// Define the data pin where your LEDs are connected
#define DATA_PIN 25

// Define the LED type (e.g., WS2812B, NeoPixel)
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB // Adjust if needed

// Create an array to hold the LED colors
CRGB leds[NUM_LEDS];

void setup()
{
    // put your setup code here, to run once:
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(100); // Adjust brightness (0-255)

    Serial.begin(115200);
    delay(1000); // Allow time for serial monitor to open
    Serial.println("Hall Effect Sensor Reactive LEDs");
}

void loop()
{
    // put your main code here, to run repeatedly:

    // Read Hall sensor value
    long hallValue = hallRead();

    CRGB newColor; // Declare newColor outside the conditional blocks

    if (hallValue == 0)
    {
        // No light if Hall value is 0
        newColor = CRGB::Black;
    }
    else if (hallValue > 0)
    {
        // Green for positive values
        int greenValue = map(hallValue, 1, 100, 50, 255); // Map to green range (adjust 1, 100 as needed)
        greenValue = constrain(greenValue, 0, 255);
        newColor = CRGB(0, greenValue, random(0, 100)); // Add random blue
    }
    else
    {
        // Red for negative values
        int redValue = map(hallValue, -100, -1, 50, 255); // Map to red range (adjust -100, -1 as needed)
        redValue = constrain(redValue, 0, 255);
        newColor = CRGB(redValue, 0, random(0, 100)); // Add random blue
    }

    // Shift the LED colors along the strip
    for (int i = NUM_LEDS - 1; i > 0; i--)
    {
        leds[i] = leds[i - 1];
    }

    // Set the first LED to the new color based on the Hall value
    leds[0] = newColor;

    // Update the LEDs
    FastLED.show();

    // Print the Hall value to the serial monitor
    Serial.print("Hall Value: ");
    Serial.println(hallValue);

    delay(50); // Adjust delay for responsiveness
}
