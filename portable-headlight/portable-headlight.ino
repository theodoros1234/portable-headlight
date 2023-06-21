#define LED_COOL 6
#define LED_WARM 9
#define BUTTON_LEFT 3
#define BUTTON_RIGHT 2
#define CHANGE_STEPS 5
#define DEBUG_MODE false

int brightness = 255, temperature = 0; // brightness: 0..255, temperature: -255..255
bool direction = false;   // false: decrease,   true: increase
bool adj_mode = false;    // false: brightness, true: color temperature
bool left_pressed = false;
bool right_pressed = false;
unsigned long pressed_since = 0;

void setLEDs() {
  int warm, cool;
  
  if (temperature >= 0) {
    // Cool
    cool = brightness;
    warm = (brightness * (255 - temperature)) / 255;
  } else {
    // Warm
    cool = (brightness * (255 + temperature)) / 255;
    warm = brightness;
  }

  if (DEBUG_MODE) {
    Serial.print("brightness=");
    Serial.print(brightness);
    Serial.print(", temperature=");
    Serial.print(temperature);
    Serial.print("cool=");
    Serial.print(cool);
    Serial.print(", warm=");
    Serial.println(warm);
  }
  
  analogWrite(LED_COOL, cool);
  analogWrite(LED_WARM, warm);
}

void toggleDirection() {
  direction = !direction;
}

void toggleMode() {
  adj_mode = !adj_mode;
}

void change() {
  if (adj_mode) {
    // Adjusting color temperature
    if (direction)
      temperature += CHANGE_STEPS;
    else
      temperature -= CHANGE_STEPS;

    // Make sure it's not out of bounds
    temperature = min(max(temperature, -255), 255);
  } else {
    // Adjusting brightness
    if (direction)
      brightness += CHANGE_STEPS;
    else
      brightness -= CHANGE_STEPS;

    // Make sure it's not out of bounds
    brightness = min(max(brightness, 0), 255);
  }
  
  setLEDs();
}

void reset() {
  brightness = 255;
  temperature = 0;
  setLEDs();
}

void setup() {
  pinMode(LED_COOL, OUTPUT);
  pinMode(LED_WARM, OUTPUT);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  
  if (DEBUG_MODE) {
    Serial.begin(115200);
    Serial.println("Started in debugging mode.");
  }
  
  reset();
  //brightness = 255;
  //temperature = 255;
  //setLEDs();
}

void loop() {
  int pressed_for = millis() - pressed_since;
  
  // Default state: No button is pressed
  if (!left_pressed && !right_pressed) {
    if (!digitalRead(BUTTON_LEFT)) {
      left_pressed = true;
      if (DEBUG_MODE)
        Serial.println("Left pressed");
    }

    if (!digitalRead(BUTTON_RIGHT)) {
      change();
      right_pressed = true;
      if (DEBUG_MODE)
        Serial.println("Right pressed");
    }
    pressed_since = millis();
  }

  // Left button pressed: Change adjustment direction or mode
  if (left_pressed) {
    // Hold left button for 2 seconds to reset to default settings
    if (pressed_for >= 2000)
      reset();

    if (digitalRead(BUTTON_LEFT)) {
      left_pressed = false;
      if (DEBUG_MODE)
        Serial.println("Left depressed");
      // Press left button to invert adjustment direction
      if (pressed_for < 500)
        toggleDirection();
      // Hold left button for half a second to toggle between adjusting brightness or color temperature
      else if (pressed_for < 2000)
        toggleMode();
    }
  }

  // Right button pressed: Turn brightness or color temperature up or down
  if (right_pressed) {
    // Hold to keep changing
    if (pressed_for >= 500) {
      change();
      pressed_since += 50;
    }
    if (digitalRead(BUTTON_RIGHT)) {
      right_pressed = false;
      if (DEBUG_MODE)
        Serial.println("Right depressed");
    }
  }
  
  delay(25);
}
