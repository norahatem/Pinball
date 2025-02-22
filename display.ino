#include "./defines.h"

int temp[8];

void setup()
{
  // put your setup code here, to run once:
  setup_ht16k33(addr_1);
  Serial.begin(9600);              // Initialize serial communication at 9600 baud
}

void loop()
{
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    String userInput = Serial.readStringUntil('\n');
    Serial.print("You typed: ");
    Serial.println(userInput);
    show(userInput);
    // for(int i =0; i< userInput.length(); i++){
    //   temp = userInput[i];
    //   Serial.println(temp);
    // }
  }
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

void show(String data){
  for(uint8_t i = 0; i < 8; i++){
    temp[i] = data[i];
    Serial.println(temp[i]);
    if(temp[i] > 64 && temp[i] < 91){
      transmit_data(addr_1, digit_place[7-i], letters_upper[temp[i]-65] & 0xFF, letters_upper[temp[i]-65] >> 8);
    } 
    else if(temp[i] > 96 && temp[i] < 123){
      transmit_data(addr_1, digit_place[7-i], letters_lower[temp[i]-97] & 0xFF, letters_lower[temp[i]-97] >> 8);
    }
    else if(temp[i] > 47 && temp[i] < 58){
      transmit_data(addr_1, digit_place[7-i], numbers[temp[i]-48] & 0xFF, numbers[temp[i]-48] >> 8);
    }
    else if(temp[i] == 32 || temp[i] == 0){
      transmit_data(addr_1, digit_place[7-i], SPACE & 0xFF, SPACE >> 8);
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