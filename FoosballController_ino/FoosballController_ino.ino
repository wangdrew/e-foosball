#include <Adafruit_NeoPixel.h>

#define R 0
#define G 1
#define B 2

#define STRIP0 0
#define STRIP1 1

#define STATE_IDLE 0
#define STATE_NEW_GAME 1
#define STATE_GAME 2
#define STATE_GOAL_A 3
#define STATE_GOAL_B 4
#define STATE_VICTORY_A 5
#define STATE_VICTORY_B 6

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

Adafruit_NeoPixel score_strip_a = Adafruit_NeoPixel(60, score_keep_a_pin, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel score_strip_b = Adafruit_NeoPixel(60, score_keep_b_pin, NEO_GRB + NEO_KHZ800);
int numStripPixels = 12;

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
//annoying to define colors per strip! 
uint32_t score_strip_a_red = score_strip_a.Color(RED[R], RED[G], RED[B]);
uint32_t score_strip_b_red = score_strip_b.Color(RED[R], RED[G], RED[B]);
uint32_t score_strip_a_orange = score_strip_a.Color(ORANGE[R], ORANGE[G], ORANGE[B]);
uint32_t score_strip_b_orange = score_strip_b.Color(ORANGE[R], ORANGE[G], ORANGE[B]);
uint32_t score_strip_a_yellow = score_strip_a.Color(YELLOW[R], YELLOW[G], YELLOW[B]);
uint32_t score_strip_b_yellow = score_strip_b.Color(YELLOW[R], YELLOW[G], YELLOW[B]);
uint32_t score_strip_a_lgreen = score_strip_a.Color(LGREEN[R], LGREEN[G], LGREEN[B]);
uint32_t score_strip_b_lgreen = score_strip_b.Color(LGREEN[R], LGREEN[G], LGREEN[B]);
uint32_t score_strip_a_green = score_strip_a.Color(GREEN[R], GREEN[G], GREEN[B]);
uint32_t score_strip_b_green = score_strip_b.Color(GREEN[R], GREEN[G], GREEN[B]);
uint32_t score_strip_a_aqua = score_strip_a.Color(AQUA[R], AQUA[G], AQUA[B]);
uint32_t score_strip_b_aqua = score_strip_b.Color(AQUA[R], AQUA[G], AQUA[B]);
uint32_t score_strip_a_blue = score_strip_a.Color(BLUE[R], BLUE[G], BLUE[B]);
uint32_t score_strip_b_blue = score_strip_b.Color(BLUE[R], BLUE[G], BLUE[B]);
uint32_t score_strip_a_purple = score_strip_a.Color(PURPLE[R], PURPLE[G], PURPLE[B]);
uint32_t score_strip_b_purple = score_strip_b.Color(PURPLE[R], PURPLE[G], PURPLE[B]);
uint32_t score_strip_a_white = score_strip_a.Color(WHITE[R], WHITE[G], WHITE[B]);
uint32_t score_strip_b_white = score_strip_b.Color(WHITE[R], WHITE[G], WHITE[B]);
uint32_t score_strip_a_black = score_strip_a.Color(BLACK[R], BLACK[G], BLACK[B]);
uint32_t score_strip_b_black = score_strip_b.Color(BLACK[R], BLACK[G], BLACK[B]);

uint32_t score_strip_a_colors[8] = {score_strip_a_red, score_strip_a_orange, score_strip_a_yellow, \
score_strip_a_lgreen, score_strip_a_green, score_strip_a_aqua, score_strip_a_blue, score_strip_a_purple};

uint32_t score_strip_b_colors[8] = {score_strip_b_red, score_strip_b_orange, score_strip_b_yellow, \
score_strip_b_lgreen, score_strip_b_green, score_strip_b_aqua, score_strip_b_blue, score_strip_b_purple};


// Mutable state variables
int state = STATE_IDLE;
int numGoalsA = 0;
int numGoalsB = 0;
int numScoreStripLedsOnA = 0;
int numScoreStripLedsOnB = 0;
int maxGoals = 5;
uint32_t score_strip_a_color = score_strip_a_colors[random(8)];
uint32_t score_strip_b_color = score_strip_b_colors[random(8)];

void setup_pins() {
  for (int strip = 0; strip < 2; strip++) {
    for (int color = 0; color < 3; color++) {
      pinMode(g_pin[strip][color], OUTPUT);
    }
  }
  pinMode(goal_a_sensor, INPUT);
  pinMode(goal_b_sensor, INPUT);
}

void output(int strip_id, int *color) {
  analogWrite(g_pin[strip_id][R], color[R]);
  analogWrite(g_pin[strip_id][G], color[G]);
  analogWrite(g_pin[strip_id][B], color[B]);
}

void turn_off_lights() {
  output(STRIP0, BLACK);
  output(STRIP1, BLACK);
}

void reset_score_strips() {
  numScoreStripLedsOnA = 0;
  numScoreStripLedsOnB = 0;
  score_strip_a_color = score_strip_a_colors[random(8)];
  score_strip_b_color = score_strip_b_colors[random(8)];
  turn_off_score_strips();
}

void turn_off_score_strips() {
  for (int i=0; i<numStripPixels; i++) {
    score_strip_a.setPixelColor(i, score_strip_a_black);
    score_strip_b.setPixelColor(i, score_strip_b_black);
  }
}

void cycle_random_color(int strip_id, int duration) {
  int micro_delay = 25; // ms
  int elapsed = 0;
  int loop_counter = 0;
  int *color = COLORS[random(NUM_COLORS)];
  
  while (elapsed < duration) {

    if (loop_counter % 2 == 1) {
      output(strip_id, BLACK);
    }
    else {
      output(strip_id, color);
    }
    
    delay(micro_delay);
    elapsed += micro_delay;
    loop_counter++;
  }
}

void check_sensors() {
  if (digitalRead(goal_a_sensor) == LOW) {
    state = STATE_GOAL_A;
  }
  
  if (digitalRead(goal_b_sensor) == LOW) {
    state = STATE_GOAL_B;
  }
}

// returns true if A is victorious
boolean incrementScoreA() {
  numGoalsA++;
  numScoreStripLedsOnA = numGoalsA*2;
  for (int i=0; i<numScoreStripLedsOnA; i++) {
    score_strip_a.setPixelColor(i, score_strip_a_color);
  }
  if (numGoalsA == maxGoals) return true;
  else return false;
}

// returns true if B is victorious
boolean incrementScoreB() {
  numGoalsB++;
  numScoreStripLedsOnB = numGoalsB*2;
  for (int i=0; i<numScoreStripLedsOnB; i++) {
    score_strip_b.setPixelColor(i, score_strip_b_color);
  }
  if (numGoalsB == maxGoals) return true;
  else return false;
}

void reset_score() {
  numGoalsA = 0;
  numGoalsB = 0;
}

void flashVictoryA() {
  int flashDelay = 500;
  // flash winning side score counter 5 times
  for (int i=0; i<5; i++) {
    for (int pixel=0; pixel<numScoreStripLedsOnA; pixel++) {
      score_strip_a.setPixelColor(pixel, score_strip_a_color);
    }
    delay(flashDelay);
    for (int pixel=0; pixel<numStripPixels; pixel++) {
      score_strip_a.setPixelColor(pixel, score_strip_a_black);
    }
    delay(flashDelay);
  }
}

void flashVictoryB() {
  int flashDelay = 500;
  // flash winning side score counter 5 times
  for (int i=0; i<5; i++) {
    for (int pixel=0; pixel<numScoreStripLedsOnA; pixel++) {
      score_strip_b.setPixelColor(pixel, score_strip_b_color);
    }
    delay(flashDelay);
    for (int pixel=0; pixel<numStripPixels; pixel++) {
      score_strip_b.setPixelColor(pixel, score_strip_b_black);
    }
    delay(flashDelay);
  }
}

void setup() {
 setup_pins;
 Serial.begin(9600);
 score_strip_a.begin();
 score_strip_a.show();
 score_strip_b.begin();
 score_strip_b.show();
}

void loop() {
  
  switch(state) {
    
    case STATE_IDLE:
      check_sensors();
      break;
        
    case STATE_GOAL_A:
      turn_off_lights();
      Serial.println("A");
      cycle_random_color(STRIP0, 2000); // ms
      turn_off_lights();
      if (incrementScoreA() == true) state = STATE_VICTORY_A;
      else state = STATE_IDLE;
      break;
      
    case STATE_GOAL_B:
      turn_off_lights();
      Serial.println("B");
      cycle_random_color(STRIP1, 2000); // ms
      turn_off_lights();
      if (incrementScoreB() == true) state = STATE_VICTORY_B;
      else state = STATE_IDLE;
      break;
      
    case STATE_VICTORY_A:
      flashVictoryA();
      state = STATE_IDLE;
      break;
      
    case STATE_VICTORY_B:
      flashVictoryB();
      state = STATE_IDLE;
      break;
    
    case STATE_NEW_GAME:
      turn_off_lights();
      reset_score_strips();
      state = STATE_IDLE;
      reset_score();

      break;
  }
}
