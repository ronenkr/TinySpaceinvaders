#include <Arduino.h>
#include <ST7586S.h>
#include <EEPROM.h>

// Constants for screen size and game settings
#define SCREEN_WIDTH 54
#define SCREEN_HEIGHT 120

#define NUM_INVADERS 40
#define NUM_INVADER_BULLETS 3

#define MAX_LIVES 3

#define INVADER_WIDTH 4
#define INVADER_HEIGHT 9

#define BUTTON_LEFT   PD5
#define BUTTON_RIGHT  PD4
#define BUTTON_FIRE   PD6  
#define BUTTON_OTHER  PD3

#define POWER_ON      PD2


#define GMS_END       0
#define GMS_PLAYING   1

//0x03,0x18,0xE0 individual pixels
//

#define PL 0x03
#define PM 0x18
#define PR 0xE0
#define PE 0x00

#define X 1
#define P(A,B,C) ((A?PL:0)|(B?PM:0)|(C?PR:0))

const unsigned char bmpEmpty1[1*8]={
  P(0,0,0),
  P(0,0,0),
  P(0,0,0),
  P(0,0,0),
  P(0,0,0),
  P(0,0,0),
  P(0,0,0),
  P(0,0,0)
};

const unsigned char bmpEmpty2[2*8]={
  P(0,0,0),P(0,0,0),
  P(0,0,0),P(0,0,0),
  P(0,0,0),P(0,0,0),
  P(0,0,0),P(0,0,0),
  P(0,0,0),P(0,0,0),
  P(0,0,0),P(0,0,0),
  P(0,0,0),P(0,0,0),
  P(0,0,0),P(0,0,0)
};

const unsigned char bmpEmptyTiny[1*2]={
  P(0,0,0),
  P(0,0,0)
};

const unsigned char bmpEnemy0[4*8]={
  P(0,0,0),P(0,0,X),P(X,0,0),P(0,0,0),
  P(0,0,0),P(0,X,X),P(X,X,0),P(0,0,0),
  P(0,0,0),P(X,X,X),P(X,X,X),P(0,0,0),
  P(0,0,X),P(X,0,X),P(X,0,X),P(X,0,0),
  P(0,0,X),P(X,X,X),P(X,X,X),P(X,0,0),
  P(0,0,0),P(0,X,0),P(0,X,0),P(0,0,0),
  P(0,0,0),P(X,0,X),P(X,0,X),P(0,0,0),
  P(0,0,X),P(0,X,0),P(0,X,0),P(X,0,0)
};

const unsigned char bmpEnemy0_2[4*8]={
  P(0,0,0),P(0,0,X),P(X,0,0),P(0,0,0),
  P(0,0,0),P(0,X,X),P(X,X,0),P(0,0,0),
  P(0,0,0),P(X,X,X),P(X,X,X),P(0,0,0),
  P(0,0,X),P(X,0,X),P(X,0,X),P(X,0,0),
  P(0,0,X),P(X,X,X),P(X,X,X),P(X,0,0),
  P(0,0,0),P(0,X,0),P(0,X,0),P(0,0,0),
  P(0,0,0),P(X,0,0),P(0,0,X),P(0,0,0),
  P(0,0,0),P(0,X,0),P(0,X,0),P(0,0,0)
};


const unsigned char bmpEnemy1[4*8]={
  P(0,0,X),P(0,0,0),P(0,0,X),P(0,0,0),
  P(0,0,0),P(X,0,0),P(0,X,0),P(0,0,0),
  P(0,0,X),P(X,X,X),P(X,X,X),P(0,0,0),
  P(0,X,X),P(0,X,X),P(X,0,X),P(X,0,0),
  P(X,X,X),P(X,X,X),P(X,X,X),P(X,X,0),
  P(X,0,X),P(X,X,X),P(X,X,X),P(0,X,0),
  P(X,0,X),P(0,0,0),P(0,0,X),P(0,X,0),
  P(0,0,0),P(X,X,0),P(X,X,0),P(0,0,0)
};

