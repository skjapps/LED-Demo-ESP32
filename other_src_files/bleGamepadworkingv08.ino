// Bluetooth composite HID device
#include <BleCompositeHID.h>
// All 3 input libraries + Xbox (XInput) emulated controller
#include <KeyboardDevice.h>
#include <MouseDevice.h>
#include <GamepadDevice.h>
// #include <XboxGamepadDevice.h>
// Graphics on screen
#include <U8g2lib.h>
// for memory compare (memcmp) and others
#include <cstring>

/////////////////////////////
//        Graphics         //
/////////////////////////////
// initialisation for the OLED display
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);  // initialization 

// Images
static const unsigned char image_bluetooth_bits[] U8X8_PROGMEM = { 0x80, 0x00, 0x40, 0x01, 0x40, 
0x02, 0x44, 0x04, 0x48, 0x04, 0x50, 0x02, 0x60, 0x01, 0xc0, 0x00, 0x60, 0x01, 0x50, 0x02, 0x48, 
0x04, 0x44, 0x04, 0x40, 0x02, 0x40, 0x01, 0x80, 0x00, 0x00, 0x00 };

static const unsigned char image_bluetooth_connected_bits[] U8X8_PROGMEM = { 0x80, 0x00, 0x40, 0x01, 
0x40, 0x02, 0x44, 0x04, 0x48, 0x04, 0x52, 0x12, 0x64, 0x09, 0xcc, 0x0c, 0x64, 0x09, 0x52, 0x12, 0x48, 
0x04, 0x44, 0x04, 0x40, 0x02, 0x40, 0x01, 0x80, 0x00, 0x00, 0x00 };

static const unsigned char image_mouse_bits[] U8X8_PROGMEM = { 0x00, 0xf0, 0x01, 0x00, 0x00, 0xf8, 0x07, 0x00, 0x00, 0xfe, 0x0f,
                                                               0x00, 0x00, 0x0e, 0x0e, 0x00, 0x00, 0x06, 0x1c, 0x00, 0x00, 0x07, 0x18, 0x00, 0x00, 0x03, 0x18, 0x00, 0xf8, 0xff, 0x18, 0x00, 0xfc, 0xff,
                                                               0x18, 0x00, 0x0c, 0x83, 0x19, 0x40, 0x6e, 0xbb, 0x19, 0xe0, 0x66, 0xbb, 0x19, 0xe0, 0x76, 0x3b, 0x1b, 0xe0, 0x76, 0x7b, 0x1b, 0xe0, 0x73,
                                                               0x7b, 0x3a, 0xe0, 0x7b, 0xfb, 0x3a, 0x60, 0x7b, 0xfb, 0x72, 0x70, 0x03, 0x03, 0xf2, 0x3c, 0xff, 0xff, 0xe3, 0x3f, 0xff, 0xff, 0xc3, 0x0f,
                                                               0xff, 0xff, 0x03, 0x00, 0xff, 0xff, 0x03, 0x00, 0xff, 0xff, 0x03, 0x00, 0xfe, 0xff, 0x03, 0x00, 0xfe, 0xff, 0x03, 0x00, 0xfe, 0xff, 0x01,
                                                               0x00, 0xfc, 0xff, 0x01, 0x00, 0xfc, 0xff, 0x00, 0x00, 0xfc, 0xff, 0x00, 0x00, 0xf0, 0x7f, 0x00, 0x00, 0xc0, 0x1f, 0x00, 0x00 };

static const unsigned char image_keyboard_bits[] U8X8_PROGMEM = { 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                                  0x00, 0x40, 0x18, 0x02, 0x00, 0x00, 0x40, 0x3c, 0x02, 0x00, 0x00, 0x40, 0x7e, 0x02, 0x00, 0x00, 0x40, 0xff, 0x02, 0x00, 0x00, 0x40, 0x18,
                                                                  0x02, 0x00, 0x00, 0x40, 0x18, 0x02, 0x00, 0x00, 0x40, 0x18, 0x02, 0x00, 0x00, 0x40, 0x18, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                                  0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x03, 0xff, 0xc0, 0x3f, 0x00, 0x00, 0x00, 0x00,
                                                                  0x00, 0x01, 0x48, 0x18, 0x12, 0x80, 0x11, 0x48, 0x18, 0x12, 0x88, 0x19, 0x48, 0x18, 0x12, 0x98, 0xfd, 0x4b, 0x18, 0xd2, 0xbf, 0xfd, 0x4b,
                                                                  0x18, 0xd2, 0xbf, 0x19, 0x48, 0x7e, 0x12, 0x98, 0x11, 0x48, 0x3c, 0x12, 0x88, 0x01, 0x48, 0x18, 0x12, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                                  0xfc, 0x03, 0xff, 0xc0, 0x3f };

