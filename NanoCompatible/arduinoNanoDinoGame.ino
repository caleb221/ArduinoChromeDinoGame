
/*Arduino version of Chrome No-Internet Game 
 * Bitmaps were all drawn using
 * piskellapp
 * feel free to use this code
 * but please give a shoutout!
 * 
 *-Caleb221
 */
#include <Wire.h>
#include<stdio.h>
#include "U8g2lib.h"
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED
#define IMG_WIDTH 8
#define IMG_HEIGHT 8
#define IMG_SIZE 8
#define HEIGHT 32 //pixel screen height
#define DNR_CRTL 13 //Control Button Pin 
#define BTN_CONFIG HIGH

int ENEMY_START = 100;//X Starting for enemy rendering
int WIDTH=128;//pixel screen width
int FLOOR_LEVEL=22;
int JUMP_PIXELS=5;
int ENEMY_PAD = 20;



void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
   pinMode(DNR_CRTL,INPUT);

 u8g.begin();
}

void loop() {

  u8g.clearBuffer();
  u8g.setFont(u8g2_font_tinytim_tf);
  u8g.drawStr(10,10,"PRESS THE");
  u8g.drawStr(10,20,"BUTTON TO");
  u8g.drawStr(10,30,"START!!");
  renderEnemy(1,80,20);
  u8g.sendBuffer();
 if(digitalRead(DNR_CRTL)==BTN_CONFIG)
  dinoGame();
 delay(100);

//Serial.println(digitalRead(DNR_CRTL));
}

//============================================================================
//        DINOSAUR CHROME GAME ATTEMPT (404-No Internet)
//============================================================================

const unsigned char dinoHexTiny[] PROGMEM = {
    0xe0, 0xb0, 0xf8, 0x2c, 0xf6, 0xbf, 0x24, 0x6c
};
const unsigned char birdHexTiny[] PROGMEM = {
  0x78, 0x18, 0x88, 0xfe, 0xff, 0x88, 0x18, 0x78

};
const unsigned char tinyCactusHex[] PROGMEM = {
  0x08,0x9D,0xB5,0xFF,0x16,0x1C,0x14,0x1C
};

struct player{
  int health;// not needed for dino game, possibly something else
  int x;
  int y;
  boolean isAlive;
  };
struct enemyEntity{
  bool render;
  int type;//1 cactus 2 is bird
  int x;
  int y;
  };
enemyEntity newEnemy();

void dinoGame()
{
 int score=0;
 
 player p  = {100,14,FLOOR_LEVEL,true};

 enemyEntity enemies[6]; 
  for(int i=0;i<6;i++)
  {   
    int rX = ENEMY_START+(ENEMY_PAD*i);
    int y=FLOOR_LEVEL;///cactus
    enemies[i] = {true,1,rX,y};
  }
  
  bool isJump = false; 
  int level=7;
  int enemySpeed=2;
  
 char buff[5];
 while(p.isAlive)
 {
//clear screen
  u8g.clearBuffer();

// check button for jump
  if((digitalRead(DNR_CRTL) ==BTN_CONFIG) && (p.y>0 ))
      p.y -=JUMP_PIXELS;
  // render Game Data  
  //dtostrf(score,5,2,buff);
  //u8g.setFont(u8g2_font_tinytim_tf);
  //u8g.drawStr(65,10,"SCORE:");
 // u8g.drawStr(100,10,buff);

  //u8g.drawStr(65,20,"LV:");
  //dtostrf(level,5,2,buff);
  //u8g.drawStr(100,20,buff);


  //process enemies (1 loop to rule them all!)
   for(int i=0;i<6;i++)
    {
      if(checkCollision(p.x,p.y,enemies[i].x,enemies[i].y))
          p.isAlive=false;
      if(enemies[i].render)
        renderEnemy(enemies[i].type,enemies[i].x,enemies[i].y);
      else{
        int rX = random(ENEMY_START,WIDTH+50)+ENEMY_PAD;
        int rType = 1;
        int y=FLOOR_LEVEL;//CACTUS y
        
            if(level>5)
            {
             rType = random(0,3);
             if(rType>1)
             {
              rType=2;
              y=random(IMG_SIZE-1,FLOOR_LEVEL);
             }
            }
        enemyEntity newE = {true,rType,rX,y};
        enemies[i] = newE;
        }   
        //MOVE ALL ENEMIES LEFT!
        enemies[i].x-=enemySpeed;
        //ERASE ENEMIES AFTER PLAYER.X
        if(enemies[i].x < 0)
          enemies[i].render = false;
    }

 //gravity!
 if(p.y<=FLOOR_LEVEL)
      p.y++;
 renderPlayer(p.x,p.y,1);  
 u8g.sendBuffer(); 
  
  score++;
  if(score%100==0)
    level++;
    
  else if(score%700==0)
  {
    score+=1000;
    enemySpeed+=2;
    level+=2;
  }
  
  }//end ALIVE
  u8g.clearBuffer();
  
u8g.setFont(u8g2_font_tinytim_tf);
//u8g.drawStr(65,10,"SCORE:");
//u8g.drawStr(100,10,buff);
u8g.drawStr(65,20,"LV:");
dtostrf(level,5,2,buff);
u8g.drawStr(100,20,buff);
u8g.drawStr(10,10,"YOU DED");
u8g.drawStr(20,30,"Press to exit");
//renderPlayer(40,24,1); 
u8g.sendBuffer(); 

// get EEPROM high score data
// compare to current score
//save if needed
//go back to application menu 

while (! digitalRead(DNR_CRTL)==BTN_CONFIG){
  }
}

void renderEnemy(int type,int x,int y)
{
  if(type ==1)
        u8g.drawXBMP( x, y, IMG_SIZE, IMG_SIZE, tinyCactusHex); 
  else if (type ==2)
        u8g.drawXBMP( x, y, IMG_SIZE, IMG_SIZE, birdHexTiny); 
      
}
void renderPlayer(int x,int y,int game)
{ 
  if (game ==1)
      u8g.drawXBMP( x, y, IMG_SIZE, IMG_SIZE, dinoHexTiny); 
  else if (game ==2)
      u8g.drawBox(x,y,4,4);
}

boolean checkCollision(int x1,int y1,int x2,int y2)
{ 
  if(x1 < x2 +IMG_WIDTH && x1+IMG_WIDTH > x2  &&
     y1 < y2+IMG_HEIGHT && y1+IMG_HEIGHT >y2)
     {
      return true; 
     }
    else
      return false;
}
