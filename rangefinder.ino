#include <Servo.h>
#include <TroykaOLED.h>
#include <Arduino.h>

#include "ranging.h"
#include "handlers.h" 

// General parameters
#define HALF_CYCLE_PERIOD_MS 3000
#define SERVO_SPEED_DEG_PER_S 360

#define BUTTON_TOGGLE_ROTATION 2
#define BUTTON_SAVE 3

#define MAX_RANGE 100 // cm
#define MAX_ANGLE 180

// Servo motor
#define SERVO_CTRL 5

// Ultrasound ranger
#define RANGER_TRIG 9
#define RANGER_ECHO 10
#define SOUND_CONSTANT 0.0344

// Display
#define DISPLAY_ADDRESS 0x3C 
#define SEGMENT_COUNT 12

#define TOP_MARGIN 12
#define BOTTOM_MARGIN 5
#define LEFT_MARGIN 7
#define MINIMUM_INDICATOR_THICKNESS 2

void drawSegment(int segment, int height);

// Display specs
int width;
int height;

// Segment width values are:
// 3 5 8 11 13 14
int16_t segmentFrontEdges[] = {0, 4, 10, 19, 31, 45, 60, 75, 87, 99, 108, 114};
int16_t segmentBackEdges[] = {3, 9, 18, 30, 44, 59, 74, 86, 98, 107, 113, 117};
int16_t measuredHeights[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int16_t savedHeights[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int16_t maxSegmentHeight;
float pixelsPerCm;

int stepCounter = 0;
int currentSegment = 0;
float rangerReadings;
float angleStep;

Servo servo;
volatile bool rotationEnabled = true;
TroykaOLED display(DISPLAY_ADDRESS);
 
void setup() 
{
  //Debug
  Serial.begin(9600);
  // Get specs and constants
  width = display.getWidth();
  height = display.getHeight();
  
  maxSegmentHeight = height - TOP_MARGIN - BOTTOM_MARGIN - MINIMUM_INDICATOR_THICKNESS;
  pixelsPerCm = (float)maxSegmentHeight * 2 / MAX_RANGE;
  angleStep = MAX_ANGLE / SEGMENT_COUNT;

  //Attach interrupts
  setupHandlers();

  // Set up ranger pins
  pinMode(RANGER_TRIG, OUTPUT);
  pinMode(RANGER_ECHO, INPUT);

  // Set up servo
  servo.attach(SERVO_CTRL);
  servo.write(angleStep / 2);

  // Start up display I2C
  display.begin();
  display.clearDisplay();
  //Draw limit line

  for(int i = 0; i < SEGMENT_COUNT; i++)
  {
    display.drawLine(LEFT_MARGIN + segmentFrontEdges[i], TOP_MARGIN - 2, LEFT_MARGIN + segmentBackEdges[i] - 1, TOP_MARGIN - 2);
    drawSegment(i, 0);
  }
}
 
void loop() 
{
  // Debounce and apply actions
  buttonLoopRoutine();

  // Get position index for pendulum-like motion

  // Read range, get new segment size
  rangerReadings = pingMean(5, 400);
  int newHeight = (int)(rangerReadings * pixelsPerCm);
    if(newHeight > maxSegmentHeight)
      newHeight = maxSegmentHeight;
  
  measuredHeights[currentSegment] = newHeight;
  drawSegment(currentSegment, newHeight);

  //Get new step
  noInterrupts();
  if(rotationEnabled)
  {
    // Move servo and wait until it is in place (no feedback available)
    stepCounter = (stepCounter + 1) % (SEGMENT_COUNT * 2);
    servo.write(constrain(angleStep * ((float)currentSegment + 0.5), 0, 180));
    delay((int)(angleStep * 1000 / SERVO_SPEED_DEG_PER_S));

    currentSegment = stepCounter;
    if(currentSegment >= SEGMENT_COUNT)
      currentSegment = SEGMENT_COUNT * 2 - 1 - stepCounter;
  }
  interrupts();
  // Proceed to next step
  delay(((float)HALF_CYCLE_PERIOD_MS / SEGMENT_COUNT - angleStep * 1000 / SERVO_SPEED_DEG_PER_S));
}

void drawSegment(int segment, int segHeight)
{
  segHeight = constrain(segHeight, 0, maxSegmentHeight);
  // Erase segment
  display.drawRect(LEFT_MARGIN + segmentFrontEdges[segment], TOP_MARGIN, \
                   LEFT_MARGIN + segmentBackEdges[segment], height-BOTTOM_MARGIN, true, BLACK);
  // Re-draw segment
  display.drawRect(LEFT_MARGIN + segmentFrontEdges[segment], height-BOTTOM_MARGIN - MINIMUM_INDICATOR_THICKNESS - segHeight, \
                   LEFT_MARGIN + segmentBackEdges[segment], height-BOTTOM_MARGIN, true, WHITE);
  // Draw save line
  //display.drawLine(LEFT_MARGIN + segmentFrontEdges[segment], height-BOTTOM_MARGIN - MINIMUM_INDICATOR_THICKNESS - savedHeights[segment], \
  //                 LEFT_MARGIN + segmentBackEdges[segment] - 1, height-BOTTOM_MARGIN - MINIMUM_INDICATOR_THICKNESS - savedHeights[segment], INVERSE);
}

void shortPressAction() 
{

}

void longPressAction() 
{
  rotationEnabled = !rotationEnabled;

  Serial.write("executed");
  display.drawRect(LEFT_MARGIN + segmentFrontEdges[currentSegment], 0, \
                   LEFT_MARGIN + segmentBackEdges[currentSegment], TOP_MARGIN - 2, true, INVERSE);
}