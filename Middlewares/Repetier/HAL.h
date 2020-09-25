/*
    This file is part of Repetier-Firmware.

    Repetier-Firmware is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Repetier-Firmware is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Repetier-Firmware.  If not, see <http://www.gnu.org/licenses/>.

    This firmware is a nearly complete rewrite of the sprinter firmware
    by kliment (https://github.com/kliment/Sprinter)
    which based on Tonokip RepRap firmware rewrite based off of Hydra-mmm firmware.


    Main author: repetier

    Initial port of HAL to Arduino Due: John Silvia
*/

/**
  This is the main Hardware Abstraction Layer (HAL).
  To make the firmware work with different processors and toolchains,
  all hardware related code should be packed into the hal files.
*/

#ifndef HAL_H
#define HAL_H
#include "type_define.h"
#include <inttypes.h>
#include "pins.h"
#include "Print.h"
//#include "fastio.h"
#include "timetick.h"
#include "tc.h"
#include "sys.h"
//#include "HardwareTimer.h"
//#include "stm32f10x.h"
#include "stm32f4xx.h" //skyblue 2006-12-13
//#include "stm32f10x_gpio.h"
#include "variant.h"     //skyblue 2006-11-26
//extern USARTClass Serial; //skyblue 2006-11-26

#define ARDUINO 101//?aá?è?±à3ìí¨1y￡??aê?ARDUINOμ?°?±?o?
// Hack to make 84 MHz Due clock work without changes to pre-existing code
// which would otherwise have problems with int overflow.
#undef F_CPU
#define F_CPU       21000000        // should be factor of F_CPU_TRUE
#define F_CPU_TRUE  168000000        // actual CPU clock frequency
#define EEPROM_BYTES 2048//4096  // bytes of eeprom we simulate
//#define SUPPORT_64_BIT_MATH  // Gives better results with high resultion deltas       //delta-skyblue

// another hack to keep AVR code happy (i.e. SdFat.cpp)
#define SPR0    0
#define SPR1    1

// force SdFat to use HAL (whether or not using SW spi)
#undef  SOFTWARE_SPI
#define TIMER0_PRESCALE 128

// Some structures assume no padding, need to add this attribute on ARM
#define PACK    __attribute__ ((packed))

#define INLINE __attribute__((always_inline))

// do not use program space memory with Due
#define PROGMEM
#define PGM_P const char *
typedef char prog_char;
#undef PSTR
#define PSTR(s) s
#undef pgm_read_byte_near
#define pgm_read_byte_near(x) (*(int8_t*)x)
#undef pgm_read_byte
#define pgm_read_byte(x) (*(int8_t*)x)
#undef pgm_read_float
#define pgm_read_float(addr) (*(const float *)(addr))
#undef pgm_read_word
//#define pgm_read_word(addr) (*(const unsigned int *)(addr))
#define pgm_read_word(addr) (*(addr))
#undef pgm_read_word_near
#define pgm_read_word_near(addr) pgm_read_word(addr)
#undef pgm_read_dword
#define pgm_read_dword(addr) (*(addr))
//#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#undef pgm_read_dword_near
#define pgm_read_dword_near(addr) pgm_read_dword(addr)
#define _BV(x) (1 << (x))

#define FSTRINGVALUE(var,value) const char var[] PROGMEM = value;
#define FSTRINGVAR(var) static const char var[] PROGMEM;
#define FSTRINGPARAM(var) PGM_P var


#define EXTRUDER_TIMER          TC0
#define EXTRUDER_TIMER_CHANNEL  0
#define EXTRUDER_TIMER_IRQ      ID_TC0
#define EXTRUDER_TIMER_VECTOR   TC0_Handler
#define PWM_TIMER               TC0
#define PWM_TIMER_CHANNEL       1
#define PWM_TIMER_IRQ           ID_TC1
#define PWM_TIMER_VECTOR        TC1_Handler
#define TIMER1_TIMER            TC2
#define TIMER1_TIMER_CHANNEL    2
#define TIMER1_TIMER_IRQ        ID_TC8
#define TIMER1_COMPA_VECTOR     TC8_Handler
#define SERVO_TIMER             TC2
#define SERVO_TIMER_CHANNEL     0
#define SERVO_TIMER_IRQ         ID_TC6
#define SERVO_COMPA_VECTOR      TC6_Handler
#define BEEPER_TIMER            TC1
#define BEEPER_TIMER_CHANNEL    0
#define BEEPER_TIMER_IRQ        ID_TC3
#define BEEPER_TIMER_VECTOR     TC3_Handler
#define DELAY_TIMER             TC1
#define DELAY_TIMER_CHANNEL     1
#define DELAY_TIMER_IRQ         ID_TC4  // IRQ not really used, needed for pmc id
#define DELAY_TIMER_CLOCK       TC_CMR_TCCLKS_TIMER_CLOCK2
#define DELAY_TIMER_PRESCALE    8

