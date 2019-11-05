#include <Tone.h>
const int SER = 3;
const int SCLK = 4;
const int RCK = 5;
const int D1 = 7;
const int D2 = 10;
const int D3 = 11;
const int D4 = 13;
const int CLK = 9; //CLK
const int CLK_INH = 6; //Clk enable active low
const int QH = 8; //Serial Data in line
const int CLR = 2; //Clear register pin
const int SH_LD = 12;
const int SPEAKER = 1;

byte holaCount =1;
byte binaryInput = 0;
byte userInput = 0;

byte clkDigit1 = 0;
byte clkDigit2 = 0;
byte randNum1 = 0;
byte randNum2 = 0;

byte upperNibble = 0;
byte lowerNibble = 0;

int scoreLeftDigit = 0;
int scoreRightDigit = 0;

int scoreCount = 0;
int memArray[2][2];
int arrayIndex = 0;
int startTime = 0;

bool solved = false;

int data[8]; 
int count = 1;
unsigned long loopCount = 0;
//           {-,g,f,e,d,c,b,a}
//Hex values 0-15 for 7 segment display
int hex[16][8] = {{1,1,0,0,0,0,0,0},{1,1,1,1,1,0,0,1},{1,0,1,0,0,1,0,0},{1,0,1,1,0,0,0,0},
                    {1,0,0,1,1,0,0,1},{1,0,0,1,0,0,1,0}, {1,0,0,0,0,0,1,0},
                    {1,1,1,1,1,0,0,0},{1,0,0,0,0,0,0,0},{1,0,0,1,1,0,0,0},
                    {1,0,0,0,1,0,0,0},{1,0,0,0,0,0,1,1},{1,1,0,0,0,1,1,0},
                    {1,0,1,0,0,0,0,1},{1,0,0,0,0,1,1,0},{1,0,0,0,1,1,1,0}};
                    
//Defined words for display
int hola[4][8] = {{1,0,0,0,1,0,0,1},{1,1,0,0,0,0,0,0},{1,1,0,0,0,1,1,1},{1,0,0,0,1,0,0,0}};
int lets[4][8] = {{1,1,0,0,0,1,1,1},{1,0,0,0,0,1,1,0},{1,0,0,0,0,1,1,1},{1,0,0,1,0,0,1,0}};
int play[4][8] = {{1,0,0,0,1,1,0,0},{1,1,0,0,0,1,1,1},{1,0,0,0,1,0,0,0},{1,0,0,1,0,0,0,1}};
int yayy[4][8] = {{1,0,0,1,0,0,0,1},{1,0,0,0,1,0,0,0},{1,0,0,1,0,0,0,1},{1,0,0,1,0,0,0,1}};
int nope[4][8] = {{1,0,1,0,1,0,1,1},{1,0,1,0,0,0,1,1},{1,0,0,0,1,1,0,0},{1,0,0,0,0,1,1,0}};
int S[8] = {1,0,0,1,0,0,1,0};
int C[8] = {1,1,0,0,0,1,1,0};
int colon[8]= {1,1,1,1,1,1,0,0};

int digitPins[7] = {7,10,11,13,7,10,11}; //Copy first three again to allow for wrap around
int digitPinsWithColon[10] = {7,10,0,11,13,7,10,0,11,13}; //Copy first three again to allow for wrap around

//Speaker init

int speakerPin = 1;

int startMusicLength = 24; // the number of notes
int rightMusicLength = 5;
int playMusicLength = 10;
char startUpNotes[] = "g g gabC Cf cd a b C"; // a space represents a rest
char rightNotes[] = "g abC";
char letsPlayNotes[] = "CCggCCggCC";
//char notes[] = "c c c c c c c";
int beats[] = {1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,2,1,2,2,2,2,1,1,1,1,1,2,1,2,1,2,2,2,2,2};
//int beats[] = {5,5,5,5,5,5,5,5,5,5,5};
int tempo = 80;


