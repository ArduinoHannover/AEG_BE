#include "AEG_BE.h"

/**
	AEG BE LCD Library
	@author Luca Zimmermann
	@version 1.1.1 20.12.2017
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
	_panelwidth(panelwidth),
	_registers(registers) {
	pinMode(_latch, OUTPUT);
	pinMode(_enable, OUTPUT);
	digitalWrite(_latch, 0);
	_bytes = panels * _registers * 10;
	_blanks = _registers * 3 - panelwidth;
	_imageBuffer = (uint8_t*) calloc(_bytes, sizeof(uint8_t));
	SPI.begin();
}

#ifdef __AVR
/**
	En-/Disables 61 Hz LCD Clock on D3 (AVR only, disabled by default)
	@param en En-/Disable
*/
void AEG_BE::enableClock(boolean en) {
	if (en) {
		// 61 Hz 1:1 duty cycle on D3
		// D3 = Output
		DDRD   |= (1 << DDD3);
		// Prescaler = 1024
		TCCR2B |= (1 << CS20) | (1 << CS21) | (1 << CS22);
		// Fast PWM and Phase Correct
		TCCR2A |= (1 << COM2B1) | (1 << WGM20) | (1 << WGM21);
		// 50% Duty cycle
		OCR2B = 127;
	} else {
		TCCR2A = 0;
	}
}
#endif

/**
	Fills the buffer the fastest way without touching invalid bits
	@param color WHITE or BLACK
*/
void AEG_BE::fillScreen(uint16_t color) {
	if (color) {
		for (uint16_t i = 0; i < _bytes; i++) {
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
		for (uint16_t i = 0; i < _bytes; _imageBuffer[i++] = 0);
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
	uint16_t t;
	switch(getRotation()) {
		case 1:
			t = x;
			x = width()  - y - 1;
			y = t;
			break;
		case 2:
			x = width()  - x - 1;
			y = height() - y - 1;
			break;
		case 3:
			t = x;
			x = y;
			y = height() - t - 1;
			break;
	}
	uint32_t pos = 0;
	// Invert values for lower half and add 1/2 size
	if (y >= 12) {
		y = 23 - y;
		x = width() - 1 - x;
		pos = _bytes << 2;
	}
	// Determine position in daisychain and add preceeding two blank bits
	pos += (x / _panelwidth) * _registers * 40 + 2;
	// Do panel-local calculations
	x %= _panelwidth;
	// Add 4 blank bits at each end of an 40 bit register
	pos += (x / 3) * 4 + x * 12;
	if (y < 6)
		pos += 6 + y;
	else
		pos += 11 - y;
	if (color)
		_imageBuffer[pos >> 3] |= 1 << (pos & 7);
	else
		_imageBuffer[pos >> 3] &= 0xFF ^ (1 << (pos & 7));
}

/**
	Outputs buffer to display at 1 MHz
*/
void AEG_BE::display(void) {
	SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
	digitalWrite(_latch, 0);
	for (uint16_t i = _bytes; i > 0; i--) {
		SPI.transfer(_imageBuffer[i - 1]);
		yield();
#ifdef DEBUG
		for (uint8_t f = 0; f < 8; f++) {
			Serial.write(((_imageBuffer[i - 1] >> (7 - f)) & 1) ? '*' : '.');
		}
		if (i % 5 == 0) Serial.write('\n');
		if (i % 50 == 0) Serial.write('\n');
#endif //DEBUG
		
	}
	digitalWrite(_latch, 1);
	SPI.endTransaction();
}
