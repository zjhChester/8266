#include <Stepper.h>


const int STEPS_PER_ROTOR_REV = 32;
const int GEAR_REDUCTION = 64;
const int STEPS_PER_REVOLUTION = STEPS_PER_ROTOR_REV * GEAR_REDUCTION;

int StepsRequired = 128;
Stepper steppermotor(STEPS_PER_ROTOR_REV, 16, 4, 5, 0);  // IN1, IN3, IN2, IN4

void setup()
{
  Serial.println("-----1");
  Serial.begin(9600);
  Serial.println("-----2");
  steppermotor.setSpeed(700);
  Serial.println("-----3");
}

void loop()
{
  Serial.println("-----4");
  Serial.println(StepsRequired);
  steppermotor.step(StepsRequired);
  delay(50);
}