#include <Adafruit_NeoPixel.h>

#define R 0
#define G 1
#define B 2

#define STATE_POWER_OFF 0
#define STATE_IDLE 1
#define STATE_IDLE_LIGHTS_OFF 2
#define STATE_NEW_GAME 3
#define STATE_GAME 4
#define STATE_GOAL_A 5
#define STATE_GOAL_B 6
#define STATE_VICTORY_A 7
#define STATE_VICTORY_B 8

#define NUM_COLORS 9

unsigned long MAX_UNSIGNED_LONG = 4294967295;

// Uno R3 pinouts
int goalStripPinMatrix[2][3] = {
    {3,5,6},   //strip 0 RGB
    {9,10,11}  //strip 1 RGB
};
int pinPowerSwitch = 2;
int pinGoalSensorA = 7;
int pinGoalSensorB = 8;
int pinScoreStripA = 12;
int pinScoreStripB = 13;


// LED strips
Adafruit_NeoPixel scoreStripA = Adafruit_NeoPixel(10, pinScoreStripA, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel scoreStripB = Adafruit_NeoPixel(10, pinScoreStripB, NEO_GRB + NEO_KHZ800);
int numStripPixels = 10;
int goalStripA = 0;
int goalStripB = 1;

// Colors for analog RGB goal lights
int colorRed[3] = {255,0,0};
int colorOrange[3] = {255,30,0};
int colorYellow[3] = {255,60,0};
int colorLiteGreen[3] = {255,255,0};
int colorGreen[3] = {0,255,0};
int colorAqua[3] = {0,255,128};
int colorBlue[3] = {0,0,255};
int colorPurple[3] = {255,0,255};
int colorWhite[3] = {255,255,255};
int colorBlack[3] = {0,0,0};
int* colorList[9] = {colorRed, colorOrange, colorYellow, colorLiteGreen, colorGreen, colorAqua, colorBlue, colorPurple, colorWhite};

//Colors for Adafruit digital RGB strips
uint32_t scoreStripRed = scoreStripA.Color(colorRed[R], colorRed[G], colorRed[B]);
uint32_t scoreStripOrange = scoreStripA.Color(colorOrange[R], colorOrange[G], colorOrange[B]);
uint32_t scoreStripYellow = scoreStripA.Color(colorYellow[R], colorYellow[G], colorYellow[B]);
uint32_t scoreStripLiteGreen = scoreStripA.Color(colorLiteGreen[R], colorLiteGreen[G], colorLiteGreen[B]);
uint32_t scoreStripGreen = scoreStripA.Color(colorGreen[R], colorGreen[G], colorGreen[B]);
uint32_t scoreStripAqua = scoreStripA.Color(colorAqua[R], colorAqua[G], colorAqua[B]);
uint32_t scoreStripBlue = scoreStripA.Color(colorBlue[R], colorBlue[G], colorBlue[B]);
uint32_t scoreStripPurple = scoreStripA.Color(colorPurple[R], colorPurple[G], colorPurple[B]);
uint32_t scoreStripWhite = scoreStripA.Color(colorWhite[R], colorWhite[G], colorWhite[B]);
uint32_t scoreStripBlack = scoreStripA.Color(colorBlack[R], colorBlack[G], colorBlack[B]);


// Game-specific constants
int maxGoals = 5;
unsigned long lightsOutTimeoutSec = 300;
unsigned long gameResetTimeoutSec = 3600;
unsigned long msInSec = 1000;

// Mutable state variables
volatile int state = STATE_NEW_GAME;
int numGoalsA = 0;
int numGoalsB = 0;
unsigned long lastGoalTs = millis();


void setupPins() {
  for (int strip = 0; strip < 2; strip++) {
    for (int color = 0; color < 3; color++) {
      pinMode(goalStripPinMatrix[strip][color], OUTPUT);
    }
  }
  pinMode(pinGoalSensorA, INPUT);
  pinMode(pinGoalSensorB, INPUT);
}

void togglePowerState() {
  if (state != STATE_POWER_OFF) state = STATE_POWER_OFF;
  else state = STATE_NEW_GAME;
}

void drawGoalStrip(int stripIdx, int *color) {
  analogWrite(goalStripPinMatrix[stripIdx][R], color[R]);
  analogWrite(goalStripPinMatrix[stripIdx][G], color[G]);
  analogWrite(goalStripPinMatrix[stripIdx][B], color[B]);
}

void turnOffGoalLights() {
  drawGoalStrip(goalStripA, colorBlack);
  drawGoalStrip(goalStripB, colorBlack);
}

void checkGoalSensors() {
  if (digitalRead(pinGoalSensorA) == LOW) state = STATE_GOAL_A;
  if (digitalRead(pinGoalSensorB) == LOW) state = STATE_GOAL_B;
}

void drawScoreStrip(int numGoals, Adafruit_NeoPixel &scoreStrip, uint32_t color) {
  int numLedsPerGoal = 2;
  // reset whole strip
  for (int i=0; i<numStripPixels; i++) {
    scoreStrip.setPixelColor(i, scoreStripBlack);
  }
  
  // set score goals LEDs
  for (int i=0; i<numGoals*numLedsPerGoal; i++) {
      scoreStrip.setPixelColor(i, color);
  }
  scoreStrip.show();
}

void turnOffScoreStrips() {
    // reset whole strip
  for (int i=0; i<numStripPixels; i++) {
    scoreStripA.setPixelColor(i, scoreStripBlack);
    scoreStripB.setPixelColor(i, scoreStripBlack);
  }
  scoreStripA.show();
  scoreStripB.show();
}

// returns true if A is victorious
void flashGoalA(int flashDuration) {
  
  // make sure the opposite score strip is lit
  drawScoreStrip(numGoalsB, scoreStripB, scoreStripRed);
  
  // flash the goal and score strip lights
  int flashDelayMs = 25; // ms
  int elapsedMs = 0;
  int numCycles = 0;
  int *color = colorList[random(NUM_COLORS)];
  
  while (elapsedMs < flashDuration) {
    if (numCycles % 2 == 1) {
      drawGoalStrip(goalStripA, colorBlack);
      drawScoreStrip(numGoalsA-1, scoreStripA, scoreStripBlue);
    }
    else {
      drawGoalStrip(goalStripA, color);
      drawScoreStrip(numGoalsA, scoreStripA, scoreStripBlue);
    }
    delay(flashDelayMs);
    elapsedMs += flashDelayMs;
    numCycles++;
  }
  
  drawScoreStrip(numGoalsA, scoreStripA, scoreStripBlue);
}

// returns true if B is victorious
void flashGoalB(int flashDuration) {
  
  // make sure the opposite score strip is lit
  drawScoreStrip(numGoalsA, scoreStripA, scoreStripBlue);
  
  // flash the goal and score strip lights
  int flashDelayMs = 25; // ms
  int elapsedMs = 0;
  int numCycles = 0;
  int *color = colorList[random(NUM_COLORS)];
  
  while (elapsedMs < flashDuration) {
    if (numCycles % 2 == 1) {
      drawGoalStrip(goalStripB, colorBlack);
      drawScoreStrip(numGoalsB-1, scoreStripB, scoreStripRed);
    }
    else {
      drawGoalStrip(goalStripB, color);
      drawScoreStrip(numGoalsB, scoreStripB, scoreStripRed);
    }
    delay(flashDelayMs);
    elapsedMs += flashDelayMs;
    numCycles++;
  }
  
  drawScoreStrip(numGoalsB, scoreStripB, scoreStripRed);
}

void resetScore() {
  numGoalsA = 0;
  numGoalsB = 0;
}

void flashVictoryA() {
  int *color = colorList[random(NUM_COLORS)];
  drawGoalStrip(goalStripA, color);
  drawGoalStrip(goalStripB, color);
  drawScoreStrip(0, scoreStripB, scoreStripRed);
  rainbowCycle(2, scoreStripA);
  turnOffGoalLights();
}

void flashVictoryB() {
  int *color = colorList[random(NUM_COLORS)];
  drawGoalStrip(goalStripA, color);
  drawGoalStrip(goalStripB, color);
  drawScoreStrip(0, scoreStripA, scoreStripBlue);
  rainbowCycle(1, scoreStripB);
  turnOffGoalLights();
  
}

void setup() {
 setupPins;
// pinMode(pinPowerSwitch, INPUT_PULLUP);
// attachInterrupt(0, togglePowerState, CHANGE);

 Serial.begin(9600);
 scoreStripA.begin();
 scoreStripA.show();
 scoreStripB.begin();
 scoreStripB.show();
 state = STATE_NEW_GAME;
}

/**
Fancy schmancy Adafruit functions for rainbow wipe
*/
void rainbowCycle(uint8_t wait, Adafruit_NeoPixel &strip) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< numStripPixels; i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / numStripPixels) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return scoreStripB.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return scoreStripB.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return scoreStripB.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void checkTimeoutPeriod() {
  unsigned long timeNow = millis();
  unsigned long timeSinceLastGoal = 0;
  if (timeNow < lastGoalTs) timeSinceLastGoal = (MAX_UNSIGNED_LONG - lastGoalTs) + timeNow;
  else timeSinceLastGoal = timeNow - lastGoalTs;

  if (state == STATE_IDLE_LIGHTS_OFF && timeSinceLastGoal > gameResetTimeoutSec * msInSec) state = STATE_NEW_GAME;
  else if (state == STATE_IDLE && timeSinceLastGoal > lightsOutTimeoutSec * msInSec) {
    turnOffGoalLights();
    turnOffScoreStrips(); 
    state = STATE_IDLE_LIGHTS_OFF;
  }
}

