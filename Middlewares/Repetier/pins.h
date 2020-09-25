#ifndef PINS_H
#define PINS_H


/*
The board assignment defines the capabilities of the motherboard and the used pins.
Each board definition follows the following scheme:

CPU_ARCH
  ARCH_AVR for AVR based boards
  ARCH_ARM for all arm based boards

STEPPER_CURRENT_CONTROL
  CURRENT_CONTROL_MANUAL  1  // mechanical poti, default if not defined
  CURRENT_CONTROL_DIGIPOT 2  // Use a digipot like RAMBO does
  CURRENT_CONTROL_LTC2600 3  // Use LTC2600 like Foltyn 3D Master
  CURRENT_CONTROL_ALLIGATOR 4  //Use External DAC like Alligator
*/

#define ARCH_AVR 1
#define ARCH_ARM 2
#define CPU_ARCH ARCH_ARM
#define CURRENT_CONTROL_MANUAL  1  // mechanical poti, default if not defined
#define CURRENT_CONTROL_DIGIPOT 2  // Use a digipot like RAMBO does
#define CURRENT_CONTROL_LTC2600 3  // Use LTC2600 like Foltyn 3D Master
#define CURRENT_CONTROL_ALLIGATOR 4  //Use External DAC like Alligator

/*
  arm does not have a eeprom build in. Therefore boards can add a
  eeprom. Board definition must set the right type of eeprom
*/

#define EEPROM_NONE 0
#define EEPROM_I2C  1
#define EEPROM_SPI_ALLIGATOR 2

/****************************************************************************/
// MKS RobinPro V1.0
// 
#if MOTHERBOARD == 519
#ifndef STM32F407xx
#error Oops!  Make sure you have 'MKS RobinPro Boards' .
#endif
#define MKS_FAN_PIN             0xa1    //WRITE()
#define MKS_HEATER1_PIN         0xa2    //WRITE()
#define MKS_HEATER2_PIN         0xa3    //WRITE()
#define MKS_BED_PIN             0xa4    //WRITE()

#define MKS_XMIN_PIN            0x21    //REG_IN
#define MKS_XMAX_PIN            0x22    //REG_IN
#define MKS_YMIN_PIN            0x23    //REG_IN
#define MKS_YMAX_PIN            0x24    //REG_IN
#define MKS_ZMIN_PIN            0x25    //REG_IN
#define MKS_ZMAX_PIN            0x26    //REG_IN

#define MKS_XSTP_PIN            0x31    //REG_OUT 
#define MKS_YSTP_PIN            0x32    //REG_OUT
#define MKS_ZSTP_PIN            0x33    //REG_OUT
#define MKS_E0STP_PIN           0x34    //REG_OUT
#define MKS_E1STP_PIN           0x35    //REG_OUT
   
#define MKS_XDIR_PIN            0x41    //REG_OUT & IN ---??? need test
#define MKS_YDIR_PIN            0x42    //REG_OUT & IN ---??? need test
#define MKS_ZDIR_PIN            0x43    //REG_OUT & IN ---??? need test
#define MKS_E0DIR_PIN           0x44    //REG_OUT
#define MKS_E1DIR_PIN           0x45    //REG_OUT

#define MKS_XENA_PIN            0x51    //REG_OUT
#define MKS_YENA_PIN            0x52    //REG_OUT
#define MKS_ZENA_PIN            0x53    //REG_OUT
#define MKS_E0ENA_PIN           0x54    //REG_OUT
#define MKS_E1ENA_PIN           0x55    //REG_OUT

#define KNOWN_BOARD
#define CPU_ARCH ARCH_ARM

//#define HEATER_PINS_INVERTED 1
#define HEATER_PINS_INVERTED 0          

/*****************************************************************
* MKS Robin V1.0 Pin Assignments 
******************************************************************/
/**/

#define ORIG_X_STEP_PIN     MKS_XSTP_PIN
#define ORIG_X_DIR_PIN      MKS_XDIR_PIN
#define ORIG_X_MIN_PIN      MKS_XMIN_PIN  
#define ORIG_X_MAX_PIN      MKS_XMAX_PIN
#define ORIG_X_ENABLE_PIN   MKS_XENA_PIN

#define ORIG_Y_STEP_PIN     MKS_YSTP_PIN
#define ORIG_Y_DIR_PIN      MKS_YDIR_PIN
#define ORIG_Y_MIN_PIN      MKS_YMIN_PIN
#define ORIG_Y_MAX_PIN      MKS_YMAX_PIN
#define ORIG_Y_ENABLE_PIN   MKS_YENA_PIN

