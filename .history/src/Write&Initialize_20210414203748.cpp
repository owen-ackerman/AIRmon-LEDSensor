#include <Arduino.h>
#include "Adafruit_CCS811.h"
#include "SD.h"
#include "SPI.h"
#include <string.h>
#include <Time.h>    
#include <stdlib.h>     /* atoi */



String identifier;
String filename; 
//bool dataSave; //data save? yes or no
bool available; //if true, data from ccs sensor is avaliable
//uint8_t driveMode; //data taking frequency in hex number
//int eCO2Threshold; 
//int TVOCThreshold;
int8_t initialTime; 

Adafruit_CCS811 ccs; // initializes ccs sensor class

String getConfigAnswer(char* question);
uint8_t driveModeConversion(String self); // input driveMode string, output formated drivemode
bool StringToBool(String b); //string to bool conversion
void configTest();
void printFile();
void FileInitializer();
void setVariables();
String getData(); //data collection function, outputs data as dataString
void SDwrite(String dataString); //writes dataString to SD card
float currentTime(); //current timestamp into julian time

void setup(){
     Serial.begin(115200);
  while(!Serial){}
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(10)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  else {Serial.println("card initialized.");}
  //pinMode(SS, OUTPUT);
  
  if (!ccs.begin()){
    Serial.println("CCS811 Error");
  }
  ccs.setDriveMode(CCS811_DRIVE_MODE_60SEC);
//SD.remove("config.txt");
configTest();
setVariables();
printFile();
FileInitializer();

}

void loop(){
  
SDwrite(getData());
  
}

void configTest(){
    
    if (SD.exists("config.txt")){
        Serial.println("config.txt file exists");
        return;
    }
    else{
        SD.remove("config.txt");
    File dataFile = SD.open("config.txt", FILE_WRITE);
    if (dataFile){
        
      Serial.println("writing to empty config file");
      dataFile.println("filename = datata; 6 character limit");
      dataFile.println("identifier = SD; 2 character limit "); 
      dataFile.close();
      
        Serial.println("defaut config.txt file created");
        delay(500);
    }
    }
}

void setVariables(){
identifier = getConfigAnswer("identifier");
String file = getConfigAnswer("filename");
filename = identifier+file+".txt";

Serial.println(filename);
//dataSave = StringToBool(getConfigAnswer("datasavebool"));

//ccs.setDriveMode(driveModeConversion((getConfigAnswer("drivemode"))));

//eCO2Threshold = atoi(getConfigAnswer("eCO2 Threshold").c_str());
//TVOCThreshold = atoi(getConfigAnswer("TVOC Threshold").c_str());

}

void printFile(){

  File dataFile = SD.open("config.txt");

  // if the file is available, write to it:
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }  
}

void FileInitializer(){

  Serial.print(filename);
  Serial.println(" FileInitializer function");
  
  if (SD.exists((char*)filename.c_str())) {
    Serial.println("File exists :)");
    SD.remove((char*)filename.c_str());
     }

 File test = SD.open(filename.c_str(), FILE_WRITE);
  if (test){
    test.println("Identifier, Seconds, eCO2 ppm, TVOC ppb");
    Serial.println("Identifier, Seconds, eCO2 ppm, TVOC ppb");
    test.close();
    test == false;
  }
if (!SD.exists((char*)filename.c_str()))
  {
  Serial.print("File does not exist");
    }
  }

String getData() {
  String dataString;
  if(ccs.available()){
    
    //Serial.println("CCS AVAILABLE");
    float eCO2 = ccs.geteCO2(); //returns eCO2 reading
    float TVOC = ccs.getTVOC(); //return TVOC reading
     if(ccs.readData() == 0){
       available = true;
    for (static bool once = true; once; once = false){
        initialTime = currentTime();
        Serial.println(initialTime);
    }
    //if (dataSave){
    float time = currentTime();
    Serial.println(time);
    dataString += identifier;
    dataString += ", ";
    dataString += time - initialTime;
    dataString += ", ";
   // }
    dataString += String(eCO2);
    dataString += ", ";
    dataString += String(TVOC);
      }
     }
     else{ available = false;}
     
  return dataString;
  
}

void SDwrite(String dataString){
   // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
 //filename
 if (available){
  File dataFile = SD.open(filename.c_str(), FILE_WRITE);
  //Serial.println(filename);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening ");
    Serial.println(filename.c_str());
  }
 }
}

float currentTime() { 

time_t t = now();
/*

   float dy = day(t);
   float mth = month(t);
   float yr = year(t);
   */
   float hr = hour(t);
   float min = minute(t);
   float sec = second(t);


//implement the conversion from gregorian to jdn
/*
double a = (14 - mth)/12;
double y = yr + 4800 - a;
double m = mth + 12*a - 3;

double jdn = (dy + (153 * m + 2)/5 + 365*y + y/4 - y/100 + y/400 - 32045 - 0.5);
double julian  = ((hr-12)/24 + min/1440 + sec/86400);
*/
float julian = sec + 60*min + 3600*hr;

return julian;
}

String getConfigAnswer(char* question){

  char* config = (char*)"config.txt";
  File dataFile = SD.open(config, FILE_READ);
  // if the file is available, write to it:
  if (dataFile) {
    if(dataFile.available()) {
      int position = 0;
      dataFile.find(question);
      position = dataFile.position();
      dataFile.seek(position + 3);
      //file retrieving syntax below
      //filename = FILENAME.txt;
      //returns FILENAME.txt
      String data;
      data = dataFile.readStringUntil(';');
      Serial.println(data);
      dataFile.close();
      return data;
    }
    else{
      Serial.print("error reading ");
    Serial.print(question);
    Serial.print(" from: ");
    Serial.println(config);
    }
    
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.print(config);
    Serial.println(" could not be opened");
  } 
}

uint8_t driveModeConversion(String self){
  
  if(self == "CCS811_DRIVE_MODE_IDLE") return CCS811_DRIVE_MODE_IDLE;
  if (self == "CCS811_DRIVE_MODE_1SEC") return CCS811_DRIVE_MODE_1SEC;
  if (self == "CCS811_DRIVE_MODE_10SEC") return CCS811_DRIVE_MODE_10SEC;
  if (self == "CCS811_DRIVE_MODE_60SEC") return CCS811_DRIVE_MODE_60SEC;
  if (self == "CCS811_DRIVE_MODE_250MS") return CCS811_DRIVE_MODE_250MS;
  else{
    return 0;
  }
}

bool StringToBool(String b){
  if (b == "True") return true;
  if (b == "true") return true;
  if (b == "False") return false;
  if (b == "false") return false;
  else return true;
}