//#define SERIAL_BUFFER_SIZE      1024
//#define SERIAL_PORT             UART
//#define SERIAL_IRQ              ID_UART
//#define SERIAL_PORT_VECTOR      UART_Handler

// TWI1 if SDA pin = 20  TWI0 for pin = 70
#define TWI_INTERFACE   		TWI1
#define TWI_ID  				ID_TWI1


#define EXTRUDER_CLOCK_FREQ     60000 // extruder stepper interrupt frequency
#define PWM_CLOCK_FREQ          3906
#define TIMER1_CLOCK_FREQ       244
#define TIMER1_PRESCALE         2


#define SERVO_CLOCK_FREQ        1000
#define SERVO_PRESCALE          2      // Using TCLOCK1 therefore 2
#define SERVO2500US             (((F_CPU_TRUE / SERVO_PRESCALE) / 1000000) * 2500)
#define SERVO5000US             (((F_CPU_TRUE / SERVO_PRESCALE) / 1000000) * 5000)

#define AD_PRESCALE_FACTOR      84  // 500 kHz ADC clock 
#define AD_TRACKING_CYCLES      4   // 0 - 15     + 1 adc clock cycles
#define AD_TRANSFER_CYCLES      1   // 0 - 3      * 2 + 3 adc clock cycles

#define ADC_ISR_EOC(channel)    (0x1u << channel)
#define ENABLED_ADC_CHANNELS    {TEMP_0_PIN, TEMP_1_PIN, TEMP_2_PIN}

#define PULLUP(IO,v)            //{pinMode(IO, (v!=LOW ? INPUT_PULLUP : INPUT)); }

// INTERVAL / (32Khz/128)  = seconds
#define WATCHDOG_INTERVAL       1024u  // 8sec  (~16 seconds max)



#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#define COMPAT_PRE1
#endif
#define	READ(x)  HAL::digitalRead(x)     
#define WRITE    HAL::digitalWrite
#define TOGGLE(x)  HAL::digitalWrite(IO,!READ(x))
#define SET_OUTPUT(x)	 HAL::pinMode(x, OUTPUT)
#define SET_INPUT(x)	 HAL::pinMode(x, INPUT_PULLUP)

//#define	READ(pin)  PIO_Get(g_APinDescription[pin].pPort, PIO_INPUT, g_APinDescription[pin].ulPin)
//#define READ_VAR(pin) (g_APinDescription[pin].pPort->PIO_PDSR & g_APinDescription[pin].ulPin ? 1 : 0) // does return 0 or pin value
//#define _READ(pin) (DIO ##  pin ## _PORT->PIO_PDSR & DIO ##  pin ## _PIN ? 1 : 0) // does return 0 or pin value
//#define READ(pin) _READ(pin)
//#define	WRITE_VAR(pin, v) PIO_SetOutput(g_APinDescription[pin].pPort, g_APinDescription[pin].ulPin, v, 0, PIO_PULLUP)
//#define	WRITE_VAR(pin, v) do{if(v) {g_APinDescription[pin].pPort->PIO_SODR = g_APinDescription[pin].ulPin;} else {g_APinDescription[pin].pPort->PIO_CODR = g_APinDescription[pin].ulPin; }}while(0)
//#define		_WRITE(port, v)			do { if (v) {DIO ##  port ## _PORT -> PIO_SODR = DIO ## port ## _PIN; } else {DIO ##  port ## _PORT->PIO_CODR = DIO ## port ## _PIN; }; } while (0)
//#define WRITE(pin,v) _WRITE(pin,v)

//#define	SET_INPUT(pin) pmc_enable_periph_clk(g_APinDescription[pin].ulPeripheralId); \
//  PIO_Configure(g_APinDescription[pin].pPort, PIO_INPUT, g_APinDescription[pin].ulPin, 0)
//#define	SET_OUTPUT(pin) PIO_Configure(g_APinDescription[pin].pPort, PIO_OUTPUT_1, \
//                                      g_APinDescription[pin].ulPin, g_APinDescription[pin].ulPinConfiguration)
//#define TOGGLE(pin) WRITE(pin,!READ(pin))
//#define TOGGLE_VAR(pin) HAL::digitalWrite(pin,!HAL::digitalRead(pin))
#undef LOW
#define LOW         0
#undef HIGH
#define HIGH        1

