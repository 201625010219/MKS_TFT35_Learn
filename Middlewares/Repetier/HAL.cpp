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

    Initial port of hardware abstraction layer to Arduino Due: John Silvia
*/

#include "Arduino.h"
#include "Repetier.h"
#include "configuration.h"
//#include <malloc.h>
//#include "stm32mks_adc.h"       //skyblue 2006-11-26
//#include "stm32mks_timer.h"     //skyblue 2006-11-26     
#include "adc.h"
#include "tim.h"

extern uint8_t PWRDN_FLG;
extern "C" uint16_t ADC_ConvertedValue[4];
//extern "C" void __cxa_pure_virtual() { }
extern "C" char *sbrk(int i);
extern long bresenham_step();

#define TEMPERATURE_COMPENSATION    8  //因为热敏上拉电压有压降，小于3V3 ，所以做补偿   
#define MAX_PULSE_FREQUENCY 100       //10000--2kHz;  1000--20kHz; 200--100kHz; 100--200kHz T=5us; 最大脉冲频率为200kHz
                                        
#define NUM_ADC_SAMPLES 2 + (1 << ANALOG_INPUT_SAMPLE)
//#if ANALOG_INPUTS > 0
//int32_t osAnalogInputBuildup[ANALOG_INPUTS];
//int32_t osAnalogSamples[ANALOG_INPUTS][ANALOG_INPUT_MEDIAN];
//int32_t osAnalogSamplesSum[ANALOG_INPUTS];
//static int32_t adcSamplesMin[ANALOG_INPUTS];
//static int32_t adcSamplesMax[ANALOG_INPUTS];
int32_t osAnalogInputBuildup[9];
int32_t osAnalogSamples[9][ANALOG_INPUT_MEDIAN];
int32_t osAnalogSamplesSum[9];
static int32_t adcSamplesMin[9];
static int32_t adcSamplesMax[9];

static int adcCounter = 0, adcSamplePos = 0;
//#endif

static   uint32_t  adcEnable = 0;

char HAL::virtualEeprom[EEPROM_BYTES];
bool HAL::wdPinged = true;
volatile uint8_t HAL::insideTimer1 = 0;
#ifndef DUE_SOFTWARE_SPI
int spiDueDividors[] = {10, 21, 42, 84, 168, 255, 255};
#endif

HAL::HAL()
{
  //ctor
}

HAL::~HAL()
{
  //dtor
}




// Set up all timer interrupts
void HAL::setupTimer() {
}


//#if ANALOG_INPUTS > 0
// Initialize ADC channels
void HAL::analogStart(void)
{
      //mksADCInit();     //skyblue 2006-11-26
#if MOTHERBOARD == 500 || MOTHERBOARD == 501
  PIO_Configure(
    g_APinDescription[58].pPort,
    g_APinDescription[58].ulPinType,
    g_APinDescription[58].ulPin,
    g_APinDescription[58].ulPinConfiguration);
  PIO_Configure(
    g_APinDescription[59].pPort,
    g_APinDescription[59].ulPinType,
    g_APinDescription[59].ulPin,
    g_APinDescription[59].ulPinConfiguration);
#endif // (MOTHERBOARD==500) || (MOTHERBOARD==501)

  // ensure we can write to ADC registers
  #if 0
  ADC->ADC_WPMR = 0x41444300u; //ADC_WPMR_WPKEY(0);
  pmc_enable_periph_clk(ID_ADC);  // enable adc clock
#endif
  for (int i = 0; i < Change_Repetier_Define.def_ANALOG_INPUTS; i++)
  {
    osAnalogInputValues[i] = 0;
    adcSamplesMin[i] = 100000;
    adcSamplesMax[i] = 0;
    adcEnable |= (0x1u << osAnalogInputChannels[i]);
    osAnalogSamplesSum[i] = 2048 * ANALOG_INPUT_MEDIAN;
    for (int j = 0; j < ANALOG_INPUT_MEDIAN; j++)
      osAnalogSamples[i][j] = 2048; // we want to prevent early error from bad starting values
  }
  // enable channels
#if 0    
  ADC->ADC_CHER = adcEnable;
  ADC->ADC_CHDR = !adcEnable;

  // Initialize ADC mode register (some of the following params are not used here)
  // HW trigger disabled, use external Trigger, 12 bit resolution
  // core and ref voltage stays on, normal sleep mode, normal not free-run mode
  // startup time 16 clocks, settling time 17 clocks, no changes on channel switch
  // convert channels in numeric order
  // set prescaler rate  MCK/((PRESCALE+1) * 2)
  // set tracking time  (TRACKTIM+1) * clock periods
  // set transfer period  (TRANSFER * 2 + 3)
  ADC->ADC_MR = ADC_MR_TRGEN_DIS | ADC_MR_TRGSEL_ADC_TRIG0 | ADC_MR_LOWRES_BITS_12 |
                ADC_MR_SLEEP_NORMAL | ADC_MR_FWUP_OFF | ADC_MR_FREERUN_OFF |
                ADC_MR_STARTUP_SUT64 | ADC_MR_SETTLING_AST17 | ADC_MR_ANACH_NONE |
                ADC_MR_USEQ_NUM_ORDER |
                ADC_MR_PRESCAL(AD_PRESCALE_FACTOR) |
                ADC_MR_TRACKTIM(AD_TRACKING_CYCLES) |
                ADC_MR_TRANSFER(AD_TRANSFER_CYCLES);

  ADC->ADC_IER = 0;             // no ADC interrupts
  ADC->ADC_CGR = 0;             // Gain = 1
  ADC->ADC_COR = 0;             // Single-ended, no offset

  // start first conversion
  ADC->ADC_CR = ADC_CR_START;
#endif    
}

//#endif

#if 0
// Print apparent cause of start/restart
void HAL::showStartReason() {
	#if 0	
  int mcu = (RSTC->RSTC_SR & RSTC_SR_RSTTYP_Msk) >> RSTC_SR_RSTTYP_Pos;
  switch (mcu) {
    case 0:
      Com::printInfoFLN(Com::tPowerUp);
      break;
    case 1:
      // this is return from backup mode on SAM
      Com::printInfoFLN(Com::tBrownOut);
    case 2:
      Com::printInfoFLN(Com::tWatchdog);
      break;
    case 3:
      Com::printInfoFLN(Com::tSoftwareReset);
      break;
    case 4:
      Com::printInfoFLN(Com::tExternalReset);
  }
  #endif
}

#endif

// Return available memory
int HAL::getFreeRam() {
#if 0	
  struct mallinfo memstruct = mallinfo();
  register char * stack_ptr asm ("sp");

  // avail mem in heap + (bottom of stack addr - end of heap addr)
  return (memstruct.fordblks + (int)stack_ptr -  (int)sbrk(0));
#endif  
}

// Reset peripherals and cpu
void HAL::resetHardware() {
  //RSTC->RSTC_CR = RSTC_CR_KEY(0xA5) | RSTC_CR_PERRST | RSTC_CR_PROCRST;
}

// from http://medialab.freaknet.org/martin/src/sqrt/sqrt.c
uint32_t HAL::integer64Sqrt(uint64_t a_nInput) {
  uint64_t op  = a_nInput;
  uint64_t res = 0;
  uint64_t one = 1uLL << 62; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type

  // "one" starts at the highest power of four <= than the argument.
  while (one > op)
    one >>= 2;
  while (one != 0)
  {
    if (op >= res + one)
    {
      op = op - (res + one);
      res = res +  2 * one;
    }
    res >>= 1;
    one >>= 2;
  }
  if (op > res) // Do arithmetic rounding to nearest integer
  {
    res++;
  }
  return res;
}


