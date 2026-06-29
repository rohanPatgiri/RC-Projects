/*
 * Smart Helmet - Helmet Unit (Transmitter)
 * Reads IR helmet sensor, MQ-3 alcohol sensor, and optional eye-blink sensor.
 * Transmits one of four status codes via RF module:
 *   SAFE | NO_HELMET | ALCOHOL | SLEEP
 *
 * Libraries required: VirtualWire (install via Arduino Library Manager)
 */

#include <VirtualWire.h>

// ── Pin Definitions ──────────────────────────────────────────────────────────
const int PIN_IR_SENSOR    = 2;
const int PIN_BLINK_SENSOR = 3;
const int PIN_ALCOHOL      = A0;
const int PIN_RF_TX        = 12;
const int PIN_LED          = 7;
const int PIN_BUZZER       = 8;

// ── Thresholds ───────────────────────────────────────────────────────────────
const int           ALCOHOL_THRESHOLD    = 400;   // Calibrate after warm-up
const unsigned long BLINK_DURATION_LIMIT = 2000;  // ms eyes closed = drowsy

// ── Globals ──────────────────────────────────────────────────────────────────
unsigned long eyesClosedSince = 0;
bool          eyesWereClosed  = false;

// ── Helpers ──────────────────────────────────────────────────────────────────
void transmitMessage(const char* msg) {
  vw_send((uint8_t*)msg, strlen(msg));
  vw_wait_tx();
}

void alertOn()  { digitalWrite(PIN_LED, HIGH); digitalWrite(PIN_BUZZER, HIGH); }
void alertOff() { digitalWrite(PIN_LED, LOW);  digitalWrite(PIN_BUZZER, LOW);  }

// ─────────────────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);

  pinMode(PIN_IR_SENSOR,    INPUT);
  pinMode(PIN_BLINK_SENSOR, INPUT);
  pinMode(PIN_LED,          OUTPUT);
  pinMode(PIN_BUZZER,       OUTPUT);
  alertOff();

  vw_set_tx_pin(PIN_RF_TX);
  vw_setup(2000);  // 2000 bps — must match receiver

  Serial.println("Smart Helmet Transmitter Ready");
  Serial.println("Warming up MQ-3 (30s)...");
  delay(30000);
  Serial.println("Warm-up done. Monitoring started.");
}

// ─────────────────────────────────────────────────────────────────────────────
void loop() {
  // 1. Read sensors
  bool helmetOn   = (digitalRead(PIN_IR_SENSOR) == LOW);  // LOW = object detected
  int  alcoholRaw = analogRead(PIN_ALCOHOL);
  bool blinkNow   = (digitalRead(PIN_BLINK_SENSOR) == HIGH);

  // 2. Drowsiness: eyes must stay closed for BLINK_DURATION_LIMIT ms
  bool drowsy = false;
  if (blinkNow) {
    if (!eyesWereClosed) {
      eyesClosedSince = millis();
      eyesWereClosed  = true;
    } else if ((millis() - eyesClosedSince) >= BLINK_DURATION_LIMIT) {
      drowsy = true;
    }
  } else {
    eyesWereClosed = false;
  }

  // 3. Determine status (priority order matters)
  const char* status;
  if      (!helmetOn)                    status = "NO_HELMET";
  else if (alcoholRaw > ALCOHOL_THRESHOLD) status = "ALCOHOL";
  else if (drowsy)                       status = "SLEEP";
  else                                   status = "SAFE";

  // 4. Local feedback
  if (strcmp(status, "SAFE") == 0) alertOff();
  else                             alertOn();

  // 5. Transmit
  transmitMessage(status);

  // 6. Serial debug
  Serial.print("Helmet: ");  Serial.print(helmetOn ? "ON" : "OFF");
  Serial.print(" | Alcohol ADC: "); Serial.print(alcoholRaw);
  Serial.print(" | Drowsy: ");      Serial.print(drowsy ? "YES" : "NO");
  Serial.print(" | TX: ");          Serial.println(status);

  delay(200);  // ~5 transmissions per second
}
