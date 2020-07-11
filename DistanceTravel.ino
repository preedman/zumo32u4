// Distance traveller
// Formulas
// 
/*
 * https://forum.pololu.com/t/measure-distance-using-encoders-zumo-32u4/19076
 * Distance traveled = Wheel rotations * circumference = (encoder ticks / counts per revolution) * circumference
 * The manual gives us CPR (count per revolution) as  75.81 × 12 ≈ 909.7 CPR.
 * https://www.pololu.com/docs/0J63/3.4
 * Circumference =  circumference = pi * diameter
 * diameter = 39mm
 * pi = 3.14159265359
 * 
 * Demo to show how you can drive straight by a certain distance.
 * Does have limitation in terms how far it can go,because of overflow of API
 * https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_encoders.html#a142d33610a12b209e257c1635b2daae6
 * 
 * 
 */

#include <Zumo32U4.h>

Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4LCD lcd;
Zumo32U4ButtonA buttonA;

 int countsLeft;
 int countsRight;
 double circumference;
 double CPR = 909.7;


void setup() {
  // put your setup code here, to run once:

  countsLeft = encoders.getCountsAndResetLeft();
  countsRight = encoders.getCountsAndResetRight();
  circumference = 3.14159265359 * 39;
  Serial.begin(9600); // send and receive at 9600 baud
}

void loop() {
  // put your main code here, to run repeatedly:

 
 
  

  // Buttons
  bool buttonPress = buttonA.getSingleDebouncedPress();

  
  
  
    if (buttonPress) {
        delay(500);
       
             
              int distance = forward(1800);   // millimeters 
                                     
              lcd.gotoXY(0, 0);
              lcd.print(distance);
              lcd.gotoXY(0, 1);
              lcd.print(circumference);
              lcd.print(" ");
              
        
    }
          
 
}

int forward(int distanceTravel) {

  int d;
  motors.setSpeeds(200,200);

  do {
    countsLeft = encoders.getCountsLeft();
    d = ( countsLeft / CPR ) * circumference;
    
  } while ( d < distanceTravel );

  motors.setSpeeds(0,0);

  return d;
}