static const unsigned char image_gamepad_bits[] U8X8_PROGMEM = { 0xc0, 0x01, 0x38, 0x00, 0xf0, 0xff, 0x7f, 0x00, 0xf0, 0xff,
                                                                 0xff, 0x00, 0xf8, 0xff, 0xff, 0x01, 0x7c, 0xff, 0xc7, 0x03, 0x3c, 0xfe, 0xd7, 0x03, 0xfc, 0xff, 0xc7, 0x03, 0x2e, 0xfa, 0x78, 0x07, 0xa6,
                                                                 0xf2, 0xba, 0x06, 0x2e, 0xfa, 0x78, 0x07, 0xfe, 0xff, 0xc7, 0x07, 0x3f, 0xfe, 0xd7, 0x0f, 0x7f, 0xff, 0xc7, 0x0f, 0xff, 0xff, 0xff, 0x0f,
                                                                 0xff, 0xff, 0xff, 0x0f, 0xff, 0x01, 0xf8, 0x0f, 0x3f, 0x00, 0xc0, 0x0f, 0x1f, 0x00, 0x80, 0x0f, 0x0f, 0x00, 0x80, 0x0f, 0x0f, 0x00, 0x00,
                                                                 0x0f, 0x06, 0x00, 0x00, 0x06 };

static const unsigned char image_left_arrow_bits[] U8X8_PROGMEM = { 0x00, 0xf8, 0x01, 0x00, 0x00, 0xfc, 0x01, 0x00, 0x00, 0xde,
                                                                    0x03, 0x00, 0x00, 0xcf, 0x03, 0x00, 0x00, 0xcf, 0x01, 0x00, 0x80, 0xe7, 0x01, 0x00, 0xc0, 0xf3, 0x00, 0x00, 0xe0, 0x79, 0x00, 0x00, 0xf0, 0x3c,
                                                                    0x00, 0x00, 0xf8, 0x3e, 0x00, 0x00, 0x78, 0x1e, 0x00, 0x00, 0x3c, 0x0f, 0x00, 0x00, 0x9e, 0xff, 0xff, 0x7f, 0xcf, 0xff, 0xff, 0xff, 0xc7, 0xff,
                                                                    0xff, 0xff, 0x07, 0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0xc7, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0x9e, 0xff, 0xff, 0x7f, 0x3c, 0x0f,
                                                                    0x00, 0x00, 0x78, 0x1e, 0x00, 0x00, 0xf8, 0x3e, 0x00, 0x00, 0xf0, 0x3c, 0x00, 0x00, 0xe0, 0x79, 0x00, 0x00, 0xc0, 0xf3, 0x00, 0x00, 0x80, 0xe7,
                                                                    0x01, 0x00, 0x80, 0xef, 0x01, 0x00, 0x00, 0xcf, 0x03, 0x00, 0x00, 0xde, 0x03, 0x00, 0x00, 0xfc, 0x01, 0x00, 0x00, 0xf8, 0x01, 0x00 };

