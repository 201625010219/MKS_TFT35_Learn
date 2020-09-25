/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              µC/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_TOUCH_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "GUI_X.h"
//#include "SZ_TouchScreen.h"
#include "mks_touch_screen.h"

void GUI_TOUCH_X_ActivateX(void) {
}

void GUI_TOUCH_X_ActivateY(void) {
}
#if 0
int  GUI_TOUCH_X_MeasureX(void) {
	TOUCH_EVENT * pTouchEvent = 0;
	
	pTouchEvent = getTouchEvent();

	if(pTouchEvent == 0)
		return 0;

//	if(pTouchEvent->state == PEN_DOWN)
	{
		return pTouchEvent->x;
	}
	//else
	{
	//	return 0;
	}
 // return ADS_Read_X();
}

int  GUI_TOUCH_X_MeasureY(void) {
 	TOUCH_EVENT * pTouchEvent = 0;

	
	pTouchEvent = getTouchEvent();
	if(pTouchEvent == 0)
		return 0;

//	if(pTouchEvent->state == PEN_DOWN)
	{
		return pTouchEvent->y;
	}
//	else
	{
	//	return 0;
	}
	// return ADS_Read_Y();
}

int GUI_TOUCH_X_MeasureXY(uint16_t *pX, uint16_t *pY)
{
	TOUCH_EVENT * pTouchEvent = 0;
	
	pTouchEvent = getTouchEvent();
	
	if(pTouchEvent == 0)
		return 0;

	 *pX = pTouchEvent->x;
	 *pY = pTouchEvent->y;

	return 1;

}
#else
static TOUCH_EVENT gTouchEvent;
static GUI_PID_STATE State;
int  GUI_TOUCH_X_MeasureX(void) {
#if 0
	TOUCH_EVENT * pTouchEvent = 0;
	
	pTouchEvent = getTouchEvent();

	if(pTouchEvent == 0)
		return 0;

//	if(pTouchEvent->state == PEN_DOWN)
	{
		return pTouchEvent->x;
	}
	//else
	{
	//	return 0;
	}
 // return ADS_Read_X();
#else
 	//if(sd_pause_flag==0)                    //skyblue add
	getTouchEvent(&gTouchEvent);
	
	if(gTouchEvent.state == PEN_UP)
  {
    gTouchEvent.x = 0;
		gTouchEvent.y = 0;
  }
	return gTouchEvent.x;
#endif
}

int  GUI_TOUCH_X_MeasureY(void) {
	#if 0
 	TOUCH_EVENT * pTouchEvent = 0;

	
	pTouchEvent = getTouchEvent();
	if(pTouchEvent == 0)
		return 0;

//	if(pTouchEvent->state == PEN_DOWN)
	{
		return pTouchEvent->y;
	}
//	else
	{
	//	return 0;
	}
	// return ADS_Read_Y();
	#else
	return gTouchEvent.y;
	#endif

}

int  GUI_TOUCH_X_MeasureState(void) 
{	
	//getTouchEvent(&gTouchEvent);
 	return gTouchEvent.state;
}

int GUI_TOUCH_X_MeasureXY(uint16_t *pX, uint16_t *pY)
{
#if 0
	TOUCH_EVENT * pTouchEvent = 0;
	
	pTouchEvent = getTouchEvent();
	
	if(pTouchEvent == 0)
		return 0;

	 *pX = pTouchEvent->x;
	 *pY = pTouchEvent->y;

	return 1;
#else
	getTouchEvent(&gTouchEvent);

	if(gTouchEvent.state == PEN_UP)
	{
		gTouchEvent.x = 0;
		gTouchEvent.y = 0;

	*pX = gTouchEvent.x;
	*pY = gTouchEvent.y;		
		return 0;
	}

	*pX = gTouchEvent.x;
	*pY = gTouchEvent.y;

	return 1;
#endif
}
#endif

