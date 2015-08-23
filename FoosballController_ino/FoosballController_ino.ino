#define R 0
#define G 1
#define B 2

#define STRIP0 0
#define STRIP1 1

#define STATE_IDLE 0
#define STATE_CALIBRATION 1
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

int accel_x_input = A0;
int accel_y_input = A1;
int accel_z_input = A2;

int state = STATE_CALIBRATION;
int accelerometer_baseline[3] = {0,0,0};
int smallDisturbance = 10;
int mediumDisturbance = 20;
int largeDisturbance = 30;
unsigned long lastAccelEvent = millis();
unsigned long debouncePeriod = 200;  // ms
char accelMessageOut = '-';

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

void turn_on_strip(int *color) {
  output(STRIP0, color);
  output(STRIP1, color);
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

void accelerometer(int* output) {
  output[0] = analogRead(accel_x_input);
  output[1] = analogRead(accel_y_input);
  output[2] = analogRead(accel_z_input);
}

void calibrate() {
  int numSamples = 10;
  
  int xVals[numSamples];
  int yVals[numSamples];
  int zVals[numSamples];
  memset(xVals,0, sizeof(xVals));
  memset(yVals,0, sizeof(yVals));
  memset(zVals,0, sizeof(zVals));
  
  int baseLine[3] = {0,0,0};
    
  for (int i = 0; i < numSamples; i++) {
    int sample[3] = {0,0,0};
    accelerometer(sample);
    xVals[i] = sample[0];
    yVals[i] = sample[1];
    zVals[i] = sample[2];
    delay(500);
  }
  
  for (int i = 0; i < numSamples; i++) {
    baseLine[0] = baseLine[0] += xVals[i];
    baseLine[1] = baseLine[1] += yVals[i];
    baseLine[2] = baseLine[2] += zVals[i];
  }
  
  baseLine[0] = baseLine[0] / numSamples;
  baseLine[1] = baseLine[1] / numSamples;
  baseLine[2] = baseLine[2] / numSamples;

  accelerometer_baseline[0] = baseLine[0];
  accelerometer_baseline[1] = baseLine[1];
  accelerometer_baseline[2] = baseLine[2];
}

void check_sensors() {
  if (digitalRead(goal_a_sensor) == LOW) {
    state = STATE_GOAL_A;
  }
  
  if (digitalRead(goal_b_sensor) == LOW) {
    state = STATE_GOAL_B;
  }
}

void check_accelerometer() {
  int sample[3] = {0,0,0};
  accelerometer(sample);
  int xDeviance = abs(sample[0] - accelerometer_baseline[0]);
  int yDeviance = abs(sample[1] - accelerometer_baseline[1]);
  int zDeviance = abs(sample[2] - accelerometer_baseline[2]);
  int totalDeviance = (xDeviance + yDeviance + zDeviance);
  char message = '-';
  
  //Debug
  if (totalDeviance > smallDisturbance){
    Serial.println(totalDeviance);
  }
  
  // Debouncing logic
  if (totalDeviance > smallDisturbance && totalDeviance < mediumDisturbance) {    // Small event
    if (accelMessageOut != 'M' || accelMessageOut != 'L') {    // Don't overwrite a large/medium event with a small event right after
      Serial.println("Set S");
      accelMessageOut = 'S';
      lastAccelEvent = millis();
    }
  } else if (totalDeviance > mediumDisturbance && totalDeviance < largeDisturbance) {    // Medium event
    Serial.println("In Medium");
    if (accelMessageOut != 'L') {    // Don't overwrite a large event with a medium event right after
      Serial.println("Set M");
      lastAccelEvent = millis();
      accelMessageOut = 'M';
    }
  } else if (totalDeviance > largeDisturbance){
    Serial.println("Set L");
    lastAccelEvent = millis();
    accelMessageOut = 'L';
  } else {
    // Send the message out after a certain period
    if (accelMessageOut != '-' && millis() > lastAccelEvent + debouncePeriod) {
      Serial.println(accelMessageOut);
      accelMessageOut = '-';
    }
  }
}

void setup() {
 setup_pins;
 Serial.begin(9600);
}

void loop() {
  
  switch(state) {
    
    case STATE_CALIBRATION:
      turn_on_strip(RED);
      calibrate();
      turn_off_lights();
      turn_on_strip(GREEN);
      delay(1000);
      turn_off_lights();
      state = STATE_IDLE;
      break;
    
    case STATE_IDLE:
      check_sensors();
      check_accelerometer();
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