static const unsigned char image_right_arrow_bits[] U8X8_PROGMEM = { 0x00, 0x80, 0x1f, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0xc0, 0x7b,
                                                                     0x00, 0x00, 0xc0, 0xf3, 0x00, 0x00, 0x80, 0xf3, 0x00, 0x00, 0x80, 0xe7, 0x01, 0x00, 0x00, 0xcf, 0x03, 0x00, 0x00, 0x9e, 0x07, 0x00, 0x00, 0x3c, 0x0f,
                                                                     0x00, 0x00, 0x7c, 0x1f, 0x00, 0x00, 0x78, 0x1e, 0x00, 0x00, 0xf0, 0x3c, 0xfe, 0xff, 0xff, 0x79, 0xff, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xe3, 0x07,
                                                                     0x00, 0x00, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xff, 0xf3, 0xfe, 0xff, 0xff, 0x79, 0x00, 0x00, 0xf0, 0x3c, 0x00, 0x00,
                                                                     0x78, 0x1e, 0x00, 0x00, 0x7c, 0x1f, 0x00, 0x00, 0x3c, 0x0f, 0x00, 0x00, 0x9e, 0x07, 0x00, 0x00, 0xcf, 0x03, 0x00, 0x80, 0xe7, 0x01, 0x00, 0x80, 0xf7,
                                                                     0x01, 0x00, 0xc0, 0xf3, 0x00, 0x00, 0xc0, 0x7b, 0x00, 0x00, 0x80, 0x3f, 0x00, 0x00, 0x80, 0x1f, 0x00 };

static const unsigned char image_up_arrow_bits[] U8X8_PROGMEM = { 0x00, 0xe0, 0x07, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf8, 0x1f, 0x00,
                                                                  0x00, 0x3e, 0x7c, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x80, 0x0f, 0xf0, 0x01, 0xc0, 0x67, 0xe6, 0x03, 0xf0, 0x73, 0xce, 0x07, 0xf8, 0x78, 0x1e, 0x1f, 0x7c,
                                                                  0x7e, 0x7e, 0x3e, 0x3e, 0x7f, 0xfe, 0x7c, 0x9f, 0x7f, 0xfe, 0xf9, 0xc7, 0x77, 0xee, 0xe3, 0xf3, 0x73, 0xce, 0xc7, 0xff, 0x70, 0x0e, 0xff, 0x7f, 0x70,
                                                                  0x0e, 0xfe, 0x3f, 0x70, 0x0e, 0xfc, 0x0c, 0x70, 0x0e, 0x30, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e,
                                                                  0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00,
                                                                  0x00, 0x70, 0x0e, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xe0, 0x07, 0x00 };

static const unsigned char image_down_arrow_bits[] U8X8_PROGMEM = { 0x00, 0xe0, 0x07, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xf0, 0x0f, 0x00,
                                                                    0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00,
                                                                    0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0e, 0x00, 0x0c, 0x70, 0x0e, 0x30, 0x3f, 0x70,
                                                                    0x0e, 0xfc, 0x7f, 0x70, 0x0e, 0xfe, 0xff, 0x70, 0x0e, 0xff, 0xe3, 0x73, 0xce, 0xcf, 0xc7, 0x77, 0xee, 0xe3, 0x9f, 0x7f, 0xfe, 0xf9, 0x3e, 0x7f, 0xfe,
                                                                    0x7c, 0x7c, 0x7e, 0x7e, 0x3e, 0xf8, 0x78, 0x1e, 0x1f, 0xe0, 0x73, 0xce, 0x0f, 0xc0, 0x67, 0xe6, 0x03, 0x80, 0x0f, 0xf0, 0x01, 0x00, 0x1f, 0xf8, 0x00,
                                                                    0x00, 0x3e, 0x7c, 0x00, 0x00, 0xf8, 0x1f, 0x00, 0x00, 0xf0, 0x0f, 0x00, 0x00, 0xe0, 0x07, 0x00 };

static const unsigned char image_music_play_button_bits[] U8X8_PROGMEM = { 0xe0, 0x03, 0x18, 0x0c, 0x04, 0x10, 0x02, 0x20, 0x32, 0x20,
                                                                           0xd1, 0x41, 0x11, 0x46, 0x11, 0x58, 0x11, 0x46, 0xd1, 0x41, 0x32, 0x20, 0x02, 0x20, 0x04, 0x10, 0x18, 0x0c, 0xe0, 0x03 };

// Inputs Display Constants
static const int MIDDLE = 62;  // (of screen)
static const int OFFSET = 40;  // (size of icons)

// Controller constants
static const int MOUSE_SPEED = 5;          // Default speed = 5
static const int MAX_STICK = 16384;        // Max movement of stick = 16384
static const int HOLD_TIME = 500;          // How long to hold for 2nd input (ms)
static const int DOUBLE_TAP_TIME = 100;    // How long to hold for 3rd input (ms)
static const int PRESS_TIME = 200;         // Time to press buttons in multi-button mode

