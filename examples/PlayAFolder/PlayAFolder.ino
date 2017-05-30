/******************************************************************************
PlayAFolder.ino
Cytron EasyMP3 Shield Library example sketch
Created by Ng Beng Chet @ Cytron Technologies Sdn Bhd 
Original Creation Date: Mar 10, 2016
https://github.com/CytronTechnologies/Cytron-EasyMP3-Shield

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

  // set callback function before call begin()
  mp3.setErrorCallback(errorCb);

  if (!mp3.begin(2, 3))
  {
    Serial.println("Initialisation failed");
    while (1);
  }
  mp3.setVolume(20);

  Serial.print("Device: ");
  Serial.println(mp3.getCurrentDevice() == 1 ? "U-Disk" : "microSD");

  Serial.print("Total files: ");
  Serial.println(mp3.getTotalFiles());

  // create folders named "01" to "03" in SD card/U-disk root directory
  // then place songs in each folder
  // the song name format is similar to the songs in folder "mp3"

  mp3.playTrackFromFolder(1, 3); //folder 1, track 3
  delay(5000); // play the song for 5 seconds

  mp3.playFolderRepeat(3); // repeat playing folder 3

}

void loop () {
  //do something here
}

void errorCb()
{
  Serial.print(F("[ERROR] "));
  switch (mp3.errorCode()) {
    case BUSY:
      Serial.println(F("Busy"));
      break;
    case SLEEPING:
      Serial.println(F("In sleep mode"));
      break;
    case SERIAL_WRONG_STACK:
      Serial.println(F("Serial wrong stack"));
      break;
    case CHECKSUM_ERR:
      Serial.println(F("Checksum error"));
      break;
    case FILE_OUT_OF_INDEX:
      Serial.println(F("File number is out of index"));
      break;
    case FILE_NOT_FOUND:
      Serial.println(F("File not found"));
      break;
    case DATA_MISMATCH:
      Serial.println(F("Data mismatch"));
      break;
    default:
      Serial.println(F("Unknown error"));

  }
  while (1)
  {
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);
    delay(1000);
  }
}
