#include "Arduboy.h"
#include "Art.h"
#include "Constants.h"
#include "GameLib.h"
#include "EEPROM.h"

//CONSTANTS
// Make an instance of arduboy used for many functions
static Arduboy arduboy;

//Hero-related variables
static BaseHero *hero;

//Enemy-related variables
static BaseEnemy *enemies[15]; //Max of 15  enemies on screen
static short activeEnemyNumber = arr_length(enemies); //Default to max size

//Scoring
static int score;
static int unsigned frameCount;
static int lastScoreMilestone;

//EEPROM info
static int eepromAddress = EEPROM_STORAGE_SPACE_START;

//Game State
#define GAME_STATE_SPLASH_SCREEN        0
#define GAME_STATE_INSTRUCTION_SCREEN   1
#define GAME_STATE_GAME_SELECTION_MENU  2
//#define GAME_STATE_OPTIONS              3
//#define GAME_STATE_RESERVED             4
//#define GAME_STATE_RESERVED_2           5
#define GAME_STATE_ACTIVE               6
#define GAME_STATE_DEATH                7
#define GAME_STATE_VICTORY              8

static short gameState = GAME_STATE_SPLASH_SCREEN;

#define GAME_MODE_CLASSIC   0
#define GAME_MODE_INFINITE  1
static char gameMode = GAME_MODE_INFINITE;

void setup() {
  //initiate arduboy instance
  arduboy.begin();

  arduboy.setFrameRate(60);

  //Set up a random seed
  randomSeed(analogRead(0));
  
  //Instantiate new hero
  hero = new BaseHero();
  hero->setSpriteArray(heroSprite);

  //Instantiate enemies
  //enemySprite
  for (int i = 0 ; i < arr_length(enemies) ; i++) {
    BaseEnemy *enemy = new BaseEnemy();
    enemy->setSpriteArray(enemySprite);
    enemies[i] = enemy;
  }

  //Set initial game state
  setInitialGameState();
  
}

void loop() {
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;
    
  arduboy.clear();

  //Game Loop
  if (gameState == GAME_STATE_SPLASH_SCREEN) {
    splashScreen();
  } else if (gameState == GAME_STATE_INSTRUCTION_SCREEN) {
      menuScreen();
  } else if (gameState == GAME_STATE_GAME_SELECTION_MENU) {
    gameModeMenu();
  } else {
    if (gameState == GAME_STATE_ACTIVE) {
      int enemiesLength = arr_length(enemies);
      operateHero();
      operateEnemies();
      if (gameMode == GAME_MODE_INFINITE) {
        calculateActiveEnemyNumber();
      }
      calculateCollisions();
      calculateAndDrawScore();
      if (gameMode == GAME_MODE_CLASSIC) {
        checkForVictory();
      }
    } else if (gameState == GAME_STATE_VICTORY) {
      victoryState();
    } else if (gameState == GAME_STATE_DEATH) {
      deathState();
    }
  }

  //Display to screen
  arduboy.display();
}



//Action methods

static void setInitialGameState() {

  //Set up Hero
  hero->setX(CENTER_X - (CHAR_WIDTH / 2));
  hero->setY(Y_MAX);
  hero->setWidth(8);
  hero->setHeight(8);
  hero->setVisible(true);

  //Set up enemies
  for (int i = 0 ; i < arr_length(enemies) ; i++) {
    BaseEnemy *enemy = enemies[i];
    int x = random(X_MAX);
    int y = 0;
    enemy->setX(x);
    enemy->setY(y);
    enemy->setWidth(8);
    enemy->setHeight(8);
    enemy->setVisible(false);
    enemies[i] = enemy;
  }

  //Set score, frame count, and other constants
  score = 0;
  frameCount = 0;
  lastScoreMilestone = 0;

  if (gameMode == GAME_MODE_INFINITE) {
    activeEnemyNumber = 1;
  } else {
    activeEnemyNumber = arr_length(enemies);
  }
}

static void operateHero() {
  //Hero
  int x = hero->getX();
  if(arduboy.pressed(RIGHT_BUTTON) && (x < X_MAX)) {
    x = x + 1;
    hero->setX(x);
  }
  if(arduboy.pressed(LEFT_BUTTON) && (x > 0)) {
    x = x - 1;
    hero->setX(x);
  }
  hero->drawSprite(arduboy);
}


static void operateEnemies() {
  //Move Enemies
  bool hasMadeVisibleThisCycle = false;
  for (int i = 0 ; i < activeEnemyNumber ; i++) {
    BaseEnemy *enemy = enemies[i];

    if (enemy->isVisible()) {
      int16_t y = enemy->getY();
      y++;
      int16_t x = random(X_MAX);
      
      if (y >= Y_MAX) {
        enemy->setY(-1); //Set just before the top of the screen
        enemy->setX(x); //Set a random X coordinate
        enemy->setVisible(false);
      } else {
        enemy->setY(y);
        //Draw Enemy
        enemy->drawSprite(arduboy);
      }
    }

    
    if (random(50) == 1 && !hasMadeVisibleThisCycle && !enemy->isVisible()) {
      hasMadeVisibleThisCycle = true;
      enemy->setVisible(true);
    }
    
    //Return to array
    enemies[i] = enemy;
  }
}

static void calculateCollisions() {
  //Calculate collision
  for (int i = 0 ; i < activeEnemyNumber ; i++) {
    BaseEnemy *enemy = enemies[i];
    if (GameLib::detectCollision(enemy, hero)) {
      gameState = GAME_STATE_DEATH;
    }
  }
}

static void calculateActiveEnemyNumber() {
  if (lastScoreMilestone + 50 == score && activeEnemyNumber < arr_length(enemies)) {
    lastScoreMilestone = score;
    activeEnemyNumber++;
  }
}

