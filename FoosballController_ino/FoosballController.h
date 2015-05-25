#define R 0
#define G 1
#define B 2

#define STRIP0 0
#define STRIP1 1

#define STATE_INIT 0
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

int g_pin[2][3] = {
  {5,6,3}, //strip 0 RGB
  {10,11,9} //strip 1 RGB
};


