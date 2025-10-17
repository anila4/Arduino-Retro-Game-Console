#define dataIn 11
#define Clk_pin 13
#define Control_Signal 10
#define rightButton 7
#define leftButton 2
#define upButton 8
#define downButton 4 

// Note: Need to make an endscreen still, will find an adequate design for game over message given limited display space

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

enum State{
  LEFT = 0,
  RIGHT = 1,
  UP = 2,
  DOWN = 3,
  START = 4
};

struct gameElement{
  int8_t x_coord;
  int8_t y_coord;
  enum State elementState;
};

struct gameElement Player[32];
struct gameElement Food;
int Player_Length = 1;    //figure out why logic doesn't allow snake to grow at length 1 for the first food item picked up 
//was just a small body follow issue (since it would move the head first, but body would shift further ahead, meaning the body was ahead)
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


void makeScreen(){

  for (int i = 0; i < Player_Length; i++){
    screen[Player[i].y_coord] |= (1<<Player[i].x_coord);

    if((i != 0) && (Player[i].x_coord == Player[0].x_coord && Player[i].y_coord == Player[0].y_coord)){
      loseCondition = true;
    }
  }
  screen[Food.y_coord] |= (1<<Food.x_coord);

  // move this logic to a sooner point in execution, or else there may be a delay.
  if((Player[0].x_coord == Food.x_coord) && (Player[0].y_coord == Food.y_coord)){
    Player_Length++;
    generateFood();
  }
}

void movePlayer(int8_t leftState, int8_t rightState, int8_t upState, int8_t downState){

  //start moving with head (change of plans)


    for(int j = Player_Length; j > 0; j--){
    //work back to front, easier since no need to store a seperate previous.position variable or account for head position
    Player[j].x_coord = Player[j-1].x_coord;
    Player[j].y_coord = Player[j-1].y_coord;
  }

  switch (Player[0].elementState){
    // now we move the head
    case 0:
      Player[0].x_coord = (Player[0].x_coord - 1 + 8)%8;   
      if(upState == HIGH){
        Player[0].elementState = 2;
      }  
      else if (downState == HIGH){
        Player[0].elementState = 3;
      }
      break;
      

    case 1:
      Player[0].x_coord = (Player[0].x_coord+1)%8;
      if(upState == HIGH){
        Player[0].elementState = 2;
      }  
      else if (downState == HIGH){
        Player[0].elementState = 3;
      }
      break;


    case 2:
      Player[0].y_coord = (Player[0].y_coord+1)%8;
      if(leftState == HIGH){
        Player[0].elementState = 0;
      }  
      else if (rightState == HIGH){
        Player[0].elementState = 1;
      }
      break;
    
    case 3:
      Player[0].y_coord = (Player[0].y_coord-1+8)%8;
      if(leftState == HIGH){
        Player[0].elementState = 0;
      }  
      else if (rightState == HIGH){
        Player[0].elementState = 1;
      }
      break;

    default:
      if (leftState == HIGH){
      Player[0].elementState = 0;
    }
      else if(rightState == HIGH){
      Player[0].elementState = 1;
    }
  
      else if (upState == HIGH){
      Player[0].elementState = 2;
    }

      else if (downState == HIGH){
      Player[0].elementState = 3;
    }
    break;
  }
  //head movement all done

  //now logic for body to catch up (depreciated, check above, now we move the head first)


}

void generateFood(){
  int8_t possibleX;
  int8_t possibleY;
  bool flag = true;
  while(flag == true){
    flag = false;
    possibleX = random(8);
    possibleY = random(8);

    for(int i = 0; i < Player_Length; i++){
      if((possibleX == Player[i].x_coord) && (possibleY == Player[i].y_coord)){
        flag = true;
      }
    }
  }

  Food.x_coord = possibleX;
  Food.y_coord = possibleY;

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

  Player[0].x_coord = 3;
  Player[0].y_coord = 3;
  Player[0].elementState = START;

  Food.x_coord = 5;
  Food.y_coord = 6;
  Food.elementState = START;
  
}





int8_t leftState = 0;
int8_t rightState = 0;
int8_t upState = 0;
int8_t downState = 0;

void playSnake() {
  // put your main code here, to run repeatedly:

  //`````````````````````````````````````````````````````````

  leftState = digitalRead(leftButton);
  rightState = digitalRead(rightButton);
  upState = digitalRead(upButton);
  downState = digitalRead(downButton);

  //````````````````````````````````````````````````````````
  if(loseCondition == false){
  clearMatrix();
  movePlayer(leftState, rightState, upState, downState);
  //checkCollision();   depreciated, logic was simple enough to implement in existing function without too much complexity
  makeScreen();
  displayPattern(screen);
  delay(120);
  }
  else{
    clearMatrix();
  }
}



