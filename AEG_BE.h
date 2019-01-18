#ifndef AEG_BE_H
#define AEG_BE_H

#include <Adafruit_GFX.h>
#include <SPI.h>
/**
	\def WHITE
	Clear pixel
*/
#define WHITE 1
/**
	\def BLACK
	Opaque pixel
*/
#define BLACK 0

#define BE10_REGISTERS  10
#define BE10_PANELWIDTH 29
#define BE11_REGISTERS  13
#define BE11_PANELWIDTH 39


/**
	\class AEG_BE
	\brief Display driver for various LCD produced by AEG (BMG|MIS)
	
	This driver supports different types of LCDs with 40 bit shift registers (COG).
	Currently officially supported are BE10 (29x24px) and BE11 (39x24px).
	Pinout of these displays is as following (starting from inside the LCD):
	
	1  [input]  DATA
	
	2  [power]  5V
	
	3  [power]  10V
	
	4  [power]  GND
	
	5  [input]  ENABLE
	
	6  [input]  LCD CLOCK
	
	7  [input]  LATCH
	
	8  [input]  DATA CLOCK
	
	9  [output] DATA
*/
class AEG_BE : public Adafruit_GFX {
	private:
		uint8_t
			_panels,     ///< Number of daisychained panels
			_latch,      ///< Latch pin
			_enable,     ///< Enable pin
			_panelwidth, ///< Width in px of each panel
			_blanks,     ///< Blank cols at end of panel
			_registers;
		uint16_t _bytes; ///< Number of bytes to write
		uint8_t* _imageBuffer; ///< Imagebuffer
		
	public:
		AEG_BE(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
#ifdef __AVR
		void enableClock(boolean);
#endif
		void enable(void);
		void disable(void);
		void drawPixel(int16_t, int16_t, uint16_t);
		void fillScreen(uint16_t);
		void display(void);
};

/**
	\class AEG_BE10
	\brief Alias constructor for BE10 displays (29x24px)
	\sa    AEG_BE
*/
class AEG_BE10 : public AEG_BE {
	public:
		AEG_BE10(uint8_t, uint8_t, uint8_t);
};

/**
	\class AEG_BE11
	\brief Alias constructor for BE11 displays (39x24px)
	\sa    AEG_BE
*/
class AEG_BE11 : public AEG_BE {
	public:
		AEG_BE11(uint8_t, uint8_t, uint8_t);
};

#endif //AEG_BE_H
