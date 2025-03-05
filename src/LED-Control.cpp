// Standard libraries
#include <Arduino.h>

#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>

// Wi-Fi Credentials
const char *ssid = "ESP32-Access-Point";
const char *password = "RGB5050&WS2812b";

// LED Configuration
#define DATA_PIN 26
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

int NUM_LEDS = 100; // Default number of LEDs, can be changed via web page
CRGB leds[180];     // Max 100 LEDs, adjust if necessary

WebServer server(80);

void handleRoot();
void handleSetNumLeds();
void handleSetLedColor();
void handleNotFound();

void handleRoot()
{
    String html = "<html><body><h1>ESP32 LED Control</h1>";
    html += "<p>Number of LEDs: <input type='number' id='numLeds' value='" + String(NUM_LEDS) + "'></p>";
    html += "<button onclick='setNumLeds()'>Set LEDs</button>";
    html += "<br><br>";

    for (int i = 0; i < NUM_LEDS; i++)
    {
        html += "<p>LED " + String(i) + ": <input type='color' id='ledColor" + String(i) + "' value='#FFFFFF'></p>";
        html += "<button onclick='setLedColor(" + String(i) + ")'>Set Color</button><br>";
    }

    html += "<script>";
    html += "function setNumLeds() {";
    html += "  var num = document.getElementById('numLeds').value;";
    html += "  window.location.href = '/setNumLeds?num=' + num;";
    html += "}";

    html += "function setLedColor(ledIndex) {";
    html += "  var color = document.getElementById('ledColor' + ledIndex).value;";
    html += "  window.location.href = '/setLedColor?index=' + ledIndex + '&color=' + color.substring(1);"; // Remove '#' from color
    html += "}";
    html += "</script>";

    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleSetNumLeds()
{
    if (server.hasArg("num"))
    {
        NUM_LEDS = server.arg("num").toInt();
        if (NUM_LEDS > 180)
            NUM_LEDS = 180; // Limit to max 100 (or adjust)
        FastLED.clear();
        FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
        server.send(200, "text/plain", "Number of LEDs set to " + String(NUM_LEDS));
    }
    else
    {
        server.send(400, "text/plain", "Invalid request");
    }
}

void handleSetLedColor()
{
    if (server.hasArg("index") && server.hasArg("color"))
    {
        int index = server.arg("index").toInt();
        String color = server.arg("color");
        if (index >= 0 && index < NUM_LEDS && color.length() == 6)
        {
            long hexValue = strtol(color.c_str(), NULL, 16);
            leds[index] = CRGB((hexValue >> 16) & 0xFF, (hexValue >> 8) & 0xFF, hexValue & 0xFF);
            FastLED.show();
            server.send(200, "text/plain", "LED " + String(index) + " color set");
        }
        else
        {
            server.send(400, "text/plain", "Invalid request");
        }
    }
    else
    {
        server.send(400, "text/plain", "Invalid request");
    }
}

void handleNotFound()
{
    server.send(404, "text/plain", "Not found");
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    WiFi.softAP(ssid, password); // Start access point mode
    Serial.print("Access Point IP address: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", handleRoot);
    server.on("/setNumLeds", handleSetNumLeds);
    server.on("/setLedColor", handleSetLedColor);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("Web server started");
}

void loop()
{
    server.handleClient();
}