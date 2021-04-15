#include <Arduino.h>
#include "Adafruit_CCS811.h"
#include "SD.h"
#include "SPI.h"
#include <stdio.h>
#include <string.h>
#include <Time.h>    
#include <stdlib.h>     /* atoi */


const int buzzerPin = 9;
int frequency = 30;
int Hz = 800;
char* Message = "your mom has fat tits i repeat ";

String charToMorse(char16_t letter);
void arrayToMorse(char* input);
void buzz(char* morse);

void setup(){
    Serial.begin(115200);
    while(!Serial){}
    noTone(buzzerPin);

    for(int i = 3; i>0; i--){
        Serial.println(i);
       // tone(buzzerPin, Hz, 500);
         //       delay(500);
        //noTone(buzzerPin);
                delay(500);

    }
  //arrayToMorse(Message);
}

void loop(){
 arrayToMorse(Message);
 //delay(1000);
}
void arrayToMorse(char* input){
    String buf[strlen(input)];
 for (unsigned i = 0; i < strlen(input); i++){
     
    buf[i] = charToMorse(input[i]);
 }
 for (unsigned i = 0; i < strlen(input); i++)
 {
    buzz((char*)buf[i].c_str());
    delay(frequency*3);
 }

}

void buzz(char* morse){
    //Serial.println(strlen(morse));
    for(unsigned i = 0; i < strlen(morse); i++){
        
        switch(morse[i])
        {
            case '.':
            tone(buzzerPin, Hz, frequency);
            delay(frequency);
            //noTone(buzzerPin);
            //Serial.println(morse[i]);
            break;
            case '-':
            tone(buzzerPin, Hz, frequency*3);
            delay(frequency*3);
            //noTone(buzzerPin);
            //Serial.println(morse[i]);
            break;
            case ' ':
            //Serial.println(morse[i]);
            delay(frequency*3);
            break;

        }
        Serial.print("between beep single delay");
        delay(frequency);
    }
}


String charToMorse(char16_t letter)
{
    switch(letter)
    {
        case 'a': case 'A':
        return ".-"; break;
        case 'b': case 'B':
        return "-..."; break;
        case 'c': case 'C':
        return "-.-."; break;
        case 'd': case 'D':
        return "-.."; break;
        case 'e': case 'E':
        return "."; break;
        case 'f': case 'F':
        return "..-."; break;
        case 'g': case 'G':
        return "--."; break;
        case 'h': case 'H':
        return "...."; break;
        case 'i': case 'I':
        return ".."; break;
        case 'j': case 'J':
        return ".---"; break;
        case 'k': case 'K':
        return "-.-"; break;
        case 'l': case 'L':
        return ".-.."; break;
        case 'm': case 'M':
        return "--"; break;
        case 'n': case 'N':
        return "-."; break;
        case 'o': case 'O':
        return "---"; break;
        case 'p': case 'P':
        return ".--."; break;
        case 'q': case 'Q':
        return "--.-"; break;
        case 'r': case 'R':
        return ".-."; break;
        case 's': case 'S':
        return "..."; break;
        case 't': case 'T':
        return "-"; break;
        case 'u': case 'U':
        return "..-"; break;
        case 'v': case 'V':
        return "...-"; break;
        case 'w': case 'W':
        return ".--"; break;
        case 'x': case 'X':
        return "-..-"; break;
        case 'y': case 'Y':
        return "-.--"; break;
        case 'z': case 'Z':
        return "--.."; break;
        case ' ':
        return " "; break;
    }
}