#ifndef DUE_SOFTWARE_SPI
// hardware SPI
#if MOTHERBOARD == 500 || MOTHERBOARD == 501
bool spiInitMaded = false;
#endif
void HAL::spiBegin()
{
#if MOTHERBOARD == 500 || MOTHERBOARD == 501
  if (spiInitMaded == false)
  {
#endif        // Configre SPI pins
#if 0
    PIO_Configure(
      g_APinDescription[SCK_PIN].pPort,
      g_APinDescription[SCK_PIN].ulPinType,
      g_APinDescription[SCK_PIN].ulPin,
      g_APinDescription[SCK_PIN].ulPinConfiguration);
    PIO_Configure(
      g_APinDescription[MOSI_PIN].pPort,
      g_APinDescription[MOSI_PIN].ulPinType,
      g_APinDescription[MOSI_PIN].ulPin,
      g_APinDescription[MOSI_PIN].ulPinConfiguration);
    PIO_Configure(
      g_APinDescription[MISO_PIN].pPort,
      g_APinDescription[MISO_PIN].ulPinType,
      g_APinDescription[MISO_PIN].ulPin,
      g_APinDescription[MISO_PIN].ulPinConfiguration);

    // set master mode, peripheral select, fault detection
    SPI_Configure(SPI0, ID_SPI0, SPI_MR_MSTR |
                  SPI_MR_MODFDIS | SPI_MR_PS);
    SPI_Enable(SPI0);
#if MOTHERBOARD == 500 || MOTHERBOARD == 501
    SET_OUTPUT(DAC0_SYNC);
#if NUM_EXTRUDER > 1
    SET_OUTPUT(DAC1_SYNC);
    WRITE(DAC1_SYNC, HIGH);
#endif
    SET_OUTPUT(SPI_EEPROM1_CS);
    SET_OUTPUT(SPI_EEPROM2_CS);
    SET_OUTPUT(SPI_FLASH_CS);
    WRITE(DAC0_SYNC, HIGH);
    WRITE(SPI_EEPROM1_CS, HIGH );
    WRITE(SPI_EEPROM2_CS, HIGH );
    WRITE(SPI_FLASH_CS, HIGH );
    WRITE(SDSS , HIGH );
#endif// MOTHERBOARD == 500 || MOTHERBOARD == 501
    PIO_Configure(
      g_APinDescription[SPI_PIN].pPort,
      g_APinDescription[SPI_PIN].ulPinType,
      g_APinDescription[SPI_PIN].ulPin,
      g_APinDescription[SPI_PIN].ulPinConfiguration);
    spiInit(1);
#endif       
#if (MOTHERBOARD==500) || (MOTHERBOARD==501)
    spiInitMaded = true;
  }
#endif
}
// spiClock is 0 to 6, relecting AVR clock dividers 2,4,8,16,32,64,128
// Due can only go as slow as AVR divider 32 -- slowest Due clock is 329,412 Hz
void HAL::spiInit(uint8_t spiClock)
{
#if 0	
#if MOTHERBOARD == 500 || MOTHERBOARD == 501
  if (spiInitMaded == false)
  {
#endif
    if (spiClock > 4) spiClock = 1;
#if MOTHERBOARD == 500 || MOTHERBOARD == 501
    // Set SPI mode 1, clock, select not active after transfer, with delay between transfers
    SPI_ConfigureNPCS(SPI0, SPI_CHAN_DAC,
                      SPI_CSR_CSAAT | SPI_CSR_SCBR(spiDueDividors[spiClock]) |
                      SPI_CSR_DLYBCT(1));
    // Set SPI mode 0, clock, select not active after transfer, with delay between transfers
    SPI_ConfigureNPCS(SPI0, SPI_CHAN_EEPROM1, SPI_CSR_NCPHA |
                      SPI_CSR_CSAAT | SPI_CSR_SCBR(spiDueDividors[spiClock]) |
                      SPI_CSR_DLYBCT(1));
#endif// MOTHERBOARD==500 || MOTHERBOARD==501
    // Set SPI mode 0, clock, select not active after transfer, with delay between transfers
    SPI_ConfigureNPCS(SPI0, SPI_CHAN, SPI_CSR_NCPHA |
                      SPI_CSR_CSAAT | SPI_CSR_SCBR(spiDueDividors[spiClock]) |
                      SPI_CSR_DLYBCT(1));
    SPI_Enable(SPI0);
#if MOTHERBOARD == 500 || MOTHERBOARD == 501
    spiInitMaded = true;
  }
#endif
#endif
}
// Write single byte to SPI
void HAL::spiSend(byte b) {
#if 0		
  // write byte with address and end transmission flag
  SPI0->SPI_TDR = (uint32_t)b | SPI_PCS(SPI_CHAN) | SPI_TDR_LASTXFER;
  // wait for transmit register empty
  while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
  // wait for receive register
  while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
  // clear status
  SPI0->SPI_RDR;
  //delayMicroseconds(1);
#endif    
}
void HAL::spiSend(const uint8_t* buf , size_t n)
{
#if 0		
  if (n == 0) return;
  for (size_t i = 0; i < n - 1; i++)
  {
    SPI0->SPI_TDR = (uint32_t)buf[i] | SPI_PCS(SPI_CHAN);
    while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
    while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
    SPI0->SPI_RDR;
    //        delayMicroseconds(1);
  }
  spiSend(buf[n - 1]);
#endif    
}

// Read single byte from SPI
uint8_t HAL::spiReceive()
{
	#if 0
  // write dummy byte with address and end transmission flag
  SPI0->SPI_TDR = 0x000000FF | SPI_PCS(SPI_CHAN) | SPI_TDR_LASTXFER;
  // wait for transmit register empty
  while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);

  // wait for receive register
  while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
  // get byte from receive register
  //delayMicroseconds(1);
  return SPI0->SPI_RDR;
  #endif  
}
#if MOTHERBOARD == 500 || MOTHERBOARD == 501

void HAL::spiSend(uint32_t chan, byte b)
{
  uint8_t dummy_read = 0;
  // wait for transmit register empty
  while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
  // write byte with address and end transmission flag
  SPI0->SPI_TDR = (uint32_t)b | SPI_PCS(chan) | SPI_TDR_LASTXFER;
  // wait for receive register
  while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
  // clear status
  while ((SPI0->SPI_SR & SPI_SR_RDRF) == 1)
    dummy_read = SPI0->SPI_RDR;
}

void HAL::spiSend(uint32_t chan , const uint8_t* buf , size_t n)
{
  uint8_t dummy_read = 0;
  if (n == 0) return;
  for (int i = 0; i < n - 1; i++)
  {
    while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
    SPI0->SPI_TDR = (uint32_t)buf[i] | SPI_PCS(chan);
    while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
    while ((SPI0->SPI_SR & SPI_SR_RDRF) == 1)
      dummy_read = SPI0->SPI_RDR;
  }
  spiSend(chan, buf[n - 1]);
}

uint8_t HAL::spiReceive(uint32_t chan)
{
  uint8_t spirec_tmp;
  // wait for transmit register empty
  while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
  while ((SPI0->SPI_SR & SPI_SR_RDRF) == 1)
    spirec_tmp =  SPI0->SPI_RDR;

  // write dummy byte with address and end transmission flag
  SPI0->SPI_TDR = 0x000000FF | SPI_PCS(chan) | SPI_TDR_LASTXFER;

  // wait for receive register
  while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
  // get byte from receive register
  return SPI0->SPI_RDR;
}
#endif
// Read from SPI into buffer
void HAL::spiReadBlock(uint8_t*buf, uint16_t nbyte)
{
  //if (nbyte-- == 0) return;
	if (nbyte == 0) return;
  for (int i = 0; i < nbyte; i++)
  {
  	#if 0 
    //while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
    SPI0->SPI_TDR = 0x000000FF | SPI_PCS(SPI_CHAN);
    while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
    buf[i] = SPI0->SPI_RDR;
    // delayMicroseconds(1);
	#endif      
  }
  buf[nbyte] = spiReceive();
}