static void calculateAndDrawScore() {
  //Calculate and Draw Score
  frameCount++;
  if (frameCount % 10 == 0) {
    score++;
  }
  arduboy.setCursor(0,0);
  arduboy.print(score);
}

static void checkForVictory() {
  if (score >= VICTORY_SCORE) {
    gameState = GAME_STATE_VICTORY;
  }
}

static void victoryState() {
  char winText[] = "You win!";
  arduboy.setCursor((CENTER_X - (sizeof(winText) * CHAR_WIDTH / 2)), CENTER_Y);
  arduboy.print(winText);
  arduboy.setCursor(0, Y_MAX);
  arduboy.print(playText);
  arduboy.setCursor((CENTER_X - (sizeof(finalScoreText) * CHAR_WIDTH / 2)), CENTER_Y - (CHAR_HEIGHT * 2) - 6);
  arduboy.print(finalScoreText);
  arduboy.setCursor((CENTER_X - (CHAR_WIDTH * 2)), CENTER_Y - (CHAR_HEIGHT) - 3);
  arduboy.print(score);
  if (arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON)) {
    gameState = GAME_STATE_GAME_SELECTION_MENU;
    setInitialGameState();
    delay(250);
  }
}

static void deathState() {
  //Save Score to EEPROM
  if (gameMode == GAME_MODE_INFINITE && score > getHighScore()) {
    saveHighScore(score);
  }
  char loseText[] = "You lose!";
  arduboy.setCursor((CENTER_X - (sizeof(loseText) * CHAR_WIDTH / 2)), CENTER_Y);
  arduboy.print(loseText);
  arduboy.setCursor(0, Y_MAX);
  arduboy.print(playText);
  arduboy.setCursor((CENTER_X - (sizeof(finalScoreText) * CHAR_WIDTH / 2)), CENTER_Y - (CHAR_HEIGHT * 2) - 6);
  arduboy.print(finalScoreText);
  arduboy.setCursor((CENTER_X - (CHAR_WIDTH * 2)), CENTER_Y - (CHAR_HEIGHT) - 3);
  arduboy.print(score);
  if (arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON)) {
     gameState = GAME_STATE_GAME_SELECTION_MENU;
    setInitialGameState();
    delay(250);
  }
}

static void splashScreen() {
  arduboy.drawBitmap(0, 0, logo, 128, 40, WHITE);
  arduboy.setCursor(0, Y_MAX);
  arduboy.print("Press any button!");
  if (arduboy.pressed(A_BUTTON) 
  || arduboy.pressed(B_BUTTON) 
  || arduboy.pressed(UP_BUTTON)
  || arduboy.pressed(DOWN_BUTTON)
  || arduboy.pressed(RIGHT_BUTTON)
  || arduboy.pressed(LEFT_BUTTON)){
    gameState = GAME_STATE_INSTRUCTION_SCREEN;
    delay(250);
  }
}

static void menuScreen() {
  int textY = 10;
  arduboy.setCursor(0,textY);
  arduboy.print("Avoid the bullets!");
  textY = textY + CHAR_HEIGHT;
  arduboy.setCursor(0, textY);
  arduboy.print("Reach 250 points!");
  textY = textY + CHAR_HEIGHT;
  arduboy.setCursor(0, textY);
  arduboy.print("Become the one true");
  textY = textY + CHAR_HEIGHT + 3;
  arduboy.setCursor(20, textY);
  arduboy.print("RODGER DODGER");
  arduboy.setCursor(0, Y_MAX);
  arduboy.print(playText);
  if (arduboy.pressed(A_BUTTON)) {
    gameState = GAME_STATE_GAME_SELECTION_MENU;
    delay(250);
  }
}

static void gameModeMenu() {
  char pointer = 175;
  char pointerLocation = 0;
  while(true) {
    arduboy.clear();

    int textY = 10;
    arduboy.setCursor(0, textY);
    arduboy.print("Choose mode");
    textY = textY + 12;
    if (pointerLocation == 0) {
      arduboy.setCursor(0, textY);
      arduboy.print(pointer);
    }
    arduboy.setCursor(6, textY);
    arduboy.print("Classic RodgerDodger");
    textY = textY + CHAR_HEIGHT + 3;
    if (pointerLocation == 1) {
      arduboy.setCursor(0, textY);
      arduboy.print(pointer);
    }
    arduboy.setCursor(6, textY);
    arduboy.print("Endless RodgerDodger");

    textY = textY + CHAR_HEIGHT + 3;
    arduboy.setCursor(6, textY);
    arduboy.print("Endless High Score: ");
    textY = textY + CHAR_HEIGHT + 3;
    arduboy.setCursor(40, textY);
    arduboy.print(getHighScore());
    
    
    if (arduboy.pressed(A_BUTTON)) {
      if (pointerLocation == 0) {
        gameMode = GAME_MODE_CLASSIC;
      } else if (pointerLocation == 1) {
        gameMode = GAME_MODE_INFINITE;
      }
      gameState = GAME_STATE_ACTIVE;
      setInitialGameState();
      break;
    } else if (arduboy.pressed(DOWN_BUTTON) || arduboy.pressed(UP_BUTTON)) {
      if (pointerLocation == 0) {
        pointerLocation = 1;
      } else if (pointerLocation == 1) {
        pointerLocation = 0;
      }
      delay(150);
    }
     arduboy.display();
  }

  pointer = NULL;
  pointerLocation = NULL;
  
}

int getHighScore() {
  return EEPROMReadLong(eepromAddress);
}

void saveHighScore(int score) {
  EEPROMWriteLong(eepromAddress, score);
}

void EEPROMWriteLong(int address, long value)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadLong(long address)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}