// Protects a variable scope for interrupts. Uses RAII to force clearance of
// Interrupt block at the end resp. sets them to previous state.
// Uses ABSEPRI to allow higher level interrupts then the one changing firmware data
#if 0
class InterruptProtectedBlock {
  public:
    INLINE void protect() {
      //__disable_irq();
      __ASM volatile("cpsid i");
    }

    INLINE void unprotect() {
     // __enable_irq();
      __ASM volatile("cpsie i");
    }

    INLINE InterruptProtectedBlock(bool later = false) {
      if (!later){
        __ASM volatile("cpsid i");
      }
    }

    INLINE ~InterruptProtectedBlock() {
      //__enable_irq();
      __ASM volatile("cpsie i");
    }
};
#else
class InterruptProtectedBlock {
    uint32_t mask;
  public:
    inline void protect() {
      //mask = __get_PRIMASK();;
      //__disable_irq();
      __ASM volatile("cpsid i");
    }

    inline void unprotect() {
     // __set_PRIMASK(mask);
      __ASM volatile("cpsie i");
    }

    inline InterruptProtectedBlock(bool later = false) {
      //mask = __get_PRIMASK();
      if (!later){
       // __disable_irq();
      __ASM volatile("cpsid i");
      }
    }

    inline ~InterruptProtectedBlock() {
      //__set_PRIMASK(mask);
      __ASM volatile("cpsie i");
    }
};
#endif

#define EEPROM_OFFSET               0
#define SECONDS_TO_TICKS(s) (unsigned long)(s*(float)F_CPU)
#define ANALOG_INPUT_SAMPLE 5
#define ANALOG_INPUT_MEDIAN 10

// Bits of the ADC converter
#define ANALOG_INPUT_BITS 10
#define ANALOG_REDUCE_BITS 0
#define ANALOG_REDUCE_FACTOR 1

// maximum available RAM
#define MAX_RAM 98303

#define bit_clear(x,y) x&= ~(1<<y) //cbi(x,y)
#define bit_set(x,y)   x|= (1<<y)//sbi(x,y)

/** defines the data direction (reading from I2C device) in i2cStart(),i2cRepStart() */
#define I2C_READ    1
/** defines the data direction (writing to I2C device) in i2cStart(),i2cRepStart() */
#define I2C_WRITE   0

#ifndef DUE_SOFTWARE_SPI
extern int spiDueDividors[];
#endif

static uint32_t    tone_pin;

/** Set max. frequency to 500000 Hz */
#define LIMIT_INTERVAL (F_CPU/500000)


typedef unsigned int speed_t;
typedef unsigned long ticks_t;
typedef unsigned long millis_t;
//typedef unsigned int flag8_t;
typedef int fast8_t;
typedef unsigned int ufast8_t;

#ifndef RFSERIAL
#define RFSERIAL Serial   // Programming port of the due
//#define RFSERIAL SerialUSB  // Native USB Port of the due
#endif

#if defined(BLUETOOTH_SERIAL) && BLUETOOTH_SERIAL > 0
#if BLUETOOTH_SERIAL == 1
#define BT_SERIAL Serial1
#elif BLUETOOTH_SERIAL == 2
#define BT_SERIAL Serial2
#elif BLUETOOTH_SERIAL == 3
#define BT_SERIAL Serial3
#elif BLUETOOTH_SERIAL == 100
#define BT_SERIAL Serial
#elif BLUETOOTH_SERIAL == 101
#define BT_SERIAL SerialUSB
#endif

class RFDoubleSerial : public Print
{
  public:
    RFDoubleSerial();
    void begin(unsigned long);
    void end();
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    using Print::write; // pull in write(str) and write(buf, size) from Print
};
extern RFDoubleSerial BTAdapter;

#endif

/*
#define OUT_P_I(p,i) Com::printF(PSTR(p),(int)(i))
#define OUT_P_I_LN(p,i) Com::printFLN(PSTR(p),(int)(i))
#define OUT_P_L(p,i) Com::printF(PSTR(p),(long)(i))
#define OUT_P_L_LN(p,i) Com::printFLN(PSTR(p),(long)(i))
#define OUT_P_F(p,i) Com::printF(PSTR(p),(float)(i))
#define OUT_P_F_LN(p,i) Com::printFLN(PSTR(p),(float)(i))
#define OUT_P_FX(p,i,x) Com::printF(PSTR(p),(float)(i),x)
#define OUT_P_FX_LN(p,i,x) Com::printFLN(PSTR(p),(float)(i),x
#define OUT_P(p) Com::printF(PSTR(p))
#define OUT_P_LN(p) Com::printFLN(PSTR(p))
#define OUT_ERROR_P(p) Com::printErrorF(PSTR(p))
#define OUT_ERROR_P_LN(p) {Com::printErrorF(PSTR(p));Com::println();}
#define OUT(v) Com::print(v)
#define OUT_LN Com::println()
#pragma   pack(1)*/
union eeval_t {
  uint8_t     b[4];
  float       f;
  uint32_t    i;
  uint16_t    s;
  long        l;
};
#pragma   pack()
class HAL
{
  public:
    // we use ram instead of eeprom, so reads are faster and safer. Writes store in real eeprom as well
    // as long as hal eeprom functions are used.
    static char virtualEeprom[EEPROM_BYTES];
    static bool wdPinged;
    
