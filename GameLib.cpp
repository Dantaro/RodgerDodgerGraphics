#include "Arduboy.h"
#include "GameLib.h"

//Sprite methods
int16_t Sprite::getX() { return x_coor;}
void Sprite::setX(int16_t x) {x_coor = x;}
int16_t Sprite::getY() { return y_coor;}
void Sprite::setY(int16_t y) {y_coor = y;}
int Sprite::getWidth() {return width;}
void Sprite::setWidth(int inWidth) {width = inWidth;}
int Sprite::getHeight() {return height;}
void Sprite::setHeight(int inHeight) {height = inHeight;}
void Sprite::setSpriteArray(const uint8_t *inSpriteArray) {spriteArray = inSpriteArray;}
void Sprite::setVisible(bool inIsVisible) { visible = inIsVisible; }
bool Sprite::isVisible() { return visible; }
void Sprite::drawSprite(Arduboy& arduboy){arduboy.drawBitmap(x_coor, y_coor, spriteArray, width, height, WHITE);}

//Baseactor1 methods
int BaseHero::getLife(){ return life; }
void BaseHero::setLife(int inLife){ life = inLife; }

//BulletHellactor1 methods
short BulletHellHero::getWeaponLevel(){ return weaponLevel; }
void BulletHellHero::setWeaponLevel(short level){ weaponLevel = level; }


//GameLib
boolean GameLib::detectCollision (Sprite* actor1, Sprite* actor2) {
  //Left Side
  bool hitXLeftMin = false;
  bool hitXLeftMax = false;
  if (actor2->getX() <= actor1->getX()) {
    hitXLeftMin = true;
  }
  if (actor2->getX() + actor2->getWidth() >= actor1->getX()) {
    hitXLeftMax = true;
  }
  
  //Right Side
  bool hitXRightMin = false;
  bool hitXRightMax = false;
  if (actor2->getX() <= actor1->getX() + actor1->getWidth()) {
    hitXRightMin = true;
  }
  if (actor2->getX() + actor2->getWidth() >= actor1->getX() + actor1->getWidth()) {
    hitXRightMax = true;
  }
  
  //Top Side
  bool hitYTopMin = false;
  bool hitYTopMax = false;
  if (actor2->getY() <= actor1->getY()) {
    hitYTopMin = true;
  }
  if (actor2->getY() + actor2->getHeight() >= actor1->getY()) {
    hitYTopMax = true;
  }
  
  //Bottom Side
  bool hitYBottomMin = false;
  bool hitYBottomMax = false;
  if (actor2->getY() <= actor1->getY() + actor1->getHeight()) {
    hitYBottomMin = true;
  }
  if (actor2->getY() + actor2->getHeight() >= actor1->getY() + actor1->getHeight()) {
    hitYBottomMax = true;
  }

  //Calculate if hit
  if (((hitXRightMin && hitXRightMax) && ((hitYTopMin && hitYTopMax) || (hitYBottomMin && hitYBottomMax))) 
  || ((hitXLeftMin && hitXLeftMax) && ((hitYTopMin && hitYTopMax) || (hitYBottomMin && hitYBottomMax)))) {
    return true;
  }

  return false;
}

