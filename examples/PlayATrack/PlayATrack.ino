/*******************************************************************************
 * DFPlayer_Mini_Mp3, This library provides a quite complete function for      * 
 * DFPlayer mini mp3 module.                                                   *
 * www.github.com/dfrobot/DFPlayer_Mini_Mp3 (github as default source provider)*
 *  DFRobot-A great source for opensource hardware and robot.                  *
 *                                                                             *
 * This file is part of the DFplayer_Mini_Mp3 library.                         *
 *                                                                             *
 * DFPlayer_Mini_Mp3 is free software: you can redistribute it and/or          *
 * modify it under the terms of the GNU Lesser General Public License as       *
 * published by the Free Software Foundation, either version 3 of              *
 * the License, or any later version.                                          *
 *                                                                             *
 * DFPlayer_Mini_Mp3 is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * DFPlayer_Mini_Mp3 is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public            *
 * License along with DFPlayer_Mini_Mp3. If not, see                           *
 * <http://www.gnu.org/licenses/>.                                             *
 *									       *
 ******************************************************************************/

/*
 *	Copyright:	DFRobot
 *	name:		DFPlayer_Mini_Mp3 sample code
 *	Author:		lisper <lisper.li@dfrobot.com>
 *	Date:		2014-05-30
 *	Description:	connect DFPlayer Mini by SoftwareSerial, this code is test on Uno
 *			Note: the mp3 files must put into mp3 folder in your tf card 
 */
#include <CytronEZMP3.h>

CytronEZMP3 mp3;

void setup () {
	Serial.begin(9600);
  if(!mp3.begin(2,3))
  {
    //Serial.println("Init failed");
    while(1);
  }
  mp3.setVolume(25);
  Serial.print("Device: ");
  Serial.println(mp3.getCurrentDevice()==1?"U-Disk":"microSD");

  Serial.print("Total files: ");
  Serial.println(mp3.getTotalFiles());
}

//
void loop () {        
	mp3.playTrack(1);
  Serial.println("Now playing track " + (String)mp3.getTrackNo());
	delay (6000);
	mp3.next();
  Serial.println("Now playing track " + (String)mp3.getTrackNo());
	delay (6000);
	mp3.prev();
  Serial.println("Now playing track " + (String)mp3.getTrackNo());
	delay (6000);
	mp3.playTrack(2);
  Serial.println("Now playing track " + (String)mp3.getTrackNo());
	delay (6000);
}

/*
   mp3_play ();		//start play
   mp3_play (5);	//play "mp3/0005.mp3"
   mp3_next ();		//play next 
   mp3_prev ();		//play previous
   mp3_set_volume (uint16_t volume);	//0~30
   mp3_set_EQ ();	//0~5
   mp3_pause ();
   mp3_stop ();
   void mp3_get_state (); 	//send get state command
   void mp3_get_volume (); 
   void mp3_get_u_sum (); 
   void mp3_get_tf_sum (); 
   void mp3_get_flash_sum (); 
   void mp3_get_tf_current (); 
   void mp3_get_u_current (); 
   void mp3_get_flash_current (); 
   void mp3_single_loop (boolean state);	//set single loop 
   void mp3_DAC (boolean state); 
   void mp3_random_play (); 
 */

