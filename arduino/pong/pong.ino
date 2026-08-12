#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definitions for joystick
#define X_PIN 34
#define Y_PIN 35
#define SW_PIN 26

// Define OLED screen resolution
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Define OLED I2C address (default is usually 0x3C or 0x3D)
#define OLED_ADDR   0x3C

// Create OLED object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Ball
int x = 8;
int y = 8;
const int r = 4;
int xv = 2;
int yv = 2;

// Paddle
const int pw = 24; // Paddle width
const int ph = 4;
int px = SCREEN_WIDTH/2 - pw/2; // Paddle starting position, centered
const int py = SCREEN_HEIGHT-12;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  Wire.begin(21,22);
  // Initialize OLED screen
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Infinite loop to terminate program
  }

  pinMode(SW_PIN, INPUT_PULLUP);
}

void update() {
  x += xv;
  y += yv;

  if (x + r/2 >= SCREEN_WIDTH || x - r/2 <= 0) {
    xv = -xv;
  }
  if (y - r/2 <= 0) {
    yv = -yv;
  }

  if (y+r/2 >= py && y <= py+ph && x <= px+pw && x >= px) {
    yv = -abs(yv);
  }
}

void render() {
  display.clearDisplay();
  display.fillCircle(x, y, r, SSD1306_WHITE);
  display.fillRect(px, py, pw, ph, SSD1306_WHITE);
  display.display();
}

void loop() {
  update();
  render();
  delay(50);

  int xread = analogRead(X_PIN);
  if (xread < 1800) {
    px -= 4;
  } else if (xread > 1900) {
    px += 4;
  }

  if (y > SCREEN_HEIGHT) {
    delay(2000);
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.println("You Lost!");
    display.setTextSize(1);
    display.println("Press the joystick to play again");
    display.display();

    while (digitalRead(SW_PIN) != 0); // Await joystick press

    x = 8;
    y = 8;
    xv = -2;
    yv = 2;
    px = SCREEN_WIDTH/2 - pw/2;
  }
}
