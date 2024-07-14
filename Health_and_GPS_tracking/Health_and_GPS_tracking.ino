#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial GSM(2, 3);
#define button 6
#define ONE_WIRE_BUS 5


unsigned long t = 0;
unsigned long start = 0;
unsigned long p = 0;
const String PHONE = "+918889986226";

TinyGPSPlus gps;
OneWire onewire(ONE_WIRE_BUS);
DallasTemperature sensors(&onewire);

void setup() {
  Serial.begin(9600);
  GSM.begin(9600);

  pinMode(button, INPUT);
  sensors.begin(); 

  GSM.print("AT+CNMI=2,2,0,0,0");
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.home();
  lcd.print("Intializing....");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BodyTemp:");
  lcd.setCursor(10, 0);
  lcd.print("Pulse:");
  start = millis();
  p = millis();
}

int mybpm = 0;
float bt = 0;
bool l = 0;
void loop() {

  sensors.requestTemperatures();  //temperature

  bt = sensors.getTempCByIndex(0);
  lcd.setCursor(0, 1);
  lcd.print(bt);
  lcd.write(223);  // Serial.print("\xB0");  //shows degrees character
  lcd.print("C");

  mybpm = 0.6 * mybpm + 0.4* map(analogRead(A0), 0, 1023, 0, 100);
  lcd.setCursor(10, 1);
  lcd.print(mybpm);
  lcd.print(" bpm");

  while (Serial.available() > 0)
    if (gps.encode(Serial.read())) {
            if (!gps.location.isValid()) {
      }
    }

  if (millis() - start > 5000 && gps.charsProcessed() < 10) {
    start = millis();
  }


  if (digitalRead(button) == 1 || bt > 45) {
    msgAll();
    lcd.clear();
    lcd.home();
    lcd.print("LOCATION IS SENT");
    lcd.setCursor(0, 1);
    lcd.print("HEALTH MSG SENT");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BodyTemp:");
    lcd.setCursor(10, 0);
    lcd.print("Pulse:");
  }

  delay(20);
}

void msgAll() {
  GSM.print("AT+CMGF=1\r");
  delay(1000);
  GSM.print("AT+CMGS=\"" + PHONE + "\"\r");
  delay(1000);
  GSM.println("Player Vivek Patel need help");
  GSM.print("http://maps.google.com/maps?q=loc:");
  GSM.print(gps.location.lat(), 6);
  GSM.print(",");
  GSM.print(gps.location.lng(), 6);

  delay(200);
  GSM.print("\nHEART RATE: ");
  GSM.print(mybpm);
  GSM.print(" bpm");
  GSM.print("\nBody temprature: ");
  GSM.print(bt);
  GSM.print(" ");
  GSM.print("C");
  GSM.write(byte(26));  //Ctrl+Z  send message command (26 in decimal).
  GSM.write(0x1A);      //ascii code for ctrl-26 // //ascii code for ctrl-26
  GSM.println((char)26);
}