// Write from buffer to SPI

void HAL::spiSendBlock(uint8_t token, const uint8_t* buf)
{
#if 0		
  SPI0->SPI_TDR = (uint32_t)token | SPI_PCS(SPI_CHAN);
  while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
  //while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
  //SPI0->SPI_RDR;
  for (int i = 0; i < 511; i++)
  {
    SPI0->SPI_TDR = (uint32_t)buf[i] | SPI_PCS(SPI_CHAN);
    while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
    while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
    SPI0->SPI_RDR;
    //        delayMicroseconds(1);

  }
  spiSend(buf[511]);
#endif   
}
#endif

/****************************************************************************************
 Setting for I2C Clock speed. needed to change  clock speed for different peripherals
****************************************************************************************/


void HAL::microsecondsWait(uint32_t us) //skyblue add
{
    uint32_t usStart, goal;
//dump
#if 0
    // get the current count
    usStart =  TC_ReadCV(DELAY_TIMER, DELAY_TIMER_CHANNEL);

    // funny math here to give good accuracy with no overflow 
    goal = usStart + 10*us-5; //((F_CPU_TRUE / (DELAY_TIMER_PRESCALE * 100000)) * us) / 10;

    // goal may have wrapped, if so wait for counter to catch up
    if(goal < usStart) {
        while(goal < TC_ReadCV(DELAY_TIMER, DELAY_TIMER_CHANNEL));
    }   
    // wait for counter to reach requested value
    while(goal > TC_ReadCV(DELAY_TIMER, DELAY_TIMER_CHANNEL));
#endif
}


#if FEATURE_SERVO
// may need further restrictions here in the future
#if defined (__SAM3X8E__)
unsigned int HAL::servoTimings[4] = {0, 0, 0, 0};
static uint8_t servoIndex = 0;
unsigned int servoAutoOff[4] = {0, 0, 0, 0};
void HAL::servoMicroseconds(uint8_t servo, int microsec, uint16_t autoOff) {
  if (microsec < 500) microsec = 0;
  if (microsec > 2500) microsec = 2500;
  servoTimings[servo] = (unsigned int)(((F_CPU_TRUE / SERVO_PRESCALE) /
                                        1000000) * microsec);
  servoAutoOff[servo] = (microsec) ? (autoOff / 20) : 0;
}


// ================== Interrupt handling ======================

// Servo timer Interrupt handler
void SERVO_COMPA_VECTOR ()
{
  InterruptProtectedBlock noInt;
  static uint32_t     interval;

  // apparently have to read status register
  TC_GetStatus(SERVO_TIMER, SERVO_TIMER_CHANNEL);

  switch (servoIndex) {
    case 0:
      if (HAL::servoTimings[0]) {
#if SERVO0_PIN > -1
        WRITE(SERVO0_PIN, HIGH);
#endif
        interval =  HAL::servoTimings[0];
      } else
        interval = SERVO2500US;
      TC_SetRC(SERVO_TIMER, SERVO_TIMER_CHANNEL, interval);
      break;
    case 1:
#if SERVO0_PIN > -1
      WRITE(SERVO0_PIN, LOW);
#endif
      TC_SetRC(SERVO_TIMER, SERVO_TIMER_CHANNEL,
               SERVO5000US - interval);
      break;
    case 2:
      if (HAL::servoTimings[1]) {
#if SERVO1_PIN > -1
        WRITE(SERVO1_PIN, HIGH);
#endif
        interval =  HAL::servoTimings[1];
      } else
        interval = SERVO2500US;
      TC_SetRC(SERVO_TIMER, SERVO_TIMER_CHANNEL, interval);
      break;
    case 3:
#if SERVO1_PIN > -1
      WRITE(SERVO1_PIN, LOW);
#endif
      TC_SetRC(SERVO_TIMER, SERVO_TIMER_CHANNEL,
               SERVO5000US - interval);
      break;
    case 4:
      if (HAL::servoTimings[2]) {
#if SERVO2_PIN > -1
        WRITE(SERVO2_PIN, HIGH);
#endif
        interval =  HAL::servoTimings[2];
      } else
        interval = SERVO2500US;
      TC_SetRC(SERVO_TIMER, SERVO_TIMER_CHANNEL, interval);
      break;
    case 5:
#if SERVO2_PIN > -1
      WRITE(SERVO2_PIN, LOW);
#endif
      TC_SetRC(SERVO_TIMER, SERVO_TIMER_CHANNEL,
               SERVO5000US - interval);
      break;
    case 6:
      if (HAL::servoTimings[3]) {
#if SERVO3_PIN > -1
        WRITE(SERVO3_PIN, HIGH);
#endif
        interval =  HAL::servoTimings[3];
      } else
        interval = SERVO2500US;
      TC_SetRC(SERVO_TIMER, SERVO_TIMER_CHANNEL, interval);
      break;
    case 7:
#if SERVO3_PIN > -1
      WRITE(SERVO3_PIN, LOW);
#endif
      TC_SetRC(SERVO_TIMER, SERVO_TIMER_CHANNEL,
               SERVO5000US - interval);
      break;
  }
  if (servoIndex & 1)
  {
    uint8_t nr = servoIndex >> 1;
    if (servoAutoOff[nr])
    {
      servoAutoOff[nr]--;
      if (servoAutoOff[nr] == 0) HAL::servoTimings[nr] = 0;
    }
  }
  servoIndex++;
  if (servoIndex > 7) servoIndex = 0;
}
#else
#error No servo support for your board, please diable FEATURE_SERVO
#endif
#endif

//TcChannel *stepperChannel = (TIMER1_TIMER->TC_CHANNEL + TIMER1_TIMER_CHANNEL);
#ifndef STEPPERTIMER_EXIT_TICKS
#define STEPPERTIMER_EXIT_TICKS 105 // at least 2,5us pause between stepper calls
#endif

/** \brief Timer interrupt routine to drive the stepper motors.
*/
unsigned long mksTimer2Cnt=0000;
void TIMER1_COMPA_VECTOR (){return;}
extern "C" void STEPPER_MOTORS_VECTOR ();
void STEPPER_MOTORS_VECTOR ()
{
/*
  mksTimer2Cnt++;
  if(mksTimer2Cnt%2==0)
    XDIR_OP = 1;
  else
    XDIR_OP = 0;  
 */
  
  volatile uint32_t delay;
  if (PrintLine::hasLines())
  {
  	if(Change_Repetier_Define.def_NONLINEAR_SYSTEM == 1)
  	{
  		delay = PrintLine::bresenhamStep_nonlinear_system();
  	}
	else
	{
    	delay = PrintLine::bresenhamStep();
	}
    HAL::allowInterrupts();
  }
  else if (Printer::zBabystepsMissing != 0) {
    Printer::zBabystep();
    delay = Printer::interval;
  } else {
    if (waitRelax == 0)
    {
#if USE_ADVANCE
      if (Printer::advanceStepsSet)
      {
        Printer::extruderStepsNeeded -= Printer::advanceStepsSet;
#if ENABLE_QUADRATIC_ADVANCE
        Printer::advanceExecuted = 0;
#endif
        Printer::advanceStepsSet = 0;
      }
      if ((!Printer::extruderStepsNeeded) && (DISABLE_E))
        Extruder::disableCurrentExtruderMotor();
#else
      if (DISABLE_E) Extruder::disableCurrentExtruderMotor();
#endif
    }
    else waitRelax--;
    
    delay = 10000;    //480us                 T=130min
  }
  InterruptProtectedBlock noInt;
  if(delay < MAX_PULSE_FREQUENCY) 
    delay=MAX_PULSE_FREQUENCY;            //最大脉冲频率为200kHz

	htim2.Instance->ARR =(uint32_t)(delay*0.8); 		
    htim2.Instance->CNT = 0;
		


}
extern "C" void mksPwrDET_IRQHandler ();
void mksPwrDET_IRQHandler ()
{
	if(sd.sdmode)
	{
	  PWRDN_FLG = 1;
	}
}

