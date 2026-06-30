/*
  -------------------------------------------------------
  Light Following Robot using Arduino Uno
  Author: Your Name
  Website: Robocraze Blog
  -------------------------------------------------------

  Components:
  - Arduino Uno
  - L298N Motor Driver
  - 2 x LDR Sensors
  - 2 x DC Geared Motors

  Connections

  Left LDR  -> A0
  Right LDR -> A1

  IN1 -> D8
  IN2 -> D9
  IN3 -> D10
  IN4 -> D11

  ENA -> D5 (PWM)
  ENB -> D6 (PWM)
*/

const int leftLDR = A0;
const int rightLDR = A1;

const int IN1 = 8;
const int IN2 = 9;
const int IN3 = 10;
const int IN4 = 11;

const int ENA = 5;
const int ENB = 6;

const int motorSpeed = 180;
const int threshold = 40;

int leftValue = 0;
int rightValue = 0;

void setup()
{
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  stopRobot();
}

void loop()
{
  leftValue = analogRead(leftLDR);
  rightValue = analogRead(rightLDR);

  Serial.print("Left: ");
  Serial.print(leftValue);
  Serial.print("    Right: ");
  Serial.println(rightValue);

  int difference = leftValue - rightValue;

  if (abs(difference) < threshold)
  {
    moveForward();
  }
  else if (difference > 0)
  {
    turnLeft();
  }
  else
  {
    turnRight();
  }

  delay(50);
}

// ---------------- Motor Functions ----------------

void moveForward()
{
  analogWrite(ENA, motorSpeed);
  analogWrite(ENB, motorSpeed);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft()
{
  analogWrite(ENA, 120);
  analogWrite(ENB, 200);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight()
{
  analogWrite(ENA, 200);
  analogWrite(ENB, 120);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void stopRobot()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