const unsigned char bmpEnemy1_2[4*8]={
  P(0,0,X),P(0,0,0),P(0,0,X),P(0,0,0),
  P(0,0,0),P(X,0,0),P(0,X,0),P(0,0,0),
  P(X,0,X),P(X,X,X),P(X,X,X),P(0,X,0),
  P(X,X,X),P(0,X,X),P(X,0,X),P(X,X,0),
  P(X,X,X),P(X,X,X),P(X,X,X),P(X,X,0),
  P(0,0,X),P(X,X,X),P(X,X,X),P(0,0,0),
  P(0,0,X),P(0,0,0),P(0,0,X),P(0,0,0),
  P(0,X,0),P(0,0,0),P(0,0,0),P(X,0,0)
};


const unsigned char bmpEnemy2[4*8]={
  P(0,0,0),P(0,X,X),P(X,X,0),P(0,0,0),
  P(0,X,X),P(X,X,X),P(X,X,X),P(X,X,0),
  P(X,X,X),P(X,X,X),P(X,X,X),P(X,X,X),
  P(X,X,X),P(0,0,X),P(X,0,0),P(X,X,X),
  P(X,X,X),P(X,X,X),P(X,X,X),P(X,X,X),
  P(0,0,X),P(X,X,0),P(0,X,X),P(X,0,0),
  P(0,X,X),P(0,0,X),P(X,0,0),P(X,X,0),
  P(0,0,X),P(X,0,0),P(0,0,X),P(X,0,0)
};

const unsigned char bmpEnemy2_2[4*8]={
  P(0,0,0),P(0,X,X),P(X,X,0),P(0,0,0),
  P(0,X,X),P(X,X,X),P(X,X,X),P(X,X,0),
  P(X,X,X),P(X,X,X),P(X,X,X),P(X,X,X),
  P(X,X,X),P(0,0,X),P(X,0,0),P(X,X,X),
  P(X,X,X),P(X,X,X),P(X,X,X),P(X,X,X),
  P(0,0,X),P(X,X,0),P(0,X,X),P(X,0,0),
  P(0,X,X),P(0,0,X),P(X,0,0),P(X,X,0),
  P(X,X,0),P(0,0,0),P(0,0,0),P(0,X,X)
};


const unsigned char bmpPlayer[5*8]={
  P(0,0,0),P(0,0,0),P(X,0,0),P(0,0,0),P(0,0,0),
  P(0,0,0),P(0,0,X),P(X,X,0),P(0,0,0),P(0,0,0),
  P(0,0,0),P(0,0,X),P(X,X,0),P(0,0,0),P(0,0,0),
  P(0,X,X),P(X,X,X),P(X,X,X),P(X,X,X),P(0,0,0),
  P(X,X,X),P(X,X,X),P(X,X,X),P(X,X,X),P(X,0,0),
  P(X,X,X),P(X,X,X),P(X,X,X),P(X,X,X),P(X,0,0),
  P(X,X,X),P(X,X,X),P(X,X,X),P(X,X,X),P(X,0,0),
  P(X,X,X),P(X,X,X),P(X,X,X),P(X,X,X),P(X,0,0),
};

const unsigned char bmpInvaderBulet[1*2]={
  P(0,X,0),
  P(0,X,0)
};


// Structure definitions
struct Entity {
    int x, y;  // Position
    unsigned char width, height;  // Size of entity
    bool active;  // Whether the entity is active or not
};

struct Player {
    struct Entity entity;
    uint16_t score;
    unsigned char lives;
};

struct Invader {
    struct Entity entity;
    bool direction;  // true for right, false for left
    char type;
    bool animationState; //true for one state false for other
};

struct Bullet {
    struct Entity entity;
    unsigned char speed;  // Speed of bullet
    bool isPlayerBullet;  // Indicates whether it's player's bullet or invader's
};

// Game state variables
//uint8_t screenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];  // Screen buffer
struct Player player;
struct Invader invaders[NUM_INVADERS];
struct Bullet playerBullet, invaderBullets[NUM_INVADER_BULLETS];

unsigned char g_invadersSpeed=6;

unsigned char g_currentSpeed=6;
bool onFlag = false;
unsigned char g_gameStatus=GMS_END;
char szScore[7]={0};



// Function prototypes
void updateBullets();
void updateInvaders();
void checkCollisions();


// Function to convert integer to string (itoa implementation)
char* itoa(int num, char* str, int base) {
    int i = 0;
    int isNegative = 0;

    // Handle 0 explicitly
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // Handle negative numbers
    if (num < 0 && base == 10) {
        isNegative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int remainder = num % base;
        str[i++] = (remainder > 9) ? (remainder - 10) + 'a' : remainder + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    // Append string terminator
    str[i] = '\0';

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start++] = str[end];
        str[end--] = temp;
    }

    return str;
}


