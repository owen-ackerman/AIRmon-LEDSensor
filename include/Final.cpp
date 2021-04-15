#include <Arduino.h>
#include "Adafruit_CCS811.h"
#include "SD.h"
#include "SPI.h"
#include <stdio.h>
#include <string.h>
#include <Time.h>    
#include <stdlib.h>     /* atoi */

//CCS811 Sensor globals
Adafruit_CCS811 ccs; //library initializer

uint8_t driveModeConversion(String self); // input driveMode string, output formated drivemode
bool StringToBool(String b); //string to bool conversion
String getData(String dataString); //data collection function, outputs data as dataString
void SDwrite(String dataString); //writes dataString to SD card
void FileInitializer(); // checks if file can be opened and writes header
double currentTime(); //current timestamp into julian time
void configTest(); //tests config file, if none found, writes default config.txt file
String getConfigAnswer(char *question); //returns answer from question in config.txt file, format filename = data; where "filename" is question and "data" is answer

void printFile();

const int CS = 10; //chip select pin for SD SPI communication
File dataFile; //initializes file class
String header = "Identifier, Seconds, eCO2 ppm, TVOC ppb";  

String filename; //filename to save data
uint8_t driveMode;
bool dataSave; //data save? yes or no
bool available;
int eCO2Threshold;
int TVOCThreshold;
String identifier;
double initialTime;

void setup() {
  Serial.begin(115200);
  if (!ccs.begin()){
    Serial.println("CCS811 Error");
  }
  while(!Serial){}
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(CS)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  else {Serial.println("card initialized.");}
  pinMode(SS, OUTPUT);


//SD.remove("config.txt");
configTest();

identifier = getConfigAnswer("identifier");
String file = getConfigAnswer("filename");
filename = identifier + file + ".txt";

dataSave = StringToBool(getConfigAnswer("datasavebool"));
driveMode = driveModeConversion(getConfigAnswer("drivemode"));

eCO2Threshold = atoi(getConfigAnswer("eCO2 Threshold").c_str());
TVOCThreshold = atoi(getConfigAnswer("TVOC Threshold").c_str());
//SD.remove((char*)filename.c_str());
Serial.println(filename);

printFile();
FileInitializer();

}

void loop(){
String dataString;
dataString = getData(dataString);
if (dataSave){
  SDwrite(dataString);
  }
}

void configTest(){
    
    if (SD.exists("config.txt")){
        Serial.println("config exists");
        return;
    }
    else{
        SD.remove("config.txt");
    dataFile = SD.open("config.txt", FILE_WRITE);
    if (dataFile){
        
      Serial.println("writing to empty config file");
      dataFile.println("filename = data;");
      dataFile.println("drivemode = CCS811_DRIVE_MODE_1SEC;");
      dataFile.println("identifier = OA;"); 
      dataFile.println("TVOC Threshold = 100;");
      dataFile.println("eCO2 Threshold = 800;");
      dataFile.println("datasavebool = true;");
      
      dataFile.println("fileEnumerator = 0;"); 
      dataFile.close();
      
        Serial.println("defaut config.txt file created");
        delay(500);
    }
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
    test.println(header);
    Serial.println(header);
    test.close();
    test == false;
  }
if (!SD.exists((char*)filename.c_str()))
  {
  Serial.print("File does not exist");
    }
  }

String getConfigAnswer(char* question){

  dataFile = SD.open("config.txt", FILE_READ);
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
      dataFile.close();
      return data;
    }
    else{
      Serial.print("error reading ");
    Serial.print(question);
    Serial.println(" from: config.txt");

    }
    
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("config.txt could not be opened");
  } 
dataFile.close();
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

String getData(String dataString) {
  
  if(ccs.available()){
    //Serial.println("CCS AVAILABLE");
    int eCO2 = ccs.geteCO2(); //returns eCO2 reading
    int TVOC = ccs.getTVOC(); //return TVOC reading
     if(!ccs.readData()){
       available = true;
    for (static bool once = true; once; once = false){
        initialTime = currentTime();
    }
    if (dataSave){
    double time = currentTime();
    
    dataString += identifier;
    dataString += ", ";
    dataString += time - initialTime;
    dataString += ", ";
    }
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
  dataFile = SD.open(filename.c_str(), FILE_WRITE);
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

double currentTime() { 

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
double julian = sec + 60*min + 3600*hr;

return julian;
}

void printFile(){
     
  dataFile = SD.open(filename.c_str());

  // if the file is available, write to it:
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }  
}
