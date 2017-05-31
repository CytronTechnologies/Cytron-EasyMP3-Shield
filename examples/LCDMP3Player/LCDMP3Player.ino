#include <CytronEZMP3.h>
#include <LiquidCrystal.h> 

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
CytronEZMP3 mp3;

byte vol = 15;  
int i = 0;
int track = 0; 
bool get_track_no = true;

//right, up, down, left, select
#define RightKey 0
#define UpKey 1
#define DownKey 2
#define LeftKey 3
#define SelectKey 4
#define NUM_KEYS 5
#define LED_PIN 13

unsigned int adc_key_val[5] ={50, 250, 400, 500, 800 };
byte key=-1;
byte oldkey=-1;
long timeout = 0;
long timeForDisp = 0;
boolean disp_flag = true;
boolean gotError = false;

// Convert ADC value to key number
byte get_key(unsigned int input)
{
  for (byte k = 0; k < NUM_KEYS; k++)
  {
    if (input < adc_key_val[k])
    {
      return k;
    }
  }
  
  return -1;
}

void Disp_vol(byte val)
{
  lcd.clear();lcd.home();
  lcd.print("Vol: "); 
  lcd.print(val); 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.clear();
  pinMode(LED_PIN, OUTPUT);
  lcd.print("CYTRON TECH.");
  lcd.setCursor(0, 1);
  lcd.print("MP3 Shield");

  // set callback function before call begin()
  mp3.setErrorCallback(errorCb);
  
  if(!mp3.begin(2,3))
  {
    lcd.clear();
    lcd.print("Init failed");
    while(1);
  }
  
  mp3.setVolume(vol);

  delay(1000);  
  lcd.clear();lcd.home();
  lcd.print("Device: ");
  lcd.setCursor(0,1);
  lcd.print(mp3.getCurrentDevice()==1?"U-Disk":"microSD");
  delay(2000);

  lcd.clear();lcd.home();
  lcd.print("Total files: ");
  lcd.setCursor(0,1);
  lcd.print(mp3.getTotalFiles());
  delay(2000);

  lcd.clear();lcd.home();
  mp3.playTrack(2);
  timeout = millis();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(get_track_no)
  {
    track = mp3.getTrackNo();
    get_track_no = false;
  }

  if(!mp3.isPlaying())
    playNext();
  
  if(disp_flag)
  {
    lcd.setCursor(0,0);lcd.print("Now Playing...  ");
    scrollToLeft(i,1,"Track "+(String)track);
  }
  
  key = get_key(analogRead(A0));
  
  //Play Next Song when SelectKey pressed
  if(key==SelectKey&&oldkey!=SelectKey)  
    playNext();// skip to next song
  
  //Adjust Volume Up
  else if(key==UpKey)
  {
    disp_flag = false;
    timeForDisp = millis();
    if(vol<30)
      vol++; 
      
     mp3.setVolume(vol); 
     Disp_vol(vol); 
     delay(200);      
  }
  
  //Adjust Volume Down
  else if(key==DownKey)
  {
     disp_flag = false;
     timeForDisp = millis();
     
     if(vol>0)
      vol--; 
      
     mp3.setVolume(vol); 
     Disp_vol(vol);
     delay(200);  
  }
  
  //Mute the Player
  else if(key==RightKey&&oldkey!=RightKey)
  { 
    
    lcd.clear();lcd.home();
    if(mp3.getVolume()==0)
    {
      lcd.print("Unmute"); 
      mp3.setVolume(vol);
    }
    else
    {
      lcd.print("Mute");
      mp3.setVolume(0);
    }
    disp_flag = false;
    timeForDisp = millis();
    //delay(200);
  }
  
  //Pause the Player
  else if(key==LeftKey&&oldkey!=LeftKey)
  {
    lcd.clear();lcd.home();
    uint8_t _stat= mp3.getCurrentStatus(); 
    if(_stat==0x02)
    {
      mp3.play();
      lcd.print("Resume");
    }
    else
    {
      mp3.pause();
      lcd.print("Pause");
    }
    disp_flag = false;
    timeForDisp = millis();
    //delay(200);
  }

  oldkey = key;
  
  if(millis()-timeout>1000)
  {
    i++;timeout = millis();
    digitalWrite(LED_PIN, i%2);
  }

  if(millis()-timeForDisp>2000)
    disp_flag = true;
}

void scrollToLeft(int index,int line,String dat)
{
  lcd.setCursor(0,line);
  dat = dat + "            ";
  index = index%dat.length();
  if(index+16>dat.length())
  {
    lcd.print(dat.substring(index,dat.length())
                  +dat.substring(0,index+16-dat.length()));
    }
  else
    lcd.print(dat.substring(index,index+16));
}


void playNext()
{
  //Serial.println("Playing next song...");
  mp3.next();
  get_track_no = true;
}

void playPrev()
{
  //Serial.println("Playing previous song...");
  delay(2000); // 2 sec is needed to initialise next song and play it
  mp3.prev();
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

