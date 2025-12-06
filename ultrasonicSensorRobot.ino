#include <Servo.h>

// Servo
Servo myservo;
const int servoPin = 13;

// Ultrasonic
const int trig = 5;
const int echo = 4;

// Motors
int in1 = 8;
int in2 = 9;
int in3 = 10;
int in4 = 11;

//Motor Speed
const int EnA = 7;
const int EnB = 12;

// ---------- Motor helpers ----------
void stopMoving() {
  digitalWrite(in1, LOW); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW); digitalWrite(in4, LOW);
}

void goForward() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
}

void goBackward() {
  digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);  digitalWrite(in4, HIGH);
}

void turnLeft() {
  digitalWrite(in1, LOW);  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW);
}

void turnRight() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);  digitalWrite(in4, HIGH);
}

// ---------- Distance ----------
int getDistanceCm() {
  unsigned long duration;
  float dist;

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH, 30000); // timeout

  if (duration == 0) {
    Serial.println("No echo (timeout)");
    return 200;
  }

  dist = (duration * 0.0343) / 2.0;
  Serial.print("Distance: ");
  Serial.println(dist);

  return (int)dist;
}

// ---------- Setup ----------
void setup() {
  Serial.begin(9600);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode (EnA, OUTPUT);
  pinMode (EnB, OUTPUT);

  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  myservo.attach(servoPin);
  myservo.write(90);
}

// ---------- Main loop ----------
void loop() {
  // Look forward
  myservo.write(90);
  delay(200);
  int distFront = getDistanceCm();

  if (distFront > 15) {
    // Clear path: go forward
    Serial.println("Forward");
    analogWrite(EnA, 100);
    analogWrite(EnB, 100);
    goForward();
    delay(100);
  } else {
    // Obstacle close: stop, scan sides
    stopMoving();
    delay(200);

    // Look left
    myservo.write(150);
    delay(300);
    int distLeft = getDistanceCm();

    if (distLeft > 10) {;
      stopMoving();
      delay(100);
      goBackward();
      delay(400);
      turnLeft();
      delay(300);   // tune how much it turns
      stopMoving();
    } else {
      // Left blocked, try right
      myservo.write(30);
      delay(300);
      int distRight = getDistanceCm();

      if (distRight > 10) {
        stopMoving();
        delay(100);
        goBackward();
        delay(400);
        turnRight();
        delay(300);
        stopMoving();
      } else {
        // Both sides blocked: reverse a bit
        stopMoving();
        delay(100);
        goBackward();
        delay(1000);
        stopMoving();
        delay(100);
        turnRight();
        delay(300);
        stopMoving();
      }
    }
    // Recenter sensor for next loop
    myservo.write(90);
    delay(200);
  }
}
