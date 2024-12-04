#include <Servo.h>

// Define pins for both stepper motors
#define stepPin1 2    // First stepper motor step pin
#define dirPin1 3     // First stepper motor direction pin
#define stepPin2 5    // Second stepper motor step pin
#define dirPin2 6     // Second stepper motor direction pin
#define MS1_1 42      // First motor MS1
#define MS2_1 44      // First motor MS2
#define MS1_2 46      // Second motor MS1
#define MS2_2 48      // Second motor MS2

// Servo setup
Servo myservo;  
const int MIN_ANGLE = 75;    // Middle position as minimum
const int MAX_ANGLE = 110;   // Maximum angle
const int START_ANGLE = 75;  // Starting at middle position
int currentServoAngle = START_ANGLE;
int servoAngleChange = 2;

// Steps calculation for 1.8° motor with 1/8 microstepping
const int MICROSTEPS = 8;                              // Using 1/8 microstepping
const int STEPS_PER_REV = 200 * MICROSTEPS;           // Full rotation (1600 steps with 1/8 microstepping)
const int STEPS_18_DEG = (200 * MICROSTEPS * 18) / 360; // 80 steps for 18 degrees with 1/8 microstepping

int segmentCount = 0;
int fullRotationCount = 0;


const int startButton = 13;
const int upButton = 12;
const int downButton = 11;

int start = 0;
int up = 0;
int down = 0;

bool isRunning = false;

void setup() {
  // Stepper setup
  pinMode(stepPin1, OUTPUT); 
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT); 
  pinMode(dirPin2, OUTPUT);

  
  // Servo setup
  myservo.attach(9);
  myservo.write(START_ANGLE);
  
  Serial.begin(9600);
  Serial.println("Starting sequence...");
  delay(1000);  // Initial delay
}

void move_motor(int stepPin, int DirPin, int steps, int speed, int direction) {
  digitalWrite(DirPin, direction);
  Serial.println(steps);

  for(int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(speed);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(speed);
  }
}

void moveServo(int newAngle) {
  if(newAngle != currentServoAngle) {
    Serial.print("Moving servo to angle: ");
    Serial.println(newAngle);
    
    myservo.write(newAngle);
    currentServoAngle = newAngle;
    delay(500); // Allow servo to reach position
  }
}

void loop() {

  start = digitalRead(startButton);
  up = digitalRead(upButton);
  down = digitalRead(downButton);


  if(start == HIGH && !isRunning){
    isRunning = true; 
  }

  if (up == HIGH){
    move_motor(stepPin1, dirPin1, STEPS_18_DEG, 400, HIGH);

  } else if (down == HIGH){
    move_motor(stepPin1,dirPin1, STEPS_18_DEG, 400, LOW);

  } else if (isRunning){

    // Move second motor 18 degrees

    move_motor(stepPin2, dirPin2, STEPS_18_DEG, 1000, HIGH);
    delay(300);

    segmentCount++;
    Serial.println("1");


    // Wait for confirmation message "2"
    bool confirmationReceived = false;
    while (!confirmationReceived) {
      if (Serial.available() > 0) {
        String response = Serial.readStringUntil('\n');
        if (response == "2") {
          confirmationReceived = true;
          Serial.println("Confirmation received, continuing sequence");
        }
      }
      delay(10); 
    }

    // If we completed 360 degrees (20 segments of 18 degrees)
    if(segmentCount >= 20 && fullRotationCount <= 8) {
      Serial.println("360 degrees completed - Moving first motor 360 degrees");

      // Move first motor 360 degrees with 1/8 microstepping
      // Breaking full rotation into 8 segments for smoother movement

      move_motor(stepPin1, dirPin1, STEPS_PER_REV, 500, HIGH);
  

      fullRotationCount++;

      // Move servo down 5 degrees after each full rotation
      
      if(currentServoAngle < MAX_ANGLE && fullRotationCount % 2 == 0) {
        int newAngle = currentServoAngle + servoAngleChange;
        if(newAngle > MAX_ANGLE) newAngle = MAX_ANGLE;
        moveServo(newAngle);
        servoAngleChange = servoAngleChange + 4;
      }

      segmentCount = 0;  // Reset segment counter
      delay(2000);
    }

    delay(1000);  // Wait between movements
  }
}