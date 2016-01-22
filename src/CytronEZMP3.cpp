#include "CytronEZMP3.h"

//
CytronEZMP3::CytronEZMP3()
{  
}
//
bool CytronEZMP3::begin(uint8_t rxpin, uint8_t txpin,int32_t baudrate)
{
	_rxpin = rxpin;
	_txpin = txpin;
  
	if(_rxpin==0&&_txpin==1)
	{
		Serial.begin(baudrate);
		_serial = &Serial;
		isHardwareSerial = true;
	}
	else
	{
		swSerial = new SoftwareSerial(_rxpin,_txpin);
		swSerial->begin(baudrate);
		_serial = swSerial;
		isHardwareSerial = false;
	}
	delay(100);
	reset();
	return init();

}

//
void CytronEZMP3::fill_uint16_bigend (uint8_t *thebuf, uint16_t data) {
	*thebuf =	(uint8_t)(data>>8); // high byte
	*(thebuf+1) =	(uint8_t)data; // low byte
}

//calc checksum (1~6 byte)
uint16_t CytronEZMP3::mp3_get_checksum (uint8_t *thebuf) {
	uint16_t sum = 0;
	for (int i=1; i<7; i++) {
		sum += thebuf[i];
	}
	return -sum;
}

//fill checksum to send_buf (7~8 byte)
void CytronEZMP3::mp3_fill_checksum () {
	uint16_t checksum = mp3_get_checksum (send_buf);
	fill_uint16_bigend (send_buf+7, checksum);
}

//
void CytronEZMP3::mp3_send_cmd (uint8_t cmd, uint16_t arg) {
	
	send_buf[3] = cmd;
	send_buf[4] = _isReply;
	fill_uint16_bigend ((send_buf+5), arg);
	mp3_fill_checksum ();
	send_func ();
	delay(100);

}

//
void CytronEZMP3::mp3_send_cmd (uint8_t cmd) {
	mp3_send_cmd(cmd, 0);
}

void CytronEZMP3::send_func () {
	for (int i=0; i<10; i++) {
		write(send_buf[i]);
	}
}

bool CytronEZMP3::init ()
{	
	delay(1000);
	while(_serial->peek()!= 0x7e)
		_serial->read();
	int stat = readForResponses(0x3f, 0x40, 5000);
	
	if(stat <= 0) return false;

#if DEBUG1
	Serial.print("\r\nSystem Init successful!\r\n");
#endif
	
	_dev = recv_buf[_received - 4];
	
	switch(_dev)
	{
		case 1: 
		case 2: break;
		case 3: _dev = 2;break;
		default: 
		#if DEBUG1
			Serial.print("\r\nPC online\r\n");
		#endif
			_dev = _dev & 0x03;
			if(_dev==3) _dev = 2;
			break;
	}

	setDevice(_dev);
	if(_dev == 1) 
		delay(2000); //U-disk takes longer time to initialise file system
	setVolume(15);
//	get_state();
	delay(100);
	#if DEBUG1
		Serial.print("\r\nInit successful\r\n");
	#endif
		
	return true;
	
}

//
void CytronEZMP3::playPhysical (uint16_t num) {
	_isReply = 0;
	mp3_send_cmd (0x03, num);
}

//
void CytronEZMP3::playPhysical () {
	_isReply = 0;
	mp3_send_cmd (0x03);
}

//
void CytronEZMP3::next () {
	_isReply = 0;
	mp3_send_cmd (0x01);
}

//
void CytronEZMP3::prev () {
	_isReply = 0;
	mp3_send_cmd (0x02);
}

//0x06 set volume 0-30
void CytronEZMP3::setVolume (uint16_t volume) {
	_isReply = 0;
	mp3_send_cmd (0x06, volume);
}

//0x07 set EQ0/1/2/3/4/5    Normal/Pop/Rock/Jazz/Classic/Bass
void CytronEZMP3::setEQ (uint16_t eq) {
	_isReply = 0;
	mp3_send_cmd (0x07, eq);
}

//0x09 set device 1/2/3/4/5 U/SD/AUX/SLEEP/FLASH
void CytronEZMP3::setDevice (uint16_t device) {
	_isReply = 0;
	mp3_send_cmd (0x09, device);
}

//
void CytronEZMP3::sleep () {
	_isReply = 0;
	mp3_send_cmd (0x0a);
}

//
void CytronEZMP3::reset () {
	_isReply = 0;
	mp3_send_cmd (0x0c);
}

//
void CytronEZMP3::play () {
	_isReply = 0;
	mp3_send_cmd (0x0d);
}

//
void CytronEZMP3::pause () {
	_isReply = 0;
	mp3_send_cmd (0x0e);
}

void CytronEZMP3::stop () {
	_isReply = 0;
	mp3_send_cmd (0x16);
}

//
void CytronEZMP3::on(){
	_isReply = 0;
	mp3_DAC(true);
}

//
void CytronEZMP3::off(){
	_isReply = 0;
	mp3_DAC(false);
}

//play mp3 file in mp3 folder in your tf card
void CytronEZMP3::playTrack (uint16_t num) {
	_isReply = 0;
	mp3_send_cmd (0x12, num);
}

uint8_t CytronEZMP3::getCurrentDevice (){
	return _dev;
}

