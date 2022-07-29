/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Arduino.h>
extern volatile unsigned long timer0_millis;

const int sensorPin = 2;
const int measureInterval = 2500;
volatile int pulseConter;
const int relay = 7;
bool resetMillisCheck = true;

unsigned int time = 0;
unsigned long timeToCutWater = 7200000L; // 2 horas
unsigned long timeToWaterPump = 40000L; // 40 segundos

void (*resetSoftware)(void) = 0;


// YF-S201
const float factorK = 7.5;

void ISRCountPulse()
{
  pulseConter++;
}

float GetFrequency()
{
  pulseConter = 0;

  interrupts();
  delay(measureInterval);
  noInterrupts();

  return (float)pulseConter * 1000 / measureInterval;
}

void resetMillis()
{
  noInterrupts();
  timer0_millis = 0;
  interrupts();
}

void setup()
{
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(sensorPin), ISRCountPulse, RISING);

  resetMillisCheck = true;
  // Se activa por primera vez la bomba y se espera a que salga agua
  digitalWrite(relay, LOW);
  delay(timeToWaterPump);
}

void loop()
{

  // obtener frecuencia en Hz
  float frequency = GetFrequency();
  //-----Enviamos por el puerto serie---------------
  Serial.print("FrecuenciaPulsos: ");
  Serial.println(frequency, 0);

  if (frequency = 0)
  {
    // La bomba se desactiva
    digitalWrite(relay, HIGH);
    if (resetMillisCheck)
    {
      resetMillisCheck = false;
      resetMillis();
    }
    Serial.println(millis());
    time = millis();
  }

  if (time > timeToCutWater)
  {
    resetSoftware();
  }
}