#define ORIG_Z_STEP_PIN     MKS_ZSTP_PIN
#define ORIG_Z_DIR_PIN      MKS_ZDIR_PIN
#define ORIG_Z_MIN_PIN      MKS_ZMIN_PIN
#define ORIG_Z_MAX_PIN      MKS_ZMAX_PIN
#define ORIG_Z_ENABLE_PIN   MKS_ZENA_PIN


#define HEATER_0_PIN     MKS_HEATER1_PIN      //HEATER_EXTR1        //skyblue 2016-12-15
#define TEMP_0_PIN       1 // Due analog pin #54

#define HEATER_1_PIN     MKS_BED_PIN      //HEATER_BED              //skyblue 2016-12-15
#define TEMP_1_PIN       0 // Due analog pin #55

#define HEATER_2_PIN     MKS_HEATER2_PIN     //HEATER_EXTR2     //skyblue 2016-12-15    
#define TEMP_2_PIN       2 // Due analog pin #56

#define HEATER_3_PIN     MKS_FAN_PIN     //FAN_PWM                  //skyblue 2016-12-15

#define TEMP_3_PIN       -1 // Due analog pin #57

#define TEMP_4_PIN       -1 // Due analog pin #58

#define ORIG_E0_STEP_PIN    MKS_E0STP_PIN
#define ORIG_E0_DIR_PIN     MKS_E0DIR_PIN
#define ORIG_E0_ENABLE_PIN  MKS_E0ENA_PIN

#define ORIG_E1_STEP_PIN    MKS_E1STP_PIN
#define ORIG_E1_DIR_PIN     MKS_E1DIR_PIN
#define ORIG_E1_ENABLE_PIN  MKS_E1ENA_PIN

#define ORIG_E2_STEP_PIN    -1
#define ORIG_E2_DIR_PIN     -1
#define ORIG_E2_ENABLE_PIN  -1

#define SDSUPPORT      true  //skyblue mask
#define SDPOWER 	   -1
#define SDSS		   70 // 4,10,52 if using HW SPI.
#define ORIG_SDCARDDETECT  69
//#define SDSS		   -1
//#define SDCARDDETECT   69
#define SDCARDDETECTINVERTED false
#define LED_PIN 	   -1//46
#define ORIG_FAN_PIN 	   HEATER_3_PIN 
#define ORIG_FAN2_PIN       -1
//#define PS_ON_PIN      -1
#define KILL_PIN	   -1
#define SUICIDE_PIN    -1  //PIN that has to be turned on right after start, to keep power flowing.

#define SDA_PIN 				20  	// 20 or 70
#define SCL_PIN 				21  	// 21 or 71


#define E0_PINS ORIG_E0_STEP_PIN,ORIG_E0_DIR_PIN,ORIG_E0_ENABLE_PIN,
#define E1_PINS ORIG_E1_STEP_PIN,ORIG_E1_DIR_PIN,ORIG_E1_ENABLE_PIN,
#define E2_PINS ORIG_E2_STEP_PIN,ORIG_E2_DIR_PIN,ORIG_E2_ENABLE_PIN,

#define TWI_CLOCK_FREQ          400000
// see eeprom device data sheet for the following values these are for 24xx256
#define EEPROM_SERIAL_ADDR      0x50   // 7 bit i2c address (without R/W bit)
#define EEPROM_PAGE_SIZE        64     // page write buffer size
#define EEPROM_PAGE_WRITE_TIME  7      // page write time in milliseconds (docs say 5ms but that is too short)
// specify size of eeprom address register
// TWI_MMR_IADRSZ_1_BYTE for 1 byte, or TWI_MMR_IADRSZ_2_BYTE for 2 byte
#define EEPROM_ADDRSZ_BYTES     TWI_MMR_IADRSZ_2_BYTE
#define ORIG_PS_ON_PIN      -1
#endif //END #if MOTHERBOARD == 519






#if MOTHERBOARD == 999
#define KNOWN_BOARD
#include "userpins.h"
#endif



#ifndef SDSSORIG
#define SDSSORIG -1
#endif

#ifndef STEPPER_CURRENT_CONTROL // Set default stepper current control if not set yet.
#define STEPPER_CURRENT_CONTROL  CURRENT_CONTROL_MANUAL
#endif

#ifndef FAN_BOARD_PIN
#define FAN_BOARD_PIN -1
#endif

#if NUM_EXTRUDER < 2
#undef E1_PINS
#define E1_PINS
#endif

#if NUM_EXTRUDER < 3
#undef E2_PINS
#define E2_PINS
#endif

#ifndef HEATER_PINS_INVERTED
#define HEATER_PINS_INVERTED 0
#endif