    HAL();
    virtual ~HAL();

    // do any hardware-specific initialization here
    static inline void hwSetup(void)
    {
     	//uint8_t tm[256];
#if EEPROM_MODE != 0			
      AT24CXX_Init();
      
     /*-------------------WRITE TEST------------------------*/
     /*
      u8 e2pr_Buf_Write[256];
	 for(int j=0;j<8;j++)
	{
		for(int i_e2pr=0;i_e2pr<256;i_e2pr++)
		{
			e2pr_Buf_Write[i_e2pr]=i_e2pr;
			if(i_e2pr == 0) e2pr_Buf_Write[i_e2pr] = j;
				
		}
		AT24CXX_Write(j*256,e2pr_Buf_Write,256);
	}
      
      eprSetLong(0,0xa1a2a3a4);
      eprSetLong(2044,0xa5a6a7a8);
*/
      /*------------------WRITE TEST-------------------------*/
		 /*
		 for(int j=0;j<256;j++)
		 	{
		 		tm[j]=0xff;
		 	}
		 for(int t=0;t<8;t++)
		 	{
		 		HAL::AT24CXX_Write(t*256, &tm[t*256], 256);
		 	}
      */
      for(int i=0;i<8;i++)
      {
	AT24CXX_Read(i*256,(u8 *)&virtualEeprom[i*256],256);
	//for(int j=0;j<256;j++)
        //  printf("0x%02X ", virtualEeprom[i*256+j]);
       // printf("\n\r\n");
      } 

			
      //while(1);
      /*
      u8 eepData[4];
      eprSetByte(0,0xa0);
      AT24CXX_Read(0,&eepData[0],1);
      printf("0x%02X ", eepData[0]);
      
      eprSetInt16(0,0xa1a2);
      AT24CXX_Read(1,&eepData[0],2);
      printf("0x%02X,0x%02X ", eepData[0],eepData[1]);
      */
      /*
      u8 eepData[4];
      eprSetLong(0,0xa1a2a3a4);
      AT24CXX_Read(0,&eepData[0],4);
      printf("0x%02X,0x%02X,0x%02X,0x%02X  ", eepData[0],eepData[1],eepData[2],eepData[3]);
      
      eprSetLong(2044,0xa5a6a7a8);
      AT24CXX_Read(2044,&eepData[0],4);
      printf("0x%02X,0x%02X,0x%02X,0x%02X  ", eepData[0],eepData[1],eepData[2],eepData[3]);
      */
 #endif     
    }

