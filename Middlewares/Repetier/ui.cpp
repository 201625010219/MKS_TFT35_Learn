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

*/

#define UI_MAIN
#include "Repetier.h"
extern const int8_t encoder_table[16] PROGMEM ;
#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>

#include "dump_define.h"
#include "draw_ui.h"

#include "draw_pre_heat.h"
#include "draw_change_speed.h"
#include "draw_fan.h"

#define INCREMENT_MIN_MAX(a,steps,_min,_max) if ( (increment<0) && (_min>=0) && (a<_min-increment*steps) ) {a=_min;} else { a+=increment*steps; if(a<_min) a=_min; else if(a>_max) a=_max;};


#if BEEPER_TYPE==2 && defined(UI_HAS_I2C_KEYS) && UI_I2C_KEY_ADDRESS!=BEEPER_ADDRESS
#error Beeper address and i2c key address must be identical
#else
#if BEEPER_TYPE==2
#define UI_I2C_KEY_ADDRESS BEEPER_ADDRESS
#endif
#endif

#if UI_AUTORETURN_TO_MENU_AFTER!=0
long ui_autoreturn_time=0;
#endif


UIDisplay uid;

//tan 20160513
int xmov_distance = 0;
int ymov_distance = 0;
int zmov_distance = 0;


volatile uint8_t sd_operate_flag=0;

UIDisplay::UIDisplay()
{

}

extern CFG_ITMES gCfgItems;

void beep(uint8_t duration,uint8_t count)
{
#if FEATURE_BEEPER
#if BEEPER_TYPE!=0
#if BEEPER_TYPE==1 && defined(BEEPER_PIN) && BEEPER_PIN>=0
    SET_OUTPUT(BEEPER_PIN);
#endif
#if BEEPER_TYPE==2
    HAL::i2cStartWait(BEEPER_ADDRESS+I2C_WRITE);
#if UI_DISPLAY_I2C_CHIPTYPE==1
    HAL::i2cWrite( 0x14); // Start at port a
#endif
#endif
    for(uint8_t i=0; i<count; i++)
    {
#if BEEPER_TYPE==1 && defined(BEEPER_PIN) && BEEPER_PIN>=0
#if defined(BEEPER_TYPE_INVERTING) && BEEPER_TYPE_INVERTING
        WRITE(BEEPER_PIN,LOW);
#else
        WRITE(BEEPER_PIN,HIGH);
#endif
#else
#if UI_DISPLAY_I2C_CHIPTYPE==0
#if BEEPER_ADDRESS == UI_DISPLAY_I2C_ADDRESS
        HAL::i2cWrite(uid.outputMask & ~BEEPER_PIN);
#else
        HAL::i2cWrite(~BEEPER_PIN);
#endif
#endif
#if UI_DISPLAY_I2C_CHIPTYPE==1
        HAL::i2cWrite((BEEPER_PIN) | uid.outputMask);
        HAL::i2cWrite(((BEEPER_PIN) | uid.outputMask)>>8);
#endif
#endif
        HAL::delayMilliseconds(duration);
#if BEEPER_TYPE==1 && defined(BEEPER_PIN) && BEEPER_PIN>=0
#if defined(BEEPER_TYPE_INVERTING) && BEEPER_TYPE_INVERTING
        WRITE(BEEPER_PIN,HIGH);
#else
        WRITE(BEEPER_PIN,LOW);
#endif
#else
#if UI_DISPLAY_I2C_CHIPTYPE==0

#if BEEPER_ADDRESS == UI_DISPLAY_I2C_ADDRESS
        HAL::i2cWrite((BEEPER_PIN) | uid.outputMask);
#else
        HAL::i2cWrite(255);
#endif
#endif
#if UI_DISPLAY_I2C_CHIPTYPE==1
        HAL::i2cWrite( uid.outputMask);
        HAL::i2cWrite(uid.outputMask>>8);
#endif
#endif
        HAL::delayMilliseconds(duration);
    }
#if BEEPER_TYPE==2
    HAL::i2cStop();
#endif
#endif
#endif
}
// Actions are events from user input. Depending on the current state, each
// action can behave differently. Other actions do always the same like home, disable extruder etc.