//Available chip select pins for HW SPI are 4 10 52 
#if (SDSS == 4) || (SDSS == 10) || (SDSS == 52) || (SDSS == 70) 
#if (SDSS == 10)
#define SPI_PIN         77
#define SPI_CHAN        0
#else
#if (SDSS == 52) 
#define SPI_PIN         86
#define SPI_CHAN        2
#else // SDSS == 4
#if (SDSS == 4)
#define SPI_PIN         87
#define SPI_CHAN        1
#else //SDSS == 77
#define SPI_PIN         77
#define SPI_CHAN        0
  #endif
#endif
#endif
#define MOSI_PIN        75
#define MISO_PIN        74
#define SCK_PIN         76
//#define DUE_SOFTWARE_SPI
#else
#define DUE_SOFTWARE_SPI
/* could be any pin with software */
#ifndef MOSI_PIN
#define MOSI_PIN		51
#endif
#ifndef MISO_PIN
#define MISO_PIN		50
#endif
#ifndef SCK_PIN
#define SCK_PIN 		52
#endif

#endif


// Original pin assignmats to be used in configuration tool
#define X_STEP_PIN ORIG_X_STEP_PIN
#define X_DIR_PIN ORIG_X_DIR_PIN
#define X_ENABLE_PIN ORIG_X_ENABLE_PIN
#define X_MIN_PIN ORIG_X_MIN_PIN
#define X_MAX_PIN ORIG_X_MAX_PIN

#define Y_STEP_PIN ORIG_Y_STEP_PIN
#define Y_DIR_PIN ORIG_Y_DIR_PIN
#define Y_ENABLE_PIN ORIG_Y_ENABLE_PIN
#define Y_MIN_PIN ORIG_Y_MIN_PIN
#define Y_MAX_PIN ORIG_Y_MAX_PIN

#define Z_STEP_PIN ORIG_Z_STEP_PIN
#define Z_DIR_PIN ORIG_Z_DIR_PIN
#define Z_ENABLE_PIN ORIG_Z_ENABLE_PIN
#define Z_MIN_PIN ORIG_Z_MIN_PIN
#define Z_MAX_PIN ORIG_Z_MAX_PIN

#define E0_STEP_PIN ORIG_E0_STEP_PIN
#define E0_DIR_PIN ORIG_E0_DIR_PIN
#define E0_ENABLE_PIN ORIG_E0_ENABLE_PIN

#define E1_STEP_PIN ORIG_E1_STEP_PIN
#define E1_DIR_PIN ORIG_E1_DIR_PIN
#define E1_ENABLE_PIN ORIG_E1_ENABLE_PIN

#define E2_STEP_PIN ORIG_E2_STEP_PIN
#define E2_DIR_PIN ORIG_E2_DIR_PIN
#define E2_ENABLE_PIN ORIG_E2_ENABLE_PIN

#define E3_STEP_PIN ORIG_E3_STEP_PIN
#define E3_DIR_PIN ORIG_E3_DIR_PIN
#define E3_ENABLE_PIN ORIG_E3_ENABLE_PIN

#define E4_STEP_PIN ORIG_E4_STEP_PIN
#define E4_DIR_PIN ORIG_E4_DIR_PIN
#define E4_ENABLE_PIN ORIG_E4_ENABLE_PIN

#define E5_STEP_PIN ORIG_E5_STEP_PIN
#define E5_DIR_PIN ORIG_E5_DIR_PIN
#define E5_ENABLE_PIN ORIG_E5_ENABLE_PIN

#define FAN_PIN ORIG_FAN_PIN
#ifdef ORIG_FAN2_PIN
#define FAN2_PIN ORIG_FAN2_PIN
#endif

#define PS_ON_PIN ORIG_PS_ON_PIN       //skyblue modidy 2016-11-29

#ifndef ORIG_SDCARDDETECT
#define ORIG_SDCARDDETECT -1
#endif
#define SDCARDDETECT ORIG_SDCARDDETECT

#define SENSITIVE_PINS {0, 1, X_STEP_PIN, X_DIR_PIN, X_ENABLE_PIN, X_MIN_PIN, X_MAX_PIN, Y_STEP_PIN, Y_DIR_PIN, Y_ENABLE_PIN, Y_MIN_PIN, Y_MAX_PIN, Z_STEP_PIN, Z_DIR_PIN, Z_ENABLE_PIN, Z_MIN_PIN, Z_MAX_PIN, LED_PIN, ORIG_PS_ON_PIN, \
						HEATER_0_PIN, HEATER_1_PIN, FAN_PIN, E0_PINS E1_PINS E2_PINS TEMP_0_PIN, TEMP_1_PIN,SDSS }
#endif