extern "C" void mksPwrPWC_IRQHandler ();
void mksPwrPWC_IRQHandler ()
{
	//GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	MKS_PW_OFF_OP = 0;
        //skyblue-2017-01-04
}


extern "C" void mksPwrDn_IRQHandler ();
void mksPwrDn_IRQHandler ()
{
#if 0  
  /*-------------TEST--------------*/
  //使用1F电容，存储数据大于50组
  //复位后，可查看virtualEeprom数组，判断写入EEPROM的数据是否正确
    EXT_3V3_CTRL = EXT3V3_OFF;  // 关闭外部3.3V电源
    for(int i=0;i<50;i++)
      HAL::eprSetInt32(EPR_SAV_FLAG+i*4,0xD0000000+i*2);
    
    //HAL::eprSetFloat(EPR_SAV_FLAG+i*4,0x12345678);

    FALA_5V_CTRL = FALA_OFF;    // 关闭法拉电容
    while(1);

  /*-------------TEST--------------*/
#endif
    
  if(sd.sdmode)
  {
  	//if(gCfgItems.have_ups == 1)
  	//{
	//	PWRDN_FLG = 1;		
        //return;
  	//}
	//else
    {
	    EXT_3V3_CTRL = EXT3V3_OFF;  // 关闭外部3.3V电源
    
	    HAL::eprSetByte(EPR_SAV_FLAG,(uint8_t)PRINTER_PWRDN_REPRINT);        //断电标志置位
	    //直接断电时，立即更新参数，然后保存
	    Printer::memoryZ = Printer::currentPosition[Z_AXIS];
	    Printer::memoryF = Printer::feedrate;
	    sd.mksWriteToEpr();
    
	  //关闭温度，非必须
	    for(uint8_t i = 0; i < CfgPrinterItems.cfg_num_extruder; i++)
	      Extruder::setTemperatureForExtruder(0, i);
	    if(CfgPrinterItems.cfg_have_heated_bed == 1)
	      Extruder::setHeatedBedTemperature(0);

	    FALA_5V_CTRL = FALA_OFF;    // 关闭法拉电容
	    while(1);
	}
  }
  FALA_5V_CTRL = FALA_OFF;    // 关闭法拉电容
}



#if !defined(HEATER_PWM_SPEED)
#define HEATER_PWM_SPEED 0
#endif
#if HEATER_PWM_SPEED < 0
#define HEATER_PWM_SPEED 0
#endif
#if HEATER_PWM_SPEED > 2
#define HEATER_PWM_SPEED 2
#endif

#if HEATER_PWM_SPEED == 0
#define HEATER_PWM_STEP 1
#define HEATER_PWM_MASK 255
#elif HEATER_PWM_SPEED == 1
#define HEATER_PWM_STEP 2
#define HEATER_PWM_MASK 254
#else
#define HEATER_PWM_STEP 4
#define HEATER_PWM_MASK 252
#endif

#if !defined(COOLER_PWM_SPEED)
#define COOLER_PWM_SPEED 0
#endif
#if COOLER_PWM_SPEED < 0
#define COOLER_PWM_SPEED 0
#endif
#if COOLER_PWM_SPEED > 2
#define COOLER_PWM_SPEED 2
#endif

#if COOLER_PWM_SPEED == 0
#define COOLER_PWM_STEP 1
#define COOLER_PWM_MASK 255
#elif COOLER_PWM_SPEED == 1
#define COOLER_PWM_STEP 2
#define COOLER_PWM_MASK 254
#else
#define COOLER_PWM_STEP 4
#define COOLER_PWM_MASK 252
#endif

#define pulseDensityModulate( pin, density,error,invert) {uint8_t carry;carry = error + (invert ? 255 - density : density); WRITE(pin, (carry < error)); error = carry;}

/**
This timer is called 3906 times per second. It is used to update
pwm values for heater and some other frequent jobs.
*/
void PWM_TIMER_VECTOR (){return;}
extern "C" void PWM_MKS_VECTOR ();
void PWM_MKS_VECTOR ()  //void PWM_TIMER_VECTOR ()