void setup() {
  
  //Serial.begin(9600);
  
  //Initialize random number seed
  randomSeed(analogRead(0));
  
  //Clear register and set clk low
  Init_P2S_Shift_Reg();
  
  //Serial to Parallel pins for shift reg.
  pinMode(SER, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(RCK, OUTPUT);
  
  //Power pins for display
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(SPEAKER, OUTPUT);
 
  ClearDisplay();
  
  startTime = millis();
           for (int i = 0; i < startMusicLength; i++)
         {
            if (startUpNotes[i] == ' ') {
              delay(beats[i] * tempo); // rest
            } else {
              playNote(startUpNotes[i], beats[i] * tempo);
            }
        
            // pause between notes
            delay(tempo); 
          }
  //Display hello until user toggles a switch
//     while(userInput == 0)
//   {
//      HolaDisplay(holaCount);
//      userInput = ReadToggleSwitches(1);
//      if(holaCount < 6)
//      {
//        holaCount += 1;
//      }
//      
//   }
   ClearDisplay();
   while(userInput != 0)
   {
     
     userInput = ReadToggleSwitches(1);
     
   }
   //Then display "Lets Play"
   while(loopCount <= 2500)
   {
     loopCount += 1;
     Display(1); 
   }
    ClearDisplay();
   loopCount = 0;
   while(loopCount <= 2500)
   {
     loopCount += 1;
     Display(2); 
   }
   ClearDisplay();
        for (int i = 0; i < playMusicLength; i++)
         {
            if (letsPlayNotes[i] == ' ') {
              delay(beats[i] * tempo); // rest
            } else {
              playNote(letsPlayNotes[i], beats[i] * tempo);
            }
        
            // pause between notes
            delay(tempo / 2); 
          }
   
}

void loop() {

//Pick 2 random numbers 0-15
  randNum1 = SelectRandomNum1();
  randNum2 = SelectRandomNum2();

//Display the numbers on the 2 right-most displays
 clkDigit1 = 0;
 clkDigit2 = 0;// 9;
 solved = false;

 while(1)
 {
    
//    if((millis()-startTime) >= 1000)
//     {
//       Serial.println(millis()-startTime,DEC);
//       clkDigit2 -= 1;
//       startTime = millis();
//       //Serial.print(startTime);
//     }
     
   GameDisplay(randNum1, randNum2, upperNibble, lowerNibble);
   
   ReadToggleSwitches(0);
  

//   Serial.println(upperNibble,DEC);
//   Serial.println(lowerNibble,DEC);
//   delay(500);
   
  
   
   if(upperNibble == randNum1 && lowerNibble == randNum2 )
   {
     loopCount = 0;
     while(loopCount<1000)
     {
       
       GameDisplay(upperNibble, lowerNibble, randNum1, randNum2);
       loopCount += 1;
     }
     ClearDisplay();
     solved = true;
     break;
   }
//   else if(clkDigit2 == 0)
//   {
//     solved = false;
//     break;
//     
//   }
   
   
 }
  if(solved)
  { 
    loopCount = 0;
             for (int i = 0; i < rightMusicLength; i++)
         {
            if (rightNotes[i] == ' ') {
              delay(beats[i] * tempo); // rest
            } else {
              playNote(rightNotes[i], beats[i] * tempo);
            }
        
            // pause between notes
            delay(tempo / 2); 
          }
    while(loopCount <= 500)
    {
      Display(3);
      loopCount += 1;
    }
        

    ClearDisplay();

    ClearDisplay();
    userInput = ReadToggleSwitches(1);
    while(userInput != 0)
    {
      userInput = ReadToggleSwitches(1);
      
    }

  }
//  else
//  { 
//        loopCount = 0;
//    while(loopCount <= 1000)
//    {
//      Display(4);
//      loopCount += 1;
//    }
//    //display score
//    loopCount = 0;
//        while(loopCount <= 1000)
//    {
//      ScoreDisplay(scoreCount);
//      loopCount += 1;
//    }
//    
//    //reset score count to 0
//    scoreCount = 0;
//    clkDigit2 = 10;
//    //reset start time
//    
//  }

//if correct display 'yay'

//else display dang

//display score, reset to 0 if one was incorrect.
   
       //reset start time
    
}

void SetDisplay()
{
//  
//   for(int j=0; j<8; j++)
//      {
//        digitalWrite(SER, hex[i][j]);
//        //Shift out data
//        digitalWrite(SCLK, LOW);
//        digitalWrite(SCLK, HIGH);
//        
//      }
//      //Cycle register clk to output data
//        digitalWrite(RCK,LOW);
//        digitalWrite(RCK,HIGH);
  
}
//********************************************
// Serial to Parallel shift register (S2P) section
//********************************************
void HolaDisplay(byte holaCount)
{
  
  for(int i=0; i<4; i++)
  {

   for(int j=0; j<8; j++)
      {
        digitalWrite(SER, hola[i][j]);
        //Shift out data
        digitalWrite(SCLK, LOW);
        digitalWrite(SCLK, HIGH);
        
      }
       ClearDisplay(); 
      //Cycle register clk to output data
        digitalWrite(RCK,LOW);
        digitalWrite(RCK,HIGH);
    
    digitalWrite(digitPins[i], HIGH);    
    digitalWrite(digitPins[i+1], LOW);
    digitalWrite(digitPins[i+2], LOW);
    digitalWrite(digitPins[i+3], LOW);    
    switch(holaCount)
    {
      case 1: delay(750);
              break;
      case 2: delay(500);
              break;
      case 3: delay(250);
              break;
      case 4: delay(100);
              break;
      case 5: delay(10);
              break;
      default: holaCount = 0;
              //No more delay
               break;

    }
  }
  
}

void Display(byte num)
{
  
  for(int i=0; i<4; i++)
  {

   for(int j=0; j<8; j++)
      {
        if(num == 1)
        {
           digitalWrite(SER, lets[i][j]);
        }
        else if(num == 2)
        {
           digitalWrite(SER, play[i][j]);
        }
        else if(num == 3)
        {
          digitalWrite(SER, yayy[i][j]);
          
        }
         else if(num == 4)
        {
          digitalWrite(SER, nope[i][j]);
          
        }
        //Shift out data
        digitalWrite(SCLK, LOW);
        digitalWrite(SCLK, HIGH);
        
      }
       ClearDisplay(); 
      //Cycle register clk to output data
        digitalWrite(RCK,LOW);
        digitalWrite(RCK,HIGH);
    
    digitalWrite(digitPins[i], HIGH);    
    digitalWrite(digitPins[i+1], LOW);
    digitalWrite(digitPins[i+2], LOW);
    digitalWrite(digitPins[i+3], LOW);    

  }
  
}


void ClearDisplay()
{
    digitalWrite(D1, LOW);    
    digitalWrite(D2, LOW);
    digitalWrite(D3, LOW);
    digitalWrite(D4, LOW);   

}

//****************************************************************
//P2S section of code
//****************************************************************
void Init_P2S_Shift_Reg()
{
   //Declare Pin Directions
   pinMode(CLK, OUTPUT);
   pinMode(CLK_INH, OUTPUT);
   pinMode(CLR, OUTPUT);
   pinMode(QH, INPUT);
   pinMode(SH_LD, OUTPUT);
   
   //Clear register and set clk low
   digitalWrite(CLK_INH,LOW); 
   delay(10);
   digitalWrite(CLR, LOW);
   delayMicroseconds(10);
   digitalWrite(CLR,HIGH);  
   delayMicroseconds(10);
   digitalWrite(CLK, LOW);

}


byte ReadToggleSwitches(byte returnType)
{
  //Reset binaryInput to 0
  binaryInput = 0;
  upperNibble = 0;
  lowerNibble = 0;
  
  //Load Parallel inputs
  digitalWrite(SH_LD, LOW); 
  delayMicroseconds(10);
  digitalWrite(CLK, LOW);
  delayMicroseconds(10);
  digitalWrite(CLK, HIGH);
  
  //Input H appears at output on next rising_clk
  data[7] = digitalRead(QH);
  delayMicroseconds(10);
  if(data[7] == 1)
  {
    if(returnType == 1)
    {
      binaryInput += 1;
    }
    else
    {
      lowerNibble += 1;
    }
  }
  ///*Begin shifting rest of byte to output
  digitalWrite(SH_LD, HIGH); 
  delayMicroseconds(10);

     for(int i = 6; i>=0; i--)
     {
    
       digitalWrite(CLK,LOW);
       delayMicroseconds(10);
       digitalWrite(CLK,HIGH);
       delayMicroseconds(10);
       data[i] = digitalRead(QH);
         if(data[i] == 1)
         {
           if(returnType == 1)
           {
               switch(i)
               {
                 case 6: binaryInput += 2;
                         break;
                 case 5: binaryInput += 4;
                         break;
                 case 4: binaryInput += 8;
                         break;
                 case 3: binaryInput += 16;
                         break;
                 case 2: binaryInput += 32;
                         break;
                 case 1: binaryInput += 64;
                         break;
                 case 0: binaryInput += 128;
                         break;
               }
           }
           else
           {
              switch(i)
               {
                 case 6: lowerNibble += 2;
                         break;
                 case 5: lowerNibble += 4;
                         break;
                 case 4: lowerNibble += 8;
                         break;
                 case 3: upperNibble += 1;
                         break;
                 case 2: upperNibble += 2;
                         break;
                 case 1: upperNibble += 4;
                         break;
                 case 0: upperNibble += 8;
                         break;
               }
             
             
           }
         }
     }
     
     return binaryInput;
  
}

//*****************************************************
// Beginning of game code
//*****************************************************

byte SelectRandomNum1()
{
  //Generate random number 0-15
  byte randNum1 = random(16);
  return randNum1;
  
}
byte SelectRandomNum2()
{
  //Generate random number 0-15
  byte randNum2 = random(16);
  return randNum2;
  
}

void GameDisplay(byte clkDigit1, byte clkDigit2, byte randNum1, byte randNum2)
{
  //ClearDisplay();
  int num;
  for(int i=0; i<4; i++)
  {
    switch(i)
    {
      case 0: num = clkDigit1;
              break;
      case 1: num = clkDigit2;
              break;
      case 2: num = randNum1;
              break;
      case 3: num = randNum2;
              break;
    }
   for(int j=0; j<8; j++)
      {
        
        digitalWrite(SER, hex[num][j]);
       
        //Shift out data
        digitalWrite(SCLK, LOW);
        digitalWrite(SCLK, HIGH);
        
      }
       ClearDisplay(); 
      //Cycle register clk to output data
        digitalWrite(RCK,LOW);
        digitalWrite(RCK,HIGH);
    
    digitalWrite(digitPins[i], HIGH);    
    digitalWrite(digitPins[i+1], LOW);
    digitalWrite(digitPins[i+2], LOW);
    digitalWrite(digitPins[i+3], LOW);    

  }
  
}
void ScoreDisplay(int score)
{
  if(score == 0)
  {
    scoreLeftDigit = 0;
    scoreRightDigit = 0;
  }
  else if(score < 10)
  {
    scoreLeftDigit = 0;
    scoreRightDigit = score;
  }
  else
  {
    scoreLeftDigit = score / 10;
    scoreRightDigit = score % 10;
  }
    
    digitalWrite(digitPinsWithColon[0], LOW);    
    digitalWrite(digitPinsWithColon[1], LOW);
    digitalWrite(digitPinsWithColon[2], LOW);
    digitalWrite(digitPinsWithColon[3], LOW); 
    digitalWrite(digitPinsWithColon[4], LOW);
    
  for(int i=0; i<5; i++)
  {

   for(int j=0; j<8; j++)
      {
        switch(i)
        {
          case 0: digitalWrite(SER, S[j]);
                  break;
          case 1: digitalWrite(SER, C[j]);
                  break;
          case 2: digitalWrite(SER, colon[j]);
                  break;
          case 3: digitalWrite(SER, hex[scoreLeftDigit][j]);
                  break;
          case 4: digitalWrite(SER, hex[scoreRightDigit][j]);
                  break;
        }       
        //Shift out data
        digitalWrite(SCLK, LOW);
        digitalWrite(SCLK, HIGH);
        
      }
    digitalWrite(digitPinsWithColon[0], LOW);    
    digitalWrite(digitPinsWithColon[1], LOW);
    digitalWrite(digitPinsWithColon[2], LOW);
    digitalWrite(digitPinsWithColon[3], LOW); 
    digitalWrite(digitPinsWithColon[4], LOW);
      //Cycle register clk to output data
        digitalWrite(RCK,LOW);
        digitalWrite(RCK,HIGH);
    
    digitalWrite(digitPinsWithColon[i], HIGH);    
    digitalWrite(digitPinsWithColon[i+1], LOW);
    digitalWrite(digitPinsWithColon[i+2], LOW);
    digitalWrite(digitPinsWithColon[i+3], LOW); 
    digitalWrite(digitPinsWithColon[i+4], LOW);

  }
  
}
  //Speaker functions
  
  void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

  // play the tone corresponding to the note name
  for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }

  
}
void BinarySound(int note, int duration) {
  int number[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  int tones[] = {1900,1800,1700,1600,1500,1400,1300,1200,1100,1000,900,800,700,600,500};

  // play the tone corresponding to the note name
  for (int i = 0; i < 16; i++) {
    if (number[i] == note) {
      playTone(tones[i], duration);
    }
  }

  
}
