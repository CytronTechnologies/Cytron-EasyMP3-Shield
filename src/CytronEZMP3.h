/******************************************************************************
CytronEZMP3.h
Cytron EasyMP3 Shield Library header File
Created by Ng Beng Chet @ Cytron Technologies Sdn Bhd 
Original Creation Date: Mar 10, 2016
https://github.com/CytronTechnologies/Cytron-EasyMP3-Shield/

Modified from DFPlayer_Mini_Mp3 Library Main Source File
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

#ifndef _EZMP3_H
#define _EZMP3_H

#include "Arduino.h"
#include "Stream.h"
#include <SoftwareSerial.h>

#define EZMP3_RX_BUFFER_LEN 20
#define PIN_BUSY A2
#define DEBUG0 0
#define DEBUG1 0

// 7E FF 06 0F 00 01 01 xx xx EF
// 0	->	7E is start code
// 1	->	FF is version
// 2	->	06 is length
// 3	->	0F is command
// 4	->	00 is no receive
// 5~6	->	01 01 is argument
// 7~8	->	checksum = 0 - ( FF+06+0F+00+01+01 )
// 9	->	EF is end code

typedef enum mp3_error_type{
	BUSY=1,
	SLEEPING,
	SERIAL_WRONG_STACK,
	CHECKSUM_ERR,
	FILE_OUT_OF_INDEX,
	FILE_NOT_FOUND,
	DATA_MISMATCH=8,
};

class CytronEZMP3: public Stream
{
  public:
	CytronEZMP3();
	~CytronEZMP3();
	bool begin(uint8_t rxpin = 2, uint8_t txpin = 3, long baudrate = 9600);
	bool begin(HardwareSerial &_hSerial, long baudrate = 9600);
	void setFolderOption (bool opt);
	bool isPlaying() {return (!digitalRead(PIN_BUSY));}
	void playPhysical (uint16_t num); 
	void playPhysical (); 
	void play();
	void next (); 
	void prev (); 
	void setVolume (uint16_t volume); //0x06 set volume 0-30
	void setEQ (uint16_t eq); //0x07 set EQ0/1/2/3/4/5    Normal/Pop/Rock/Jazz/Classic/Bass
	void setDevice (uint16_t device); //0x09 set device 1/2/3/4/5 U/SD/AUX/SLEEP/FLASH
	void sleep (); 
	void reset (); 
	void pause (); 
	void stop (); 
	void on();
	void off();
	void playAll (boolean state = true); 
	void playTrack (uint16_t num); //specify a mp3 file in mp3 folder in your tf card, "mp3_play (1);" mean play "mp3/0001.mp3"
	uint8_t getCurrentDevice ();
	uint8_t getCurrentStatus ();
	uint16_t getVolume (); 
	uint16_t getTotalFiles (uint8_t disk);
	uint16_t getTotalFiles ();
	uint16_t getTrackNo(uint8_t disk);
	uint16_t getTrackNo();
	void playTrackFromFolder(uint8_t folder, uint8_t track);
	void playFolderRepeat(uint8_t folder);
	void repeatTrack (uint16_t num); 
	void randomPlay (); 
	void setErrorCallback(void (*func)());
	uint8_t errorCode();
	
	int available();
	size_t write(uint8_t);
	int read();
	int peek();
	void flush();
	void listen();
	
  protected:
	Stream * _serial;
	
  private:
	uint8_t _rxpin;
	uint8_t _txpin;
	SoftwareSerial *swSerial;
	bool isHardwareSerial;
	bool _isReply;
	int _received;
	uint8_t _dev;
	uint16_t _totalFiles;
	uint16_t _track;
	uint16_t _vol;
	uint8_t _state;
	bool isManyFolder;
        uint8_t _errorCode;
	void (*errMsg)() = NULL;
	
	uint8_t send_buf[10] = {0x7E, 0xFF, 06, 00, 00, 00, 00, 00, 00, 0xEF};
	uint8_t recv_buf[EZMP3_RX_BUFFER_LEN];
	void mp3_send_cmd (uint8_t cmd, uint16_t arg);
	void mp3_send_cmd (uint8_t cmd);
	uint16_t mp3_get_checksum (uint8_t *thebuf); 
	void mp3_fill_checksum ();
	void fill_uint16_bigend (uint8_t *thebuf, uint16_t data);
	void send_func();
	void playManyTracksFromSingleFolder(uint8_t folder, uint16_t track);
	int readForResponses(uint16_t cmd, uint16_t fail, unsigned int timeout, bool returnVal=false);
	
	bool init();
	void mp3_DAC (boolean state);
	void get_state ();
	void get_u_sum (); 
	void get_tf_sum (); 
	//void mp3_get_flash_sum ();
	void get_tf_current (); 
	void get_u_current ();
	//void mp3_get_flash_current ();	
	void single_loop (boolean state); //set single loop 
	uint8_t retVal[2];
	int timedRead();
  	uint16_t dev_delay;
};


#endif