    static uint32_t integer64Sqrt(uint64_t a);
    // return val'val
    static inline unsigned long U16SquaredToU32(unsigned int val)
    {
      return (unsigned long) val * (unsigned long) val;
    }
    static inline unsigned int ComputeV(long timer, long accel)
    {
      return static_cast<unsigned int>((static_cast<int64_t>(timer) * static_cast<int64_t>(accel)) >> 18);
      //return ((timer>>8)*accel)>>10;
    }
    // Multiply two 16 bit values and return 32 bit result
    static inline unsigned long mulu16xu16to32(unsigned int a, unsigned int b)
    {
      return (unsigned long) a * (unsigned long) b;
    }
    // Multiply two 16 bit values and return 32 bit result
    static inline unsigned int mulu6xu16shift16(unsigned int a, unsigned int b)
    {
      return ((unsigned long)a * (unsigned long)b) >> 16;
    }
    static inline unsigned int Div4U2U(unsigned long a, unsigned int b)
    {
      return ((unsigned long)a / (unsigned long)b);
    }
    /*static inline void digitalWrite(uint8_t pin, uint8_t value)
    {
      WRITE_VAR(pin, value);
    }
    static inline uint8_t digitalRead(uint8_t pin)
    {
      return READ_VAR(pin);
    }
    static inline void pinMode(uint8_t pin, uint8_t mode)
    {
      if (mode == INPUT) {
        SET_INPUT(pin);
      }
      else SET_OUTPUT(pin);
    }*/
    static long CPUDivU2(speed_t divisor) {
      return F_CPU / divisor;
    }
    static inline void delayMicroseconds(uint32_t usec)
    { //usec += 3;
    //tan 20161128
    	//microsecondsWait(usec);
    	volatile uint32_t STEPPER_DELAY=0;
    	#if 0
    	uint32_t usec_bak;
			usec_bak = usec;
    	while(1)
    	{
    		usec = usec_bak;
	    	GPIOB->BRR = GPIO_Pin_3;
				while(usec--)
				{
	    		STEPPER_DELAY = 5;//usec;
	    		while(STEPPER_DELAY--);
				}
				GPIOB->BSRR = GPIO_Pin_3;
				usec = usec_bak;
				while(usec--)
				{
	    		STEPPER_DELAY = 5;//usec;
	    		while(STEPPER_DELAY--);
				}

    	}
			#endif
			
			if((usec != 0)&&(usec != 0xffffffff))
			{
				while(usec--)
				{
		    		STEPPER_DELAY = 5;//usec;
		    		while(STEPPER_DELAY--);
				}
			}
      /*uint32_t n = usec * (F_CPU_TRUE / 3000000);
      asm volatile(
        "L2_%=_delayMicroseconds:"       "\n\t"
        "subs   %0, #1"                 "\n\t"
        "bge    L2_%=_delayMicroseconds" "\n"
        : "+r" (n) :
      );*/
    }
    static inline void delayMilliseconds(unsigned int delayMs)
    {
      unsigned int del;
      while (delayMs > 0) {
        del = delayMs > 100 ? 100 : delayMs;
        delay(del);
        delayMs -= del;
#if FEATURE_WATCHDOG
        HAL::pingWatchdog();
#endif
      }
    }
    static inline void tone(uint8_t pin, int frequency) {
      // set up timer counter 1 channel 0 to generate interrupts for
      // toggling output pin.
      /*SET_OUTPUT(pin);
      tone_pin = pin;
      pmc_set_writeprotect(false);
      pmc_enable_periph_clk((uint32_t)BEEPER_TIMER_IRQ);
      // set interrupt to lowest possible priority
      NVIC_SetPriority((IRQn_Type)BEEPER_TIMER_IRQ, NVIC_EncodePriority(4, 6, 3));
      TC_Configure(BEEPER_TIMER, BEEPER_TIMER_CHANNEL, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC |
                   TC_CMR_TCCLKS_TIMER_CLOCK4);  // TIMER_CLOCK4 -> 128 divisor
      uint32_t rc = VARIANT_MCK / 128 / frequency;
      TC_SetRA(BEEPER_TIMER, BEEPER_TIMER_CHANNEL, rc / 2);                   // 50% duty cycle
      TC_SetRC(BEEPER_TIMER, BEEPER_TIMER_CHANNEL, rc);
      TC_Start(BEEPER_TIMER, BEEPER_TIMER_CHANNEL);
      BEEPER_TIMER->TC_CHANNEL[BEEPER_TIMER_CHANNEL].TC_IER = TC_IER_CPCS;
      BEEPER_TIMER->TC_CHANNEL[BEEPER_TIMER_CHANNEL].TC_IDR = ~TC_IER_CPCS;
      NVIC_EnableIRQ((IRQn_Type)BEEPER_TIMER_IRQ);*/
    }
    static inline void noTone(uint8_t pin) {
      /*TC_Stop(TC1, 0);
      WRITE_VAR(pin, LOW);*/
    }

