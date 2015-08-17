#define R 0
#define G 1
#define B 2

#define STRIP0 0
#define STRIP1 1

#define STATE_IDLE 0
#define STATE_BEGIN_GAME 1
#define STATE_GAME 2
#define STATE_GOAL_A 3
#define STATE_GOAL_B 4
#define STATE_BALL_RETURN 5

#define NUM_COLORS 9

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

// Uno R3 pinouts
int g_pin[2][3] = {
    {3,5,6},   //strip 0 RGB
    {9,10,11}  //strip 1 RGB
};
int goal_a_sensor = 7;
int goal_b_sensor = 8;

// Mini Pro pinouts 
//int g_pin[2][3] = {
//  {5,6,3}, //strip 0 RGB
//  {10,11,9} //strip 1 RGB
//};
//int goal_a_sensor = 12;
//int goal_b_sensor = 8;

int state = STATE_IDLE;

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

void setup() {
 setup_pins;
 Serial.begin(9600);
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
      state = STATE_IDLE;
      break;
      
    case STATE_GOAL_B:
      turn_off_lights();
      Serial.println("B");
      cycle_random_color(STRIP1, 2000); // ms
      turn_off_lights();
      state = STATE_IDLE;
      break;
  }
}