void updateScore(){
  uint16_t oldScore;
  oldScore = EEPROM_read(0)|EEPROM_read(1)<<8;

  if(player.score>oldScore){
    EEPROM_write(0,player.score&0xFF);
    EEPROM_write(1,(player.score>>8)&0xFF);
  }
  
}

void drawGameOver(){
  ST7586S_clear();
  ST7586S_putString("SPACE INVADERS CRAP EDITION!",0,10);
  ST7586S_putString("PRESS FIRE TO START",0,20);
  ST7586S_putString("TOP SCORE:",0,115);

  player.score = EEPROM_read(0)|EEPROM_read(1)<<8;
  
  itoa(player.score,szScore,10);
  //sprintf(szScore,"%d",player.score);
  //sprintf(szScore,"%d",randSeed);
  ST7586S_putString(szScore,20,115);

}


void initInvaders(){
  // Initialize invaders
    int invaderSpacing = 1;  // Example spacing between invaders
    int invaderWidth = INVADER_WIDTH;  // Example width of each invader
    int invaderHeight = INVADER_HEIGHT;  // Example height of each invader
    int invaderRows = NUM_INVADERS / 8;  // Number of rows of invaders
    
    for (int i = 0; i < NUM_INVADERS; i++) {
        int row = i / 8;  // Determine current row
        int col = i % 8;  // Determine current column
        invaders[i].entity.x = invaderSpacing * col + invaderWidth * col;  // Adjust x position based on column
        invaders[i].entity.y = 10 + invaderHeight * row;  // Adjust y position based on row
        invaders[i].entity.width = invaderWidth;
        invaders[i].entity.height = invaderHeight;
        invaders[i].entity.active = true;  // Invader is active at start
        invaders[i].direction = true;  // Example: initially move right
        invaders[i].animationState=false;
        invaders[i].type = row;
    }
}

// Initialize game entities and state
void initGame() {

    ST7586S_clear();

    g_invadersSpeed=6;
    g_currentSpeed= g_invadersSpeed;
  
    // Initialize player
    player.entity.x = SCREEN_WIDTH / 2;  // Example starting position at the center
    player.entity.y = SCREEN_HEIGHT - 20;  // Example starting y position near the bottom
    player.entity.width = 5;  // Example size
    player.entity.height = 5;
    player.entity.active = true;  // Player is active at start
    player.score = 0;
    player.lives = MAX_LIVES;  // Set initial lives
    
    initInvaders();
    
    // Initialize player bullet
    playerBullet.entity.width = 1;  // Example bullet width
    playerBullet.entity.height = 3;  // Example bullet height
    playerBullet.entity.active = false;  // Bullet is initially inactive
    playerBullet.speed = 4;  // Example bullet speed

    // Initialize invader bullets
    for (int i = 0; i < NUM_INVADER_BULLETS; i++) {
        invaderBullets[i].entity.width = 1;  // Example bullet width
        invaderBullets[i].entity.height = 2;  // Example bullet height
        invaderBullets[i].entity.active = false;  // Bullet is initially inactive
        invaderBullets[i].speed = 1;  // Example bullet speed
    }

    for(int i=0;i<27;i++){
      ST7586S_putString(";",i*2,SCREEN_HEIGHT-12);
    }
    
    // Seed the random number generator for invader movements or shooting (if applicable)
    //srand(time(NULL));
}

// Update player and invader bullets
void updateBullets() {
    // Update player's bullet
    if (playerBullet.entity.active) {
        ST7586S_putString(" ",playerBullet.entity.x,playerBullet.entity.y+4);
        playerBullet.entity.y -= playerBullet.speed;
        
        // Check if bullet is out of bounds
        if (playerBullet.entity.y < 0) {
            playerBullet.entity.active = false;
            ST7586S_putString(" ",playerBullet.entity.x,0);
        }
    }
    
    // Update invader bullets
    for (int i = 0; i < NUM_INVADER_BULLETS; i++) {
        if (invaderBullets[i].entity.active) {
            invaderBullets[i].entity.y += invaderBullets[i].speed;
            
            // Check if bullet is out of bounds
            if (invaderBullets[i].entity.y >= SCREEN_HEIGHT-12) {
                invaderBullets[i].entity.active = false;
            }
        }
    }
}