    static inline void eprSetByte(unsigned int pos, uint8_t value)
    {
      eeval_t v;
      v.b[0] = value;
      eprBurnValue(pos, 1, v);
      *(uint8_t*)&virtualEeprom[pos] = value;
    }
    static inline void eprSetInt16(unsigned int pos, int16_t value)
    {
      eeval_t v;
      v.s = value;
      eprBurnValue(pos, 2, v);
      memcopy2(&virtualEeprom[pos],&value);
    }
    static inline void eprSetInt32(unsigned int pos, int32_t value)
    {
      eeval_t v;
			unsigned int pos1;
			pos1 = pos;
      v.i = value;
      eprBurnValue(pos, 4, v);
      memcopy4(&virtualEeprom[pos],&value);
			v = eprGetValue(pos1,4);
    }
    static inline void eprSetLong(unsigned int pos, long value)
    {
      eeval_t v;
      v.l = value;
      eprBurnValue(pos, sizeof(long), v);
      memcopy4(&virtualEeprom[pos],&value);
			v = eprGetValue(pos,sizeof(long));
    }
    static inline void eprSetFloat(unsigned int pos, float value)
    {
      eeval_t v;
      v.f = value;
      eprBurnValue(pos, sizeof(float), v);
      memcopy4(&virtualEeprom[pos],&value);
			v = eprGetValue(pos,sizeof(float));
    }
    static inline uint8_t eprGetByte(unsigned int pos)
    {
      return *(uint8_t*)&virtualEeprom[pos];
    }
    static inline int16_t eprGetInt16(unsigned int pos)
    {
      int16_t v;
      memcopy2(&v,&virtualEeprom[pos]);
      return v;
    }
    static inline int32_t eprGetInt32(unsigned int pos)
    {
      int32_t v;
      memcopy4(&v,&virtualEeprom[pos]);
      return v;
    }
    static inline long eprGetLong(unsigned int pos)
    {
      int32_t v;
      memcopy4(&v,&virtualEeprom[pos]);
      return v;
    }
    static inline float eprGetFloat(unsigned int pos) {
      float v;
      memcopy4(&v,&virtualEeprom[pos]);
      return v;
    }

    // Write any data type to EEPROM
    static inline void eprBurnValue(unsigned int pos, int size, union eeval_t newvalue)
    {
      //void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
      AT24CXX_Write(pos,&newvalue.b[0],size);

    }

    // Read any data type from EEPROM that was previously written by eprBurnValue
    static inline union eeval_t eprGetValue(unsigned int pos, int size)
    {
      eeval_t v;
    //void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
      AT24CXX_Read(pos,&v.b[0],size);
      return v;       
}
    static inline void allowInterrupts()
    {
      //TIM2->CR1 |= TIM_CR1_CEN;
      //TIM4->CR1 |= TIM_CR1_CEN;
      __ASM volatile("cpsie i");
      
    }
    static inline void forbidInterrupts()
    {
      //TIM2->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
      //TIM4->CR1 &= (uint16_t)(~((uint16_t)TIM_CR1_CEN));
      __ASM volatile("cpsid i");
    }

    static inline unsigned long timeInMilliseconds()
    {
      //**return millis();
    }
    static inline char readFlashByte(PGM_P ptr)
    {
      //**return pgm_read_byte(ptr);
    }
    static inline void serialSetBaudrate(long baud)
    {
#if defined(BLUETOOTH_SERIAL) && BLUETOOTH_SERIAL > 0
      //**BTAdapter.begin(baud);
#else
     //** RFSERIAL.begin(baud);
#endif
    }
    static inline bool serialByteAvailable()
    {
#if defined(BLUETOOTH_SERIAL) && BLUETOOTH_SERIAL > 0
      //**return BTAdapter.available();
#else
      //**return RFSERIAL.available();
#endif
    }
    static inline uint8_t serialReadByte()
    {
#if defined(BLUETOOTH_SERIAL) && BLUETOOTH_SERIAL > 0
     //** return BTAdapter.read();
#else
      //**return RFSERIAL.read();
#endif
    }
    static inline void sdpause()
    {
      //**RFSERIAL.sdPause();
    }
    static inline void sdcontinue()
    {
      //**RFSERIAL.sdContinue();
    }
    static inline int Usartcheck()
    {
      //**return RFSERIAL.check();
    }

    static inline void serialWriteByte(char b)
    {
#if defined(BLUETOOTH_SERIAL) && BLUETOOTH_SERIAL > 0
      //**BTAdapter.write(b);
#else
      //**RFSERIAL.write(b);
#endif
    }
    static inline void serialFlush()
    {
#if defined(BLUETOOTH_SERIAL) && BLUETOOTH_SERIAL > 0
      //**BTAdapter.flush();
#else
      //**RFSERIAL.flush();
#endif
    }
		//tan 20160817
		static inline void MoremenuCmd(void)
		{
			//**RFSERIAL.MoremenuCmd();
		}
		
		static inline void Leveling_move_action(void)
		{
			//**RFSERIAL.Leveling_move_action();
		}
		static inline void filamentchange(void)
		{
			//**RFSERIAL.filamentchange();
		}

		
		//tan0906
		static void get_Temperature_ad_value();
		
    static void setupTimer();
    static void showStartReason();
    static int getFreeRam();
    static void resetHardware();
    static void  pinMode( uint32_t ulPin, uint32_t ulMode );
    static int digitalRead( uint32_t ulPin );
    static void digitalWrite( uint32_t ulPin, uint32_t ulVal );

