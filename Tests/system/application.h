#ifndef FAKE_application_h
#define FAKE_application_h

#include <functional>
#include <iostream>

#include "Arduino.h"
#include "Servo.h"
#include "WString.h"

#define PRODUCT_ID(v)
#define PRODUCT_VERSION(v)

#define Serial1 Serial
#define uint unsigned int
#define pin_t uint8_t
#define FALSE 0
#define TRUE 1
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define digitalPinToPort(P) 0
#define digitalPinToBitMask(P) 0
#define portOutputRegister(port) 0
#define portInputRegister(port) 0
#define digitalPinHasPWM(P) 0
#define SPI_CLOCK_DIV2 0
#define SPI_CLOCK_DIV8 0
#define shiftOut(sid, sclk, mb, d)
#define HEX 0

#if defined(_WIN32) || defined(WIN32)
#define __attribute__(A)
#endif

class _RGB {
public:
	void control(bool val) {};
	void color(char r, char g, char b) {};
};
extern _RGB RGB;

class _Particle {
public:
	void syncTime() {};
};

extern _Particle Particle;

class Print {
};

class Timer {
public:
	typedef std::function<void(void)> timer_callback_fn;

	Timer(unsigned period, timer_callback_fn callback_, bool one_shot = false) {
	}

	template <typename T>
	Timer(unsigned period, void (T::*handler)(), T& instance, bool one_shot = false) : Timer(period, std::bind(handler, &instance), one_shot)
	{
	}
};

typedef struct {
	uint8_t to_be_filled;
	uint8_t BSRRH;
	uint8_t BSRRL;
} GPIO_TypeDef;

typedef enum PinMode {
	INPUT,
	OUTPUT,
	INPUT_PULLUP,
	INPUT_PULLDOWN,
	AF_OUTPUT_PUSHPULL, //Used internally for Alternate Function Output PushPull(TIM, UART, SPI etc)
	AF_OUTPUT_DRAIN,    //Used internally for Alternate Function Output Drain(I2C etc). External pullup resistors required.
	AN_INPUT,           //Used internally for ADC Input
	AN_OUTPUT,          //Used internally for DAC Output
	PIN_MODE_NONE = 0xFF
};

typedef enum PinFunction {
	PF_NONE,
	PF_DIO,
	PF_TIMER,
	PF_ADC,
	PF_DAC
};

typedef struct Pin_Info {
	GPIO_TypeDef* gpio_peripheral;
	pin_t gpio_pin;
	uint8_t gpio_pin_source;
	uint8_t adc_channel;
	uint16_t timer_ch;
	PinMode pin_mode;
	uint16_t timer_ccr;
	int32_t user_property;
} STM32_Pin_Info;
extern Pin_Info PIN_MAP[];

#ifdef __cplusplus
extern "C" {
#endif

	STM32_Pin_Info* HAL_Pin_Map(void);
	void HAL_Pin_Mode(pin_t pin, PinMode mode);
	int32_t HAL_GPIO_Read(pin_t pin);
	void noInterrupts();
	void interrupts();
	void delayMicroseconds(uint ms);

#ifdef __cplusplus
}
#endif


class TwoWire
{
public:
	inline void setClock(uint32_t speed) {
		setSpeed(speed);
	}
	void setSpeed(uint32_t);
	void enableDMAMode(bool);
	void stretchClock(bool);
	void begin();
	void begin(uint8_t);
	void begin(int);
	void beginTransmission(uint8_t);
	void beginTransmission(int);
	void end();
	uint8_t endTransmission(void);
	uint8_t endTransmission(uint8_t);
	uint8_t requestFrom(uint8_t, uint8_t);
	uint8_t requestFrom(uint8_t, uint8_t, uint8_t);
	uint8_t requestFrom(int, int);
	uint8_t requestFrom(int, int, int);
	virtual size_t write(uint8_t);
	virtual size_t write(const uint8_t *, size_t);
	virtual int available(void);
	virtual int read(void);
	virtual int peek(void);
	virtual void flush(void);
	void onReceive(void(*)(int));
	void onRequest(void(*)(void));

	bool lock();
	bool unlock();

	inline size_t write(unsigned long n) { return write((uint8_t)n); }
	inline size_t write(long n) { return write((uint8_t)n); }
	inline size_t write(unsigned int n) { return write((uint8_t)n); }
	inline size_t write(int n) { return write((uint8_t)n); }

	bool isEnabled(void);

	/**
	* Attempts to reset this I2C bus.
	*/
	void reset();
};

extern TwoWire Wire;

class SPIClass;

typedef void(*wiring_spi_dma_transfercomplete_callback_t)(void);
typedef void(*wiring_spi_select_callback_t)(uint8_t);

enum FrequencyScale
{
	HZ = 1,
	KHZ = HZ * 1000,
	MHZ = KHZ * 1000,
	SPI_CLK_SYSTEM = 0,         // represents the system clock speed
	SPI_CLK_ARDUINO = 16 * MHZ,
	SPI_CLK_CORE = 72 * MHZ,
	SPI_CLK_PHOTON = 60 * MHZ
};

typedef enum SPI_Mode
{
	SPI_MODE_MASTER = 0, SPI_MODE_SLAVE = 1
};

class SPIClass {
public:
	SPIClass();
	virtual ~SPIClass() {};

	void begin();
	void begin(uint16_t);
	void begin(SPI_Mode mode, uint16_t);
	void end();

	void setBitOrder(uint8_t);
	void setDataMode(uint8_t);

	static void usingInterrupt(uint8_t) {};

	int32_t beginTransaction();
	void endTransaction();

	/**
	* Sets the clock speed that the divider is relative to. This does not change
	* the assigned clock speed until the next call to {@link #setClockDivider}
	* @param value   The clock speed reference value
	* @param scale   The clock speed reference scalar
	*
	* E.g.
	*
	* setClockDividerReference(ARDUINO);
	* setClockDividerReference(16, MHZ);
	*
	* @see #setClockDivider
	*/
	void setClockDividerReference(unsigned value, unsigned scale = HZ);

	/**
	* Sets the clock speed as a divider relative to the clock divider reference.
	* @param divider SPI_CLOCK_DIVx where x is a power of 2 from 2 to 256.
	*/
	void setClockDivider(uint8_t divider);

	/**
	* Sets the absolute clock speed. This will select the clock divider that is no greater than
	* {@code value*scale}.
	* @param value
	* @param scale
	* @return the actual clock speed set.
	*/
	unsigned setClockSpeed(unsigned value, unsigned scale = HZ);


	/*
	* Test method to compute the divider needed to attain a given clock frequency.
	*/
	static void computeClockDivider(unsigned reference, unsigned targetSpeed, uint8_t& divider, unsigned& clock);

	byte transfer(byte _data);
	void transfer(void* tx_buffer, void* rx_buffer, size_t length, wiring_spi_dma_transfercomplete_callback_t user_callback);

	void attachInterrupt();
	void detachInterrupt();

	bool isEnabled(void);

	void onSelect(wiring_spi_select_callback_t user_callback);
	void transferCancel();
	int32_t available();

	bool trylock()
	{
		return true;
	}

	void lock()
	{
	}

	void unlock()
	{
	}
};


extern SPIClass SPI;

#endif