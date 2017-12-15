#include "AEG_BE10.h"

AEG_BE10::AEG_BE10(
	uint8_t panels, 
	uint8_t latch,
	uint8_t enable) :
	Adafruit_GFX(29 * panels, 24),
	_panels(panels),
	_latch(latch),
	_enable(enable) {
	TCCR2B = (TCCR2B & 0b11111000) | 6;
	pinMode(3, OUTPUT);
	analogWrite(3, 127);
	pinMode(_latch, OUTPUT);
	pinMode(_enable, OUTPUT);
	digitalWrite(_latch, LOW);
	_bytes = panels * 100;
	_imageBuffer = calloc(_bytes, sizeof(uint8_t));
	SPI.begin();
}

void AEG_BE10::fillScreen(uint16_t color) {
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

void AEG_BE10::drawPixel(int16_t x, int16_t y, uint16_t color) {
	if (x < 0 || y < 0 || x >= width() || y >= height()) return;
	uint16_t pos = 2;
	if (y < 12) {
		pos += (x + x / 29) * 12 + x / 3 * 4;
		if (y < 6)
			pos += 6 + y;
		else
			pos += 11 - y;
	} else {
		pos += (_bits << 2) + (width() - 1 - x - x / 29) * 12 + (width() - 1 - x) / 3 * 4;
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

void AEG_BE10::display(void) {
	uint16_t pos;
	uint8_t x, y;
	SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
	digitalWrite(_latch, 0);
	for (uint8_t i = _bytes - 1; i < _bytes; i--) {
		SPI.transfer(_imageBuffer[i]);
	}
	digitalWrite(_latch, 1);
	SPI.endTransaction();
}