void updateInvaders() {
    // Constants for movement
    //int invaderSpeed = 1;  // Example invader movement speed

    if(g_currentSpeed>0){
      g_currentSpeed--;
      return;
    }
    g_currentSpeed = g_invadersSpeed;
    
    // Check if any invader reaches screen boundary
    bool hitBoundary = false;
    bool bOnce = false;
    bool bAnyAlive = false;
    for (int i = 0; i < NUM_INVADERS; i++) {
        if (invaders[i].entity.active) {
          bAnyAlive=true; //found at least one alive
            if(!bOnce){ //clears the space once every check
              for(unsigned char j=0;j<48;j++){
                  ST7586S_clearLine(invaders[i].entity.y+j);
              }
              bOnce=true;
            }

            invaders[i].animationState=!invaders[i].animationState;
            
            // Update invader position based on direction
            if (invaders[i].direction) {
                // Move right
                invaders[i].entity.x += 1;
            } else {
                // Move left
                invaders[i].entity.x -= 1;
            }

            // Check if invader reaches either side of the screen
            if (invaders[i].entity.x <= 0 || invaders[i].entity.x + INVADER_WIDTH >= 53) {
                hitBoundary = true;
            }

            // Check if any invader reaches bottom of the screen
            if (invaders[i].entity.y + INVADER_HEIGHT >= SCREEN_HEIGHT - 20) {
                // Restart the game (reset all entities)
                updateScore();
                drawGameOver();
                delay(1000);
                g_gameStatus = GMS_END;
                return;  // Exit function early to prevent further updates
            }
        }
    }


    if(!bAnyAlive){ //every one is dead
      player.score+=100; //cleared a level
      initInvaders();//repopulate the invaders
      if(g_invadersSpeed>0){ //make the invaders faster
        g_invadersSpeed--;
      }
      return;
    }

    // If any invader hits the boundary, reverse direction for all invaders
    if (hitBoundary) {
        for (int i = 0; i < NUM_INVADERS; i++) {
            if (invaders[i].entity.active) {
                invaders[i].direction = !invaders[i].direction;  // Reverse direction
                invaders[i].entity.y += INVADER_HEIGHT;  // Move down one row
            }
        }
    }


  // Randomly create invader bullets
    int randomInvaderIndex = rand() % NUM_INVADERS;
    if (invaders[randomInvaderIndex].entity.active) {
        // Find an inactive bullet to use
        for (int j = 0; j < NUM_INVADER_BULLETS; j++) {
            if (!invaderBullets[j].entity.active) {
                // Position the bullet at the invader's position
                invaderBullets[j].entity.x = invaders[randomInvaderIndex].entity.x + invaders[randomInvaderIndex].entity.width / 2;
                invaderBullets[j].entity.y = invaders[randomInvaderIndex].entity.y + invaders[randomInvaderIndex].entity.height;
                invaderBullets[j].entity.active = true;
                break;
            }
        }
    }
    
}

