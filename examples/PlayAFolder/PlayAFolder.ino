/******************************************************************************
PlayAFolder.ino
Cytron EasyMP3 Shield Library example sketch
Created by Ng Beng Chet @ Cytron Technologies Sdn Bhd 
Original Creation Date: Mar 10, 2016
https://github.com/CytronTechnologies/CytronWiFiShield

Modified from DFPlayer_Mini_Mp3 Library examples
Credit to lisper <lisper.li@dfrobot.com> @ DFRobot
Original Creation Date: May 30, 2014
https://github.com/DFRobot/DFPlayer-Mini-mp3

!!! Description Here !!!

Development environment specifics:
	IDE: Arduino 1.6.7
	Hardware Platform: Arduino Uno or any other compatible boards
	Cytron EasyMP3 Shield Version: 1.0

Distributed as-is; no warranty is given.
******************************************************************************/
#include <CytronEZMP3.h>

CytronEZMP3 mp3;

void setup () {
  
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  
  if(!mp3.begin(2, 3))
  {
    Serial.println("Init failed");
    while(1);
  }
  mp3.setVolume(20);
  
  mp3.setErrMsg(blinking);  
  
  Serial.print("Device: ");
  Serial.println(mp3.getCurrentDevice()==1?"U-Disk":"microSD");

  Serial.print("Total files: ");
  Serial.println(mp3.getTotalFiles());

  //delay(1000);
  mp3.playTrackFromFolder(1, 5); //folder 1, track 5
  delay(20000);

  mp3.playFolderRepeat(3); // repeat playing folder 3
  
}

void loop () {
  //do something here
}

void blinking()
{
  Serial.println("Error!");
  Serial.print("Current status: ");
  Serial.println(mp3.getCurrentStatus());
  while(1)
  {
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);
    delay(1000);
  }
}