    // SPI related functions

#ifdef DUE_SOFTWARE_SPI
    // bitbanging transfer
    // run at ~100KHz (necessary for init)
    static uint8_t spiTransfer(uint8_t b)  // using Mode 0
    {
      /*for (int bits = 0; bits < 8; bits++) {
        if (b & 0x80) {
          WRITE(MOSI_PIN, HIGH);
        } else {
          WRITE(MOSI_PIN, LOW);
        }
        b <<= 1;

        WRITE(SCK_PIN, HIGH);
        delayMicroseconds(5);

        if (READ(MISO_PIN)) {
          b |= 1;
        }
        WRITE(SCK_PIN, LOW);
        delayMicroseconds(5);
      }
      return b;*/   //**
    }
    static inline void spiBegin()
    {
     /* SET_OUTPUT(SDSS);
      WRITE(SDSS, HIGH);
      SET_OUTPUT(SCK_PIN);
      SET_INPUT(MISO_PIN);
      SET_OUTPUT(MOSI_PIN);*/  //**
    }

    static inline void spiInit(uint8_t spiClock)
    {
     /* WRITE(SDSS, HIGH);
      WRITE(MOSI_PIN, HIGH);
      WRITE(SCK_PIN, LOW);*/  //**
    }
    static inline uint8_t spiReceive()
    {
     /* WRITE(SDSS, LOW);
      uint8_t b = spiTransfer(0xff);
      WRITE(SDSS, HIGH);
      return b;*/    //**
    }
    static inline void spiReadBlock(uint8_t*buf, uint16_t nbyte)
    {
     /* if (nbyte == 0) return;
      WRITE(SDSS, LOW);
      for (int i = 0; i < nbyte; i++)
      {
        buf[i] = spiTransfer(0xff);
      }
      WRITE(SDSS, HIGH);*/   //**

    }
    static inline void spiSend(uint8_t b) {
      /*WRITE(SDSS, LOW);
      uint8_t response = spiTransfer(b);
      WRITE(SDSS, HIGH);*/  //**
    }

    static inline void spiSend(const uint8_t* buf , size_t n)
    {
     /* if (n == 0) return;
      WRITE(SDSS, LOW);
      for (uint16_t i = 0; i < n; i++) {
        spiTransfer(buf[i]);
      }
      WRITE(SDSS, HIGH);*/  //**
    }

    inline __attribute__((always_inline))
    static void spiSendBlock(uint8_t token, const uint8_t* buf)
    {
/*      WRITE(SDSS, LOW);
      spiTransfer(token);

      for (uint16_t i = 0; i < 512; i++)
      {
        spiTransfer(buf[i]);
      }
      WRITE(SDSS, HIGH);*/  //**
    }

#else

    // hardware SPI
    static void spiBegin();
    // spiClock is 0 to 6, relecting AVR clock dividers 2,4,8,16,32,64,128
    // Due can only go as slow as AVR divider 32 -- slowest Due clock is 329,412 Hz
    static void spiInit(uint8_t spiClock);
    // Write single byte to SPI
    static void spiSend(byte b);
    static void spiSend(const uint8_t* buf , size_t n);
#if MOTHERBOARD == 500 || MOTHERBOARD == 501
    static void spiSend(uint32_t chan , const uint8_t* buf , size_t n);
    static void spiSend(uint32_t chan, byte b);
    static uint8_t spiReceive(uint32_t chan);
#endif
    // Read single byte from SPI
    static uint8_t spiReceive();
    // Read from SPI into buffer
    static void spiReadBlock(uint8_t*buf, uint16_t nbyte);

    // Write from buffer to SPI

    static void spiSendBlock(uint8_t token, const uint8_t* buf);
#endif  /*DUE_SOFTWARE_SPI*/