// Check collisions between bullets, player, and invaders
void checkCollisions() {
    // Check player bullet collisions with invaders
    if (playerBullet.entity.active) {
        for (int i = 0; i < NUM_INVADERS; i++) {
            if (invaders[i].entity.active) {
                if (playerBullet.entity.x >= invaders[i].entity.x &&
                    playerBullet.entity.x < invaders[i].entity.x + invaders[i].entity.width &&
                    playerBullet.entity.y >= invaders[i].entity.y &&
                    playerBullet.entity.y < invaders[i].entity.y + invaders[i].entity.height) {
                    
                    // Collision detected
                    switch(invaders[i].type){ //score is based on the inavder type
                      case 0:{
                        player.score+=30;
                        break;
                      }

                      case 1:
                      case 2:{
                        player.score+=20;
                        break;
                      }

                      case 3:
                      case 4:{
                        player.score+=10;
                        break;
                      }
                      
                    }
                    //player.score++;  // Increase score
                    invaders[i].entity.active = false;  // Deactivate invader
                    playerBullet.entity.active = false;  // Deactivate player bullet

                    ST7586S_putString(" ",playerBullet.entity.x,playerBullet.entity.y+1); //delete the bullet
                    ST7586S_drawBitmap(bmpEmpty2,invaders[i].entity.x, invaders[i].entity.y,2,8); //Delete the inavder from the screen
                    ST7586S_drawBitmap(bmpEmpty2,invaders[i].entity.x+2, invaders[i].entity.y,2,8);//seconds part
                }
            }
        }
    }
    
    // Check invader bullet collisions with player
    for (int i = 0; i < NUM_INVADER_BULLETS; i++) {
        if (invaderBullets[i].entity.active) {
            if (invaderBullets[i].entity.x >= player.entity.x &&
                invaderBullets[i].entity.x < player.entity.x + player.entity.width &&
                invaderBullets[i].entity.y >= player.entity.y &&
                invaderBullets[i].entity.y < player.entity.y + player.entity.height) {
                
                // Collision detected
                // Example: Player loses a life or game over logic
                //player.entity.active = false;  // Example: Deactivate player
                
                invaderBullets[i].entity.active = false;  // Deactivate invader bullet
                if(player.lives>0){
                  player.lives--;
                  ST7586S_clearLine(112);
                  ST7586S_clearLine(113);
                  ST7586S_clearLine(114);
                  ST7586S_clearLine(115);
                  ST7586S_clearLine(116);
                  ST7586S_clearLine(117);
                  ST7586S_clearLine(118);
                  ST7586S_clearLine(119);
                  //ST7586S_drawBitmap(bmpEmpty2,37+player.lives*6, 112,2,8);
                  //ST7586S_drawBitmap(bmpEmpty2,37+player.lives*6+3, 112,2,8);
                }else{
                  updateScore();
                  drawGameOver();
                  delay(1000);
                  g_gameStatus = GMS_END;
                }
                
            }
        }
    }
}

void initRandomSeed(){
    uint8_t low, high;
    uint8_t pin=PC4;
    
    ADC1->CSR = pin & 15;
    bitSet(ADC1->CR2, 3); // right align

    ADC1->CR1 |= ADC1_CR1_ADON;

    // second write starts the conversion
    //bitSet(ADC1->CR1, 0);
    ADC1->CR1 |= ADC1_CR1_ADON;

    // EOC is set when the conversion finishes
    while (! (ADC1->CSR & ADC1_FLAG_EOC));
    ADC1->CSR &= ~(ADC1_FLAG_EOC);  // important! clear the flag

    // in right align mode we have to read DRL first
    low  = ADC1->DRL;
    high = ADC1->DRH;

    // turn off the ADC, free the pin for digital use again
    bitClear(ADC1->CR1, 0);

    srand((high << 8) | low);

}

/* **********************************************************
/*                       Void Setup                         *
/* ******************************************************** */
void setup(){
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1); //CPU speed
  initRandomSeed();
  //randomSeed(analogRead(0));
  
  ST7586S_connection(PC5, PC6, PC4, PC7, PA3); // sclk,sdin,dc,reset,sce
  ST7586S_begin_full(160,120, CHIP_ST7586S);

  

  pinMode(BUTTON_LEFT,INPUT_PULLUP);
  pinMode(BUTTON_RIGHT,INPUT_PULLUP);

  pinMode(BUTTON_FIRE,INPUT_PULLUP);
  pinMode(BUTTON_OTHER,INPUT_PULLUP);
  

  pinMode(POWER_ON,OUTPUT);

  digitalWrite(POWER_ON,LOW);
  
  initGame();
  g_gameStatus=GMS_END;
  drawGameOver();

  
}