///////////////////////////
// Program State Control //
///////////////////////////
// This needs a better name? the main state of whats happening
int main_state;
// Mode is the function of the controller (mouse, keyboard, or gamepad)
int mode;


//////////////////////////////
//      Input Handler       //
//////////////////////////////
#define BUTTON_A_PIN 23
#define BUTTON_LEFT_PIN 16
#define BUTTON_RIGHT_PIN 17
#define BUTTON_UP_PIN 19
#define BUTTON_DOWN_PIN 18
static const int inputPins[] = { BUTTON_LEFT_PIN, BUTTON_RIGHT_PIN, BUTTON_UP_PIN, 
                                  BUTTON_DOWN_PIN, BUTTON_A_PIN };

// In order of inputs:
// Left, Right, Up, Down, A, B
//  0  ,   1  , 2 ,  3  , 4, 5
const char *inputsNames[] = {
  "Left",
  "Right",
  "Up",
  "Down",
  "A",
  "B",
};
int inputs[] = { 0, 0, 0, 0, 0 };              // Original Inputs
int inputsPrev[] = { 0, 0, 0, 0, 0 };          // storing the previous one for compare
int inputsReleased[] = { 0, 0, 0, 0, 0 };      // storing the released buttons for one update
int inputsHoldTime[] = { 0, 0, 0, 0, 0 };      // storing time for which button is held
int inputsHoldInstance[] = { 0, 0, 0, 0, 0 };  // storing the time the instant the button is held.
int inputsPressed[] = { 0, 0, 0, 0, 0 };       // storing the instance pressed buttons for one cycle
int inputReleaseTime;                          // Release button press after duration millis
int gamepadControlScheme;                      // Choosing control scheme

// Key lists for loop
int arrowKeys[] = {
  KEY_LEFT,
  KEY_RIGHT,
  KEY_UP,
  KEY_DOWN,
};

// How many inputs pressed at a time
int numInputs;
// The double press counter
int tapCount;
// Timeout timer for double tap
int doubleTapTimeout;

// The HID libraries and composite bluetooth built on BLEGamepad
// 100 = 100% battery, to change...
// GamepadDevice *gamepad;
GamepadDevice *gamepad;
KeyboardDevice *keyboard;
MouseDevice *mouse;
// Create a BleGamepadConfiguration object to store all of the options
BLEHostConfiguration bleHostConfig;  
char serialNumber[10];
BleCompositeHID compositeHID("SKJAPPS Controller", "SKJAPPS", 100);

////////////////////////////
//     Initial Setup      //
////////////////////////////
void setup() {
  randomSeed(analogRead(0)); // Seed the random number generator

  u8g2.begin();  // start the u8g2 library

  // Default graphics settings
  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);

  // Display Splash Screen on controller
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont29_tr);
  u8g2.drawStr(8, 43, "SKJAPPS");
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawStr(98, 60, "v0.7");
  u8g2.sendBuffer();

  // Setting up serial debug output
  Serial.begin(115200);
  Serial.println("Booting SKJAPPS Controller Software...");

  // Setting Inputs
  for (int i = 0; i < 5; i++) {
    pinMode(inputPins[i], INPUT_PULLUP);
  }

  // Setting control variables
  mode = 1;                 // default = gamepad
  main_state = 1;           // start at mode select screen
  gamepadControlScheme = 1; // Basic control scheme

  tapCount = 0;             // The double tap count

  // Xbox controller setup
  // XboxOneSControllerDeviceConfiguration* config = new XboxOneSControllerDeviceConfiguration();
  // BLEHostConfiguration hostConfig = config->getIdealHostConfiguration();
  // // Set up gamepad
  // gamepad = new XboxGamepadDevice(config);

  // BLE Gamepad Config struct
  // Set up Gamepad
  GamepadConfiguration gamepadConfig;
  gamepadConfig.setControllerType(CONTROLLER_TYPE_GAMEPAD);
  gamepadConfig.setAutoReport(false);
  // Only Left Thumbstick
  gamepadConfig.setWhichAxes(true, true, false, false, false, false, false, false);
  // Start the gamepad service
  gamepad = new GamepadDevice(gamepadConfig);

  // Set up keyboard
  KeyboardConfiguration keyboardConfig;
  keyboardConfig.setAutoReport(false);
  keyboard = new KeyboardDevice(keyboardConfig);

  // Set up mouse
  MouseConfiguration mouseConfig;
  mouseConfig.setAutoReport(false);
  mouse = new MouseDevice(mouseConfig); 

  // Add all devices to the composite HID device to manage them
  compositeHID.addDevice(keyboard);
  compositeHID.addDevice(mouse);
  compositeHID.addDevice(gamepad);

  // Logitech Standard Gamepad
  bleHostConfig.setVid(0x046D);
  bleHostConfig.setPid(0xC216);
  // Generate random serial number
  sprintf(serialNumber, "%d", random(999, 999999999));
  bleHostConfig.setSerialNumber(serialNumber);

  // Start Bluetooth controller
  compositeHID.begin(bleHostConfig);

  // Allow Splash to show
  delay(1000);
}