{
  //InterruptProtectedBlock noInt;
  // apparently have to read status register
  //TC_GetStatus(PWM_TIMER, PWM_TIMER_CHANNEL);

  static uint8_t pwm_count_cooler = 0;
  static uint8_t pwm_count_heater = 0;
 // static uint8_t pwm_pos_set[NUM_PWM];
 // static uint8_t pwm_cooler_pos_set[NUM_EXTRUDER];
 static uint8_t pwm_pos_set[11];
 static uint8_t pwm_cooler_pos_set[6];
 
  if (pwm_count_heater == 0 && !PDM_FOR_EXTRUDER)
  {
#if defined(EXT0_HEATER_PIN) && EXT0_HEATER_PIN > -1
    if ((pwm_pos_set[0] = (pwm_pos[0] & HEATER_PWM_MASK)) > 0) WRITE(EXT0_HEATER_PIN, !HEATER_PINS_INVERTED);
#endif
#if defined(EXT1_HEATER_PIN) && EXT1_HEATER_PIN > -1 && !MIXING_EXTRUDER
if(CfgPrinterItems.cfg_num_extruder > 1)
{
    if ((pwm_pos_set[1] = (pwm_pos[1] & HEATER_PWM_MASK)) > 0) WRITE(EXT1_HEATER_PIN, !HEATER_PINS_INVERTED);
}
#endif
#if defined(EXT2_HEATER_PIN) && EXT2_HEATER_PIN > -1 && !MIXING_EXTRUDER
if(CfgPrinterItems.cfg_num_extruder > 2)
{
    if ((pwm_pos_set[2] = (pwm_pos[2] & HEATER_PWM_MASK)) > 0) WRITE(EXT2_HEATER_PIN, !HEATER_PINS_INVERTED);
}
#endif
#if defined(EXT3_HEATER_PIN) && EXT3_HEATER_PIN > -1 && !MIXING_EXTRUDER
if(CfgPrinterItems.cfg_num_extruder > 3)
{
    if ((pwm_pos_set[3] = (pwm_pos[3] & HEATER_PWM_MASK)) > 0) WRITE(EXT3_HEATER_PIN, !HEATER_PINS_INVERTED);
}
#endif
#if defined(EXT4_HEATER_PIN) && EXT4_HEATER_PIN > -1 && !MIXING_EXTRUDER
if(CfgPrinterItems.cfg_num_extruder > 4)
{
    if ((pwm_pos_set[4] = (pwm_pos[4] & HEATER_PWM_MASK)) > 0) WRITE(EXT4_HEATER_PIN, !HEATER_PINS_INVERTED);
}
#endif
#if defined(EXT5_HEATER_PIN) && EXT5_HEATER_PIN > -1 && !MIXING_EXTRUDER
if(CfgPrinterItems.cfg_num_extruder > 5)
{
    if ((pwm_pos_set[5] = (pwm_pos[5] & HEATER_PWM_MASK)) > 0) WRITE(EXT5_HEATER_PIN, !HEATER_PINS_INVERTED);
}
#endif
#if HEATED_BED_HEATER_PIN > -1
if(CfgPrinterItems.cfg_have_heated_bed == 1)
{
    if ((pwm_pos_set[CfgPrinterItems.cfg_num_extruder] = pwm_pos[CfgPrinterItems.cfg_num_extruder]) > 0) WRITE(HEATED_BED_HEATER_PIN, !HEATER_PINS_INVERTED);
}
#endif
  }
  if (pwm_count_cooler == 0 && !PDM_FOR_COOLER)
  {
#if defined(EXT0_HEATER_PIN) && EXT0_HEATER_PIN > -1 && EXT0_EXTRUDER_COOLER_PIN > -1
    if ((pwm_cooler_pos_set[0] = (extruder[0].coolerPWM & COOLER_PWM_MASK)) > 0) WRITE(EXT0_EXTRUDER_COOLER_PIN, 1);
#endif
#if !SHARED_COOLER && defined(EXT1_HEATER_PIN) && EXT1_HEATER_PIN > -1
if(CfgPrinterItems.cfg_num_extruder > 1)
{
#if EXT1_EXTRUDER_COOLER_PIN > -1 && EXT1_EXTRUDER_COOLER_PIN != EXT0_EXTRUDER_COOLER_PIN
    if ((pwm_cooler_pos_set[1] = (extruder[1].coolerPWM & COOLER_PWM_MASK)) > 0) WRITE(EXT1_EXTRUDER_COOLER_PIN, 1);
#endif
}
#endif
#if !SHARED_COOLER && defined(EXT2_HEATER_PIN) && EXT2_HEATER_PIN > -1 
if(CfgPrinterItems.cfg_num_extruder > 2)
{
#if EXT2_EXTRUDER_COOLER_PIN>-1
    if ((pwm_cooler_pos_set[2] = (extruder[2].coolerPWM & COOLER_PWM_MASK)) > 0) WRITE(EXT2_EXTRUDER_COOLER_PIN, 1);
#endif
}
#endif
#if !SHARED_COOLER && defined(EXT3_HEATER_PIN) && EXT3_HEATER_PIN > -1 
if(CfgPrinterItems.cfg_num_extruder > 3)
{
#if EXT3_EXTRUDER_COOLER_PIN>-1
    if ((pwm_cooler_pos_set[3] = (extruder[3].coolerPWM & COOLER_PWM_MASK)) > 0) WRITE(EXT3_EXTRUDER_COOLER_PIN, 1);
#endif
}
#endif
#if !SHARED_COOLER && defined(EXT4_HEATER_PIN) && EXT4_HEATER_PIN > -1 
if(CfgPrinterItems.cfg_num_extruder > 4)
{
#if EXT4_EXTRUDER_COOLER_PIN>-1
    if ((pwm_cooler_pos_set[4] = (extruder[4].coolerPWM & COOLER_PWM_MASK)) > 0) WRITE(EXT4_EXTRUDER_COOLER_PIN, 1);
#endif
}
#endif
#if !SHARED_COOLER && defined(EXT5_HEATER_PIN) && EXT5_HEATER_PIN > -1 
if(CfgPrinterItems.cfg_num_extruder > 1)
{
#if EXT5_EXTRUDER_COOLER_PIN>-1
    if ((pwm_cooler_pos_set[5] = (extruder[5].coolerPWM & COOLER_PWM_MASK)) > 0) WRITE(EXT5_EXTRUDER_COOLER_PIN, 1);
#endif
}
#endif
#if FAN_BOARD_PIN > -1 && SHARED_COOLER_BOARD_EXT == 0
        if((pwm_pos_set[Change_Repetier_Define.def_PWM_BOARD_FAN] = (pwm_pos[Change_Repetier_Define.def_PWM_BOARD_FAN] & COOLER_PWM_MASK)) > 0) WRITE(FAN_BOARD_PIN,1);
#endif
#if FAN_PIN > -1 && FEATURE_FAN_CONTROL
        if((pwm_pos_set[Change_Repetier_Define.def_PWM_FAN1] = (pwm_pos[Change_Repetier_Define.def_PWM_FAN1] & COOLER_PWM_MASK)) > 0) /*FAN_OP=1;*/WRITE(FAN_PIN,1);
#endif
#if FAN2_PIN > -1 && FEATURE_FAN2_CONTROL
		if((pwm_pos_set[Change_Repetier_Define.def_PWM_FAN2] = (pwm_pos[Change_Repetier_Define.def_PWM_FAN2] & COOLER_PWM_MASK)) > 0) WRITE(FAN2_PIN,1);
#endif
#if defined(FAN_THERMO_PIN) && FAN_THERMO_PIN > -1
		if((pwm_pos_set[Change_Repetier_Define.def_PWM_FAN_THERMO] = (pwm_pos[Change_Repetier_Define.def_PWM_FAN_THERMO] & COOLER_PWM_MASK)) > 0) WRITE(FAN_THERMO_PIN,1);
#endif
  }
#if defined(EXT0_HEATER_PIN) && EXT0_HEATER_PIN > -1
#if PDM_FOR_EXTRUDER
  pulseDensityModulate(EXT0_HEATER_PIN, pwm_pos[0], pwm_pos_set[0], HEATER_PINS_INVERTED);
#else
  if (pwm_pos_set[0] == pwm_count_heater && pwm_pos_set[0] != HEATER_PWM_MASK) WRITE(EXT0_HEATER_PIN, HEATER_PINS_INVERTED);
#endif
#if EXT0_EXTRUDER_COOLER_PIN > -1
#if PDM_FOR_COOLER
  pulseDensityModulate(EXT0_EXTRUDER_COOLER_PIN, extruder[0].coolerPWM, pwm_cooler_pos_set[0], false);
#else
  if (pwm_cooler_pos_set[0] == pwm_count_cooler && pwm_cooler_pos_set[0] != COOLER_PWM_MASK) WRITE(EXT0_EXTRUDER_COOLER_PIN, 0);
#endif
#endif
#endif
#if defined(EXT1_HEATER_PIN) && EXT1_HEATER_PIN > -1 && !MIXING_EXTRUDER
if(CfgPrinterItems.cfg_num_extruder > 1)
{
#if PDM_FOR_EXTRUDER
  pulseDensityModulate(EXT1_HEATER_PIN, pwm_pos[1], pwm_pos_set[1], HEATER_PINS_INVERTED);
#else
  if (pwm_pos_set[1] == pwm_count_heater && pwm_pos_set[1] != HEATER_PWM_MASK) WRITE(EXT1_HEATER_PIN, HEATER_PINS_INVERTED);
#endif
#if !SHARED_COOLER && defined(EXT1_EXTRUDER_COOLER_PIN) && EXT1_EXTRUDER_COOLER_PIN > -1 && EXT1_EXTRUDER_COOLER_PIN != EXT0_EXTRUDER_COOLER_PIN
#if PDM_FOR_COOLER
  pulseDensityModulate(EXT1_EXTRUDER_COOLER_PIN, extruder[1].coolerPWM, pwm_cooler_pos_set[1], false);
#else
  if (pwm_cooler_pos_set[1] == pwm_count_cooler && pwm_cooler_pos_set[1] != COOLER_PWM_MASK) WRITE(EXT1_EXTRUDER_COOLER_PIN, 0);
#endif
#endif
}
#endif
#if defined(EXT2_HEATER_PIN) && EXT2_HEATER_PIN > -1 && !MIXING_EXTRUDER
if(CfgPrinterItems.cfg_num_extruder > 2)
{
#if PDM_FOR_EXTRUDER
  pulseDensityModulate(EXT2_HEATER_PIN, pwm_pos[2], pwm_pos_set[2], HEATER_PINS_INVERTED);
#else
  if (pwm_pos_set[2] == pwm_count_heater && pwm_pos_set[2] != HEATER_PWM_MASK) WRITE(EXT2_HEATER_PIN, HEATER_PINS_INVERTED);
#endif
#if !SHARED_COOLER && EXT2_EXTRUDER_COOLER_PIN > -1
#if PDM_FOR_COOLER
  pulseDensityModulate(EXT2_EXTRUDER_COOLER_PIN, extruder[2].coolerPWM, pwm_cooler_pos_set[2], false);
#else
  if (pwm_cooler_pos_set[2] == pwm_count_cooler && pwm_cooler_pos_set[2] != COOLER_PWM_MASK) WRITE(EXT2_EXTRUDER_COOLER_PIN, 0);
#endif
#endif
}
#endif
#if defined(EXT3_HEATER_PIN) && EXT3_HEATER_PIN > -1 && !MIXING_EXTRUDER
if(CfgPrinterItems.cfg_num_extruder > 3)
{
#if PDM_FOR_EXTRUDER
  pulseDensityModulate(EXT3_HEATER_PIN, pwm_pos[3], pwm_pos_set[3], HEATER_PINS_INVERTED);
#else
  if (pwm_pos_set[3] == pwm_count_heater && pwm_pos_set[3] != HEATER_PWM_MASK) WRITE(EXT3_HEATER_PIN, HEATER_PINS_INVERTED);
#endif
#if !SHARED_COOLER && EXT3_EXTRUDER_COOLER_PIN > -1
#if PDM_FOR_COOLER
  pulseDensityModulate(EXT3_EXTRUDER_COOLER_PIN, extruder[3].coolerPWM, pwm_cooler_pos_set[3], false);
#else
  if (pwm_cooler_pos_set[3] == pwm_count_cooler && pwm_cooler_pos_set[3] != COOLER_PWM_MASK) WRITE(EXT3_EXTRUDER_COOLER_PIN, 0);
#endif
#endif
}
#endif
#if defined(EXT4_HEATER_PIN) && EXT4_HEATER_PIN > -1 && !MIXING_EXTRUDER
if(CfgPrinterItems.cfg_num_extruder > 4)
{
#if PDM_FOR_EXTRUDER
  pulseDensityModulate(EXT4_HEATER_PIN, pwm_pos[4], pwm_pos_set[4], HEATER_PINS_INVERTED);
#else
  if (pwm_pos_set[4] == pwm_count_heater && pwm_pos_set[4] != HEATER_PWM_MASK) WRITE(EXT4_HEATER_PIN, HEATER_PINS_INVERTED);
#endif
#if !SHARED_COOLER && EXT4_EXTRUDER_COOLER_PIN > -1
#if PDM_FOR_COOLER
  pulseDensityModulate(EXT4_EXTRUDER_COOLER_PIN, extruder[4].coolerPWM, pwm_cooler_pos_set[4], false);
#else
  if (pwm_cooler_pos_set[4] == pwm_count_cooler && pwm_cooler_pos_set[4] != COOLER_PWM_MASK) WRITE(EXT4_EXTRUDER_COOLER_PIN, 0);
#endif
#endif
}
#endif
#if defined(EXT5_HEATER_PIN) && EXT5_HEATER_PIN > -1 && !MIXING_EXTRUDER
if(CfgPrinterItems.cfg_num_extruder > 5)
{
#if PDM_FOR_EXTRUDER
  pulseDensityModulate(EXT5_HEATER_PIN, pwm_pos[5], pwm_pos_set[5], HEATER_PINS_INVERTED);
#else
  if (pwm_pos_set[5] == pwm_count_heater && pwm_pos_set[5] != HEATER_PWM_MASK) WRITE(EXT5_HEATER_PIN, HEATER_PINS_INVERTED);
#endif
#if !SHARED_COOLER && EXT5_EXTRUDER_COOLER_PIN > -1
#if PDM_FOR_COOLER
  pulseDensityModulate(EXT5_EXTRUDER_COOLER_PIN, extruder[5].coolerPWM, pwm_cooler_pos_set[5], false);
#else
  if (pwm_cooler_pos_set[5] == pwm_count_cooler && pwm_cooler_pos_set[5] != COOLER_PWM_MASK) WRITE(EXT5_EXTRUDER_COOLER_PIN, 0);
#endif
#endif
}
#endif
#if FAN_BOARD_PIN > -1  && SHARED_COOLER_BOARD_EXT == 0
#if PDM_FOR_COOLER
    pulseDensityModulate(FAN_BOARD_PIN, pwm_pos[Change_Repetier_Define.def_PWM_BOARD_FAN], pwm_pos_set[Change_Repetier_Define.def_PWM_BOARD_FAN], false);
#else
    if(pwm_pos_set[Change_Repetier_Define.def_PWM_BOARD_FAN] == pwm_count_cooler && pwm_pos_set[CfgPrinterItems.cfg_num_extruder + 1] != COOLER_PWM_MASK) WRITE(FAN_BOARD_PIN,0);
#endif
#endif
#if FAN_PIN > -1 && FEATURE_FAN_CONTROL
    if(fanKickstart == 0)
    {
#if PDM_FOR_COOLER
        pulseDensityModulate(FAN_PIN, pwm_pos[Change_Repetier_Define.def_PWM_FAN1], pwm_pos_set[Change_Repetier_Define.def_PWM_FAN1], false);
#else
        if(pwm_pos_set[Change_Repetier_Define.def_PWM_FAN1] == pwm_count_cooler && pwm_pos_set[Change_Repetier_Define.def_PWM_FAN1] != COOLER_PWM_MASK) /*FAN_OP=0;*/WRITE(FAN_PIN,0);
#endif
    }
#endif
#if FAN2_PIN > -1 && FEATURE_FAN2_CONTROL
if(fan2Kickstart == 0)
{
	#if PDM_FOR_COOLER
	pulseDensityModulate(FAN2_PIN, pwm_pos[Change_Repetier_Define.def_PWM_FAN2], pwm_pos_set[Change_Repetier_Define.def_PWM_FAN2], false);
	#else
	if(pwm_pos_set[Change_Repetier_Define.def_PWM_FAN2] == pwm_count_cooler && pwm_pos_set[Change_Repetier_Define.def_PWM_FAN2] != COOLER_PWM_MASK) WRITE(FAN2_PIN,0);
	#endif
}
#endif
#if defined(FAN_THERMO_PIN) && FAN_THERMO_PIN > -1
	#if PDM_FOR_COOLER
	pulseDensityModulate(FAN_THERMO_PIN, pwm_pos[Change_Repetier_Define.def_PWM_FAN_THERMO], pwm_pos_set[Change_Repetier_Define.def_PWM_FAN_THERMO], false);
	#else
	if(pwm_pos_set[PWM_FAN_THERMO] == pwm_count_cooler && pwm_pos_set[Change_Repetier_Define.def_PWM_FAN_THERMO] != COOLER_PWM_MASK) WRITE(FAN_THERMO_PIN,0);
	#endif
#endif
#if HEATED_BED_HEATER_PIN > -1
if(CfgPrinterItems.cfg_have_heated_bed == 1)
{
#if PDM_FOR_EXTRUDER
  pulseDensityModulate(HEATED_BED_HEATER_PIN, pwm_pos[CfgPrinterItems.cfg_num_extruder], pwm_pos_set[CfgPrinterItems.cfg_num_extruder], HEATER_PINS_INVERTED);
#else
  if (pwm_pos_set[CfgPrinterItems.cfg_num_extruder] == pwm_count_heater && pwm_pos_set[CfgPrinterItems.cfg_num_extruder] != HEATER_PWM_MASK) WRITE(HEATED_BED_HEATER_PIN, HEATER_PINS_INVERTED);
#endif
}
#endif



  //noInt.unprotect();
  counterPeriodical++; // Appxoimate a 100ms timer
  //if (counterPeriodical >= 390) //  (int)(F_CPU/40960))
  if (counterPeriodical >= 80) //  (int)(F_CPU/40960))  
  {
    counterPeriodical = 0;
    executePeriodical = 1;
#if FEATURE_FAN_CONTROL
    if (fanKickstart) fanKickstart--;
#endif
#if FEATURE_FAN2_CONTROL
    if (fan2Kickstart) fan2Kickstart--;
#endif
  }


// read analog values -- only read one per interrupt
//#if ANALOG_INPUTS > 0
if(Change_Repetier_Define.def_ANALOG_INPUTS > 0)
  // conversion finished?
  //if ((ADC->ADC_ISR & adcEnable) == adcEnable) //modify by skyblue
  {
    //adcCounter++;
    /*
    uhADCxConvertedValue[0]	----	TB
    uhADCxConvertedValue[1]	----	TH1
    uhADCxConvertedValue[2]	----	TH2
    
	*/
	/*
    for (int i = 0; i < Change_Repetier_Define.def_ANALOG_INPUTS; i++) {
			osAnalogInputBuildup[i] += uhADCxConvertedValue[i];          //skyblue adc   CfgPrinterItems.cfg_ext0_tempsensor_type
    }*/
        for(int i=0;i<3;i++)
        {
          uhADCxConvertedValue[i] += TEMPERATURE_COMPENSATION;
          if(uhADCxConvertedValue[i] > 1023)   
            uhADCxConvertedValue[i]=1023; 
        }
	switch(Change_Repetier_Define.def_ANALOG_INPUTS)
		{
		case 1:
			osAnalogInputBuildup[0] += uhADCxConvertedValue[1];
			break;
		case 2:
			if(CfgPrinterItems.cfg_have_heated_bed ==1)
				{
				osAnalogInputBuildup[0] += uhADCxConvertedValue[1];
				osAnalogInputBuildup[1] += uhADCxConvertedValue[0];
				}
			else
				{
				osAnalogInputBuildup[0] += uhADCxConvertedValue[1];
				osAnalogInputBuildup[1] += uhADCxConvertedValue[2];
				}
			break;
		case 3:
			osAnalogInputBuildup[0] += uhADCxConvertedValue[1];
			osAnalogInputBuildup[1] += uhADCxConvertedValue[2];
			osAnalogInputBuildup[2] += uhADCxConvertedValue[0];
			break;
		default:
			break;
		}
	
      if(++adcCounter >= (1 << ANALOG_INPUT_SAMPLE))
      {
        for(int i = 0; i < Change_Repetier_Define.def_ANALOG_INPUTS; i++) {
        osAnalogInputValues[i] = osAnalogInputBuildup[i] >>(ANALOG_INPUT_BITS+ANALOG_INPUT_SAMPLE-12);
        osAnalogInputBuildup[i] = 0;
        }
        adcCounter=0;
      }
     // for i
    /*
    if (adcCounter >= NUM_ADC_SAMPLES) {
      adcCounter = 0;
      adcSamplePos++;
      if (adcSamplePos >= ANALOG_INPUT_MEDIAN)
        adcSamplePos = 0;
    }*/
    //ADC->ADC_CR = ADC_CR_START; // reread values
  }

//#endif // ANALOG_INPUTS > 0
  pwm_count_cooler += COOLER_PWM_STEP;
  pwm_count_heater += HEATER_PWM_STEP;
  UI_FAST; // Short timed user interface action
#if FEATURE_WATCHDOG
  if(HAL::wdPinged) {
     WDT->WDT_CR = 0xA5000001;
     HAL::wdPinged = false;
  }
#endif
}