void drawGame() {
    // Draw player
    if (player.entity.active) {
      ST7586S_drawBitmap(bmpPlayer,player.entity.x, player.entity.y,5,8);
        //drawEntity(&player.entity, PLAYER_COLOR);
        
    }

    struct Entity *entity;

    // Draw invaders
    for (int i = 0; i < NUM_INVADERS; i++) {
        if (invaders[i].entity.active) {
          
          entity = &(invaders[i].entity);
            //drawEntity(&invaders[i].entity, INVADER_COLOR);
          switch(invaders[i].type){
            
            case 0:{
              if(invaders[i].animationState){
                ST7586S_drawBitmap(bmpEnemy0_2,entity->x, entity->y,4,8);
              }else{
                ST7586S_drawBitmap(bmpEnemy0,entity->x, entity->y,4,8);
              }
              break;
            }

            case 1:
            case 2:{
              if(invaders[i].animationState){
                ST7586S_drawBitmap(bmpEnemy1_2,entity->x, entity->y,4,8);
              }else{
                ST7586S_drawBitmap(bmpEnemy1,entity->x, entity->y,4,8);
              }
              
              break;
            }

            case 3:
            case 4:{
              if(invaders[i].animationState){
                ST7586S_drawBitmap(bmpEnemy2_2,entity->x, entity->y,4,8);
              }else{
                ST7586S_drawBitmap(bmpEnemy2,entity->x, entity->y,4,8);  
              }
              
              break;
            }
          }
          
        }
    }

    // Draw player bullet
    if (playerBullet.entity.active) {
      ST7586S_putString("'",playerBullet.entity.x,playerBullet.entity.y);
        //drawEntity(&playerBullet.entity, PLAYER_BULLET_COLOR);
    }

    // Draw invader bullets
    for (int i = 0; i < NUM_INVADER_BULLETS; i++) {
        if (invaderBullets[i].entity.active) {
          //ST7586S_putString("'",invaderBullets[i].entity.x,invaderBullets[i].entity.y);
          ST7586S_drawBitmap(bmpEmptyTiny,invaderBullets[i].entity.x,invaderBullets[i].entity.y-2,1,2);
          
          ST7586S_drawBitmap(bmpInvaderBulet,invaderBullets[i].entity.x,invaderBullets[i].entity.y,1,2);
          
            //drawEntity(&invaderBullets[i].entity, INVADER_BULLET_COLOR);
        }
    }

    // Draw mother ship (if active)
    /*if (motherShip.active) {
        drawEntity(&motherShip.entity, MOTHER_SHIP_COLOR);
    }

    // Example: Draw score and other UI elements
    // Example: snprintf(screenBuffer + SCREEN_WIDTH * (SCREEN_HEIGHT - 1), SCREEN_WIDTH, "Score: %d", player.score);
    */

    ST7586S_putString("SCORE:",0,115);
    
    //sprintf(szScore,"%d",player.score);
    itoa(player.score,szScore,10);
    ST7586S_putString(szScore,12,115);

    for(int i=0;i<player.lives;i++){
      ST7586S_drawBitmap(bmpPlayer,37+i*6, 112,5,8);
    }

}

void processInput(){
  if (digitalRead(BUTTON_RIGHT)==0 ){
    //ST7586S_putString("<",0,110);
    if(player.entity.x>0){
      ST7586S_drawBitmap(bmpEmpty2,player.entity.x+3,player.entity.y,2,8);
      player.entity.x-=2;
    }
  }else{
    //ST7586S_putString(" ",0,110);
  }

  if (digitalRead(BUTTON_LEFT)==0 ){
    //ST7586S_putString(">",6,110);
    if(player.entity.x<SCREEN_WIDTH-5){
      ST7586S_drawBitmap(bmpEmpty2,player.entity.x,player.entity.y,2,8);
      player.entity.x+=2;
    }
  }else{
    //ST7586S_putString(" ",6,110);
  }

  if (digitalRead(BUTTON_FIRE)==0 || digitalRead(BUTTON_OTHER)==0){
    //ST7586S_putString("*",3,110);
      if(!playerBullet.entity.active){
        playerBullet.entity.active = true;
        playerBullet.entity.x = player.entity.x+2;
        playerBullet.entity.y = SCREEN_HEIGHT-20;

    }
  }else{
    //ST7586S_putString(" ",3,110);
  }
  
}

/* **********************************************************
/*                      Void Loop                           *
/* ******************************************************** */ 
unsigned long ulTimer;
void loop(){
    if(g_gameStatus == GMS_PLAYING){
  // Process player input
     processInput();

    // Update game state
    updateBullets();
    
    updateInvaders();
    checkCollisions();

    // Draw game state to screen buffer
    if(g_gameStatus == GMS_PLAYING){ //The game could have ended
        if(millis()-ulTimer>=1000){
          digitalWrite(POWER_ON,onFlag); //Used to kick the power converter ip5306 so that it wont go to sleep
          onFlag=!onFlag;
          ulTimer = millis();
        }
        drawGame();
    }
    }else{ //in Game END
      digitalWrite(POWER_ON,LOW); //Set the power to shut off 
      if (digitalRead(BUTTON_FIRE)==0 || digitalRead(BUTTON_OTHER)==0){
        initGame();
        g_gameStatus = GMS_PLAYING;
      }
    }
    //delay(80);    
}
