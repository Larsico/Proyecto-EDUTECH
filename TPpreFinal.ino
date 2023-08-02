#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

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

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C y dimensiones del display LCD

const int pirPin = 9;
const int pirRelayPin = 7;
const int puertaRelayPin = 8;
char passcode[] = "1234";
bool isLocked = true;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(pirRelayPin, OUTPUT);
  pinMode(puertaRelayPin, OUTPUT);

  keypad.addEventListener(keypadEvent);

  lcd.init(); // Inicializa el display LCD
  lcd.backlight(); // Enciende la retroiluminación
  lcd.setCursor(0, 0);
  lcd.print("Puerta: Cerrada");
}

void loop() {
  int pirValue = digitalRead(pirPin);

  if (pirValue == HIGH) {
    digitalWrite(pirRelayPin, HIGH);
  } else {
    digitalWrite(pirRelayPin, LOW);
  }

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