////////////////////////////
//       Main Loop        //
////////////////////////////
void loop() {
  // Reset numInputs
  numInputs = 0;
  // Reset held inputs
  memset(inputsReleased, 0, sizeof(inputsReleased));
  // Reset pressed inputs
  memset(inputsPressed, 0, sizeof(inputsPressed));

  // The main controller intelligence.
  for (int i = 0; i < 5; i++) {
    // Read button states into array
    inputs[i] = !(digitalRead(inputPins[i]));
    numInputs += inputs[i];
    // This is the instant the button is pressed.
    if ((inputs[i] == 1) && (inputsPrev[i] == 0)) {
      // Mark the input as pressed.
      inputsPressed[i] = 1;
      inputsHoldInstance[i] = millis();
    }
    // Whilst the button is held
    if ((inputs[i] == 1) && (inputsPrev[i] == 1)) {
      // Update hold time.
      inputsHoldTime[i] = millis() - inputsHoldInstance[i];
      // Serial.printf("Hold time for input %s = %lu ms\n", inputsNames[i], inputsHoldTime[i]);
    }
    // This is when the button is released.
    if ((inputs[i] == 0) && (inputsPrev[i] == 1)) {
      // Mark the input as released.
      inputsReleased[i] = 1;
      // Reset both hold timers.
      inputsHoldInstance[i] = 0;
    }

    inputsPrev[i] = inputs[i];  // Update previous state for next iteration
  }

  // Clear the graphics buffer
  u8g2.clearBuffer();

  // Draw the status bar
  StatusBarScreen();

  // Choosing feature
  FeatureControl();

  // Draw the frame required
  u8g2.sendBuffer();

  // Before loop end processing
  for (int i = 0; i < 5; i++) {
    // This is when the button is released.
    if (inputsReleased[i]) {
        // Reset after processing
        inputsHoldTime[i] = 0;
    }
  }
}

////////////////////////////////
//      Control States        //
////////////////////////////////
void FeatureControl() {
  switch (main_state) {
    case 0:
      Serial.printf("Idle state...?");
    case 1:
      // Choosing Mode State
      ModeSelect();
      break;
    case 2:
      // Control Scheme
      ControlSchemeSelect();
      break;
    case 3:
      // "Waiting for connection" State
      ConnectWait();
      break;
    case 4:
      // Main Connected Controls State.
      BleGamepadRunner();
      break;
  }
}

// Mode Selection State
void ModeSelect() {

  // Choosing Mode Process
  if (inputsPressed[4]) {
    // BleSetup();
    main_state = 2;
  } else if (inputsPressed[0]) {
    mode = (mode + 1) % 3 + 1;  // Increment by 1 for left button
  } else if (inputsPressed[1]) {
    mode = mode % 3 + 1;  // No change for right button (keeps current value)
  }

  // Display the mode graphics
  ModeSelectScreen();
}

// Control Scheme Selection State
void ControlSchemeSelect() {

  // Choosing Control Scheme Process
  if (inputsPressed[4]) {
    main_state = 3;
  } else if (inputsPressed[0]) {
    gamepadControlScheme = (gamepadControlScheme + 1) % 3 + 1;  // Increment by 1 for left button
  } else if (inputsPressed[1]) {
    gamepadControlScheme = gamepadControlScheme % 3 + 1;  // No change for right button (keeps current value)
  }

  // Display the mode graphics
  ControlSchemeSelectScreen();
}

