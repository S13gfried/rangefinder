#pragma once

#ifndef RANGER_TRIG
  #define RANGER_TRIG 9
#endif
#ifndef RANGER_ECHO
  #define RANGER_ECHO 10
#endif
#ifndef SOUND_CONSTANT
  #define SOUND_CONSTANT 0.0344
#endif
#ifndef RANGER_INIT_DELAY_MCS 
  #define RANGER_INIT_DELAY_MCS 2
#endif

float ping(int pulseWidthMcs  = 2);
float pingMean(int sampleCount = 10, int deltaMicros = 100, int pulseWidthMcs = 2);


float ping(int pulseWidthMcs)
{
  //Clear pin in case it's been set to 1
  digitalWrite(RANGER_TRIG, 0);
  delayMicroseconds(RANGER_INIT_DELAY_MCS);

  //Fire ultrasonic pulse
  digitalWrite(RANGER_TRIG, 1);
  delayMicroseconds(pulseWidthMcs);
  digitalWrite(RANGER_TRIG, 0);

  //Receive & perform calculations
  long deltaTime = pulseIn(RANGER_ECHO, 1);
  float distance = (float)deltaTime * SOUND_CONSTANT / 2;

  return distance;
}

float pingMean(int sampleCount, int deltaMicros, int pulseWidthMcs)
{
  float distanceCumulative = 0.0;
  
  for(int i = 0; i < sampleCount; i++)
  {
    distanceCumulative += ping(pulseWidthMcs);
    delayMicroseconds(deltaMicros);
  }

  return distanceCumulative / (float)sampleCount;
}