uint8_t CytronEZMP3::getCurrentStatus (){
	
	_isReply = 0;
	mp3_send_cmd (0x42);
	readForResponses(0x42, 0x40, 5000);
	_state = recv_buf[6];
	return _state;
}

//
uint16_t CytronEZMP3::getVolume () {
	
	_isReply = 0;
	mp3_send_cmd (0x43);
	readForResponses(0x43, 0x40, 5000);
	_vol = recv_buf[5]*256+recv_buf[6];
	return _vol;
}

uint16_t CytronEZMP3::getTotalFiles (uint8_t disk){
	
	_totalFiles = 0;
	if(!(_dev == 1||_dev == 2))
		return _totalFiles;
	
	switch(disk)
	{
		case 1: get_u_sum();break;
		case 2: get_tf_sum();break;
	}

	return _totalFiles;
}

uint16_t CytronEZMP3::getTotalFiles (){
	return getTotalFiles(_dev);
}

//
void CytronEZMP3::get_u_sum () {
	
	_isReply = 0;
	mp3_send_cmd (0x47);
	readForResponses(0x47, 0x40, 5000);
	_totalFiles = recv_buf[5]*256+recv_buf[6];
}

//
void CytronEZMP3::get_tf_sum () {
	_isReply = 0;
	mp3_send_cmd (0x48);
	readForResponses(0x48, 0x40, 5000);
	_totalFiles = recv_buf[5]*256+recv_buf[6];
}

//
//void CytronEZMP3::get_flash_sum () {
//	mp3_send_cmd (0x49);
//}
//
uint16_t CytronEZMP3::getTrackNo(uint8_t disk){
	
	if(!(_dev == 1||_dev == 2))
		return 0;
		
	switch(disk)
	{
		case 1: get_u_current();break;
		case 2: get_tf_current();break;
	}

	return _track;
}

uint16_t CytronEZMP3::getTrackNo(){
	return getTrackNo(_dev);
}

//
void CytronEZMP3::get_tf_current () {
	_isReply = 0;
	mp3_send_cmd (0x4c);
	readForResponses(0x4c, 0x40, 5000);
	_track = recv_buf[5]*256+recv_buf[6];
}

//
void CytronEZMP3::get_u_current () {
	_isReply = 0;
	mp3_send_cmd (0x4b);
	readForResponses(0x4b, 0x40, 5000);
	_track = recv_buf[5]*256+recv_buf[6];
}

//
//void CytronEZMP3::get_flash_current () {
//	mp3_send_cmd (0x4d);
//}
//

void CytronEZMP3::playAll(boolean state){
	mp3_send_cmd (0x11, state);
}

void CytronEZMP3::single_loop (boolean state) {
	mp3_send_cmd (0x19, !state);
}

//add 
void CytronEZMP3::repeatTrack (uint16_t num) {
	playTrack (num);
	delay (10);
	single_loop (true); 
	//mp3_send_cmd (0x19, !state);
}


//
void CytronEZMP3::mp3_DAC (boolean state) {
	mp3_send_cmd (0x1a, !state);
}

//
void CytronEZMP3::randomPlay () {
	mp3_send_cmd (0x18);
}

int CytronEZMP3::available()
{
	int available = _serial->available();
	if(available==0) delay(30);
	
	return _serial->available();
}

size_t CytronEZMP3::write(uint8_t c)
{
	return _serial->write(c);
}

int CytronEZMP3::read()
{
	return _serial->read();
}

int CytronEZMP3::peek()
{
	return _serial->peek();
}

void CytronEZMP3::flush()
{
	_serial->flush();
}

void CytronEZMP3::listen()
{
	if(!isHardwareSerial)
	{
		if(!swSerial->isListening())
			swSerial->listen();
	}
}

int CytronEZMP3::readForResponses(uint16_t cmd, uint16_t fail, unsigned int timeout)
{
	_received = 0; // received keeps track of number of chars read
	
	memset(recv_buf, '\0', EZMP3_RX_BUFFER_LEN);	
	
	_serial->setTimeout(1000);
	
	while(_serial->available()<=0) // in case there is really nothing coming back
	{
		if(timeout == 0) return -2;
		timeout--;delay(1);
	}
	
	if(_serial->available()) //wait until first byte is 0x7e
	{
		while(_serial->peek()!=0x7e)
			_serial->read();
	}
	
	_received = _serial->readBytes(recv_buf , 20);
	
#if DEBUG0
	Serial.write(recv_buf, _received);
#endif

#if DEBUG1
	//possible outcome:
	
	if(_received == 0) 
		Serial.print("\r\nno data\r\n");
	
	else if (_received < 10)
		Serial.print("\r\ndata missing\r\n");
	
	else if (_received > 20)
		Serial.print("\r\ndata overflow\r\n");
	
	else if (_received > 10)
		Serial.print("\r\nextra data\r\n");
	
	else
		Serial.print("\r\ngood data\r\n");
#endif
	
	if(_received > 6)
	{
		if(recv_buf[_received-1] == 0xef)
		{
			if(recv_buf[_received - 7] == cmd) return 1;
			else if(recv_buf[_received - 7] == fail) return 0;
		}
	}
			
	return -1;
}