// "Waiting for Connection" State
void ConnectWait() {

  // Go Back to mode select
  if (inputsHoldTime[4] > 3000) {
    main_state = 1;
  }

  // Display the wait graphics
  ConnectWaitScreen();

  // Run the gamepad check if connected.
  BleGamepadRunner();
}


//////////////////////////
//   Controller Logic   //
//////////////////////////
// Named "Runner"(s) to avoid confusion

// The Gamepad State
void BleGamepadRunner() {
  if (compositeHID.isConnected()) {
    // Connected state
    main_state = 4;

    // Hold A for 10 seconds to exit controller mode
    if (inputsHoldTime[4] > 3000) {
      // Exiting controller
      Serial.printf("Leaving controller mode... \n");
      // Reset all
      gamepad->resetButtons();
      gamepad->sendGamepadReport();
      mouse->resetButtons();
      mouse->sendMouseReport();
      keyboard->resetKeys();
      keyboard->sendKeyReport();
      // Return to mode menu
      main_state = 1;
    }

    // Using specific mode (gamepad, mouse or keyboard)
    switch (mode) {

      // Gamepad
      case 1:
        switch(gamepadControlScheme) {
          // Control scheme 1 = Default (A Only)
          default:
            // Press Button 1 for A
            if (inputs[4]) {
              gamepad->press(BUTTON_2);
              gamepad->sendGamepadReport();
            } else {
              gamepad->release(BUTTON_2);
              gamepad->sendGamepadReport();
            }
            break;
          // Control scheme 2: holding button = B, double tap = Y
          case 2:
            // If there is still a release pending
            if (inputReleaseTime) {
              // milliseconds after to release.
              if (millis() >= (inputReleaseTime + PRESS_TIME)) {
                gamepad->resetButtons();
                gamepad->sendGamepadReport();
                // Reset inputReleaseTime
                inputReleaseTime = 0;
              }
            }
            // If the double tap timed out
            if (doubleTapTimeout) {
              // milliseconds to timeout.
              if (millis() >= (doubleTapTimeout + (DOUBLE_TAP_TIME * 2))) {
                // Delayed A Press For missed double tap timeout
                Serial.printf("Pressed A \n");
                gamepad->press(BUTTON_2);
                gamepad->sendGamepadReport();
                inputReleaseTime = millis(); 
                // reset tap count after timeout.
                tapCount = 0;
                // reset doubleTapTimeout
                doubleTapTimeout = 0;
              }
            }
            // When the button is released
            if ((inputsReleased[4] == 1) && (!inputReleaseTime)) {
              Serial.printf("Pressed for %d ms \n", inputsHoldTime[4]);
              // Double Tap.
              if (inputsHoldTime[4] <= DOUBLE_TAP_TIME) {
                tapCount += 1;
                // Double tapped
                if(tapCount > 1) {
                  // Press Y
                  gamepad->press(BUTTON_1);
                  gamepad->sendGamepadReport();
                  inputs[4] = 3;
                  inputReleaseTime = millis();
                  // reset tap count and timeout
                  tapCount = 0;
                  doubleTapTimeout = 0;
                } else { doubleTapTimeout = millis(); }
                // Start Timeout 
              }
              // Hold
              else if (inputsHoldTime[4] >= HOLD_TIME) {
                // Press B
                Serial.printf("Pressed B \n");
                gamepad->press(BUTTON_3);
                gamepad->sendGamepadReport();
                inputs[4] = 2;
                inputReleaseTime = millis();
              }
              // Normal 
              else {
                // Press A
                Serial.printf("Pressed A \n");
                gamepad->press(BUTTON_2);
                gamepad->sendGamepadReport();
                inputReleaseTime = millis();
              }
            }
            break;
        }

        // 0 = Left, 16384 = Middle, 32767 = Right
        // Move Left Stick
        if(gamepadControlScheme == 3) {
          // Use directional on 3
          gamepad->setHats(inputs[0], inputs[1], inputs[2], inputs[3]);
        } else {
          // Using joystick default
          gamepad->setLeftThumb((MAX_STICK + (inputs[0] * -MAX_STICK) 
                                + (inputs[1] * (MAX_STICK - 1))),
                              (MAX_STICK + (inputs[2] * -MAX_STICK) 
                                + (inputs[3] * (MAX_STICK - 1))));
        }
        
        gamepad->sendGamepadReport();

        break;

      // Mouse
      case 2:

        // If there is still a release pending
        if (inputReleaseTime) {
          // milliseconds after to release.
          if (millis() >= (inputReleaseTime + PRESS_TIME)) {
            mouse->resetButtons();
            mouse->sendMouseReport();
            // Reset inputReleaseTime
            inputReleaseTime = 0;
          }
        }

        // Press Button for Mouse Click
        if(inputs[4]) {
          // MOUSE_LEFT (default)
          mouse->mousePress();
          // Showing Left Click
          inputs[4] = 4;
        } else {
          mouse->mouseRelease();
        }
        mouse->sendMouseReport();

        // Right Click on hold.
        if (inputsHoldTime[4] >= HOLD_TIME) {
          Serial.printf("Right Click \n");
          mouse->mousePress(MOUSE_LOGICAL_RIGHT_BUTTON);
          mouse->sendMouseReport();
          inputReleaseTime = millis();
          // Showing Right Click
          inputs[4] = 5;
        }

        // Move Mouse
        mouse->mouseMove(((-max((inputsHoldTime[0]/MOUSE_SPEED), MOUSE_SPEED)) + (max((inputsHoldTime[1]/MOUSE_SPEED), MOUSE_SPEED))),
                         ((-max((inputsHoldTime[2]/MOUSE_SPEED), MOUSE_SPEED)) + (max((inputsHoldTime[3]/MOUSE_SPEED), MOUSE_SPEED))));
        mouse->sendMouseReport();

        break;

      // Keyboard
      case 3:

        // Press Button for Enter
        switch(gamepadControlScheme) {
          // Holding inputs
          case 1:
            if (inputs[4]) {
              keyboard->keyPress(KEY_SPACE);
              keyboard->sendKeyReport();
              // Showing Enter
              inputs[4] = 6;
            }

            // Loop Arrow Keys
            for (int i = 0; i < 4; i++) {
              if (inputs[i]) {
                keyboard->keyPress(arrowKeys[i]);
                keyboard->sendKeyReport();
              }
            }
            break;
          // Pressing inputs
          case 2:
            if (inputsPressed[4]) {
              keyboard->keyPress(KEY_ENTER);
              keyboard->sendKeyReport();
              // Showing Space
              inputs[4] = 7;
            }

            // Arrow Keys
            // Loop Arrow Keys
            for (int i = 0; i < 4; i++) {
              if (inputsPressed[i]) {
                keyboard->keyPress(arrowKeys[i]);
                keyboard->sendKeyReport();
              }
            }
            break;
        }
        
        keyboard->resetKeys();
        keyboard->sendKeyReport();
        break;
    }

    // Show Inputs screen
    InputsDisplayScreen(numInputs);

  } else {
    // Lost connection, return to waiting screen.
    main_state = 3;
  }
}


