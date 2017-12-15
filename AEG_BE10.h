#ifndef AEG_BE10_H
#define AEG_BE10_H

#include <Adafruit_GFX.h>
#include <SPI.h>

#define WHITE 1
#define BLACK 0

class AEG_BE10 : public Adafruit_GFX {
	private:
		uint8_t
			_panels,
			_latch,
			_enable;
		uint16_t _bits;
		uint8_t* _imageBuffer;
		
	public:
		AEG_BE10(uint8_t, uint8_t, uint8_t);
		void drawPixel(int16_t, int16_t, uint16_t);
		void fillScreen(uint16_t);
		void display(void);
};

#endif //AEG_BE10_H