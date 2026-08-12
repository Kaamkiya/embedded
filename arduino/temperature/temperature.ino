#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Wire.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
DHT dht11(18, DHT11);

void setup() {
  Serial.begin(9600);
  Wire.begin(21, 22);
  dht11.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("oops display no worky :(");
  }

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
}

void loop() {
  float h = dht11.readHumidity();
  float temp = dht11.readTemperature();

  if (isnan(temp) || isnan(h)) {
    Serial.println("oops didn't read hehe");
  } else {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("%\tTemp: ");
    Serial.print(temp);
    Serial.println(" C");
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(temp);
  display.println("*C");
  display.print(h);
  display.print("%");
  display.display();

  delay(2000);
}
