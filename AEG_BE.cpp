#include "AEG_BE.h"

/**
	AEG BE LCD Library
	@author Luca Zimmermann
	@version 1.1 18.12.2017
*/

/**
	\brief Initializes a \c AEG_BE object
	@param panels Number of daisychained panels
	@param latch  Latch Pin on Arduino
	@param enable Enable pin on Arduino
*/
AEG_BE10::AEG_BE10(
	uint8_t panels, 
	uint8_t latch,
	uint8_t enable) :
	AEG_BE(
		panels,
		latch,
		enable,
		BE10_PANELWIDTH,
		BE10_REGISTERS
	) {}

/**
	\brief Initializes a \c AEG_BE object
	@param panels Number of daisychained panels
	@param latch  Latch Pin on Arduino
	@param enable Enable pin on Arduino
*/
AEG_BE11::AEG_BE11(
	uint8_t panels, 
	uint8_t latch,
	uint8_t enable) :
	AEG_BE(
		panels,
		latch,
		enable,
		BE11_PANELWIDTH,
		BE11_REGISTERS
	) {}

/**
	\brief Initializes pins, buffer and other functions
	
	Constructor sets pin 3 to output 122 Hz for LCD clock, and latch/enable as outputs.
	MOSI and SCK are used for fast SPI transmission to the shift registers at 1 MHz.
	
	@param panels     Number of daisychained panels
	@param latch      Latch Pin on Arduino
	@param enable     Enable pin on Arduino
	@param panelwidth Pixels in width on a single panel
	@param registers  Number of 40 bit shift registers in one row (top/bottom half)
*/
AEG_BE::AEG_BE(
	uint8_t panels, 
	uint8_t latch,
	uint8_t enable,
	uint8_t panelwidth,
	uint8_t registers) :
	Adafruit_GFX(panelwidth * panels, 24),
	_panels(panels),
	_latch(latch),
	_enable(enable),
	_panelwidth(panelwidth) {
	// 61 Hz 1:1 duty cycle on D3
	// D3 = Output
	DDRD   |= (1 << DDD3);
	// Prescaler = 1024
	TCCR2B |= (1 << CS20) | (1 << CS21) | (1 << CS22);
	// Fast PWM and Phase Correct
	TCCR2A |= (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
	// 50% Duty cycle
	OCR2B = 127;
	pinMode(_latch, OUTPUT);
	pinMode(_enable, OUTPUT);
	digitalWrite(_latch, 0);
	_bytes = panels * registers * 10;
	_blanks = registers * 3 - panelwidth;
	_imageBuffer = (uint8_t*) calloc(_bytes, sizeof(uint8_t));
	SPI.begin();
}

/**
	Fills the buffer the fastest way without touching invalid bits
	@param color WHITE or BLACK
*/
void AEG_BE::fillScreen(uint16_t color) {
	if (color) {
		for (uint8_t i = 0; i < _bytes; i++) {
			switch (i % 5) {
				case 0:
					_imageBuffer[i] = 0xFC;
					break;
				case 4:
					_imageBuffer[i] = 0x3F;
					break;
				default:
					_imageBuffer[i] = 0xFF;
			}
		}
	} else {
		for (uint8_t i = 0; i < _bytes; _imageBuffer[i++] = 0);
	}
}

/**
	Enables output of values in registers
*/
void AEG_BE::enable(void) {
	digitalWrite(_enable, 0);
}

/**
	Disables output of values in registers
*/
void AEG_BE::disable(void) {
	digitalWrite(_enable, 0);
}

/**
	Draws pixel to buffer on corresponding position
	@param x     Position on x axis, starting with 0=left
	@param y     Position on y axis, starting with 0=top
	@param color WHITE or BLACK
*/
void AEG_BE::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if (x < 0 || y < 0 || x >= width() || y >= height()) return;
	uint16_t pos = 2;
	if (y < 12) {
		pos += (x + x / _panelwidth * _blanks) * 12 + x / 3 * 4;
		if (y < 6)
			pos += 6 + y;
		else
			pos += 11 - y;
	} else {
		pos += (_bytes << 2) + (width() - 1 - x - x / 29) * 12 + (width() - 1 - x) / 3 * 4;
		if (y < 18)
			pos += y - 12;
		else
			pos += 29 - y;
	}
	if (color)
		_imageBuffer[pos >> 3] |= 1 << (pos & 7);
	else
		_imageBuffer[pos >> 3] &= 0xFF ^ (1 << (pos & 7));
}

/**
	Outputs buffer to display at 1 MHz
*/
void AEG_BE::display(void) {
	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
	digitalWrite(_latch, 0);
	for (uint8_t i = _bytes - 1; i < _bytes; i--) {
		SPI.transfer(_imageBuffer[i]);
	}
	digitalWrite(_latch, 1);
	SPI.endTransaction();
}
