#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Tone.h>

Tone tone1;

int Scount = 10; // count seconds
int Mcount = 5; // count minutes
int Hcount = 0; // count hours

long secMillis = 0; // store last time for second add
long interval = 1000; // interval for seconds

char password[4];
char entered[4];
int currentLength = 0;

int ledPin = 4; //red LED
int ledPin2 = 3; //yellow LED
int ledPin3 = 2; //green LED

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD address and size: 16x2

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {5, A5, A4, A2};
byte colPins[COLS] = {A1, A0, A3};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  tone1.begin(9);

  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Code: ");
  
  while (currentLength < 4) {
    lcd.setCursor(currentLength + 6, 1);
    lcd.cursor();
    char key = keypad.getKey();
    if (key != NO_KEY) {
      if (key != '*' && key != '#') {
        lcd.print(key);
        password[currentLength++] = key;
        tone1.play(NOTE_C6, 200);
      }
    }
  }

  lcd.noCursor();
  lcd.clear();
  lcd.print("You've Entered: ");
  lcd.setCursor(6, 1);
  lcd.print(password);
  tone1.play(NOTE_E6, 200);
  delay(3000);
  lcd.clear();
  currentLength = 0;
}

void loop() {
  timer();
  char key2 = keypad.getKey();
  
  if (key2 == '*') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Code: ");
    currentLength = 0;
    
    while (currentLength < 4) {
      timer();
      char key2 = keypad.getKey();
      if (key2 == '#') {
        currentLength = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Code: ");
      } else if (key2 != NO_KEY) {
        lcd.setCursor(currentLength + 7, 0);
        lcd.cursor();
        lcd.print(key2);
        entered[currentLength++] = key2;
        tone1.play(NOTE_C6, 200);
        delay(100);
        lcd.noCursor();
        lcd.setCursor(currentLength + 6, 0);
        lcd.print("*");
        lcd.setCursor(currentLength + 7, 0);
        lcd.cursor();
      }
    }

    if (comparePasswords()) {
      lcd.noCursor();
      lcd.clear();
      lcd.print("Bomb Defused");
      digitalWrite(ledPin3, HIGH);
      delay(2500);
      lcd.setCursor(0, 1);
      lcd.print("Reset the Bomb");
      delay(1000000);
    } else {
      handleWrongPassword();
    }
  }
}

bool comparePasswords() {
  for (int i = 0; i < 4; i++) {
    if (entered[i] != password[i]) return false;
  }
  return true;
}

void handleWrongPassword() {
  lcd.noCursor();
  lcd.clear();
  lcd.print("Wrong Password!");
  if (Hcount > 0) {
    Hcount--;
  }
  if (Mcount > 0) {
    Mcount -= 59;
  }
  if (Scount > 0) {
    Scount -= 59;
  }
  delay(1500);
  currentLength = 0;
}

void timer() {
  if (Hcount <= 0 && Mcount < 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("The Bomb Has ");
    lcd.setCursor(0, 1);
    lcd.print("Exploded!");
    
    while (true) {
      blinkLEDs();
    }
  }

  updateLCD();
  updateTimer();
}

void updateLCD() {
  lcd.setCursor(0, 1);
  lcd.print("Timer:");
  lcd.setCursor(7, 1);
  lcd.print(Hcount < 10 ? "0" : "");
  lcd.print(Hcount);
  lcd.print(":");
  lcd.setCursor(10, 1);
  lcd.print(Mcount < 10 ? "0" : "");
  lcd.print(Mcount);
  lcd.print(":");
  lcd.setCursor(13, 1);
  lcd.print(Scount < 10 ? "0" : "");
  lcd.print(Scount);
}

void updateTimer() {
  if (Scount > 0) {
    unsigned long currentMillis = millis();
    if (currentMillis - secMillis > interval) {
      tone1.play(NOTE_G5, 200);
      secMillis = currentMillis;
      Scount--;
      digitalWrite(ledPin2, HIGH);
      delay(10);
      digitalWrite(ledPin2, LOW);
      delay(10);
    }
  }
  if (Scount < 0) {
    Scount = 59;
    Mcount--;
  }
  if (Mcount < 0) {
    Mcount = 59;
    Hcount--;
  }
}

void blinkLEDs() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(ledPin, HIGH);
    tone1.play(NOTE_A2, 90);
    delay(100);
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin2, HIGH);
    tone1.play(NOTE_A2, 90);
    delay(100);
    digitalWrite(ledPin2, LOW);
    digitalWrite(ledPin3, HIGH);
    tone1.play(NOTE_A2, 90);
    delay(100);
    digitalWrite(ledPin3, LOW);
  }
}