    // I2C Support
//skyblue 2006-12-05
/*--------------------------MKS EEPROM-------------------------*/    
static void AT24CXX_Init(void);
static void AT24CXX_WriteByte(uint8_t Reg, uint8_t Value);
static uint8_t AT24CXX_ReadByte(uint8_t Reg);
static HAL_StatusTypeDef AT24CXX_PageWrite(uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
static HAL_StatusTypeDef AT24CXX_Write(uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
static HAL_StatusTypeDef AT24CXX_Read(uint16_t MemAddress, uint8_t* pBuffer, uint32_t BufferSize);
static HAL_StatusTypeDef AT24CXX_IsDeviceReady(uint32_t Trials);
static uint8_t AT24CXX_Check(void);
static uint8_t I2Cx_Read(uint8_t Addr, uint8_t Reg, uint16_t MemAddSize);
static void I2Cx_Write(uint8_t Addr, uint8_t Reg, uint16_t MemAddSize, uint8_t Value);
static HAL_StatusTypeDef I2Cx_ReadMultiple(uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef I2Cx_WriteMultiple(uint8_t Addr, uint16_t Reg, uint16_t MemAddSize, uint8_t *Buffer, uint16_t Length);
static HAL_StatusTypeDef I2Cx_IsDeviceReady(uint16_t DevAddress, uint32_t Trials);
static void I2Cx_Error(uint8_t Addr);

  static void delay_us(u32 nus);
/*--------------------------MKS EEPROM-------------------------*/

    // Watchdog support
    inline static void startWatchdog() {
      //WDT->WDT_MR = WDT_MR_WDRSTEN | WATCHDOG_INTERVAL | (WATCHDOG_INTERVAL << 16);
      //WDT->WDT_CR = 0xA5000001;
    };
    inline static void stopWatchdog() {}
    inline static void pingWatchdog() {
#if FEATURE_WATCHDOG
      wdPinged = true;
#endif
    };

    inline static float maxExtruderTimerFrequency() {
      return (float)F_CPU_TRUE/32;
    }
#if FEATURE_SERVO
    static unsigned int servoTimings[4];
    static void servoMicroseconds(uint8_t servo, int ms, uint16_t autoOff);
#endif

//#if ANALOG_INPUTS > 0
    static void analogStart(void);
//#endif
#if USE_ADVANCE
    static void resetExtruderDirection();
#endif
		static void microsecondsWait(uint32_t us);
    static volatile uint8_t insideTimer1;

};
/*---------------------GPIO--------------------------------*/
#define ON      1
#define OFF     0

/*-------------电机步进-----------------*/
#define	XSTEP_OP  PEout(6)
#define	YSTEP_OP  PEout(3)
#define	ZSTEP_OP  PEout(0)
#define	E0STEP_OP PGout(10)
#define	E1STEP_OP PDout(3)
/*-------------电机方向-----------------*/
#define	XDIR_OP  PEout(5)
#define	YDIR_OP  PEout(2)
#define	ZDIR_OP  PBout(9)
#define	E0DIR_OP PGout(9)
#define	E1DIR_OP PAout(15)

#define	XDIR_IP  PEin(5)
#define	YDIR_IP  PEin(2)
#define	ZDIR_IP  PBin(9)
#define	E0DIR_IP PGin(9)
#define	E1DIR_IP PAin(15)
/*-------------电机使能-----------------*/
#define	XENA_OP  PCout(13)
#define	YENA_OP  PEout(4)
#define	ZENA_OP  PEout(1)
#define	E0ENA_OP PBout(8)
#define	E1ENA_OP PDout(6)
/*-------------限位开关-----------------*/
#define XMIN_OP   PGin(8)              
#define XMAX_OP   PGin(7)      
#define YMIN_OP   PGin(6)      
#define YMAX_OP   PGin(5)      
#define ZMIN_OP   PGin(4)
#define ZMAX_OP   PGin(3)
#define Z_PROBE_OP	PGin(4)

/*-------------法拉电容-----------------*/
#define FALA_ON         1
#define FALA_OFF        0
#define EXT3V3_ON       0
#define EXT3V3_OFF      1

#define FALA_5V_CTRL PAout(0)      //1-FALA5V ON;0- FALA5V OFF         //FALA
#define EXT_3V3_CTRL PAout(6)      //0-EXT3V3 ON;1- EXT3V3 OFF

//**#define SD_DET_IP PBin(0)
#define SD_DET_IP PDin(3)


//#define TEST_OP   PBout(0)
#define MKS_TEST_OP  PBout(1)


//**#define SPEAKER   PFout(11) 
#define SPEAKER   PAout(2)
#define RESET_1963   PDout(6)


#define	MKS_FAN_OP          PFout(1)
#define	MKS_HEATER1_OP      PFout(3)
#define	MKS_HEATER2_OP      PFout(2)
#define	MKS_BED_OP          PFout(4)

#define	MKS_PW_OFF_OP       PGout(1)
#define	MKS_MT_DET1_OP      PFin(14)
#define	MKS_MT_DET2_OP      PFin(13)

/*-------------LCD CONTROL IO----------------*/
#define	MKS_LCD_RST            PDout(13)
#define	MKS_LCD_ON             PDout(12)
/*-------------WIFI CONTROL IO----------------*/
#define	MKS_WIFI_RST            PGout(2)
#define	MKS_WIFI_CS             PBout(12)

/*---------------------GPIO--------------------------------*/
#endif // HAL_H