//////////////////////////////////////
//        Single Run Functions      //
//////////////////////////////////////

// The function to be called to setup the BleLibrary
// void BleSetup() {

//   switch(mode) {
//     // Setup BLEGamepad
//     case 1:
//       compositeHID.begin();
//       break;
//     // Setup BleMouse or BleKeyboard
//     case 2:
//     case 3:
//       compositeHID.begin();
//       break;
//   }

// }

////////////////////////
// Graphics Functions //
////////////////////////

// The Top Status bar with "battery" and connection status
void StatusBarScreen() {
  // u8g2.drawXBM(101, 2, 24, 16, image_battery_full_bits);
  if (compositeHID.isConnected()) {
    u8g2.drawXBM(2, 2, 14, 16, image_bluetooth_connected_bits);
    u8g2.setFont(u8g2_font_profont15_tr);
    u8g2.drawStr(18, 14, "Connected!");
  }
  u8g2.drawLine(124, 19, 2, 19);
}

// Displays Mode selection Screen
void ModeSelectScreen() {
  // Display Selection
  switch (mode) {
    case 1:
      // Gamepad Mode
      u8g2.drawFrame(2, 23, 37, 37);
      break;
    case 2:
      // Mouse Mode
      u8g2.drawFrame(43, 23, 37, 37);
      break;
    case 3:
      // Keyboard Mode
      u8g2.drawFrame(80, 23, 44, 37);
      break;
  }
  // Draw option icons
  u8g2.drawXBM(46, 26, 32, 31, image_mouse_bits);
  u8g2.drawXBM(82, 27, 40, 26, image_keyboard_bits);
  u8g2.drawXBM(7, 31, 28, 20, image_gamepad_bits);
}


