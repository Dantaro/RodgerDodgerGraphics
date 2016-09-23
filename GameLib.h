#ifndef GameLib_h
#define GameLib_h

#include "Arduboy.h"

//Defined classes
class Sprite {
  private:
    int16_t x_coor;
    int16_t y_coor;
    int width;
    int height;
    const uint8_t *spriteArray;
    bool visible = true;
  public:
    int16_t getX();
    void setX(int16_t x);
    int16_t getY();
    void setY(int16_t y);
    int getWidth();
    void setWidth(int inWidth);
    int getHeight();
    void setHeight(int inHeight);
    void setVisible(bool inIsVisible);
    bool isVisible();
    void setSpriteArray(const uint8_t *inSpriteArray);
    void drawSprite(Arduboy& arduboy);
};

//class BaseEnemy
class BaseEnemy: public Sprite {
  
};


class BaseHero: public Sprite  {
  //Extends Sprite
  private:
    int life;
  public:
    int getLife();
    void setLife(int life);
};

class BulletHellHero: public Sprite {
  //Extends BaseHero
  private:
    short weaponLevel;
  public:
    short getWeaponLevel();
    void setWeaponLevel(short level);

};

class GameLib {
  public:
    static boolean detectCollision(Sprite* actor1, Sprite* actor2);
};

#endif
