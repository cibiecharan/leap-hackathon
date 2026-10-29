
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>
#include "HX711.h"


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET
);


Adafruit_BME280 bme;


const int LOADCELL_DOUT_PIN = 16;
const int LOADCELL_SCK_PIN = 4;

HX711 scale;

const float CALIBRATION_FACTOR = -102.426;


const int TARE_BUTTON_PIN = 5;


void setup() {
  Serial.begin(115200);


  Wire.begin(21, 22);


  pinMode(TARE_BUTTON_PIN, INPUT_PULLUP);


  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("OLED initialization failed!");
    while (true) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("WEIGHT MONITOR");
  display.println();
  display.println("Initializing...");
  display.display();


  bool bmeFound = bme.begin(0x76);

  if (!bmeFound) {
    bmeFound = bme.begin(0x77);
  }

  if (!bmeFound) {
    Serial.println("BME280 not detected!");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("BME280 ERROR!");
    display.println("Check wiring");
    display.display();

    while (true) {
      delay(100);
    }
  }


  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  if (!scale.wait_ready_timeout(3000)) {
    Serial.println("HX711 not detected!");

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("HX711 ERROR!");
    display.println("Check wiring");
    display.display();

    while (true) {
      delay(100);
    }
  }


  scale.set_scale(CALIBRATION_FACTOR);


  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Initial Tare...");
  display.println("Keep platform empty");
  display.display();

  scale.tare(20);

  Serial.println("System ready!");

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("System Ready!");
  display.display();

  delay(1000);
}

void loop() {

  static bool lastButtonState = HIGH;
  static unsigned long lastDebounceTime = 0;

  bool buttonState = digitalRead(TARE_BUTTON_PIN);

  if (buttonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > 40) {
    static bool stableButtonState = HIGH;

    if (buttonState != stableButtonState) {
      stableButtonState = buttonState;

     
      if (stableButtonState == LOW) {
        Serial.println("Taring...");

        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(15, 20);
        display.println("TARING");
        display.setTextSize(1);
        display.setCursor(12, 45);
        display.println("Keep platform empty");
        display.display();

   
        if (scale.wait_ready_timeout(2000)) {
          scale.tare(20);
          Serial.println("Tare complete!");
        } else {
          Serial.println("HX711 not ready!");
        }
      }
    }
  }

  lastButtonState = buttonState;


  float weight = 0.0;

  if (scale.is_ready()) {
    weight = scale.get_units(5);
  }


  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  

 
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);


  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("WEIGHT & ENVIRONMENT");
  display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

  
  display.setCursor(0, 14);
  display.print("Weight: ");
  display.print(weight, 2);
  display.println(" g");


  display.setCursor(0, 27);
  display.print("Temp: ");
  display.print(temperature, 1);
  display.println(" C");


  display.setCursor(0, 39);
  display.print("Humidity: ");
  display.print(humidity, 1);
  display.println(" %");



  display.display();


  Serial.print("Weight: ");
  Serial.print(weight, 2);
  Serial.println(" g");

  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.println(" C");

  Serial.print("Humidity: ");
  Serial.print(humidity, 1);
  Serial.println(" %");



  Serial.println("----------------------------");

  delay(500);
}