// Displays Control Scheme Selection Screen
void ControlSchemeSelectScreen() {
  // Control Scheme Selection
  u8g2.setFont(u8g2_font_profont29_tr);
  switch (gamepadControlScheme) {
    case 1:
      // Default Mode
      u8g2.drawEllipse(23, 37, 17, 17);
      break;
    case 2:
      // Changed Mode
      u8g2.drawEllipse(63, 37, 17, 17);
      break;
    case 3:
      // third Mode
      u8g2.drawEllipse(102, 37, 17, 17);
      break;
  }
  // Draw option numbers
  u8g2.drawStr(18, 47, "1");
  u8g2.drawStr(57, 47, "2");
  u8g2.drawStr(96, 47, "3");
}

// Displays Connection Waiting Screen
void ConnectWaitScreen() {
  u8g2.drawXBM(20, 32, 14, 16, image_bluetooth_bits);
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.drawStr(42, 39, "Waiting for");
  u8g2.drawStr(44, 49, "Connection");
}

// Displays the Current Inputs Pressed on Screen
void InputsDisplayScreen(int numInputs) {
  // X pos where inputs start getting drawn
  int basePosition = MIDDLE - ((OFFSET / 2) * (numInputs - 1));

  // Count how many have already been drawn
  int drawCounter = 0;

  // Loop for each possible input (0 to 4)
  for (int i = 0; i < 5; i++) {
    // Calculate X position with offset
    int xPosition = basePosition + (drawCounter * OFFSET);

    // Draw elements based on button state and position
    switch (i) {
      case 0:
        if (inputs[4]) {
          // Draw button elements at xPosition
          u8g2.drawEllipse(xPosition, 42, 15, 15);
          u8g2.drawEllipse(xPosition, 42, 16, 16);
          u8g2.setFont(u8g2_font_profont29_tr);
          // Drawing the correct button in the circle
          switch(inputs[4]) {
            case 1:
              u8g2.drawStr(xPosition - 7, 51, "A");
              break;
            case 2:
              u8g2.drawStr(xPosition - 7, 51, "B");
              break;
            case 3:
              u8g2.drawStr(xPosition - 7, 51, "Y");
              break;
            case 4:
              u8g2.drawStr(xPosition - 7, 51, "L");
              break;
            case 5:
              u8g2.drawStr(xPosition - 7, 51, "R");
              break;
            case 6:
              u8g2.drawStr(xPosition - 7, 51, "E");
              break;
            case 7:
              u8g2.drawStr(xPosition - 7, 51, "S");
              break;
            default:
              u8g2.drawStr(xPosition - 7, 51, "?!");
              break;
          }
          drawCounter++;
        }
        break;
      case 1:
        if (inputs[0]) {
          u8g2.drawXBM(xPosition - 16, 26, 32, 32, image_left_arrow_bits);
          drawCounter++;
        }
        break;
      case 2:
        if (inputs[1]) {
          u8g2.drawXBM(xPosition - 16, 26, 32, 32, image_right_arrow_bits);
          drawCounter++;
        }
        break;
      case 3:
        if (inputs[2]) {
          u8g2.drawXBM(xPosition - 16, 26, 32, 32, image_up_arrow_bits);
          drawCounter++;
        }
        break;
      case 4:
        if (inputs[3]) {
          u8g2.drawXBM(xPosition - 16, 26, 32, 32, image_down_arrow_bits);
          drawCounter++;
        }
        break;
    }
  }
}