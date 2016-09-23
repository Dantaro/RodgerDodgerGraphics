#ifndef Constants_h
#define Constants_h

// Width of each char including inter-character space
#define CHAR_WIDTH 6
// Height of each char
#define CHAR_HEIGHT 8
//Define the Center of the screen to make it easy to print things there
#define CENTER_X (WIDTH / 2)
#define CENTER_Y (HEIGHT / 2)
//Screen Size variables;
#define X_MAX (WIDTH - (CHAR_WIDTH) + 1)
#define Y_MAX (HEIGHT - CHAR_HEIGHT)
char playText[] = "Press A to start!";
char finalScoreText[] = "Your final score:";

#define arr_length(x) (sizeof(x) / sizeof(*x))

//Scoreing
#define VICTORY_SCORE 250

#endif
