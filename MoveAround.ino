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
LSM303 lsm303; // Accelerometer


// Global Variables

int left_sensor;
int right_sensor;
int centerLeftSensor;
int centerRightSensor;
int turnSpeed = 200;
int maxNumberOfTurns = 10;
int16_t x; 
int16_t y;
int32_t magnitudeSquared;
 int countsLeft;
  int countsRight;
bool hitSomething = false;
bool leftPathClear = false;
bool rightPathClear = false;


#define Kp 1
#define STRAIGHTFACTOR 1  // Adjust this to correct for minor curve.  Should be in the 0.9 to 1.1 range
#define SPEED 300

void setup() {
  // put your setup code here, to run once:
  // Proximity sensors
  proxSensors.initThreeSensors();

  // Accelerometer
  Wire.begin();
  lsm303.init();    // initisalise the accelartomer
  lsm303.enableDefault();  // here as well
  
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

  

  if (buttonPress) {
    
    delay(500);
    for (int i = 0; i < maxNumberOfTurns; i++) {
          forward();
          if (hitSomething) {
              reverse();
              turnRight(90);
              leftPathClear = false;
               rightPathClear = false;
          } else {
             
             if (rightPathClear) {
               turnRight(90);
               leftPathClear = false;
               rightPathClear = false;
             } else {
                turnLeft(90);
                leftPathClear = false;
                rightPathClear = false;
             }
          } 
    }  
     
        
    
  }
  
}

void scanLeftRight() {

  leftPathClear = false;
  rightPathClear = false;

  proxSensors.read();

  left_sensor = proxSensors.countsLeftWithLeftLeds();
  centerLeftSensor = proxSensors.countsFrontWithLeftLeds();
  centerRightSensor = proxSensors.countsFrontWithRightLeds();
  right_sensor = proxSensors.countsRightWithRightLeds();

  if (centerLeftSensor < centerRightSensor) {            //  this test indicates left might be more clear than right 
    leftPathClear = true;
  } else {
    rightPathClear = true;
  }
  
}

// Back up!
void reverse() {
  countsLeft = encoders.getCountsAndResetLeft();   
  countsRight = encoders.getCountsAndResetRight();   
  motors.setSpeeds(-200,-200);   
  do {     
    countsLeft = encoders.getCountsLeft();     
    countsRight = encoders.getCountsRight();   
  }  while(countsLeft>-1000&&countsRight>-1000);
  
  motors.setSpeeds(0,0);
  hitSomething = false;
}

void forward() {
  
     

     int currentSpeedLeft=SPEED;
     int currentSpeedRight=SPEED;
     int countsLeft = encoders.getCountsAndResetLeft();
     int countsRight = encoders.getCountsAndResetRight();
     int error;
      int correction;
     long randNumber;
      motors.setSpeeds(currentSpeedLeft,currentSpeedRight);    // set the speed
      
     while(1) {
     // Proximity sensors
          proxSensors.read();

          left_sensor = proxSensors.countsLeftWithLeftLeds();
          centerLeftSensor = proxSensors.countsFrontWithLeftLeds();
          centerRightSensor = proxSensors.countsFrontWithRightLeds();
          right_sensor = proxSensors.countsRightWithRightLeds();

          // Accelerometer
          lsm303.read();
          int16_t x = lsm303.a.x;
          int16_t y = lsm303.a.y;
          int32_t magnitudeSquared = (int32_t)x * x + (int32_t)y * y;  // this number is used to figure out if the robot has bumped into anything


          if (centerLeftSensor >= 5 && centerRightSensor >= 5) {  // something is in front of robot while going forward
                if (centerLeftSensor < centerRightSensor) {            //  this test indicates left might be more clear than right 
                   leftPathClear = true;
                   break;
                } else if (centerLeftSensor > centerRightSensor) {     // right is clearer
                   rightPathClear = true;
                   break;
                } else if (centerLeftSensor == centerRightSensor) {    // both the same - toss a coin
                   randNumber = random(10, 21);
                   if (randNumber <= 15 ) {
                        leftPathClear = true;
                        break;
                   } else {
                        rightPathClear = true;
                        break;
                   }
                  
                }
          }
                            
          
           
          
          if (magnitudeSquared > 250000000) {   // hit something
                hitSomething = true;
                break;
          }
          
          countsLeft = encoders.getCountsLeft();   // do some fancy calcs to compensate for moving off centre
          countsRight = encoders.getCountsRight();
          error = countsLeft-STRAIGHTFACTOR*countsRight;
          correction = Kp*error;
          currentSpeedRight = SPEED + correction;
          
          motors.setSpeeds(currentSpeedLeft,currentSpeedRight);  // set the speed again
      }   
   
}

// Turn left
void turnLeft(int degrees) {
  turnSensorReset();
  motors.setSpeeds(-turnSpeed, turnSpeed);
  int angle = 0;
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    lcd.gotoXY(0, 0);
    lcd.print(angle);
    lcd.print(" ");
  } while (angle < degrees);
  motors.setSpeeds(0, 0);
  delay(500);
}


// Turn right
void turnRight(int degrees) {
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