//Tan0906
void HAL::get_Temperature_ad_value()
{
#if 0
	
	// read analog values -- only read one per interrupt
	//#if ANALOG_INPUTS > 0
	if(Change_Repetier_Define.def_ANALOG_INPUTS > 0)
	  // conversion finished?
	  //if ((ADC->ADC_ISR & adcEnable) == adcEnable) //modify by skyblue
	  {
		//adcCounter++;
		for (int i = 0; i < Change_Repetier_Define.def_ANALOG_INPUTS; i++) {
		  //int32_t cur = ADC->ADC_CDR[osAnalogInputChannels[i]];
		  //osAnalogInputBuildup[i] += cur;
		  int32_t cur = analogRead(osAnalogInputChannels[i]);
		  osAnalogInputBuildup[i] += cur;		   //skyblue adc
		  //adcSamplesMin[i] = RMath::min(adcSamplesMin[i], cur);
		  //adcSamplesMax[i] = RMath::max(adcSamplesMax[i], cur);
		 /* if (adcCounter >= NUM_ADC_SAMPLES)	   // store new conversion result
		  {
			// Strip biggest and smallest value and round correctly
		   osAnalogInputBuildup[i] = osAnalogInputBuildup[i] + (1 << (ANALOG_INPUT_SAMPLE - 1)) - (adcSamplesMin[i] + adcSamplesMax[i]);
			adcSamplesMin[i] = 100000;
			adcSamplesMax[i] = 0;
			osAnalogSamplesSum[i] -= osAnalogSamples[i][adcSamplePos];
			osAnalogSamplesSum[i] += (osAnalogSamples[i][adcSamplePos] = osAnalogInputBuildup[i] >> ANALOG_INPUT_SAMPLE);
			osAnalogInputValues[i] = osAnalogSamplesSum[i] / ANALOG_INPUT_MEDIAN;
			osAnalogInputValues[i] = 3912;
			osAnalogInputBuildup[i] = 0;
			//osAnalogInputValues[i] = osAnalogInputBuildup[i] >>(ANALOG_INPUT_BITS+ANALOG_INPUT_SAMPLE-12);
			//osAnalogInputBuildup[i] = 0;
		  } // adcCounter >= NUM_ADC_SAMPLES
		  */
		}
		  if(++adcCounter >= (1 << ANALOG_INPUT_SAMPLE))
		  {
			for(int i = 0; i < Change_Repetier_Define.def_ANALOG_INPUTS; i++) {
			osAnalogInputValues[i] = osAnalogInputBuildup[i] >>(ANALOG_INPUT_BITS+ANALOG_INPUT_SAMPLE-12);
			osAnalogInputBuildup[i] = 0;
			}
			adcCounter=0;
		  }
		 // for i
		/*
		if (adcCounter >= NUM_ADC_SAMPLES) {
		  adcCounter = 0;
		  adcSamplePos++;
		  if (adcSamplePos >= ANALOG_INPUT_MEDIAN)
			adcSamplePos = 0;
		}*/
		//ADC->ADC_CR = ADC_CR_START; // reread values
	  }
#endif //tan0906

}



