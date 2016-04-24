#include <Adafruit_NeoPixel.h>

#define R 0
#define G 1
#define B 2

#define STATE_IDLE 0
#define STATE_NEW_GAME 1
#define STATE_GAME 2
#define STATE_GOAL_A 3
#define STATE_GOAL_B 4
#define STATE_VICTORY_A 5
#define STATE_VICTORY_B 6
#define STATE_IDLE_LIGHTS_OFF 7

#define NUM_COLORS 9

// Uno R3 pinouts
int g_pin[2][3] = {
    {3,5,6},   //strip 0 RGB
    {9,10,11}  //strip 1 RGB
};
int goal_a_sensor = 7;
int goal_b_sensor = 8;
int score_keep_a_pin = 12;
int score_keep_b_pin = 13;

// LED strips
Adafruit_NeoPixel scoreStripA = Adafruit_NeoPixel(10, score_keep_a_pin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel scoreStripB = Adafruit_NeoPixel(10, score_keep_b_pin, NEO_GRB + NEO_KHZ800);
int numStripPixels = 10;
int goalStripA = 0;
int goalStripB = 1;

// Colors for analog RGB goal lights
int RED[3] = {255,0,0};
int ORANGE[3] = {255,30,0};
int YELLOW[3] = {255,60,0};
int LGREEN[3] = {255,255,0};
int GREEN[3] = {0,255,0};
int AQUA[3] = {0,255,128};
int BLUE[3] = {0,0,255};
int PURPLE[3] = {255,0,255};
int WHITE[3] = {255,255,255};
int BLACK[3] = {0,0,0};
int* COLORS[9] = {RED, ORANGE, YELLOW, LGREEN, GREEN, AQUA, BLUE, PURPLE, WHITE};

//Colors for Adafruit digital RGB strips
uint32_t score_strip_red = scoreStripA.Color(RED[R], RED[G], RED[B]);
uint32_t score_strip_orange = scoreStripA.Color(ORANGE[R], ORANGE[G], ORANGE[B]);
uint32_t score_strip_yellow = scoreStripA.Color(YELLOW[R], YELLOW[G], YELLOW[B]);
uint32_t score_strip_lgreen = scoreStripA.Color(LGREEN[R], LGREEN[G], LGREEN[B]);
uint32_t score_strip_green = scoreStripA.Color(GREEN[R], GREEN[G], GREEN[B]);
uint32_t scoreStripAqua = scoreStripA.Color(AQUA[R], AQUA[G], AQUA[B]);
uint32_t scoreStripBlue = scoreStripA.Color(BLUE[R], BLUE[G], BLUE[B]);
uint32_t score_strip_purple = scoreStripA.Color(PURPLE[R], PURPLE[G], PURPLE[B]);
uint32_t score_strip_white = scoreStripA.Color(WHITE[R], WHITE[G], WHITE[B]);
uint32_t scoreStripBlack = scoreStripA.Color(BLACK[R], BLACK[G], BLACK[B]);


// Game-specific constants
int maxGoals = 5;
unsigned long lightsOutTimeout = 300*1000; // in ms
unsigned long gameResetTimeout = 3600*1000; // in ms

// Mutable state variables
int state = STATE_NEW_GAME;
int numGoalsA = 0;
int numGoalsB = 0;
unsigned long lastActivityMs = millis();

void setup_pins() {
  for (int strip = 0; strip < 2; strip++) {
    for (int color = 0; color < 3; color++) {
      pinMode(g_pin[strip][color], OUTPUT);
    }
  }
  pinMode(goal_a_sensor, INPUT);
  pinMode(goal_b_sensor, INPUT);
}

void drawGoalStrip(int strip_id, int *color) {
  analogWrite(g_pin[strip_id][R], color[R]);
  analogWrite(g_pin[strip_id][G], color[G]);
  analogWrite(g_pin[strip_id][B], color[B]);
}

void turnOffGoalLights() {
  drawGoalStrip(goalStripA, BLACK);
  drawGoalStrip(goalStripB, BLACK);
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

void check_sensors() {
  if (digitalRead(goal_a_sensor) == LOW) state = STATE_GOAL_A;
  if (digitalRead(goal_b_sensor) == LOW) state = STATE_GOAL_B;
}

void drawScoreStrip(int numGoals, Adafruit_NeoPixel &score_strip) {
  int numLedsPerGoal = 2;
  // reset whole strip
  for (int i=0; i<numStripPixels; i++) {
    score_strip.setPixelColor(i, scoreStripBlack);
  }
  
  // set score goals LEDs
  for (int i=0; i<numGoals*numLedsPerGoal; i++) {
      score_strip.setPixelColor(i, scoreStripBlue);
  }
  
  score_strip.show();
}

// returns true if A is victorious
boolean incrementScoreA(int flashDuration) {
  numGoalsA++;
  
  // make sure the opposite score strip is lit
  drawScoreStrip(numGoalsB, scoreStripB);
  
  // flash the goal and score strip lights
  int micro_delay = 25; // ms
  int elapsed = 0;
  int loop_counter = 0;
  int *color = COLORS[random(NUM_COLORS)];
  
  while (elapsed < flashDuration) {
    if (loop_counter % 2 == 1) {
      drawGoalStrip(goalStripA, BLACK);
      drawScoreStrip(numGoalsA-1, scoreStripA);
    }
    else {
      drawGoalStrip(goalStripA, color);
      drawScoreStrip(numGoalsA, scoreStripA);
    }
    delay(micro_delay);
    elapsed += micro_delay;
    loop_counter++;
  }
  
  drawScoreStrip(numGoalsA, scoreStripA);
  if (numGoalsA == maxGoals) return true;
  else return false;
}

// returns true if B is victorious
boolean incrementScoreB(int flashDuration) {
  numGoalsB++;
  
  // make sure the opposite score strip is lit
  drawScoreStrip(numGoalsA, scoreStripA);
  
  // flash the goal and score strip lights
  int micro_delay = 25; // ms
  int elapsed = 0;
  int loop_counter = 0;
  int *color = COLORS[random(NUM_COLORS)];
  
  while (elapsed < flashDuration) {
    if (loop_counter % 2 == 1) {
      drawGoalStrip(goalStripB, BLACK);
      drawScoreStrip(numGoalsB-1, scoreStripB);
    }
    else {
      drawGoalStrip(goalStripB, color);
      drawScoreStrip(numGoalsB, scoreStripB);
    }
    delay(micro_delay);
    elapsed += micro_delay;
    loop_counter++;
  }
  
  drawScoreStrip(numGoalsB, scoreStripB);
  if (numGoalsB == maxGoals) return true;
  else return false;
}

void reset_score() {
  numGoalsA = 0;
  numGoalsB = 0;
}

void flashVictoryA() {
  int *color = COLORS[random(NUM_COLORS)];
  drawGoalStrip(goalStripA, color);
  drawGoalStrip(goalStripB, color);
  drawScoreStrip(0, scoreStripB);
  rainbowCycle(2, scoreStripA);
  turnOffGoalLights();
}

void flashVictoryB() {
  int *color = COLORS[random(NUM_COLORS)];
  drawGoalStrip(goalStripA, color);
  drawGoalStrip(goalStripB, color);
  drawScoreStrip(0, scoreStripA);
  rainbowCycle(1, scoreStripB);
  turnOffGoalLights();
  
}

void setup() {
 setup_pins;
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

void check_timeout() {
  unsigned long timeNow = millis();
  unsigned long timeSinceLastGoal = 0;
  if (timeNow < lastActivityMs) timeSinceLastGoal = (4294967295 - lastActivityMs) + timeNow;
  else timeSinceLastGoal = timeNow - lastActivityMs;
  if (timeSinceLastGoal > gameResetTimeout) state = STATE_NEW_GAME;
  else if (state == STATE_IDLE && timeSinceLastGoal > lightsOutTimeout) {
    turnOffGoalLights();
    turnOffScoreStrips(); 
    state = STATE_IDLE_LIGHTS_OFF;
  }
}



void loop() {
  
  switch(state) {
    
    case STATE_IDLE:
      check_sensors();
      check_timeout();
      break;
    
    case STATE_IDLE_LIGHTS_OFF:
      check_sensors();
      check_timeout();
      break;
    
    case STATE_GOAL_A:
      turnOffGoalLights();
      Serial.println("A");
      lastActivityMs = millis();
      if (incrementScoreA(2000) == true) state = STATE_VICTORY_A;
      else state = STATE_IDLE;
      break;
      
    case STATE_GOAL_B:
      turnOffGoalLights();
      Serial.println("B");
      lastActivityMs = millis();
      if (incrementScoreB(2000) == true) state = STATE_VICTORY_B;
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
      state = STATE_IDLE;
      reset_score();
      lastActivityMs = millis();
      break;
  }
}