void UIDisplay::executeAction(int action)
{
	int increment;
	/*millis_t actTime = HAL::timeInMilliseconds();
    millis_t dtReal;
    millis_t dt = dtReal = actTime-lastNextPrev;
    lastNextPrev = actTime;
    if(dt<SPEED_MAX_MILLIS) dt = SPEED_MAX_MILLIS;
    if(dt>SPEED_MIN_MILLIS)
    {
        dt = SPEED_MIN_MILLIS;
        lastNextAccumul = 1;
    }
    float f = (float)(SPEED_MIN_MILLIS-dt)/(float)(SPEED_MIN_MILLIS-SPEED_MAX_MILLIS);
    lastNextAccumul = 1.0f+(float)SPEED_MAGNIFICATION*f*f*f;*/

//#if UI_HAS_KEYS==1
    bool skipBeep = false;
   /*  if(action & UI_ACTION_TOPMENU)   // Go to start menu
    {
        action -= UI_ACTION_TOPMENU;
        menuLevel = 0;
    }
   if(action>=2000 && action<3000)
    {
        setStatusP(ui_action);
    }
    else
    */
        switch(action)
        {

		 case UI_ACTION_INC_FEEDRATE_MULTIPLY:	//move speed increase
			int fr = Printer::feedrateMultiply;
			increment = gCfgItems.stepPrintSpeed;
	        INCREMENT_MIN_MAX(fr,1,25,500);
	        Commands::changeFeedrateMultiply(fr);
			//tan 20160505
			disp_print_speed();
			break;

		case UI_ACTION_DEC_FEEDRATE_MULTIPLY:	//move in speed decrease
			//int fr = Printer::feedrateMultiply;
			increment = 0 - gCfgItems.stepPrintSpeed;
	        INCREMENT_MIN_MAX(Printer::feedrateMultiply,1,25,500);
	        Commands::changeFeedrateMultiply(Printer::feedrateMultiply);
			//tan 20160505
			disp_print_speed();			
			break;

		case UI_ACTION_INC_EXTRUCTOR_MULTIPLY:	//extruct in speed increase
			increment = gCfgItems.stepPrintSpeed;;
	        INCREMENT_MIN_MAX(Printer::extrudeMultiply,1,25,500);
        	Com::printFLN(Com::tFlowMultiply,(int)Printer::extrudeMultiply);
			//tan 20160505
			disp_print_speed();			
			break;

		case UI_ACTION_DEC_EXTRUCTOR_MULTIPLY:	//extruct in speed decrease
			increment = 0 - gCfgItems.stepPrintSpeed;
	       	INCREMENT_MIN_MAX(Printer::extrudeMultiply,1,25,500);
        	Com::printFLN(Com::tFlowMultiply,(int)Printer::extrudeMultiply);
			//tan 20160505
			disp_print_speed();			
			break;

		case UI_ACTION_MAX_FEEDRATE_X:	//X 给进速度
        	INCREMENT_MIN_MAX(Printer::maxFeedrate[0],1,1,1000);
        	break;
			
    	case UI_ACTION_MAX_FEEDRATE_Y:	//Y 给进速度
        	INCREMENT_MIN_MAX(Printer::maxFeedrate[1],1,1,1000);
        	break;
			
    	case UI_ACTION_MAX_FEEDRATE_Z:	//Z 给进速度
	        INCREMENT_MIN_MAX(Printer::maxFeedrate[2],1,1,1000);
	        break;
			
      
        case UI_ACTION_EMERGENCY_STOP:
            Commands::emergencyStop();
            break;
        case UI_ACTION_HOME_ALL:
			if(CfgPrinterItems.cfg_drive_system == DELTA)
			{
				Printer::homeAxis_delta(true,true,true);
			}
			else
			{
            	Printer::homeAxis(true,true,true);
			}
            Commands::printCurrentPosition(PSTR("UI_ACTION_HOMEALL "));
            break;
        case UI_ACTION_HOME_X:
			if(CfgPrinterItems.cfg_drive_system == DELTA)
			{
				Printer::homeAxis_delta(true,false,false);
			}
			else
			{
            	Printer::homeAxis(true,false,false);
			}
            Commands::printCurrentPosition(PSTR("UI_ACTION_HOMEALL "));
            break;
        case UI_ACTION_HOME_Y:
			if(CfgPrinterItems.cfg_drive_system == DELTA)
			{
				Printer::homeAxis_delta(false,true,false);
			}
			else
			{
            	Printer::homeAxis(false,true,false);
			}
            Commands::printCurrentPosition(PSTR("UI_ACTION_HOMEALL "));
            break;
        case UI_ACTION_HOME_Z:
			if(CfgPrinterItems.cfg_drive_system == DELTA)
			{
				Printer::homeAxis_delta(false,false,true);
			}
			else
			{
            	Printer::homeAxis(false,false,true);
			}
            Commands::printCurrentPosition(PSTR("UI_ACTION_HOMEALL "));
            break;
        case UI_ACTION_SET_ORIGIN:
            Printer::setOrigin(0,0,0);
            break;
        case UI_ACTION_DEBUG_ECHO:
            //if(Printer::debugEcho()) Printer::debugLevel-=1;
            //else Printer::debugLevel+=1;
            Printer::toggleEcho();
            break;
        case UI_ACTION_DEBUG_INFO:
            //if(Printer::debugInfo()) Printer::debugLevel-=2;
            //else Printer::debugLevel+=2;
            Printer::toggleInfo();
            break;
        case UI_ACTION_DEBUG_ERROR:
            //if(Printer::debugErrors()) Printer::debugLevel-=4;
            //else Printer::debugLevel+=4;
            Printer::toggleErrors();
            break;

        case UI_ACTION_DEBUG_DRYRUN:
            //if(Printer::debugDryrun()) Printer::debugLevel-=8;
            //else Printer::debugLevel+=8;
            Printer::toggleDryRun();
            if(Printer::debugDryrun())   // simulate movements without printing
            {
                Extruder::setTemperatureForExtruder(0,0);
//#if NUM_EXTRUDER>1
if(CfgPrinterItems.cfg_num_extruder > 1)
{
                Extruder::setTemperatureForExtruder(0,1);
}
//#endif
//#if NUM_EXTRUDER>2
if(CfgPrinterItems.cfg_num_extruder > 2)
{
                Extruder::setTemperatureForExtruder(0,2);
}
//#endif
//#if HAVE_HEATED_BED==true
			if(CfgPrinterItems.cfg_have_heated_bed ==1)
			{
                Extruder::setHeatedBedTemperature(0);
			}
//#endif
            }
            break;
        case UI_ACTION_POWER:
#if PS_ON_PIN>=0 // avoid compiler errors when the power supply pin is disabled
            Commands::waitUntilEndOfAllMoves();
            SET_OUTPUT(PS_ON_PIN); //GND
            TOGGLE(PS_ON_PIN);
#endif
            break;
#if CASE_LIGHTS_PIN > 0
        case UI_ACTION_LIGHTS_ONOFF:
            TOGGLE(CASE_LIGHTS_PIN);
            UI_STATUS(UI_TEXT_LIGHTS_ONOFF);
            break;
#endif
        case UI_ACTION_PREHEAT_PLA:
            UI_STATUS(UI_TEXT_PREHEAT_PLA);
            Extruder::setTemperatureForExtruder(UI_SET_PRESET_EXTRUDER_TEMP_PLA,0);
//#if NUM_EXTRUDER>1
if(CfgPrinterItems.cfg_num_extruder > 1)
{
            Extruder::setTemperatureForExtruder(UI_SET_PRESET_EXTRUDER_TEMP_PLA,1);
}
//#endif
//#if NUM_EXTRUDER>2
if(CfgPrinterItems.cfg_num_extruder>2)
{
            Extruder::setTemperatureForExtruder(UI_SET_PRESET_EXTRUDER_TEMP_PLA,2);
}
//#endif
//#if HAVE_HEATED_BED==true
			if(CfgPrinterItems.cfg_have_heated_bed ==1)
				{
            Extruder::setHeatedBedTemperature(UI_SET_PRESET_HEATED_BED_TEMP_PLA);
				}
//#endif
            break;
        case UI_ACTION_PREHEAT_ABS:
            UI_STATUS(UI_TEXT_PREHEAT_ABS);
            Extruder::setTemperatureForExtruder(UI_SET_PRESET_EXTRUDER_TEMP_ABS,0);
//#if NUM_EXTRUDER>1
if(CfgPrinterItems.cfg_num_extruder>1)
            Extruder::setTemperatureForExtruder(UI_SET_PRESET_EXTRUDER_TEMP_ABS,1);
//#endif
//#if NUM_EXTRUDER>2
if(CfgPrinterItems.cfg_num_extruder>2)
            Extruder::setTemperatureForExtruder(UI_SET_PRESET_EXTRUDER_TEMP_ABS,2);
//#endif
//#if HAVE_HEATED_BED==true
			if(CfgPrinterItems.cfg_have_heated_bed ==1)
			{
            Extruder::setHeatedBedTemperature(UI_SET_PRESET_HEATED_BED_TEMP_ABS);
			}
//#endif
            break;
        case UI_ACTION_COOLDOWN:
            UI_STATUS(UI_TEXT_COOLDOWN);
            Extruder::setTemperatureForExtruder(0,0);
//#if NUM_EXTRUDER>1
if(CfgPrinterItems.cfg_num_extruder>1)
            Extruder::setTemperatureForExtruder(0,1);
//#endif
//#if NUM_EXTRUDER>2
if(CfgPrinterItems.cfg_num_extruder>2)
            Extruder::setTemperatureForExtruder(0,2);
//#endif
//#if HAVE_HEATED_BED==true
			if(CfgPrinterItems.cfg_have_heated_bed ==1)
			{
            	Extruder::setHeatedBedTemperature(0);
			}
//#endif
            break;
        case UI_ACTION_HEATED_BED_OFF:
//#if HAVE_HEATED_BED==true
			if(CfgPrinterItems.cfg_have_heated_bed ==1)
			{
            	Extruder::setHeatedBedTemperature(0);
			}
//#endif
            break;
        case UI_ACTION_EXTRUDER0_OFF:
            Extruder::setTemperatureForExtruder(0,0);
            break;
        case UI_ACTION_EXTRUDER1_OFF:
//#if NUM_EXTRUDER>1
if(CfgPrinterItems.cfg_num_extruder>1)
{
            Extruder::setTemperatureForExtruder(0,1);
}
//#endif
            break;
        case UI_ACTION_EXTRUDER2_OFF:
//#if NUM_EXTRUDER>2
if(CfgPrinterItems.cfg_num_extruder>2)
{
            Extruder::setTemperatureForExtruder(0,2);
}
//#endif
            break;
        case UI_ACTION_DISABLE_STEPPER:
            Printer::kill(true);
            break;
        case UI_ACTION_RESET_EXTRUDER:
            Printer::currentPositionSteps[3] = 0;
            break;
        case UI_ACTION_EXTRUDER_RELATIVE:
            Printer::relativeExtruderCoordinateMode=!Printer::relativeExtruderCoordinateMode;
            break;
        case UI_ACTION_SELECT_EXTRUDER0:
            Extruder::selectExtruderById(0);
            break;
        case UI_ACTION_SELECT_EXTRUDER1:
//#if NUM_EXTRUDER>1
if(CfgPrinterItems.cfg_num_extruder>1)
{
            Extruder::selectExtruderById(1);
}
//#endif
            break;
        case UI_ACTION_SELECT_EXTRUDER2:
//#if NUM_EXTRUDER>2
if(CfgPrinterItems.cfg_num_extruder >2 )
{
            Extruder::selectExtruderById(2);
}
//#endif
            break;
//#if EEPROM_MODE!=0
#if 0   //modify 2            
        case UI_ACTION_STORE_EEPROM:
            EEPROM::storeDataIntoEEPROM(false);
            pushMenu((void*)&ui_menu_eeprom_saved,false);
            BEEP_LONG;
            skipBeep = true;
            break;
        case UI_ACTION_LOAD_EEPROM:
            EEPROM::readDataFromEEPROM();
            Extruder::selectExtruderById(Extruder::current->id);
            pushMenu((void*)&ui_menu_eeprom_loaded,false);
            BEEP_LONG;
            skipBeep = true;
            break;
#endif
#if SDSUPPORT
     /*   case UI_ACTION_SD_DELETE:
            if(sd.sdactive)
            {
                pushMenu((void*)&ui_menu_sd_fileselector,false);
            }
            else
            {
                UI_ERROR(UI_TEXT_NOSDCARD);
            }
            break;
        case UI_ACTION_SD_PRINT:
            if(sd.sdactive)
            {
                pushMenu((void*)&ui_menu_sd_fileselector,false);
            }
            break;*/

		/*case UI_ACTION_SD_PRINT:
			if (sd.selectFile(filename, false))
			{
				sd.startPrint();
				BEEP_LONG;
				menuLevel = 0;
			}
			break;*/

        case UI_ACTION_SD_PAUSE:        //暂停
            sd_operate_flag |= SD_PAUSE_BIT;
            break;
        case UI_ACTION_SD_CONTINUE:     //继续
            sd_operate_flag |= SD_CONTINUE_BIT;
            break;
        case UI_ACTION_SD_STOP:         //停止
            sd_operate_flag |= SD_STOP_BIT;
            break;
        case UI_ACTION_SD_UNMOUNT:
            sd.unmount();
            break;
        case UI_ACTION_SD_MOUNT:
            sd.mount();
            break;
        case UI_ACTION_MENU_SDCARD:
          //  pushMenu((void*)&ui_menu_sd,false);
            break;
#endif
#if FAN_PIN>-1
        case UI_ACTION_FAN_OFF:
            Commands::setFanSpeed(0,false);
			//tan 20160506
			disp_fan_speed();
            break;
        case UI_ACTION_FAN_25:
            Commands::setFanSpeed(64,false);
			//tan 20160506
			disp_fan_speed();
            break;
        case UI_ACTION_FAN_50:
            Commands::setFanSpeed(128,false);
			//tan 20160506
			disp_fan_speed();
            break;
        case UI_ACTION_FAN_75:
            Commands::setFanSpeed(192,false);
			//tan 20160506
			disp_fan_speed();
            break;
        case UI_ACTION_FAN_FULL:
            Commands::setFanSpeed(255,false);
			//tan 20160506
			disp_fan_speed();
            break;
#endif
       /* case UI_ACTION_MENU_XPOS:
            pushMenu((void*)&ui_menu_xpos,false);
            break;
        case UI_ACTION_MENU_YPOS:
            pushMenu((void*)&ui_menu_ypos,false);
            break;
        case UI_ACTION_MENU_ZPOS:
            pushMenu((void*)&ui_menu_zpos,false);
            break;
        case UI_ACTION_MENU_XPOSFAST:
            pushMenu((void*)&ui_menu_xpos_fast,false);
            break;
        case UI_ACTION_MENU_YPOSFAST:
            pushMenu((void*)&ui_menu_ypos_fast,false);
            break;
        case UI_ACTION_MENU_ZPOSFAST:
            pushMenu((void*)&ui_menu_zpos_fast,false);
            break;
        case UI_ACTION_MENU_QUICKSETTINGS:
            pushMenu((void*)&ui_menu_quick,false);
            break;
        case UI_ACTION_MENU_EXTRUDER:
            pushMenu((void*)&ui_menu_extruder,false);
            break;
        case UI_ACTION_MENU_POSITIONS:
            pushMenu((void*)&ui_menu_positions,false);
            break;
            
#ifdef UI_USERMENU1
        case UI_ACTION_SHOW_USERMENU1:
            pushMenu((void*)&UI_USERMENU1,false);
            break;
#endif
#ifdef UI_USERMENU2
        case UI_ACTION_SHOW_USERMENU2:
            pushMenu((void*)&UI_USERMENU2,false);
            break;
#endif
#ifdef UI_USERMENU3
        case UI_ACTION_SHOW_USERMENU3:
            pushMenu((void*)&UI_USERMENU3,false);
            break;
#endif
#ifdef UI_USERMENU4
        case UI_ACTION_SHOW_USERMENU4:
            pushMenu((void*)&UI_USERMENU4,false);
            break;
#endif
#ifdef UI_USERMENU5
        case UI_ACTION_SHOW_USERMENU5:
            pushMenu((void*)&UI_USERMENU5,false);
            break;
#endif
#ifdef UI_USERMENU6
        case UI_ACTION_SHOW_USERMENU6:
            pushMenu((void*)&UI_USERMENU6,false);
            break;
#endif
#ifdef UI_USERMENU7
        case UI_ACTION_SHOW_USERMENU7:
            pushMenu((void*)&UI_USERMENU7,false);
            break;
#endif
#ifdef UI_USERMENU8
        case UI_ACTION_SHOW_USERMENU8:
            pushMenu((void*)&UI_USERMENU8,false);
            break;
#endif
#ifdef UI_USERMENU9
        case UI_ACTION_SHOW_USERMENU9:
            pushMenu((void*)&UI_USERMENU9,false);
            break;
#endif
#ifdef UI_USERMENU10
        case UI_ACTION_SHOW_USERMENU10:
            pushMenu((void*)&UI_USERMENU10,false);
            break;
#endif
            */
        case UI_ACTION_X_UP:
            PrintLine::moveRelativeDistanceInStepsReal(Printer::axisStepsPerMM[X_AXIS],0,0,0,Printer::homingFeedrate[X_AXIS],false);
            break;
        case UI_ACTION_X_DOWN:
            PrintLine::moveRelativeDistanceInStepsReal(-Printer::axisStepsPerMM[X_AXIS],0,0,0,Printer::homingFeedrate[X_AXIS],false);
            break;
        case UI_ACTION_Y_UP:
            PrintLine::moveRelativeDistanceInStepsReal(0,Printer::axisStepsPerMM[1],0,0,Printer::homingFeedrate[1],false);
            break;
        case UI_ACTION_Y_DOWN:
            PrintLine::moveRelativeDistanceInStepsReal(0,-Printer::axisStepsPerMM[1],0,0,Printer::homingFeedrate[1],false);
            break;
        case UI_ACTION_Z_UP:
            PrintLine::moveRelativeDistanceInStepsReal(0,0,Printer::axisStepsPerMM[2],0,Printer::homingFeedrate[2],false);
            break;
        case UI_ACTION_Z_DOWN:
            PrintLine::moveRelativeDistanceInStepsReal(0,0,-Printer::axisStepsPerMM[2],0,Printer::homingFeedrate[2],false);
            break;
        case UI_ACTION_EXTRUDER_UP:
            PrintLine::moveRelativeDistanceInStepsReal(0,0,0,Printer::axisStepsPerMM[3],UI_SET_EXTRUDER_FEEDRATE,false);
            break;
        case UI_ACTION_EXTRUDER_DOWN:
            PrintLine::moveRelativeDistanceInStepsReal(0,0,0,-Printer::axisStepsPerMM[3],UI_SET_EXTRUDER_FEEDRATE,false);
            break;
        case UI_ACTION_EXTRUDER_TEMP_UP:
        {
            int tmp ;
			if((last_disp_state == PRINT_READY_UI)||(last_disp_state == FILAMENTCHANGE_UI))
			{
				Extruder::current->tempControl.targetTemperatureC = gCfgItems.preheat_desireSprayerTemp[Extruder::current->id];
			}
			tmp = (int)(Extruder::current->tempControl.targetTemperatureC)+gCfgItems.stepHeat;

            if(tmp<=UI_SET_MIN_EXTRUDER_TEMP) 
            {
				tmp = UI_SET_MIN_EXTRUDER_TEMP;
            }
            else if(tmp>=CfgPrinterItems.cfg_ui_set_maxtemp) 
            {
				tmp = CfgPrinterItems.cfg_ui_set_maxtemp;
            }
			
            Extruder::setTemperatureForExtruder(tmp,Extruder::current->id);
			
			gCfgItems.preheat_desireSprayerTemp[Extruder::current->id] = tmp;
			//Tan20160406	
			disp_desire_temp();
        }
        break;
        case UI_ACTION_EXTRUDER_TEMP_DOWN:
        {
            int tmp ;
			if((last_disp_state == PRINT_READY_UI)||(last_disp_state == FILAMENTCHANGE_UI))
			{
				Extruder::current->tempControl.targetTemperatureC = gCfgItems.preheat_desireSprayerTemp[Extruder::current->id];
			}
			tmp = (int)(Extruder::current->tempControl.targetTemperatureC)-gCfgItems.stepHeat;

            if(tmp<=UI_SET_MIN_EXTRUDER_TEMP) 
            {
				tmp = UI_SET_MIN_EXTRUDER_TEMP;
            }
            Extruder::setTemperatureForExtruder(tmp,Extruder::current->id);
			gCfgItems.preheat_desireSprayerTemp[Extruder::current->id] = tmp;
			//Tan20160406	
			disp_desire_temp();
        }
        break;
        case UI_ACTION_HEATED_BED_UP:
//#if HAVE_HEATED_BED==true
		if(CfgPrinterItems.cfg_have_heated_bed ==1)
        {
        	
            int tmp;
			if((last_disp_state == PRINT_READY_UI)||(last_disp_state == FILAMENTCHANGE_UI))
			{
				heatedBedController.targetTemperatureC = gCfgItems.preheat_desireBedTemp;
			}
			tmp = (int)heatedBedController.targetTemperatureC+gCfgItems.stepHeat;

            if(tmp<=UI_SET_MIN_HEATED_BED_TEMP) 
            {
				tmp = UI_SET_MIN_HEATED_BED_TEMP;   
            }
            else if(tmp>=CfgPrinterItems.cfg_ui_set_heated_bed_maxtemp)
            {
				tmp = CfgPrinterItems.cfg_ui_set_heated_bed_maxtemp;     
            }
            Extruder::setHeatedBedTemperature(tmp);
			gCfgItems.preheat_desireBedTemp= tmp;
			//Tan20160406	
			disp_desire_temp();
        }
//#endif
        break;
//#if MAX_HARDWARE_ENDSTOP_Z
        case UI_ACTION_SET_MEASURED_ORIGIN:
        {
            Printer::updateCurrentPosition();
            Printer::zLength -= Printer::currentPosition[Z_AXIS];
            Printer::currentPositionSteps[Z_AXIS] = 0;
            Printer::updateDerivedParameter();
#if NONLINEAR_SYSTEM
            //transformCartesianStepsToDeltaSteps(Printer::currentPositionSteps, Printer::currentDeltaPositionSteps);   //delta-skyblue
#endif
            Printer::updateCurrentPosition(true);
            Com::printFLN(Com::tZProbePrinterHeight,Printer::zLength);
#if EEPROM_MODE!=0
            EEPROM::storeDataIntoEEPROM(false);
            Com::printFLN(Com::tEEPROMUpdated);
#endif
            //Commands::printCurrentPosition(); //mask by skyblue
        }
        break;
//#endif
        case UI_ACTION_SET_P1:
//#ifdef SOFTWARE_LEVELING
#if 0   //modify          
            for (uint8_t i=0; i<3; i++)
            {
                Printer::levelingP1[i] = Printer::currentPositionSteps[i];     
            }
#endif
            break;
        case UI_ACTION_SET_P2:
//#ifdef SOFTWARE_LEVELING
#if 0   //modify          
            for (uint8_t i=0; i<3; i++)
            {
                Printer::levelingP2[i] = Printer::currentPositionSteps[i];
            }
#endif
            break;
        case UI_ACTION_SET_P3:
//#ifdef SOFTWARE_LEVELING
#if 0   //modify          
            for (uint8_t i=0; i<3; i++)
            {
                Printer::levelingP3[i] = Printer::currentPositionSteps[i];
            }
#endif
            break;
        case UI_ACTION_CALC_LEVEL:
//#ifdef SOFTWARE_LEVELING
#if 0 // modify 1          
            int32_t factors[4];
            PrintLine::calculatePlane(factors, Printer::levelingP1, Printer::levelingP2, Printer::levelingP3);
            Com::printFLN(Com::tLevelingCalc);
            Com::printFLN(Com::tTower1, PrintLine::calcZOffset(factors, Printer::deltaAPosXSteps, Printer::deltaAPosYSteps) * Printer::invAxisStepsPerMM[2]);
            Com::printFLN(Com::tTower2, PrintLine::calcZOffset(factors, Printer::deltaBPosXSteps, Printer::deltaBPosYSteps) * Printer::invAxisStepsPerMM[2]);
            Com::printFLN(Com::tTower3, PrintLine::calcZOffset(factors, Printer::deltaCPosXSteps, Printer::deltaCPosYSteps) * Printer::invAxisStepsPerMM[2]);
            
#endif
            break;
        case UI_ACTION_HEATED_BED_DOWN:
//#if HAVE_HEATED_BED==true
		if(CfgPrinterItems.cfg_have_heated_bed ==1)
        {
            int tmp = (int)heatedBedController.targetTemperatureC-gCfgItems.stepHeat;
			//Tan20160406
            //if(tmp<UI_SET_MIN_HEATED_BED_TEMP) tmp = 0;
            if(tmp<=UI_SET_MIN_HEATED_BED_TEMP) tmp = UI_SET_MIN_HEATED_BED_TEMP;
            Extruder::setHeatedBedTemperature(tmp);
			gCfgItems.preheat_desireBedTemp= tmp;
			//Tan20160406	
			disp_desire_temp();			
        }
//#endif
        break;
        case UI_ACTION_FAN_UP:
			//20160506
            //Commands::setFanSpeed(Printer::getFanSpeed()+32,false);
            Commands::setFanSpeed(Printer::getFanSpeed()+1,false);
			disp_fan_speed();
            break;
        case UI_ACTION_FAN_DOWN:
			//20160506
            //Commands::setFanSpeed(Printer::getFanSpeed()-32,false);
            Commands::setFanSpeed(Printer::getFanSpeed()-1,false);
			disp_fan_speed();
            break;
        case UI_ACTION_KILL:
            Commands::emergencyStop();
            break;
        case UI_ACTION_RESET:
            HAL::resetHardware();
            break;
        case UI_ACTION_PAUSE:
            Com::printFLN(PSTR("RequestPause:"));
            break;
#ifdef DEBUG_PRINT
        case UI_ACTION_WRITE_DEBUG:
            Com::printF(PSTR("Buf. Read Idx:"),(int)GCode::bufferReadIndex);
            Com::printF(PSTR(" Buf. Write Idx:"),(int)GCode::bufferWriteIndex);
            Com::printF(PSTR(" Comment:"),(int)GCode::commentDetected);
            Com::printF(PSTR(" Buf. Len:"),(int)GCode::bufferLength);
            Com::printF(PSTR(" Wait resend:"),(int)GCode::waitingForResend);
            Com::printFLN(PSTR(" Recv. Write Pos:"),(int)GCode::commandsReceivingWritePosition);
            Com::printF(PSTR("Min. XY Speed:"),Printer::minimumSpeed);
            Com::printF(PSTR(" Min. Z Speed:"),Printer::minimumZSpeed);
            Com::printF(PSTR(" Buffer:"),PrintLine::linesCount);
            Com::printF(PSTR(" Lines pos:"),(int)PrintLine::linesPos);
            Com::printFLN(PSTR(" Write Pos:"),(int)PrintLine::linesWritePos);
            Com::printFLN(PSTR("Wait loop:"),debugWaitLoop);
            Com::printF(PSTR("sd mode:"),(int)sd.sdmode);
            Com::printF(PSTR(" pos:"),sd.sdpos);
            Com::printFLN(PSTR(" of "),sd.filesize);
            break;
#endif
			
	    case UI_ACTION_FANSPEED:
	        Commands::setFanSpeed(Printer::getFanSpeed()+increment*3,false);
	        break;
	    case UI_ACTION_XPOSITION:
#if UI_SPEEDDEPENDENT_POSITIONING
	  /*  {
	    	increment = gCfgItems.move_dist * 10.0;
	        float d = 0.01*(float)increment*lastNextAccumul;
	        if(fabs(d)*2000>Printer::maxFeedrate[X_AXIS]*dtReal)
	            d *= Printer::maxFeedrate[X_AXIS]*dtReal/(2000*fabs(d));
	        long steps = (long)(d*Printer::axisStepsPerMM[X_AXIS]);
	        steps = ( increment<0 ? RMath::min(steps,(long)increment) : RMath::max(steps,(long)increment));
	        PrintLine::moveRelativeDistanceInStepsReal(steps,0,0,0,Printer::maxFeedrate[X_AXIS],true);
	    }*/
		{
			float d = gCfgItems.move_dist;
			long steps = (long)(d*Printer::axisStepsPerMM[X_AXIS]);
			
			xmov_distance += steps;
			//steps = ( increment<0 ? RMath::min(steps,(long)increment) : RMath::max(steps,(long)increment));
			if((0<(Printer::currentPosition[X_AXIS] - Printer::xMin))&&((Printer::currentPosition[X_AXIS] - Printer::xMin)<abs(d)))
			{
				if(d > 0)
					steps = (long)((Printer::currentPosition[X_AXIS] - Printer::xMin)*Printer::axisStepsPerMM[X_AXIS]);
				else 
					steps = (long)((-(Printer::currentPosition[X_AXIS] - Printer::xMin))*Printer::axisStepsPerMM[X_AXIS]);
			}
			
			if(abs(Printer::currentPosition[X_AXIS] - Printer::xMin)<0.2)
			{
				if(steps < 0)
				{
					steps = 0;
				}
				PrintLine::moveRelativeDistanceInStepsReal(steps,0,0,0,Printer::maxFeedrate[X_AXIS],true);
			}

			else if(abs(Printer::currentPosition[X_AXIS] - Printer::xLength)<0.2)
			{
				if(steps > 0)
				{
					steps = 0;
				}
				PrintLine::moveRelativeDistanceInStepsReal(steps,0,0,0,Printer::maxFeedrate[X_AXIS],true);
			}
			else if((Printer::currentPosition[X_AXIS] > Printer::xMin)&&(Printer::currentPosition[X_AXIS] < Printer::xLength))
			{
				PrintLine::moveRelativeDistanceInStepsReal(steps,0,0,0,Printer::maxFeedrate[X_AXIS],true);
			}
			
		}
#else
		
	    PrintLine::moveRelativeDistanceInStepsReal(increment,0,0,0,Printer::homingFeedrate[X_AXIS],true);
#endif
	    Commands::printCurrentPosition(PSTR("UI_ACTION_XPOSITION "));
            sd.mksUpdateXYZE();
	    break;
	    case UI_ACTION_YPOSITION:
#if UI_SPEEDDEPENDENT_POSITIONING
	   /* {
	    	increment = gCfgItems.move_dist * 10.0;
	        float d = 0.01*(float)increment*lastNextAccumul;
	        if(fabs(d)*2000>Printer::maxFeedrate[Y_AXIS]*dtReal)
	            d *= Printer::maxFeedrate[Y_AXIS]*dtReal/(2000*fabs(d));
	        long steps = (long)(d*Printer::axisStepsPerMM[Y_AXIS]);
	        steps = ( increment<0 ? RMath::min(steps,(long)increment) : RMath::max(steps,(long)increment));
	        PrintLine::moveRelativeDistanceInStepsReal(0,steps,0,0,Printer::maxFeedrate[Y_AXIS],true);
	    }*/
	    {
			float d = gCfgItems.move_dist;
			long steps = (long)(d*Printer::axisStepsPerMM[Y_AXIS]);
			ymov_distance += steps;
	        //steps = ( increment<0 ? RMath::min(steps,(long)increment) : RMath::max(steps,(long)increment));       
			if((0<(Printer::currentPosition[Y_AXIS] - Printer::yMin))&&((Printer::currentPosition[Y_AXIS] - Printer::yMin)<abs(d)))
			{
				if(d > 0)
					steps = (long)((Printer::currentPosition[Y_AXIS] - Printer::yMin)*Printer::axisStepsPerMM[Y_AXIS]);
				else 
					steps = (long)((-(Printer::currentPosition[Y_AXIS] - Printer::yMin))*Printer::axisStepsPerMM[Y_AXIS]);
			}

			if(abs(Printer::currentPosition[Y_AXIS] - Printer::yMin)<0.2)
			{
				if(steps < 0)
				{
					steps = 0;
				}
				PrintLine::moveRelativeDistanceInStepsReal(0,steps,0,0,Printer::maxFeedrate[Y_AXIS],true);
			}

			else if(abs(Printer::currentPosition[Y_AXIS] - Printer::yLength) < 0.2)
			{
				if(steps > 0)
				{
					steps = 0;
				}
				PrintLine::moveRelativeDistanceInStepsReal(0,steps,0,0,Printer::maxFeedrate[Y_AXIS],true);
			}
			else if((Printer::currentPosition[Y_AXIS] > Printer::yMin)&&(Printer::currentPosition[Y_AXIS] < Printer::yLength))
			{
	        	PrintLine::moveRelativeDistanceInStepsReal(0,steps,0,0,Printer::maxFeedrate[Y_AXIS],true);
			}
		}
#else
		
	    PrintLine::moveRelativeDistanceInStepsReal(0,increment,0,0,Printer::homingFeedrate[Y_AXIS],true);
#endif
	    Commands::printCurrentPosition(PSTR("UI_ACTION_YPOSITION "));
            sd.mksUpdateXYZE();
	    break;
	    case UI_ACTION_ZPOSITION:
#if UI_SPEEDDEPENDENT_POSITIONING
	   /*   {
	  	increment = gCfgItems.move_dist * 10.0;
	        float d = 0.01*(float)increment*lastNextAccumul;
	        if(fabs(d)*2000>Printer::maxFeedrate[Z_AXIS]*dtReal)
	            d *= Printer::maxFeedrate[Z_AXIS]*dtReal/(2000*fabs(d));
	        long steps = (long)(d*Printer::axisStepsPerMM[Z_AXIS]);
	        steps = ( increment<0 ? RMath::min(steps,(long)increment) : RMath::max(steps,(long)increment));
	        PrintLine::moveRelativeDistanceInStepsReal(0,0,steps,0,Printer::maxFeedrate[Z_AXIS],true);
	    }*/
	{
	    float d = gCfgItems.move_dist;
		long steps = (long)(d*Printer::axisStepsPerMM[Z_AXIS]);
		zmov_distance += steps;
	   //steps = ( increment<0 ? RMath::min(steps,(long)increment) : RMath::max(steps,(long)increment));
   		if((0<(Printer::currentPosition[Z_AXIS] - Printer::zMin))&&((Printer::currentPosition[Z_AXIS] - Printer::zMin)<abs(d)))
		{
			if(d > 0)
				steps = (long)((Printer::currentPosition[Z_AXIS] - Printer::zMin)*Printer::axisStepsPerMM[Z_AXIS]);
			else 
				steps = (long)((-(Printer::currentPosition[Z_AXIS] - Printer::zMin))*Printer::axisStepsPerMM[Z_AXIS]);
		}
			
	   if(abs(Printer::currentPosition[Z_AXIS] - Printer::zMin)<0.2)
	   {
		   if(steps < 0)
		   {
			   steps = 0;
		   }
		   PrintLine::moveRelativeDistanceInStepsReal(0,0,steps,0,Printer::maxFeedrate[Z_AXIS],true);
	   }
	   
	   else if(abs(Printer::currentPosition[Z_AXIS] - Printer::zLength)<0.2)
	   {
		   if(steps > 0)
		   {
			   steps = 0;
		   }
		   PrintLine::moveRelativeDistanceInStepsReal(0,0,steps,0,Printer::maxFeedrate[Z_AXIS],true);
	   }
	   else if((Printer::currentPosition[Z_AXIS] > Printer::zMin)&&(Printer::currentPosition[Z_AXIS] < Printer::zLength))
	   {
	   		PrintLine::moveRelativeDistanceInStepsReal(0,0,steps,0,Printer::maxFeedrate[Z_AXIS],true);
	   }

	}
#else
		
	    PrintLine::moveRelativeDistanceInStepsReal(0,0,increment,0,Printer::homingFeedrate[Z_AXIS],true);
#endif
	    Commands::printCurrentPosition(PSTR("UI_ACTION_ZPOSITION "));
            sd.mksUpdateXYZE();
	    break;
	    case UI_ACTION_XPOSITION_FAST:
	        PrintLine::moveRelativeDistanceInStepsReal(Printer::axisStepsPerMM[0]*increment,0,0,0,Printer::homingFeedrate[0],true);
	        Commands::printCurrentPosition(PSTR("UI_ACTION_XPOSITION_FAST "));
	        break;
	    case UI_ACTION_YPOSITION_FAST:
	        PrintLine::moveRelativeDistanceInStepsReal(0,Printer::axisStepsPerMM[1]*increment,0,0,Printer::homingFeedrate[1],true);
	        Commands::printCurrentPosition(PSTR("UI_ACTION_YPOSITION_FAST "));
	        break;
	    case UI_ACTION_ZPOSITION_FAST:
	        PrintLine::moveRelativeDistanceInStepsReal(0,0,Printer::axisStepsPerMM[2]*increment,0,Printer::homingFeedrate[2],true);
	        Commands::printCurrentPosition(PSTR("UI_ACTION_ZPOSITION_FAST "));
	        break;
	    case UI_ACTION_EPOSITION:
			increment = gCfgItems.extruStep;
	        PrintLine::moveRelativeDistanceInSteps(0,0,0,Printer::axisStepsPerMM[3]*increment,gCfgItems.extruSpeed,true,false);
	        Commands::printCurrentPosition(PSTR("UI_ACTION_EPOSITION "));
	        break;
	    case UI_ACTION_ZPOSITION_NOTEST:
	        Printer::setNoDestinationCheck(true);
#if UI_SPEEDDEPENDENT_POSITIONING
	  /*    {
	      float d = 0.01*(float)increment*lastNextAccumul;
	        if(fabs(d)*2000>Printer::maxFeedrate[Z_AXIS]*dtReal)
	            d *= Printer::maxFeedrate[Z_AXIS]*dtReal/(2000*fabs(d));
	        long steps = (long)(d*Printer::axisStepsPerMM[Z_AXIS]);
	        steps = ( increment<0 ? RMath::min(steps,(long)increment) : RMath::max(steps,(long)increment));
	        PrintLine::moveRelativeDistanceInStepsReal(0,0,steps,0,Printer::maxFeedrate[Z_AXIS],true);
	    }*/
	{
		float d = gCfgItems.move_dist;
		long steps = (long)(d*Printer::axisStepsPerMM[Z_AXIS]);
		//steps = ( increment<0 ? RMath::min(steps,(long)increment) : RMath::max(steps,(long)increment));
		PrintLine::moveRelativeDistanceInStepsReal(0,0,steps,0,Printer::maxFeedrate[Z_AXIS],true);
	}
#else
	    PrintLine::moveRelativeDistanceInStepsReal(0,0,increment,0,Printer::homingFeedrate[Z_AXIS],true);
#endif
	        Commands::printCurrentPosition(PSTR("UI_ACTION_ZPOSITION "));
	        Printer::setNoDestinationCheck(false);
	    break;
	    case UI_ACTION_ZPOSITION_FAST_NOTEST:
	        Printer::setNoDestinationCheck(true);
	        PrintLine::moveRelativeDistanceInStepsReal(0,0,Printer::axisStepsPerMM[Z_AXIS]*increment,0,Printer::homingFeedrate[Z_AXIS],true);
	        //Commands::printCurrentPosition();
	        Printer::setNoDestinationCheck(false);
	        break;
	    case UI_ACTION_Z_BABYSTEPS:
	        {
	            previousMillisCmd = HAL::timeInMilliseconds();
	            if(increment > 0) {
	                if((int)Printer::zBabystepsMissing+BABYSTEP_MULTIPLICATOR<127)
	                    Printer::zBabystepsMissing+=BABYSTEP_MULTIPLICATOR;
	            } else {
	                if((int)Printer::zBabystepsMissing-BABYSTEP_MULTIPLICATOR>-127)
	                    Printer::zBabystepsMissing-=BABYSTEP_MULTIPLICATOR;
	            }
	        }
	        break;
	    case UI_ACTION_HEATED_BED_TEMP:
//#if HAVE_HEATED_BED==true
		if(CfgPrinterItems.cfg_have_heated_bed ==1)
	    {
	        int tmp = (int)heatedBedController.targetTemperatureC;
	        if(tmp<UI_SET_MIN_HEATED_BED_TEMP) tmp = 0;
	        tmp+=increment;
	        if(tmp==1) tmp = UI_SET_MIN_HEATED_BED_TEMP;
	        if(tmp<UI_SET_MIN_HEATED_BED_TEMP) tmp = 0;
	        //else if(tmp>UI_SET_MAX_HEATED_BED_TEMP) tmp = UI_SET_MAX_HEATED_BED_TEMP;
	        else if(tmp>CfgPrinterItems.cfg_ui_set_heated_bed_maxtemp) tmp = CfgPrinterItems.cfg_ui_set_heated_bed_maxtemp;
	        Extruder::setHeatedBedTemperature(tmp);
	    }
//#endif
	    break;
	    case UI_ACTION_EXTRUDER0_TEMP:
	    {
	        int tmp = (int)extruder[0].tempControl.targetTemperatureC;
	        if(tmp<UI_SET_MIN_EXTRUDER_TEMP) tmp = 0;
	        tmp+=increment;
	        if(tmp==1) tmp = UI_SET_MIN_EXTRUDER_TEMP;
	        if(tmp<UI_SET_MIN_EXTRUDER_TEMP) tmp = 0;
	        //else if(tmp>UI_SET_MAX_EXTRUDER_TEMP) tmp = UI_SET_MAX_EXTRUDER_TEMP;
	        else if(tmp>CfgPrinterItems.cfg_ui_set_maxtemp) tmp = CfgPrinterItems.cfg_ui_set_maxtemp;
	        Extruder::setTemperatureForExtruder(tmp,0);
	    }
	    break;
	    case UI_ACTION_EXTRUDER1_TEMP:
//#if NUM_EXTRUDER>1
if(CfgPrinterItems.cfg_num_extruder>1)
	    {
	        int tmp = (int)extruder[1].tempControl.targetTemperatureC;
	        tmp+=increment;
	        if(tmp==1) tmp = UI_SET_MIN_EXTRUDER_TEMP;
	        if(tmp<UI_SET_MIN_EXTRUDER_TEMP) tmp = 0;
	        //else if(tmp>UI_SET_MAX_EXTRUDER_TEMP) tmp = UI_SET_MAX_EXTRUDER_TEMP;
	        else if(tmp>CfgPrinterItems.cfg_ui_set_maxtemp) tmp = CfgPrinterItems.cfg_ui_set_maxtemp;
	        Extruder::setTemperatureForExtruder(tmp,1);
	    }
//#endif
	    break;
	    case UI_ACTION_FEEDRATE_MULTIPLY:
	    {
	        int fr = Printer::feedrateMultiply;
	        INCREMENT_MIN_MAX(fr,1,25,500);
	        Commands::changeFeedrateMultiply(fr);
	    }
	    break;
	    case UI_ACTION_FLOWRATE_MULTIPLY:
	    {
	        INCREMENT_MIN_MAX(Printer::extrudeMultiply,1,25,500);
	        Com::printFLN(Com::tFlowMultiply,(int)Printer::extrudeMultiply);
	    }
	    break;
	    case UI_ACTION_STEPPER_INACTIVE:
	        stepperInactiveTime -= stepperInactiveTime % 1000;
	        INCREMENT_MIN_MAX(stepperInactiveTime,60000UL,0,10080000UL);
	        break;
	    case UI_ACTION_MAX_INACTIVE:
	        maxInactiveTime -= maxInactiveTime % 1000;
	        INCREMENT_MIN_MAX(maxInactiveTime,60000UL,0,10080000UL);
	        break;
	    case UI_ACTION_PRINT_ACCEL_X:
	        INCREMENT_MIN_MAX(Printer::maxAccelerationMMPerSquareSecond[X_AXIS],100,0,10000);
	        Printer::updateDerivedParameter();
	        break;
	    case UI_ACTION_PRINT_ACCEL_Y:
//#if DRIVE_SYSTEM!=3
			if(CfgPrinterItems.cfg_drive_system != DELTA)
			{
	        	INCREMENT_MIN_MAX(Printer::maxAccelerationMMPerSquareSecond[Y_AXIS],1,0,10000);
			}
//#else
			else
			{
	        	INCREMENT_MIN_MAX(Printer::maxAccelerationMMPerSquareSecond[Y_AXIS],100,0,10000);
			}
//#endif
	        Printer::updateDerivedParameter();
	        break;
	    case UI_ACTION_PRINT_ACCEL_Z:
	        INCREMENT_MIN_MAX(Printer::maxAccelerationMMPerSquareSecond[Z_AXIS],100,0,10000);
	        Printer::updateDerivedParameter();
	        break;
	    case UI_ACTION_MOVE_ACCEL_X:
	        INCREMENT_MIN_MAX(Printer::maxTravelAccelerationMMPerSquareSecond[X_AXIS],100,0,10000);
	        Printer::updateDerivedParameter();
	        break;
	    case UI_ACTION_MOVE_ACCEL_Y:
	        INCREMENT_MIN_MAX(Printer::maxTravelAccelerationMMPerSquareSecond[Y_AXIS],100,0,10000);
	        Printer::updateDerivedParameter();
	        break;
	    case UI_ACTION_MOVE_ACCEL_Z:
//#if DRIVE_SYSTEM!=3
		if(CfgPrinterItems.cfg_drive_system != DELTA)
		{
	        INCREMENT_MIN_MAX(Printer::maxTravelAccelerationMMPerSquareSecond[Z_AXIS],1,0,10000);
		}
//#else
		else
		{
	        INCREMENT_MIN_MAX(Printer::maxTravelAccelerationMMPerSquareSecond[Z_AXIS],100,0,10000);
		}
//#endif
	        Printer::updateDerivedParameter();
	        break;
	    case UI_ACTION_MAX_JERK:
	        INCREMENT_MIN_MAX(Printer::maxJerk,0.1,1,99.9);
	        break;
//#if DRIVE_SYSTEM!=3
	    case UI_ACTION_MAX_ZJERK:
			if(CfgPrinterItems.cfg_drive_system != DELTA)
	        	INCREMENT_MIN_MAX(Printer::maxZJerk,0.1,0.1,99.9);
	        break;
//#endif
	    case UI_ACTION_HOMING_FEEDRATE_X:
	        INCREMENT_MIN_MAX(Printer::homingFeedrate[0],1,5,1000);
	        break;
	    case UI_ACTION_HOMING_FEEDRATE_Y:
	        INCREMENT_MIN_MAX(Printer::homingFeedrate[1],1,5,1000);
	        break;
	    case UI_ACTION_HOMING_FEEDRATE_Z:
	        INCREMENT_MIN_MAX(Printer::homingFeedrate[2],1,1,1000);
	        break;
	   /* case UI_ACTION_MAX_FEEDRATE_X:
	        INCREMENT_MIN_MAX(Printer::maxFeedrate[0],1,1,1000);
	        break;
	    case UI_ACTION_MAX_FEEDRATE_Y:
	        INCREMENT_MIN_MAX(Printer::maxFeedrate[1],1,1,1000);
	        break;
	    case UI_ACTION_MAX_FEEDRATE_Z:
	        INCREMENT_MIN_MAX(Printer::maxFeedrate[2],1,1,1000);
	        break;*/
	    case UI_ACTION_STEPS_X:
	        INCREMENT_MIN_MAX(Printer::axisStepsPerMM[0],0.1,0,999);
	        Printer::updateDerivedParameter();
	        break;
	    case UI_ACTION_STEPS_Y:
	        INCREMENT_MIN_MAX(Printer::axisStepsPerMM[1],0.1,0,999);
	        Printer::updateDerivedParameter();
	        break;
	    case UI_ACTION_STEPS_Z:
	        INCREMENT_MIN_MAX(Printer::axisStepsPerMM[2],0.1,0,999);
	        Printer::updateDerivedParameter();
	        break;
	    case UI_ACTION_BAUDRATE:
//#if EEPROM_MODE!=0
#if 0   //modify 3              
	    {
	        char p=0;
	        int32_t rate;
	        do
	        {
	            rate = pgm_read_dword(&(baudrates[p]));
	            if(rate==baudrate) break;
	            p++;
	        }
	        while(rate!=0);
	        if(rate==0) p-=2;
	        p+=increment;
	        if(p<0) p = 0;
	        rate = pgm_read_dword(&(baudrates[p]));
	        if(rate==0) p--;
	        baudrate = pgm_read_dword(&(baudrates[p]));
	    }
#endif
	    break;
#ifdef TEMP_PID
	    case UI_ACTION_PID_PGAIN:
	        INCREMENT_MIN_MAX(Extruder::current->tempControl.pidPGain,0.1,0,200);
	        break;
	    case UI_ACTION_PID_IGAIN:
	        INCREMENT_MIN_MAX(Extruder::current->tempControl.pidIGain,0.01,0,100);
	        Extruder::selectExtruderById(Extruder::current->id);
	        break;
	    case UI_ACTION_PID_DGAIN:
	        INCREMENT_MIN_MAX(Extruder::current->tempControl.pidDGain,0.1,0,200);
	        break;
	    case UI_ACTION_DRIVE_MIN:
	        INCREMENT_MIN_MAX(Extruder::current->tempControl.pidDriveMin,1,1,255);
	        break;
	    case UI_ACTION_DRIVE_MAX:
	        INCREMENT_MIN_MAX(Extruder::current->tempControl.pidDriveMax,1,1,255);
	        break;
	    case UI_ACTION_PID_MAX:
	        INCREMENT_MIN_MAX(Extruder::current->tempControl.pidMax,1,1,255);
	        break;
#endif
	    case UI_ACTION_X_OFFSET:
	        INCREMENT_MIN_MAX(Extruder::current->xOffset,1,-99999,99999);
	        Extruder::selectExtruderById(Extruder::current->id);
	        break;
	    case UI_ACTION_Y_OFFSET:
	        INCREMENT_MIN_MAX(Extruder::current->yOffset,1,-99999,99999);
	        Extruder::selectExtruderById(Extruder::current->id);
	        break;
	    case UI_ACTION_EXTR_STEPS:
	        INCREMENT_MIN_MAX(Extruder::current->stepsPerMM,1,1,9999);
	        Extruder::selectExtruderById(Extruder::current->id);
	        break;
	    case UI_ACTION_EXTR_ACCELERATION:
	        INCREMENT_MIN_MAX(Extruder::current->maxAcceleration,10,10,99999);
	        Extruder::selectExtruderById(Extruder::current->id);
	        break;
	    case UI_ACTION_EXTR_MAX_FEEDRATE:
	        INCREMENT_MIN_MAX(Extruder::current->maxFeedrate,1,1,999);
	        Extruder::selectExtruderById(Extruder::current->id);
	        break;
	    case UI_ACTION_EXTR_START_FEEDRATE:
	        INCREMENT_MIN_MAX(Extruder::current->maxStartFeedrate,1,1,999);
	        Extruder::selectExtruderById(Extruder::current->id);
	        break;
	    case UI_ACTION_EXTR_HEATMANAGER:
	        INCREMENT_MIN_MAX(Extruder::current->tempControl.heatManager,1,0,3);
	        break;
	    case UI_ACTION_EXTR_WATCH_PERIOD:
	        INCREMENT_MIN_MAX(Extruder::current->watchPeriod,1,0,999);
	        break;
#if RETRACT_DURING_HEATUP
	    case UI_ACTION_EXTR_WAIT_RETRACT_TEMP:
	        //INCREMENT_MIN_MAX(Extruder::current->waitRetractTemperature,1,100,UI_SET_MAX_EXTRUDER_TEMP);
	        INCREMENT_MIN_MAX(Extruder::current->waitRetractTemperature,1,100,CfgPrinterItems.cfg_ui_set_maxtemp);
	        break;
	    case UI_ACTION_EXTR_WAIT_RETRACT_UNITS:
	        INCREMENT_MIN_MAX(Extruder::current->waitRetractUnits,1,0,99);
	        break;
#endif
#ifdef USE_ADVANCE
#ifdef ENABLE_QUADRATIC_ADVANCE
	    case UI_ACTION_ADVANCE_K:
//	        INCREMENT_MIN_MAX(Extruder::current->advanceK,1,0,200); //modify 3
	        break;
#endif
	    case UI_ACTION_ADVANCE_L:
	        //INCREMENT_MIN_MAX(Extruder::current->advanceL,1,0,600);       //modify 4
	        break;
#endif
	    
        }
  
    if(!skipBeep)
        BEEP_SHORT
#if UI_AUTORETURN_TO_MENU_AFTER!=0
        ui_autoreturn_time=HAL::timeInMilliseconds()+UI_AUTORETURN_TO_MENU_AFTER;
#endif
//#endif
}
void UIDisplay::mediumAction()
{
#if UI_HAS_I2C_ENCODER>0
    ui_check_slow_encoder();
#endif
}
void UIDisplay::slowAction()
{
    unsigned long time = HAL::timeInMilliseconds();
    uint8_t refresh=0;

#if 0 //dump
#if UI_HAS_KEYS==1
    // Update key buffer
    HAL::forbidInterrupts();
    if((flags & 9)==0)
    {
        flags|=8;
        HAL::allowInterrupts();
#if defined(UI_I2C_HOTEND_LED) || defined(UI_I2C_HEATBED_LED) || defined(UI_I2C_FAN_LED)
        {
            // check temps and set appropriate leds
            int led= 0;
#if NUM_EXTRUDER>0 && defined(UI_I2C_HOTEND_LED)
            led |= (tempController[Extruder::current->id]->targetTemperatureC > 0 ? UI_I2C_HOTEND_LED : 0);
#endif
#if HAVE_HEATED_BED && defined(UI_I2C_HEATBED_LED)
            led |= (heatedBedController.targetTemperatureC > 0 ? UI_I2C_HEATBED_LED : 0);
#endif
#if FAN_PIN>=0 && defined(UI_I2C_FAN_LED)
            led |= (Printer::getFanSpeed() > 0 ? UI_I2C_FAN_LED : 0);
#endif
            // update the leds
            uid.outputMask= ~led&(UI_I2C_HEATBED_LED|UI_I2C_HOTEND_LED|UI_I2C_FAN_LED);
        }
#endif
        int nextAction = 0;
        ui_check_slow_keys(nextAction);
        if(lastButtonAction!=nextAction)
        {
            lastButtonStart = time;
            lastButtonAction = nextAction;
            HAL::forbidInterrupts();
            flags|=2; // Mark slow action
        }
        HAL::forbidInterrupts();
        flags-=8;
    }
    HAL::forbidInterrupts();
    if((flags & 4)==0)
    {
        flags |= 4;
        // Reset click encoder
        HAL::forbidInterrupts();
        int8_t epos = encoderPos;
        encoderPos=0;
        HAL::allowInterrupts();
        if(epos)
        {
            nextPreviousAction(epos);
            BEEP_SHORT
            refresh=1;
        }
        if(lastAction!=lastButtonAction)
        {
            if(lastButtonAction==0)
            {
                if(lastAction>=2000 && lastAction<3000)
                {
                    statusMsg[0] = 0;
                }
                lastAction = 0;
                HAL::forbidInterrupts();
                flags &= ~3;
            }
            else if(time-lastButtonStart>UI_KEY_BOUNCETIME)     // New key pressed
            {
                lastAction = lastButtonAction;
                executeAction(lastAction);
                nextRepeat = time+UI_KEY_FIRST_REPEAT;
                repeatDuration = UI_KEY_FIRST_REPEAT;
            }
        }
        else if(lastAction<1000 && lastAction)     // Repeatable key
        {
            if(time-nextRepeat<10000)
            {
                executeAction(lastAction);
                repeatDuration -= UI_KEY_REDUCE_REPEAT;
                if(repeatDuration<UI_KEY_MIN_REPEAT) repeatDuration = UI_KEY_MIN_REPEAT;
                nextRepeat = time+repeatDuration;
            }
        }
        HAL::forbidInterrupts();
        flags -=4;
    }
    HAL::allowInterrupts();
#endif
#if UI_AUTORETURN_TO_MENU_AFTER!=0
    if(menuLevel>0 && ui_autoreturn_time<time)
    {
        lastSwitch = time;
        menuLevel=0;
        activeAction = 0;
    }
#endif
    if(menuLevel==0 && time>4000)
    {
        if(time-lastSwitch>UI_PAGES_DURATION)
        {
            lastSwitch = time;
#if !defined(UI_DISABLE_AUTO_PAGESWITCH) || !UI_DISABLE_AUTO_PAGESWITCH
            menuPos[0]++;
            if(menuPos[0]>=UI_NUM_PAGES)
                menuPos[0]=0;
#endif
            refresh = 1;
        }
        else if(time-lastRefresh>=1000) refresh=1;
    }
    else if(time-lastRefresh>=800)
    {
        UIMenu *men = (UIMenu*)menu[menuLevel];
        uint8_t mtype = pgm_read_byte((void*)&(men->menuType));
        //if(mtype!=1)
        refresh=1;
    }
    if(refresh)
    {
        if (menuLevel > 1 || Printer::isAutomount())
        {
            shift++;
            if(shift+UI_COLS>MAX_COLS+1)
                shift = -2;
        }
        else
            shift = -2;

        refreshPage();
        lastRefresh = time;
    }
}
void UIDisplay::fastAction()
{
#if UI_HAS_KEYS==1
    // Check keys
    HAL::forbidInterrupts();
    if((flags & 10)==0)
    {
        flags |= 8;
        HAL::allowInterrupts();
        int nextAction = 0;
        ui_check_keys(nextAction);
        if(lastButtonAction!=nextAction)
        {
            lastButtonStart = HAL::timeInMilliseconds();
            lastButtonAction = nextAction;
            HAL::forbidInterrupts();
            flags|=1;
        }
        HAL::forbidInterrupts();
        flags-=8;
    }
    HAL::allowInterrupts();
#endif
#endif
}
#if UI_ENCODER_SPEED==0
const int8_t encoder_table[16] PROGMEM = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0}; // Full speed
#elif UI_ENCODER_SPEED==1
const int8_t encoder_table[16] PROGMEM = {0,0,-1,0,0,0,0,1,1,0,0,0,0,-1,0,0}; // Half speed
#else
//const int8_t encoder_table[16] PROGMEM = {0,0,0,0,0,0,0,0,1,0,0,0,0,-1,0,0}; // Quart speed
//const int8_t encoder_table[16] PROGMEM = {0,1,0,0,-1,0,0,0,0,0,0,0,0,0,0,0}; // Quart speed
const int8_t encoder_table[16] PROGMEM = {0,0,0,0,0,0,0,0,0,0,0,-1,0,0,1,0}; // Quart speed
#endif