void reportGoalToGameManager(char goalAorB) {
  char toSend[5];
  sprintf(toSend,"e:%c,%d,%d", goalAorB, numGoalsA, numGoalsB);
  Serial.println(toSend);
}

void reportNewGameToGameManager() {
  Serial.println("e:newgame");
}

void loop() {
  
  switch(state) {
    
    case STATE_POWER_OFF:
      break;
          
    case STATE_IDLE:
      checkGoalSensors();
      checkTimeoutPeriod();
      break;
    
    case STATE_IDLE_LIGHTS_OFF:
      checkGoalSensors();
      checkTimeoutPeriod();
      break;
    
    case STATE_GOAL_A:
      numGoalsA++;
      turnOffGoalLights();
      reportGoalToGameManager('A');
      lastGoalTs = millis();
      flashGoalA(2000);
      if (numGoalsA == maxGoals) state = STATE_VICTORY_A;
      else state = STATE_IDLE;
      break;
      
    case STATE_GOAL_B:
      numGoalsB++;
      turnOffGoalLights();
      reportGoalToGameManager('B');
      lastGoalTs = millis();
      flashGoalB(2000);
      if (numGoalsB == maxGoals) state = STATE_VICTORY_B;
      else state = STATE_IDLE;
      break;
      
    case STATE_VICTORY_A:
      flashVictoryA();
      state = STATE_NEW_GAME;
      break;
      
    case STATE_VICTORY_B:
      flashVictoryB();
      state = STATE_NEW_GAME;
      break;
    
    case STATE_NEW_GAME:
      turnOffGoalLights();
      turnOffScoreStrips();
      resetScore();
      lastGoalTs = millis();
      state = STATE_IDLE;
      reportNewGameToGameManager();
      break;
  }
}
