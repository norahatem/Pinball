#include "./defines.h"

//char userInput[DISPLAY_SIZE + 1] = {0};

//int temp[DISPLAY_SIZE];

uint16_t score = 0;
uint8_t previous_score = score;
uint16_t highest_score = 0;
uint8_t lives = 3;
uint8_t score_multiplier = 1;
//String name = "";
char name[DISPLAY_SIZE+1] = "A";
char currentLetter = 'A';


String userInput; //simulate data received from other microcontroller

command receivedCommand = No_command; // start with nothing
gameMode currentMode = STANDBY;

boolean invalidate= true;

const unsigned long interval = 250; // 1 second
unsigned long previousMillis = 0;  // Stores the last time the function was called
unsigned long currentMillis = millis();
uint8_t seconds = 0;
boolean normalMode = true;

void setup()
{
  // put your setup code here, to run once:
  setup_ht16k33(addr_1);
  setup_ht16k33(addr_2);
  reset_displays();
  // disabling the timer for now as it kind of causes issues with serial - i guess
  // Timer1.initialize(1000000);
  // Timer1.attachInterrupt(updateDisplay);
  Serial.begin(9600);
  updateDisplay();
}

void loop()
{
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;  // Update time
    manage(); //call manage function
  }
  // put your main code here, to run repeatedly:
  //check for user input -- simulating the commands from the other arduino 
  if(Serial.available()){
    String userInput = Serial.readStringUntil('\n');
    Serial.print("You typed: ");
    Serial.println(userInput);
    receivedCommand = userInput.toInt();
    //invalidate = true;
    //call process command isf there's a command input 
    processCommand();
    //updateDisplay();
    Serial.println(currentMode);
  }
}

void manage(){
  if(!normalMode)   seconds += 1;
  if(seconds>= 20 ){
    seconds = 0;
    normalMode = true;
    score_multiplier = 1;
    invalidate = true;
  }
  updateDisplay();
}

void reset_displays(){
  show(addr_1, "");
  show(addr_2, "");
}

void processCommand(){
  switch(receivedCommand){
    case No_command:
      break;
    case START:
      lives = 3;
      score = 0;
      memset(name, 0, 9);
      name[0] = 'A'; name[1] = '\0';
      currentMode = START_GAME;
      break;
    case LIFE_LOST:
      if(lives <= 0) break;
      lives -= 1;
      if(lives == 0){
        currentMode = GAME_OVER;
        if(score > highest_score){
          currentMode = NAME_ENTRY;
          highest_score = score;
        }
        break;
      }
      //might want to show -1 LIVES when a life is lost
      //show(addr_2, "-1 LIVES");
      break;
    case FRENZY_com:
      seconds = 0;
      normalMode = false;
      currentMode = FRENZY;
      score_multiplier = 2;
      //start a timer interrupt to come and disable the frenzy mode after 5 seconds --done
      break;
    case FREEZE_com:
      seconds = 0;
      normalMode = false;
      seconds = 0;
      currentMode = FREEZE;
      //start a timer interrupt to come and disable the freeze mode after 5 seconds --done
      break;
    case LOW_FRUIT:
      score += (10*score_multiplier);
      //currentMode = START_GAME;
      break;
    case HIGH_FRUIT:
      score += (20*score_multiplier);
      //currentMode = START_GAME;
      break;
    case BOMB:
      if(score >= (10*score_multiplier) && currentMode!= FREEZE){
        score -= (10*score_multiplier);
      }
      //currentMode = START_GAME;
      break;
  /////////////////////////////////////////////////////////////////////////////////////
  case NEXT_LETTER:
    if(currentMode != NAME_ENTRY) break;
    currentLetter += 1;
    if(currentLetter > 'Z'){
      currentLetter = 'A'; //this is to loop around, there is probably a better way to do it but yea :|
    }
    name[strlen(name) - 1] = currentLetter;
    break;
  case PREVIOUS_LETTER:
    if(currentMode != NAME_ENTRY) break;
    currentLetter -= 1;
    if(currentLetter <'A'){
      currentLetter = 'Z'; //to loop around the other way,
    }
    name[strlen(name) - 1] = currentLetter;
    break;
  case LETTER_CHOSEN:
    if(currentMode != NAME_ENTRY) break;
    currentLetter = 'A'; //reset curent char to A so that the user can loop through agan
    name[strlen(name)] = currentLetter;
    break;
  case NAME_ENTERED:
    if(currentMode != NAME_ENTRY) break;
    //probably what i could do is display high name and then wait to receive a reset signal and then go back to standby mode!!!
    //the displaying is already done! -- however should remove the A that i add after all names for next character
    //name = name.remove last character
    if(strlen(name) > 0){
      name[strlen(name) - 1] = '\0'; //remove last letter added when the letter is chosen
    }
    break;
  case RESET:
    currentMode = STANDBY;
    break;
  }
  invalidate=true;
}


