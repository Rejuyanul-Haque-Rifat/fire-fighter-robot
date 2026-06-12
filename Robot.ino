const uint8_t IN1 = 5;
const uint8_t IN2 = 6;
const uint8_t IN3 = 7;
const uint8_t IN4 = 8;

const uint8_t TRIG_PIN = 3;
const uint8_t ECHO_PIN = 4;
const uint8_t RELAY_PIN = 10;
const uint8_t ARM_PIN = 9;
const uint8_t SMOKE_PIN = A0;

const uint16_t SMOKE_THRESHOLD = 400;
const uint8_t OBSTACLE_DISTANCE = 20;
const uint32_t PULSE_TIMEOUT = 30000;

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(ARM_PIN, OUTPUT);
  pinMode(SMOKE_PIN, INPUT);
}

uint16_t getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, PULSE_TIMEOUT);

  if (duration == 0) {
    return 999;
  }
  return duration / 58;
}

uint16_t getSmokeLevel() {
  uint16_t level = 0;
  for (uint8_t i = 0; i < 5; i++) {
    level += analogRead(SMOKE_PIN);
    delay(5);
  }
  return level / 5;
}

void stopMovement() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void moveForward() {
  stopMovement();
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward(uint16_t durationMs) {
  stopMovement();
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(durationMs);
  stopMovement();
}

void rotateLeft(uint16_t durationMs) {
  stopMovement();
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  delay(durationMs);
  stopMovement();
}

void rotateRight(uint16_t durationMs) {
  stopMovement();
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  delay(durationMs);
  stopMovement();
}

void avoidObstacle() {
  moveBackward(400);
  rotateRight(400);
  
  if (getDistance() < OBSTACLE_DISTANCE) {
    rotateLeft(800);
  }
}

void extinguishFire() {
  digitalWrite(ARM_PIN, HIGH);
  digitalWrite(RELAY_PIN, HIGH);

  delay(5000);

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(ARM_PIN, LOW);
  delay(1000);

  unsigned long waitStart = millis();
  while (getSmokeLevel() > (SMOKE_THRESHOLD - 50)) {
    if (millis() - waitStart > 10000) {
      break;
    }
    delay(100);
  }
}

void loop() {
  uint16_t smokeLevel = getSmokeLevel();
  uint16_t distance = getDistance();

  Serial.print("Smoke: ");
  Serial.print(smokeLevel);
  Serial.print(" Distance: ");
  Serial.println(distance);

  if (smokeLevel > SMOKE_THRESHOLD) {
    stopMovement();
    extinguishFire();
  } else if (distance < OBSTACLE_DISTANCE) {
    stopMovement();
    avoidObstacle();
  } else {
    moveForward();
  }
}
