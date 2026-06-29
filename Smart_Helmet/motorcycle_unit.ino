/*
 * Smart Helmet - Motorcycle Unit (Receiver)
 * Listens for status codes from the helmet unit via RF.
 * Controls relay, LCD, LEDs, and buzzer accordingly.
 *
 * Status → Action:
 *   SAFE      → Relay ON,  LCD: "Engine Ready",     Green LED
 *   NO_HELMET → Relay OFF, LCD: "Wear Helmet!",     Red LED + Buzzer
 *   ALCOHOL   → Relay OFF, LCD: "Alcohol Detected", Red LED + Buzzer
 *   SLEEP     → Relay OFF, LCD: "Drowsy! Stop!",    Red LED + Buzzer
 *
 * Libraries required:
 *   VirtualWire       (install via Arduino Library Manager)
 *   LiquidCrystal_I2C (install via Arduino Library Manager)
 */

#include <VirtualWire.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ── Pin Definitions ──────────────────────────────────────────────────────────
const int PIN_RF_RX   = 11;
const int PIN_RELAY   = 7;
const int PIN_LED_GRN = 5;
const int PIN_LED_RED = 6;
const int PIN_BUZZER  = 8;

// ── LCD ──────────────────────────────────────────────────────────────────────
// Change 0x27 to 0x3F if display stays blank (run I2C scanner to confirm)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ── Timing ───────────────────────────────────────────────────────────────────
const unsigned long SIGNAL_TIMEOUT_MS = 3000;
unsigned long lastReceiveTime = 0;

// Track last status to avoid unnecessary LCD refreshes
char lastStatus[16] = "";

// ── Output helpers ───────────────────────────────────────────────────────────
void setSafe() {
  digitalWrite(PIN_RELAY,   HIGH);
  digitalWrite(PIN_LED_GRN, HIGH);
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_BUZZER,  LOW);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("  Engine Ready  ");
  lcd.setCursor(0, 1); lcd.print("  Ride Safely!  ");
}

void setWarning(const char* line1, const char* line2) {
  digitalWrite(PIN_RELAY,   LOW);
  digitalWrite(PIN_LED_GRN, LOW);
  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_BUZZER,  HIGH);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(line1);
  lcd.setCursor(0, 1); lcd.print(line2);
}

void setNoSignal() {
  digitalWrite(PIN_RELAY,   LOW);
  digitalWrite(PIN_LED_GRN, LOW);
  digitalWrite(PIN_LED_RED, HIGH);
  digitalWrite(PIN_BUZZER,  LOW);
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("  No Signal...  ");
  lcd.setCursor(0, 1); lcd.print("Check Helmet TX ");
}

void applyStatus(const char* status) {
  if      (strcmp(status, "SAFE")      == 0) setSafe();
  else if (strcmp(status, "NO_HELMET") == 0) setWarning("  Wear Helmet!  ", " Engine Blocked ");
  else if (strcmp(status, "ALCOHOL")   == 0) setWarning("Alcohol Detected", " Engine Blocked ");
  else if (strcmp(status, "SLEEP")     == 0) setWarning(" Drowsy! Stop!  ", " Engine Blocked ");
  else                                       setWarning("  Unknown Code  ", " Engine Blocked ");
}

// ─────────────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  pinMode(PIN_RELAY,   OUTPUT);
  pinMode(PIN_LED_GRN, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_BUZZER,  OUTPUT);

  // Safe defaults — engine off until a SAFE signal is received
  digitalWrite(PIN_RELAY,   LOW);
  digitalWrite(PIN_LED_GRN, LOW);
  digitalWrite(PIN_LED_RED, LOW);
  digitalWrite(PIN_BUZZER,  LOW);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0); lcd.print(" Smart Helmet   ");
  lcd.setCursor(0, 1); lcd.print(" System Ready   ");
  delay(2000);
  lcd.clear();

  vw_set_rx_pin(PIN_RF_RX);
  vw_setup(2000);  // Must match transmitter baud rate
  vw_rx_start();

  lastReceiveTime = millis();
  Serial.println("Smart Helmet Receiver Ready");
}

// ─────────────────────────────────────────────────────────────────────────────
void loop() {
  uint8_t buf[VW_MAX_MESSAGE_LEN];
  uint8_t bufLen = VW_MAX_MESSAGE_LEN;

  // 1. Check for incoming RF message
  if (vw_get_message(buf, &bufLen)) {
    buf[bufLen] = '\0';
    const char* received = (char*)buf;

    lastReceiveTime = millis();

    // Only update outputs when the status actually changes
    if (strcmp(received, lastStatus) != 0) {
      strncpy(lastStatus, received, sizeof(lastStatus) - 1);
      lastStatus[sizeof(lastStatus) - 1] = '\0';

      applyStatus(received);

      Serial.print("Received: ");
      Serial.println(received);
    }
  }

  // 2. Signal timeout — treat lost connection as unsafe
  if ((millis() - lastReceiveTime) > SIGNAL_TIMEOUT_MS) {
    if (strcmp(lastStatus, "TIMEOUT") != 0) {
      strncpy(lastStatus, "TIMEOUT", sizeof(lastStatus) - 1);
      setNoSignal();
      Serial.println("Warning: No signal from helmet unit.");
    }
  }
}