void updateDisplay(){
  char buffer[9];
  sprintf(buffer, "%06d %d", score, lives);
  if(normalMode && ((currentMode == FREEZE) || currentMode == FRENZY)){
  currentMode = START_GAME;
  }
  if(!invalidate) return;
  if(currentMode != STANDBY) show(addr_1, String(buffer));
  switch(currentMode){
    case STANDBY:
      show(addr_2, "READY TO");
      show(addr_1, "PLAY");
      break;
    case START_GAME:
      show(addr_2, "PLAYING");
      show(addr_1, String(buffer));
      break;
    case GAME_OVER:
      //need to stop accepting any input unless it's start
      //how to do it
      show(addr_2, "GAMEOVER");
      if(score> highest_score){
        currentMode = NAME_ENTRY;
        highest_score = score;
        name[0] = 'A';
        name[1] = '\0';
      }
      break;
    case FREEZE:
      show(addr_2, "FREEZE");
      break;
    case FRENZY:
      show(addr_2, "FRENZY");
      //score_multiplier = 2;
      break;
    case NAME_ENTRY:
      show(addr_2, String(name));
      // Serial.print("name: ");
      // Serial.println(name);
      break;
  }
  invalidate=false;
}

void setup_ht16k33(uint8_t addr)
{
  Wire.begin();
  // start oscillation
  Wire.beginTransmission(addr);
  Wire.write(0x21);
  Wire.endTransmission();
  //...................
  // set brightness level --max
  Wire.beginTransmission(addr);
  Wire.write(0xEF);
  Wire.endTransmission();
  //......................
  // display ON, blinking OFF
  Wire.beginTransmission(addr);
  Wire.write(0x81);
  Wire.endTransmission();
}


void show(uint8_t addr, String data){
  for(uint8_t i = 0; i < DISPLAY_SIZE; i++){
    //temp[i] = data[i];
    if(data[i] >= A_UPPER && data[i] <= Z_UPPER){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], letters_upper[data[i]-A_UPPER] & 0xFF, letters_upper[data[i]-A_UPPER] >> 8);
    } 
    else if(data[i] >= a_lower && data[i] <= z_lower){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], letters_lower[data[i]-a_lower] & 0xFF, letters_lower[data[i]-a_lower] >> 8);
    }
    else if(data[i] >= zero && data[i] <= nine){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], numbers[data[i]- zero] & 0xFF, numbers[data[i]- zero] >> 8);
    }
    else if(data[i] == 32 || data[i] == 0){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], SPACE & 0xFF, SPACE >> 8);
    } 
    else if(data[i] == 43){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], PLUS & 0xFF, PLUS >> 8);
    }
    else if(data[i] == 45){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], MINUS & 0xFF, MINUS >> 8);
    }
    else if(data[i] == 42){
      transmit_data(addr, digit_place[DISPLAY_SIZE -1 -i], MULTIPLY & 0xFF, MULTIPLY >> 8);
    }
  }
}


void transmit_data(uint8_t addr, uint8_t reg, uint8_t byte1, uint8_t byte2){
  Wire.beginTransmission(addr);
  Wire.write(reg);
  Wire.write(byte1);
  Wire.write(byte2);
  Wire.endTransmission();
}