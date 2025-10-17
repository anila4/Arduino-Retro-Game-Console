#define dataIn 11
#define Clk_pin 13
#define Control_Signal 10
#define rightButton 7
#define leftButton 2
#define upButton 8
#define downButton 4 

// project is unfinished, player movement and shooting have been solved
// must introduce enemy logic and game loss/win logic

//````````````````````````````````````````````````````````````````````
//sending info to max7219
void sendbyte(byte reg, byte data){

  digitalWrite(Control_Signal, LOW);

  shiftOut(dataIn, Clk_pin, MSBFIRST, reg);
  shiftOut(dataIn, Clk_pin, MSBFIRST, data);

  digitalWrite(Control_Signal, HIGH);
}

//setting up registers for max7219
void MAX7219_setup(){

  //check the max7219 datasheet for more info
  sendbyte(0x09, 0x00);
  sendbyte(0x0A, 0x04);
  sendbyte(0x0B, 0x07);
  sendbyte(0x0C, 0x01);
  sendbyte(0x0F,0x00);
}

//``````````````````````````````````````````````````````````````````````

enum element_Type{
  PLAYER = 0,
  ALIEN = 1,
};

struct gameElement{
  int8_t x_coord;
  int8_t y_coord;
  enum element_Type type; 
  bool active;
};

struct gameElement Player[4];
// add something for enemies
struct gameElement Bullets[8];   // since an 8*8 is being used, can't fire too many bullets anyways, more efficient and safe
byte screen[8];
bool loseCondition = false;


//````````````````````````````````````````````````````

void clearMatrix(){

  for(int8_t i = 0; i < 8; i++){
    screen[i] = 0b00000000;
  }
}

void displayPattern(const byte pattern[8]){

  for (int8_t i = 0; i < 8; i++){

    sendbyte(i+1, pattern[i]);

  }
}

//````````````````````````````````````````````````````````````````

void makePlayer(){

  Player[0].x_coord = 3;
  Player[0].y_coord = 1;
  Player[1].x_coord = 2;
  Player[1].y_coord = 0;
  Player[2].x_coord = 3;
  Player[2].y_coord = 0;
  Player[3].x_coord = 4;
  Player[3].y_coord = 0;

  for(int i = 0; i < 4; i++){

    Player[i].active = true;

  }

}

void makeScreen(){
  // print player
  for(int i = 0; i < 4; i++){
    screen[Player[i].y_coord] |= (1 << Player[i].x_coord);
  }

  //print
  for(int i = 0; i < 8; i++){
    if(Bullets[i].active == true){
    screen[Bullets[i].y_coord] |= (1 << Bullets[i].x_coord);
    }
  }
}

void movePlayer(int8_t leftState, int8_t rightState){

  int8_t direction;
  if (leftState == HIGH && (Player[1].x_coord != 0)){
    direction = -1;
  }
  else if (rightState == HIGH && (Player[3].x_coord != 7)){
    direction = 1;
  }
  else{
    direction = 0;
  }
  for(int i = 0; i < 4; i++){
    Player[i].x_coord += direction;
  }
}

void shoot(struct gameElement shooter, int8_t direction){
  int8_t current_bullet = -1;

  for(int i = 0; i < 8; i++){
    if(Bullets[i].active == false){
      current_bullet = i;
      break;
    }
  }
  
  //in case none of the bullet spots were open
  if(current_bullet == -1){
    return;
  }

  else{
    Bullets[current_bullet].active = true;
    Bullets[current_bullet].x_coord = shooter.x_coord;
    Bullets[current_bullet].y_coord = shooter.y_coord + direction;
    Bullets[current_bullet].type = shooter.type;    
  }
}

void moveBullets(){

  int8_t direction;

  for(int i = 0; i < 8; i++){
    if(Bullets[i].active == true){
      switch(Bullets[i].type){
        case 0:
          direction = 1;
          break;
        case 1:
          direction = -1;
          break;
      }
      
      Bullets[i].y_coord += direction;
      
      // if bullet passes the boundaries
      if(Bullets[i].y_coord > 7 || Bullets[i].y_coord < 0){
        Bullets[i].x_coord = 0;
        Bullets[i].y_coord = 0;
        Bullets[i].active = false;
      }
    }

  }
}


//`````````````````````````````````````````````````````````````````

void setup() {
  // put your setup code here, to run once:

  pinMode(dataIn, OUTPUT);
  pinMode(Clk_pin, OUTPUT);
  pinMode(Control_Signal, OUTPUT);
  pinMode(rightButton, INPUT_PULLUP);
  pinMode(leftButton, INPUT_PULLUP);
  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton,INPUT_PULLUP);
  MAX7219_setup();
  clearMatrix();

  //Make the player
  makePlayer();   //temporary fix for development, may be changed to better optimize code or introduce enemies (or seperate function will be implemented)
  for(int i = 0; i < 8; i++){
    Bullets[i].active = false;
  }
  
}





int8_t leftState = 0;
int8_t rightState = 0;
int8_t upState = 0;
int8_t downState = 0;

void loop() {
  // put your main code here, to run repeatedly:

  //`````````````````````````````````````````````````````````

  leftState = digitalRead(leftButton);
  rightState = digitalRead(rightButton);
  upState = digitalRead(upButton);
  downState = digitalRead(downButton);

  //````````````````````````````````````````````````````````
  if(loseCondition == false){
  clearMatrix();
  movePlayer(leftState, rightState);
  moveBullets();
  if(upState == HIGH){
    shoot(Player[0], 1);
  }
  makeScreen();
  displayPattern(screen);
  delay(120);
  }
  else{
    clearMatrix();
  }
}