/** \brief Timer routine for extruder stepper.

Several methods need to move the extruder. To get a optimal
result, all methods update the printer_state.extruderStepsNeeded
with the number of additional steps needed. During this
interrupt, one step is executed. This will keep the extruder
moving, until the total wanted movement is achieved. This will
be done with the maximum allowable speed for the extruder.
*/
#if USE_ADVANCE
TcChannel *extruderChannel = (EXTRUDER_TIMER->TC_CHANNEL + EXTRUDER_TIMER_CHANNEL);
#define SLOW_EXTRUDER_TICKS  (F_CPU_TRUE / 32 / 1000) // 250us on direction change
#define NORMAL_EXTRUDER_TICKS  (F_CPU_TRUE / 32 / EXTRUDER_CLOCK_FREQ) // 500us on direction change
#ifndef ADVANCE_DIR_FILTER_STEPS
#define ADVANCE_DIR_FILTER_STEPS 2
#endif

static int extruderLastDirection = 0;
void HAL::resetExtruderDirection() {
  extruderLastDirection = 0;
}
// EXTRUDER_TIMER IRQ handler
void EXTRUDER_TIMER_VECTOR (){return;}
extern "C" void EXTRUDER_MKS_VECTOR ();
void EXTRUDER_MKS_VECTOR ()   //void EXTRUDER_TIMER_VECTOR ()
{
  return;
#if 0
  InterruptProtectedBlock noInt;
  // apparently have to read status register
  //TC_GetStatus(EXTRUDER_TIMER, EXTRUDER_TIMER_CHANNEL);
  extruderChannel->TC_SR; // faster replacement for above line!
  
  if (!Printer::isAdvanceActivated()) {
    return; // currently no need
  }
  if (!Printer::isAdvanceActivated()) return; // currently no need
  if (Printer::extruderStepsNeeded > 0 && extruderLastDirection != 1)
  {
    if(Printer::extruderStepsNeeded >= ADVANCE_DIR_FILTER_STEPS) {
      Extruder::setDirection(true);
      extruderLastDirection = 1;
      //extruderChannel->TC_RC = SLOW_EXTRUDER_TICKS;
      extruderChannel->TC_RC = Printer::maxExtruderSpeed;
    } else { 
      extruderChannel->TC_RC = Printer::maxExtruderSpeed;
    }
  }
  else if (Printer::extruderStepsNeeded < 0 && extruderLastDirection != -1)
  {
    if(-Printer::extruderStepsNeeded >= ADVANCE_DIR_FILTER_STEPS) {
      Extruder::setDirection(false);
      extruderLastDirection = -1;
      //extruderChannel->TC_RC = SLOW_EXTRUDER_TICKS;
      extruderChannel->TC_RC = Printer::maxExtruderSpeed;
   } else { 
      extruderChannel->TC_RC = Printer::maxExtruderSpeed;
   }
  } 
  else if (Printer::extruderStepsNeeded != 0)
  {
    Extruder::step();
    Printer::extruderStepsNeeded -= extruderLastDirection;
    extruderChannel->TC_RC = Printer::maxExtruderSpeed;
    Printer::insertStepperHighDelay();
    Extruder::unstep();
  }
#endif    
}
#endif

