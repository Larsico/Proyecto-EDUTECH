#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include "RTClib.h"

#define DHTTYPE DHT22   

#define DHT_Ext_PIN 10
#define DHT_Int_PIN 11


const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {33, 35, 37, 39};
byte colPins[COLS] = {25, 27, 29, 31};

RTC_DS1307 rtc;

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

LiquidCrystal_I2C lcd(0x26, 16, 2); 

LiquidCrystal_I2C lcd2(0x27, 16, 4); 

DHT dht_Ext(DHT_Ext_PIN, DHTTYPE);
DHT dht_Int(DHT_Int_PIN, DHTTYPE);

const int pirPin = 9;
const int pirRelayPin = 7;
const int puertaRelayPin = 8;
char passcode[] = "1234";
bool isLocked = true;

long previousMillis = 0;
long intervalo = 5000;
long currentMillis = 0;
DateTime now;
float Temp_Ext = 0.0;
float Temp_Int = 0.0;
float Hum_Ext = 0.0;
float Hum_Int = 0.0;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(pirRelayPin, OUTPUT);
  pinMode(puertaRelayPin, OUTPUT);

  keypad.addEventListener(keypadEvent);

  lcd.init();
  lcd2.init();
  lcd.backlight(); 
  lcd2.backlight();
  lcd.setCursor(0, 0);
  lcd2.setCursor(0, 0);
  lcd.print("Puerta: Cerrada");

  Serial.begin(115200);
}

void Mostrar_Lcd_Interno()
{
  lcd2.clear();
  lcd2.setCursor(0, 0);
  lcd2.print(Armar_Hora());
  lcd2.setCursor(0, 1);
  lcd2.print("T.I: " + String(Temp_Int) + " ");
  lcd2.print("T.E: " + String(Temp_Ext));
  lcd2.setCursor(0, 2);
  lcd2.print("H.I: " + String(Hum_Int) + " ");
  lcd2.print("H.E: " + String(Hum_Ext));
  
  
}

void Leer_Sensores()
{
  Temp_Ext = dht_Ext.readTemperature();
  Temp_Int = dht_Int.readTemperature();
  delay(500);
  Hum_Ext = dht_Ext.readHumidity();
  Hum_Int = dht_Int.readHumidity();
}


String Armar_Hora()
{
  String tiempo = "";
  tiempo += String(now.hour());
  tiempo += ":";
  tiempo += String(now.minute());
  tiempo += "-";
  tiempo += String(now.day());
  tiempo += "/";
  tiempo += String(now.month());
  tiempo += "/";
  tiempo += String(now.year());
 
  return tiempo;
}


void loop() {

  currentMillis = millis();
  int pirValue = digitalRead(pirPin);

  now = rtc.now();

  if (pirValue == HIGH) {
    digitalWrite(pirRelayPin, HIGH);
  } else {
    digitalWrite(pirRelayPin, LOW);
  }

    if (currentMillis - previousMillis >= intervalo) 
    {
      Leer_Sensores();
      previousMillis = currentMillis;
      Mostrar_Lcd_Interno();
    }
  //Serial.println("...");
  keypad.getKey();
}

void desbloquearPuerta() {
  digitalWrite(puertaRelayPin, HIGH);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Puerta: Abierta");
  delay(2000);
  digitalWrite(puertaRelayPin, LOW);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Puerta: Cerrada");
}

void keypadEvent(KeypadEvent eKey) {
  static String passEntered;
  
  if (keypad.getState() == PRESSED) {
    if (eKey == '*') {
      passEntered = "";
    } else if (eKey == '#') {
      if (passEntered == passcode) {
        desbloquearPuerta();
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Clave incorrecta");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Estado: Cerrado");
      }
      passEntered = "";
    } else {
      passEntered += eKey;
    }

    lcd.setCursor(0, 1);
    lcd.print("Clave: " + passEntered);
  }
}

