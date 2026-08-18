#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

#define PIN_GREEN_LED  10
#define PIN_RED_LED    11
#define PIN_BUZZER     12
#define PIN_SERVO      13

#define SERVO_LOCKED_ANGLE    0
#define SERVO_UNLOCKED_ANGLE  90

#define MAX_PASSWORD_LENGTH   4
#define MAX_FAILED_ATTEMPTS   3
#define AUTO_LOCK_DELAY_MS    5000UL
#define LOCKOUT_DURATION_MS   30000UL

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo lockServo;

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

const char MASTER_PASSWORD[MAX_PASSWORD_LENGTH + 1] = "1234";
char inputBuffer[MAX_PASSWORD_LENGTH + 1];
byte bufferIndex = 0;
byte failedAttemptCount = 0;

enum SystemState {
  STATE_LOCKED,
  STATE_AUTHENTICATING,
  STATE_UNLOCKED,
  STATE_LOCKOUT
};

SystemState currentState = STATE_LOCKED;
unsigned long stateTimer = 0;

void updateLCD(const char* line1, const char* line2);
void resetInputBuffer();
void processKeyPress(char key);
void verifyPassword();
void unlockLocker();
void lockLocker();
void triggerAlarm();
void handleLockout();

void setup() {
  Serial.begin(9600);
  pinMode(PIN_GREEN_LED, OUTPUT);
  pinMode(PIN_RED_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  digitalWrite(PIN_GREEN_LED, LOW);
  digitalWrite(PIN_RED_LED, LOW);
  digitalWrite(PIN_BUZZER, LOW);

  lockServo.attach(PIN_SERVO);
  lockLocker();

  lcd.init();
  lcd.backlight();
  
  updateLCD("Anti-Theft Lock", "System Ready");
  delay(2000);
  
  resetInputBuffer();
  updateLCD("Enter Password:", "");
}

void loop() {
  if (currentState == STATE_UNLOCKED) {
    if (millis() - stateTimer >= AUTO_LOCK_DELAY_MS) {
      lockLocker();
    }
  }

  if (currentState == STATE_LOCKOUT) {
    handleLockout();
    return;
  }

  char keyPressed = customKeypad.getKey();
  if (keyPressed) {
    processKeyPress(keyPressed);
  }
}

void updateLCD(const char* line1, const char* line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void resetInputBuffer() {
  memset(inputBuffer, 0, sizeof(inputBuffer));
  bufferIndex = 0;
}

void processKeyPress(char key) {
  digitalWrite(PIN_BUZZER, HIGH);
  delay(40);
  digitalWrite(PIN_BUZZER, LOW);

  if (key == '*') {
    resetInputBuffer();
    updateLCD("Enter Password:", "");
    return;
  }

  if (key == '#') {
    if (bufferIndex > 0) {
      verifyPassword();
    }
    return;
  }

  if (bufferIndex < MAX_PASSWORD_LENGTH) {
    inputBuffer[bufferIndex] = key;
    bufferIndex++;
    inputBuffer[bufferIndex] = '\0';

    char maskedDisplay[MAX_PASSWORD_LENGTH + 1];
    for (int i = 0; i < bufferIndex; i++) {
      maskedDisplay[i] = '*';
    }
    maskedDisplay[bufferIndex] = '\0';

    updateLCD("Enter Password:", maskedDisplay);
  }
}

void verifyPassword() {
  updateLCD("Authenticating...", "");
  delay(800);

  if (strcmp(inputBuffer, MASTER_PASSWORD) == 0) {
    failedAttemptCount = 0;
    unlockLocker();
  } else {
    failedAttemptCount++;

    digitalWrite(PIN_RED_LED, HIGH);
    updateLCD("Access Denied!", "Wrong Password");
    
    digitalWrite(PIN_BUZZER, HIGH);
    delay(1000);
    digitalWrite(PIN_BUZZER, LOW);
    digitalWrite(PIN_RED_LED, LOW);

    if (failedAttemptCount >= MAX_FAILED_ATTEMPTS) {
      triggerAlarm();
    } else {
      resetInputBuffer();
      updateLCD("Enter Password:", "");
    }
  }
}

void unlockLocker() {
  currentState = STATE_UNLOCKED;
  stateTimer = millis();

  digitalWrite(PIN_GREEN_LED, HIGH);
  lockServo.write(SERVO_UNLOCKED_ANGLE);
  
  updateLCD("Access Granted!", "Locker Unlocked");
}

void lockLocker() {
  currentState = STATE_LOCKED;
  digitalWrite(PIN_GREEN_LED, LOW);
  lockServo.write(SERVO_LOCKED_ANGLE);
  
  resetInputBuffer();
  updateLCD("Locker Locked", "Enter Password");
}

void triggerAlarm() {
  currentState = STATE_LOCKOUT;
  stateTimer = millis();

  updateLCD("SYSTEM LOCKED!", "Too Many Failed");
}

void handleLockout() {
  unsigned long elapsed = millis() - stateTimer;

  if (elapsed < LOCKOUT_DURATION_MS) {
    digitalWrite(PIN_RED_LED, (millis() / 250) % 2);
    digitalWrite(PIN_BUZZER, (millis() / 500) % 2);

    unsigned long secondsRemaining = (LOCKOUT_DURATION_MS - elapsed) / 1000;
    char timerStr[16];
    sprintf(timerStr, "Wait %lu sec...", secondsRemaining);
    
    lcd.setCursor(0, 1);
    lcd.print(timerStr);
    lcd.print("   ");
  } else {
    digitalWrite(PIN_RED_LED, LOW);
    digitalWrite(PIN_BUZZER, LOW);
    failedAttemptCount = 0;
    currentState = STATE_LOCKED;
    resetInputBuffer();
    updateLCD("System Recovered", "Enter Password");
  }
}