// IRQ handler for tone generator
void BEEPER_TIMER_VECTOR () {
  static bool     toggle;

  //TC_GetStatus(BEEPER_TIMER, BEEPER_TIMER_CHANNEL);

  //WRITE_VAR(tone_pin, toggle);
  WRITE(tone_pin, toggle);
  toggle = !toggle;
}

#if defined(BLUETOOTH_SERIAL) && BLUETOOTH_SERIAL > 0
RFDoubleSerial::RFDoubleSerial() {
}
void RFDoubleSerial::begin(unsigned long baud) {
  RFSERIAL.begin(baud);
  BT_SERIAL.begin(BLUETOOTH_BAUD);
}
void RFDoubleSerial::end() {
  RFSERIAL.end();
  BT_SERIAL.end();
}
int RFDoubleSerial::available(void) {
  int x = RFSERIAL.available();
  if (x > 0) return x;
  return BT_SERIAL.available();
}
int RFDoubleSerial::peek(void) {
  if(RFSERIAL.available())
    return RFSERIAL.peek();
  return BT_SERIAL.peek();
}
int RFDoubleSerial::read(void) {
  if(RFSERIAL.available())
    return RFSERIAL.read();
  return BT_SERIAL.read();
}
void RFDoubleSerial::flush(void) {
  RFSERIAL.flush();
  BT_SERIAL.flush();
}
size_t RFDoubleSerial::write(uint8_t c) {
  size_t r = RFSERIAL.write(c);
  BT_SERIAL.write(c);
  return r;
}
RFDoubleSerial BTAdapter;
#endif

// Dummy function to overload weak arduino function that always disables
// watchdog. We do not need that as we do this our self.
void watchdogSetup(void) {
}

/*----------------add-------------------*/

void HAL::pinMode( uint32_t ulPin, uint32_t ulMode )
{
#if 0		//skyblue modidy 2016-11-29	
	if ( ulPin > PINS_COUNT )
    {
        return ;
    }

  GPIO_TypeDef *gpio_port = g_APinDescription[ulPin].pPort;
  uint16_t gpio_pin = g_APinDescription[ulPin].ulPin;

  GPIO_InitTypeDef GPIO_InitStructure;

#if defined (STM32F10X_HD) || (STM32F10X_MD)
  RCC_APB2PeriphClockCmd(g_APinDescription[ulPin].ulPeripheral,ENABLE);
#elif defined (STM32F40_41xxx)
  RCC_AHB1PeriphClockCmd(g_APinDescription[ulPin].ulPeripheral,ENABLE);
#endif

  GPIO_InitStructure.GPIO_Pin = gpio_pin;

	switch ( ulMode )
    {
        case INPUT:
#if defined (STM32F10X_HD) || (STM32F10X_MD)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#elif defined (STM32F40_41xxx)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
          GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
#endif
        break ;

        case INPUT_PULLUP:
#if defined (STM32F10X_HD) || (STM32F10X_MD)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
#elif defined (STM32F40_41xxx)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
          GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
#endif
        break ;

        case INPUT_PULLDOWN:
#if defined (STM32F10X_HD) || (STM32F10X_MD)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
#elif defined (STM32F40_41xxx)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
          GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
#endif
        break;

        case OUTPUT:
#if defined (STM32F10X_HD) || (STM32F10X_MD)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
          GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#elif defined (STM32F40_41xxx)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
          GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
          GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
          GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#endif
        break ;

        case AF_OUTPUT_PUSHPULL:	//Used internally for Alternate Function Output PushPull(TIM, UART, SPI etc)
#if defined (STM32F10X_HD) || (STM32F10X_MD)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
          GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#elif defined (STM32F40_41xxx)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
          GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
          GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
          GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
#endif
        break;

        case AF_OUTPUT_DRAIN:		//Used internally for Alternate Function Output Drain(I2C etc)
#if defined (STM32F10X_HD) || (STM32F10X_MD)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
          GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#elif defined (STM32F40_41xxx)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
          GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
          GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#endif
        break;

        case AN_INPUT:				//Used internally for ADC Input
#if defined (STM32F10X_HD) || (STM32F10X_MD)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
#elif defined (STM32F40_41xxx)
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
          GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
#endif
        break;

        default:
        break ;
    }

  GPIO_Init(gpio_port, &GPIO_InitStructure);
#endif //skyblue modidy 2016-11-29    
}


void HAL::digitalWrite( uint32_t ulPin, uint32_t ulVal )
{
//#define FAN_PIN             0xa1
//#define HEATER1_PIN         0xa2
//#define HEATER2_PIN         0xa3
//#define BED_PIN             0xa4

  
	//skyblue modidy 2016-11-29
    switch(ulPin)
    {
    case MKS_FAN_PIN:
        MKS_FAN_OP = ulVal;
      break;
    case MKS_HEATER1_PIN:
        MKS_HEATER1_OP = ulVal;
      break;
    case MKS_HEATER2_PIN:
        MKS_HEATER2_OP = ulVal;
      break;
    case MKS_BED_PIN:
        MKS_BED_OP = ulVal;
      break;
    default:break;  
    }
}

int HAL::digitalRead( uint32_t ulPin )
{
#if 0		//skyblue modidy 2016-11-29	
	if ( ulPin > PINS_COUNT )
    {
        return LOW ;
    }

	return GPIO_ReadInputDataBit(g_APinDescription[ulPin].pPort,g_APinDescription[ulPin].ulPin) ;
#endif //skyblue modidy 2016-11-29	
}
volatile u16 delayCnt;	
void HAL::delay_us(volatile u32 nus)
{
      while(nus--)
      {
        delayCnt = 5;  //10,8,6 -ok          
        while(delayCnt--);
      }
	  //skyblue-2017-01-04

}
/*--------------------------MKS EEPROM-------------------------*/
#include "at24cxx.cpp"
/*--------------------------MKS EEPROM-------------------------*/
