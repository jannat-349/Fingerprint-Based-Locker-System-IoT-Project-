#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
const int rs = 7, en = 6, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

uint8_t id;
bool enrolled = false;

void setup() {
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  lcd.begin(16, 2);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    lcd.clear();
    lcd.print("Sensor Found!");
    delay(2000);
    lcd.clear();
  } else {
    lcd.clear();
    lcd.print("Please wait...");
    while (1) { delay(1); }
  }
}

void countdown() {
  for (int i = 10; i >= 0; i--) {
    lcd.clear();
    lcd.print("Time left: ");
    lcd.print(i);
    lcd.print("s");
    delay(1000);
  }
}

uint8_t getFingerprintEnroll() {
  int p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        break;
      case FINGERPRINT_NOFINGER:
        lcd.clear();
        lcd.print("Press ---->");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
      case FINGERPRINT_IMAGEFAIL:
      default:
        lcd.clear();
        lcd.print("Try Again!");
        delay(2000);
        lcd.clear();
        break;
    }
  }

  // Image captured, now converting to template

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    default:
      lcd.clear();
      lcd.print("Try Again!");
      delay(2000);
      lcd.clear();
      return p;
  }

  // First template created, now ask for another finger

  lcd.clear();
  lcd.print("Remove finger");
  delay(2000);
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  lcd.clear();
  lcd.print("Place again");
  delay(2000);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        break;
      case FINGERPRINT_NOFINGER:
        lcd.clear();
        lcd.print("Press ---->");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
      case FINGERPRINT_IMAGEFAIL:
      default:
        lcd.clear();
        lcd.print("Try Again!");
        delay(2000);
        lcd.clear();
        break;
    }
  }

  // Second image captured, now converting to template

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      break;
    default:
      lcd.clear();
      lcd.print("Try Again!");
      delay(2000);
      lcd.clear();
      return p;
  }

  // Both templates created, now creating model

  p = finger.createModel();
  switch (p) {
    case FINGERPRINT_OK:
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
    case FINGERPRINT_ENROLLMISMATCH:
    default:
      lcd.clear();
      lcd.print("Try Again!");
      delay(2000);
      lcd.clear();
      return p;
  }

  // Model created, now storing the model

  p = finger.storeModel(id);
  switch (p) {
    case FINGERPRINT_OK:
      lcd.clear();
      lcd.print("Stored!");
      lcd.setCursor(0, 1);
      lcd.print("Thank You!");
      enrolled = true;
      delay(2000);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
    case FINGERPRINT_BADLOCATION:
    case FINGERPRINT_FLASHERR:
    default:
      lcd.clear();
      lcd.print("Try Again!");
      delay(2000);
      lcd.clear();
      return p;
  }
  return true;
}

void loop() {
  if (!enrolled) {
    lcd.print("Press ---->");
    while (!getFingerprintEnroll())
      ;
  } else {
    lcd.clear();
    lcd.print("Unlocked!");
    delay(1000);
    digitalWrite(12, HIGH);
    countdown();
    lcd.clear();
    lcd.print("Locking...");
    delay(3000);
    digitalWrite(12, LOW);
    lcd.clear();
    while (1) {
      lcd.print("Locked!");
      lcd.setCursor(0, 1);
      lcd.print("Press to unlock>");
    }
  }
}
