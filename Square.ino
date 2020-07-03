// --- Includes ---

#include <Wire.h> // Used for Accelerometer
#include <Zumo32U4.h>
#include "TurnSensor.h"

Zumo32U4ButtonA buttonA;
Zumo32U4Motors motors;
Zumo32U4LCD lcd;
Zumo32U4Encoders encoders;
L3G gyro;
Zumo32U4ProximitySensors proxSensors;

// --- Enums ---

// This enum is used for our "state machine."
enum State {
  pause_state,
  forward_state,
  stop_state
  
};

// --- Constants ---

const uint16_t motorSpeed = 300;
const uint16_t turnSpeed = 200;
const int acceleration = 2;
const int maxNumberOfTurns = 4;

#define SPEED 300

#define Kp 1
#define STRAIGHTFACTOR 1  // Adjust this to correct for minor curve.  Should be in the 0.9 to 1.1 range

// --- Global Variables ---

State state = pause_state;
int curSpeed = 0;
int turnsCounter = 0;
int left_sensor;
int right_sensor;
int centerLeftSensor;
int centerRightSensor;

void setup() {

  // Proximity sensors
  proxSensors.initThreeSensors();
  
  // put your setup code here, to run once:
  // Gyrometer
  turnSensorSetup();   // setup turnsenor
  delay(500);
  turnSensorReset();
}

void loop() {
  // put your main code here, to run repeatedly:

   // Buttons
  bool buttonPress = buttonA.getSingleDebouncedPress();

  // Proximity sensors
  proxSensors.read();

  left_sensor = proxSensors.countsLeftWithLeftLeds();
  centerLeftSensor = proxSensors.countsFrontWithLeftLeds();
  centerRightSensor = proxSensors.countsFrontWithRightLeds();
  right_sensor = proxSensors.countsRightWithRightLeds();

  if (buttonPress) {
    
    delay(500);
    if (centerLeftSensor < 4 && centerRightSensor < 4) {   // nothing in front so go
      for (int i = 0; i < maxNumberOfTurns; i++) {
        forwardStraight(4000);
        turnRight(90);
     }
    } else {
      turnRight(180);   // turn right 180 degrees
    }
 
  }

   // --- Set motor speed

//  if (state != pause_state && curSpeed < motorSpeed) {    // if moving, and current speed is < max speed
//    curSpeed += acceleration;                            // then accelerate forward
//  }
//  if (state == pause_state) {
//    stop();
//    curSpeed = 0;
//  }
  

}

// Turn right
void turnRight(int degrees) {
  turnsCounter++;
  turnSensorReset();
  motors.setSpeeds(turnSpeed, -turnSpeed);
  int angle = 0;
  lcd.gotoXY(0, 0);
  lcd.print("Right  ");
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    lcd.gotoXY(0, 0);
    lcd.print(angle);
    lcd.print(" ");
  } while (angle > -degrees);
  motors.setSpeeds(0, 0);
  delay(500);
}

// Stop
void stop() {
  motors.setSpeeds(0, 0);
}



void forwardStraight(int distance)
{
  int currentSpeedLeft=SPEED;
  int currentSpeedRight=SPEED;
  int error;
  int correction;
  int countsLeft = encoders.getCountsAndResetLeft();
  int countsRight = encoders.getCountsAndResetRight();

  motors.setSpeeds(currentSpeedLeft,currentSpeedRight);
  do {
     // Proximity sensors
     proxSensors.read();

     left_sensor = proxSensors.countsLeftWithLeftLeds();
     centerLeftSensor = proxSensors.countsFrontWithLeftLeds();
     centerRightSensor = proxSensors.countsFrontWithRightLeds();
     right_sensor = proxSensors.countsRightWithRightLeds();

     if (centerLeftSensor >= 5 && centerRightSensor >= 5) {  // something is in front of robot while going forward
          break;
     }
     
    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft-STRAIGHTFACTOR*countsRight;
    correction = Kp*error;
    currentSpeedRight = SPEED + correction;
    motors.setSpeeds(currentSpeedLeft,currentSpeedRight);
  }  while(countsLeft<distance && countsRight<distance);
  motors.setSpeeds(0,0);
}
