#include "gui.h"
#include "button.h"
#include "draw_ui.h"
#include "at24cxx.h"
#include "spi_flash.h"
#include "pic_manager.h"


#ifndef GUI_FLASH
#define GUI_FLASH
#endif

static GUI_HWIN hLevelingParaWnd;
//static BUTTON_STRUCT AutoLevelEnable_text,BLtouchEnable_text,ProbePort_text;
//static BUTTON_STRUCT AutoLevelEnable,BLtouchEnable,ProbePort;
//
//static BUTTON_STRUCT ProbeXoffset_text,ProbeYoffset_text,ProbeZoffset_text;
//static BUTTON_STRUCT ProbeXYSpeed_text,ProbeZSpeed_text;
//static BUTTON_STRUCT ProbeXoffset_value,ProbeYoffset_value,ProbeZoffset_value;
//static BUTTON_STRUCT ProbeXYSpeed_value,ProbeZSpeed_value;
//static BUTTON_STRUCT ProbeXoffset_default,ProbeYoffset_default,ProbeZoffset_default;
//static BUTTON_STRUCT ProbeXYSpeed_default,ProbeZSpeed_default;
static BUTTON_STRUCT AutoLevelEnable_text;
static BUTTON_STRUCT AutoLevelEnable;
static BUTTON_STRUCT z_speed_text,xy_speed_text,z_high_text;
static BUTTON_STRUCT z_speed_value,xy_speed_value,z_high_value;
static BUTTON_STRUCT z_speed_default,xy_speed_default,z_high_default;
static BUTTON_STRUCT auto_LevelCmd_text,auto_Leveling_cmd,auto_LevelCmd_default;



static BUTTON_STRUCT button_next,button_previous,button_back;

static uint8_t current_page;//0:上一页，1:下一页

inline void gcode_M500() {
//  (void)settings.save();
}

static void btnHandle_rest()
{
    AutoLevelEnable_text.btnHandle = 0;
//    BLtouchEnable_text.btnHandle = 0;
//    ProbePort_text.btnHandle = 0;
    AutoLevelEnable.btnHandle = 0;
//    BLtouchEnable.btnHandle = 0;
//    ProbePort.btnHandle = 0;
//    ProbeXoffset_text.btnHandle = 0;
//    ProbeYoffset_text.btnHandle = 0;
//    ProbeZoffset_text.btnHandle = 0;
//    ProbeXYSpeed_text.btnHandle = 0;   
//    ProbeZSpeed_text.btnHandle = 0;
//    ProbeXoffset_value.btnHandle = 0;
//    ProbeYoffset_value.btnHandle = 0;   
//    ProbeZoffset_value.btnHandle = 0;
//    ProbeXYSpeed_value.btnHandle = 0;    
//    ProbeZSpeed_value.btnHandle = 0;   
//    ProbeXoffset_default.btnHandle = 0;
//    ProbeYoffset_default.btnHandle = 0;
//    ProbeZoffset_default.btnHandle = 0;
	  z_speed_text.btnHandle = 0;
	  xy_speed_text.btnHandle = 0;
	  z_high_text.btnHandle = 0;
	  z_speed_value.btnHandle = 0;
	  xy_speed_value.btnHandle = 0;
	  z_high_value.btnHandle = 0;	  
	  z_speed_default.btnHandle = 0;
	  xy_speed_default.btnHandle = 0;
	  z_high_default.btnHandle = 0;


	  auto_LevelCmd_text.btnHandle = 0;
	  auto_Leveling_cmd.btnHandle = 0;
	  auto_LevelCmd_default.btnHandle = 0;

    button_previous.btnHandle = 0;
    button_next.btnHandle = 0;
    button_back.btnHandle = 0;
}

static void cbLevelingParaWin(WM_MESSAGE * pMsg) {

    struct PressEvt *press_event;
    switch (pMsg->MsgId)
    {
    	case WM_PAINT:
			GUI_SetColor(0xff5449);

			GUI_FillRect(10, 90, 470,90);
			if (current_page != 1)
			{
            GUI_FillRect(10, 140, 470,140);
            GUI_FillRect(10, 190, 470,190);
            GUI_FillRect(10, 240, 470,240);
			}
    		break;
    	case WM_TOUCH:
    	 	press_event = (struct PressEvt *)pMsg->Data.p;
    		
    		break;
    	case WM_TOUCH_CHILD:
    		press_event = (struct PressEvt *)pMsg->Data.p;

    		break;
    		
    	case WM_NOTIFY_PARENT:
    		if(pMsg->Data.v == WM_NOTIFICATION_RELEASED)
    		{
    			if(pMsg->hWinSrc == button_back.btnHandle)
    			{
    			    current_page = 0;
    				last_disp_state = LEVELING_PARA_UI;
    				Clear_LevelingPara();
    				draw_return_ui();
    			}	
                else if(pMsg->hWinSrc == button_next.btnHandle)
                {
                    current_page = 1;
                    last_disp_state = LEVELING_PARA_UI;
                    Clear_LevelingPara();
                    draw_LevelingPara();
                }
                else if(pMsg->hWinSrc == button_previous.btnHandle)
                {
                    current_page = 0;
                    last_disp_state = LEVELING_PARA_UI;
                    Clear_LevelingPara();
                    draw_LevelingPara();
                }                
    			else if(pMsg->hWinSrc == AutoLevelEnable.btnHandle)
    			{

//                    if(mksCfg.bed_leveling_method == 8)
//                    {
//                        mksCfg.bed_leveling_method=0;
//						gCfgItems.leveling_mode = 0;
//                        BUTTON_SetBmpFileName(AutoLevelEnable.btnHandle, "bmp_disable.bin",1);
//                        if(gCfgItems.multiple_language != 0)
//                            BUTTON_SetText(AutoLevelEnable.btnHandle,machine_menu.disable);
//	
//                    }
//                    else
//                    {
//                        mksCfg.bed_leveling_method=3;
//			    		gCfgItems.leveling_mode = 1;
//                        BUTTON_SetBmpFileName(AutoLevelEnable.btnHandle, "bmp_enable.bin",1);
//                        if(gCfgItems.multiple_language != 0)
//                            BUTTON_SetText(AutoLevelEnable.btnHandle,machine_menu.enable);                        
//                    }
//	                    epr_write_data(EPR_BED_LEVELING_METHOD,&mksCfg.bed_leveling_method,1);
//						mksCfg.bed_leveling_method = (1<<mksCfg.bed_leveling_method);
//						epr_write_data(EPR_LEVELING_MODE,(uint8_t *)&gCfgItems.leveling_mode,sizeof(gCfgItems.leveling_mode));
					if(gCfgItems.leveling_mode == 1)
					{
						gCfgItems.leveling_mode = 0;
                        BUTTON_SetBmpFileName(AutoLevelEnable.btnHandle, "bmp_disable.bin",1);
						 BUTTON_SetBmpFileName(auto_Leveling_cmd.btnHandle, "bmp_disable.bin",1);
			 			BUTTON_SetBmpFileName(auto_Leveling_cmd.btnHandle, "bmp_value_blank_grey.bin",1);
						BUTTON_SetBmpFileName(auto_LevelCmd_default.btnHandle, "bmp_value_blank_grey.bin",1);
                        if(gCfgItems.multiple_language != 0)
                            BUTTON_SetText(AutoLevelEnable.btnHandle,machine_menu.disable);
					}
					else 
					{
						gCfgItems.leveling_mode = 1;
							BUTTON_SetBmpFileName(AutoLevelEnable.btnHandle, "bmp_enable.bin",1);
						BUTTON_SetBmpFileName(auto_Leveling_cmd.btnHandle, "bmp_value_blank.bin",1);
						BUTTON_SetBmpFileName(auto_LevelCmd_default.btnHandle, "bmp_value_blank.bin",1);
                        if(gCfgItems.multiple_language != 0)
                            BUTTON_SetText(AutoLevelEnable.btnHandle,machine_menu.enable);
							
					}
					HAL::AT24CXX_Write(BAK_LEVELING_MODE_ADDR,(uint8_t *)&gCfgItems.leveling_mode,1);	
    				
    			}
//    			else if(pMsg->hWinSrc == BLtouchEnable.btnHandle)
//    			{
//                    if(mksCfg.mkstouch==1)
//                    {
//                        mksCfg.mkstouch=0;
//                        BUTTON_SetBmpFileName(BLtouchEnable.btnHandle, "bmp_disable.bin",1);
//                        if(gCfgItems.multiple_language != 0)
//                            BUTTON_SetText(BLtouchEnable.btnHandle,machine_menu.disable);
//                    }
//                    else
//                    {
//                        mksCfg.mkstouch=1;
//                        BUTTON_SetBmpFileName(BLtouchEnable.btnHandle, "bmp_enable.bin",1);
//                        if(gCfgItems.multiple_language != 0)
//                            BUTTON_SetText(BLtouchEnable.btnHandle,machine_menu.enable);     
//
//			       if(mksCfg.mkstouch ==1 && mksCfg.bed_leveling_method != NULL_BED_LEVELING)
//				{
//					mksCfg.z_min_probe_endstop_inverting = 0;
//					if(mksCfg.z_min_probe_pin_mode == 1)	//ZMIN
//						mksCfg.z_min_endstop_inverting = 0;
//					else if(mksCfg.z_min_probe_pin_mode == 2)	//ZMAX	
//						mksCfg.z_max_endstop_inverting = 0;
//				}
//                    }
//                    epr_write_data(EPR_MKSTOUCH,&mksCfg.mkstouch,1);
//
//    				
//    			}
//    			else if(pMsg->hWinSrc == ProbePort.btnHandle)
//    			{
//                    if(mksCfg.z_min_probe_pin_mode==1)
//                    {
//                        mksCfg.z_min_probe_pin_mode=2;
//                        BUTTON_SetBmpFileName(ProbePort.btnHandle, "bmp_enable.bin",1);
//                        if(gCfgItems.multiple_language != 0)
//                            BUTTON_SetText(ProbePort.btnHandle,machine_menu.z_max);
//                    }
//                    else
//                    {
//                        mksCfg.z_min_probe_pin_mode=1;
//                        BUTTON_SetBmpFileName(ProbePort.btnHandle, "bmp_disable.bin",1);
//                        if(gCfgItems.multiple_language != 0)
//                            BUTTON_SetText(ProbePort.btnHandle,machine_menu.z_min);                        
//                    }
//                    epr_write_data(EPR_Z_MIN_PROBE_PIN_MODE,&mksCfg.z_min_probe_pin_mode,1);
//
//                
//                    
//    			}       
//    			else if(pMsg->hWinSrc == ProbeXoffset_value.btnHandle)
//    			{
//    				last_disp_state = LEVELING_PARA_UI;
//    				Clear_LevelingPara();
//                    value=xoffset;
//                    draw_NumberKey();                   
//
//    			}
//    			else if(pMsg->hWinSrc == ProbeYoffset_value.btnHandle)
//    			{
//    				last_disp_state = LEVELING_PARA_UI;
//    				Clear_LevelingPara();
//                    value=yoffset;
//                    draw_NumberKey();
//
//    			}  
//    			else if(pMsg->hWinSrc == ProbeZoffset_value.btnHandle)
//    			{
//    				last_disp_state = LEVELING_PARA_UI;
//    				Clear_LevelingPara();
//                    value=zoffset;
//                    draw_NumberKey();
//
//    			} 
//    			else if(pMsg->hWinSrc == ProbeXYSpeed_value.btnHandle)
//    			{
//    				last_disp_state = LEVELING_PARA_UI;
//    				Clear_LevelingPara();
//                    value=xyspeed;
//                    draw_NumberKey();
//
//   				}  
//    			else if(pMsg->hWinSrc == ProbeZSpeed_value.btnHandle)
//    			{
//    				last_disp_state = LEVELING_PARA_UI;
//    				Clear_LevelingPara();
//                    value=zspeed;
//                    draw_NumberKey();
//
//    			} 
//    			else if(pMsg->hWinSrc == ProbeXoffset_default.btnHandle)
//    			{
//    			      mksCfg.x_probe_offset_from_extruder = 0;
//                    memset(cmd_code,0,sizeof(cmd_code));
//                    sprintf(cmd_code,"%.1f",mksCfg.x_probe_offset_from_extruder);
//                    BUTTON_SetText(ProbeXoffset_value.btnHandle,cmd_code);
//                    epr_write_data(EPR_X_PROBE_OFFSET_FROM_EXTRUDER, (uint8_t *)&mksCfg.x_probe_offset_from_extruder,sizeof(float));                    
//    			}   
//    			else if(pMsg->hWinSrc == ProbeYoffset_default.btnHandle)
//    			{
//    			    mksCfg.y_probe_offset_from_extruder = 0;
//                    memset(cmd_code,0,sizeof(cmd_code));
//                    sprintf(cmd_code,"%.1f",mksCfg.y_probe_offset_from_extruder);
//                    BUTTON_SetText(ProbeYoffset_value.btnHandle,cmd_code);
//                    epr_write_data(EPR_Y_PROBE_OFFSET_FROM_EXTRUDER, (uint8_t *)&mksCfg.y_probe_offset_from_extruder,sizeof(float));
//
//
//    			}  
//    			else if(pMsg->hWinSrc == ProbeZoffset_default.btnHandle)
//    			{
//    			    zprobe_zoffset = 0;
//                    memset(cmd_code,0,sizeof(cmd_code));
//                    sprintf(cmd_code,"%.1f",zprobe_zoffset);
//                    BUTTON_SetText(ProbeZoffset_value.btnHandle,cmd_code);
//                    gcode_M500();	//保存参数
//    			} 
//    			else if(pMsg->hWinSrc == ProbeXYSpeed_default.btnHandle)
//    			{
//    			    mksCfg.xy_probe_speed = 4000;
//                    memset(cmd_code,0,sizeof(cmd_code));
//                    sprintf(cmd_code,"%.1f",mksCfg.xy_probe_speed);
//                    BUTTON_SetText(ProbeXYSpeed_default.btnHandle,cmd_code);
//                    epr_write_data(EPR_XY_PROBE_SPEED, (uint8_t *)&mksCfg.xy_probe_speed,sizeof(float));
//
//
//    			} 
//    			else if(pMsg->hWinSrc == ProbeZSpeed_default.btnHandle)
//    			{
//    			    mksCfg.z_probe_speed_fast = 600;
//                    mksCfg.z_probe_speed_slow = 300;
//                    memset(cmd_code,0,sizeof(cmd_code));
//                    sprintf(cmd_code,"%.1f",mksCfg.z_probe_speed_fast);
//                    BUTTON_SetText(ProbeZSpeed_default.btnHandle,cmd_code);
//                    epr_write_data(EPR_Z_PROBE_SPEED_FAST, (uint8_t *)&mksCfg.z_probe_speed_fast,sizeof(float));
//                    epr_write_data(EPR_Z_PROBE_SPEED_SLOW, (uint8_t *)&mksCfg.z_probe_speed_slow,sizeof(float));
//
//
//    			}    
				else if(pMsg->hWinSrc == z_speed_value.btnHandle)
				{
					last_disp_state = LEVELING_PARA_UI;
					Clear_LevelingPara();
					  value=z_speed_level;
					  draw_NumberKey();

				} 
				else if(pMsg->hWinSrc == xy_speed_value.btnHandle)
				{
					last_disp_state = LEVELING_PARA_UI;
					Clear_LevelingPara();
					  value=xy_speed_level;
					  draw_NumberKey();

					}  
				else if(pMsg->hWinSrc == z_high_value.btnHandle)
				{
					last_disp_state = LEVELING_PARA_UI;
					Clear_LevelingPara();
					  value=z_high_level;
					  draw_NumberKey();

				} 

				else if(pMsg->hWinSrc == auto_Leveling_cmd.btnHandle)
				{
					last_disp_state = LEVELING_PARA_UI;
					Clear_LevelingPara();
					  valueType=auto_levelCmd;
					  draw_Keyboard();
				} 
				else if(pMsg->hWinSrc == z_speed_default.btnHandle)
				{
					  gCfgItems.leveling_z_speed = 1500;
					  memset(cmd_code,0,sizeof(cmd_code));
					  sprintf(cmd_code,"%d",gCfgItems.leveling_z_speed);
					  BUTTON_SetText(z_speed_value.btnHandle,cmd_code);
					  gcode_M500(); //保存参数
					  HAL::AT24CXX_Write(BAK_LEVELING_Z_SPEED_ADDR,(uint8_t *)&gCfgItems.leveling_z_speed,2);	
				} 
				else if(pMsg->hWinSrc == xy_speed_default.btnHandle)
				{
					 gCfgItems.leveling_xy_speed = 3000;
					  memset(cmd_code,0,sizeof(cmd_code));
					  sprintf(cmd_code,"%d",gCfgItems.leveling_xy_speed);
					  BUTTON_SetText(xy_speed_value.btnHandle,cmd_code);
//					  epr_write_data(EPR_XY_PROBE_SPEED, (uint8_t *)&mksCfg.xy_probe_speed,sizeof(float));
					 HAL::AT24CXX_Write(BAK_LEVELING_XY_SPEED_ADDR,(uint8_t *)&gCfgItems.leveling_xy_speed,2);	
				} 
				else if(pMsg->hWinSrc == z_high_default.btnHandle)
				{
					 gCfgItems.leveling_z_high= 10;
//					  mksCfg.z_probe_speed_slow = 300;
					  memset(cmd_code,0,sizeof(cmd_code));
					  sprintf(cmd_code,"%d",gCfgItems.leveling_z_high);
					  BUTTON_SetText(z_high_value.btnHandle,cmd_code);
//					  epr_write_data(EPR_Z_PROBE_SPEED_FAST, (uint8_t *)&mksCfg.z_probe_speed_fast,sizeof(float));
//					  epr_write_data(EPR_Z_PROBE_SPEED_SLOW, (uint8_t *)&mksCfg.z_probe_speed_slow,sizeof(float));
					HAL::AT24CXX_Read(BAK_LEVELING_Z_HIGH_ADDR,(uint8_t *)&gCfgItems.leveling_z_high,2);


				}  
				else if(pMsg->hWinSrc == auto_LevelCmd_default.btnHandle)
				{
					memset(cmd_code,0,sizeof(cmd_code));
				   strcpy((char*)cmd_code, "G28;G29;");
//				   SPI_FLASH_SectorErase(SD_INF_ADDR);
				   flash_reWriteInf((u8 *)cmd_code,BUTTON_AUTOLEVELING_ADDR,(strlen(cmd_code)+1));
				   memset(cmd_code,0,sizeof(cmd_code));
				   SPI_FLASH_BufferRead((u8 *)cmd_code,BUTTON_AUTOLEVELING_ADDR,201);
				   BUTTON_SetText(auto_Leveling_cmd.btnHandle, cmd_code);  
				} 
    		}
    		break;
    		
    	default:
    		WM_DefaultProc(pMsg);
    	}
}


void draw_LevelingPara()
{   
    int i;


    btnHandle_rest();
    
    if(disp_state_stack._disp_state[disp_state_stack._disp_index] != LEVELING_PARA_UI)
    {
        disp_state_stack._disp_index++;
        disp_state_stack._disp_state[disp_state_stack._disp_index] = LEVELING_PARA_UI;
    }
    disp_state = LEVELING_PARA_UI;
    
    GUI_SetBkColor(gCfgItems.background_color);
    GUI_SetColor(gCfgItems.title_color);
    GUI_Clear();
    
    GUI_DispStringAt(machine_menu.LevelingSubConfTitle, TITLE_XPOS, TITLE_YPOS);

    hLevelingParaWnd = WM_CreateWindow(0, 0, LCD_WIDTH, LCD_HEIGHT, WM_CF_SHOW, cbLevelingParaWin, 0);

    if(current_page != 1)
    {
        AutoLevelEnable_text.btnHandle = BUTTON_CreateEx(10,50,240,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
        AutoLevelEnable.btnHandle = BUTTON_CreateEx(370,50,90,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

//        BLtouchEnable_text.btnHandle = BUTTON_CreateEx(10,100,240,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//        BLtouchEnable.btnHandle = BUTTON_CreateEx(370,100,90,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//
//        ProbePort_text.btnHandle = BUTTON_CreateEx(10,150,240,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//        ProbePort.btnHandle = BUTTON_CreateEx(370,150,90,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

	   auto_LevelCmd_text.btnHandle = BUTTON_CreateEx(10,100,240,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	   auto_Leveling_cmd.btnHandle = BUTTON_CreateEx(270,100+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	   auto_LevelCmd_default.btnHandle = BUTTON_CreateEx(370,100+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

	   z_speed_text.btnHandle = BUTTON_CreateEx(10,150,240,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	   z_speed_value.btnHandle = BUTTON_CreateEx(270,150+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	   z_speed_default.btnHandle = BUTTON_CreateEx(370,150+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

	   xy_speed_text.btnHandle = BUTTON_CreateEx(10,200,240,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	   xy_speed_value.btnHandle = BUTTON_CreateEx(270,200+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
	   xy_speed_default.btnHandle = BUTTON_CreateEx(370,200+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//        if(mksCfg.bed_leveling_method == 8)
		if(gCfgItems.leveling_mode == 1)
		{
			BUTTON_SetBmpFileName(AutoLevelEnable.btnHandle, "bmp_enable.bin",1);  //bmp_value_blank_grey
			BUTTON_SetBmpFileName(auto_Leveling_cmd.btnHandle, "bmp_value_blank.bin",1);	
		}        
        else
        {
        	BUTTON_SetBmpFileName(AutoLevelEnable.btnHandle, "bmp_disable.bin",1);
			BUTTON_SetBmpFileName(auto_Leveling_cmd.btnHandle, "bmp_value_blank_grey.bin",1);
        }
           
		 BUTTON_SetBmpFileName(z_speed_value.btnHandle, "bmp_value_blank.bin",1);        	 
		 BUTTON_SetBmpFileName(xy_speed_value.btnHandle, "bmp_value_blank.bin",1);       

		 BUTTON_SetBmpFileName(z_speed_default.btnHandle, "bmp_default.bin",1); 	
		 BUTTON_SetBmpFileName(xy_speed_default.btnHandle, "bmp_default.bin",1); 
		 BUTTON_SetBmpFileName(auto_LevelCmd_default.btnHandle, "bmp_default.bin",1);

		
		 BUTTON_SetBmpFileName(z_speed_text.btnHandle,NULL,1);
		 BUTTON_SetBmpFileName(xy_speed_text.btnHandle,NULL,1);  
		 BUTTON_SetBmpFileName(AutoLevelEnable_text.btnHandle,NULL,1);	
		 BUTTON_SetBmpFileName(auto_LevelCmd_text.btnHandle,NULL,1);	

		 BUTTON_SetBitmapEx(AutoLevelEnable.btnHandle,0,&bmp_struct90X30,0,5); 
     	BUTTON_SetBitmapEx(z_speed_value.btnHandle,0,&bmp_struct70X28,0,0);
        BUTTON_SetBitmapEx(xy_speed_value.btnHandle,0,&bmp_struct70X28,0,0); 
        BUTTON_SetBitmapEx(auto_Leveling_cmd.btnHandle,0,&bmp_struct70X28,0,0);
        BUTTON_SetBitmapEx(auto_LevelCmd_default.btnHandle,0,&bmp_struct70X28,0,0);
     	BUTTON_SetBitmapEx(z_speed_default.btnHandle,0,&bmp_struct70X28,0,0);
        BUTTON_SetBitmapEx(xy_speed_default.btnHandle,0,&bmp_struct70X28,0,0); 

        BUTTON_SetTextAlign(AutoLevelEnable_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );
        BUTTON_SetTextAlign(z_speed_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );   
        BUTTON_SetTextAlign(xy_speed_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );
        BUTTON_SetTextAlign(auto_LevelCmd_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER ); 

		BUTTON_SetTextAlign(AutoLevelEnable.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
		BUTTON_SetTextAlign(z_speed_value.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
		BUTTON_SetTextAlign(xy_speed_value.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );   
		BUTTON_SetTextAlign(auto_Leveling_cmd.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
		
		BUTTON_SetTextAlign(z_speed_default.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
		BUTTON_SetTextAlign(xy_speed_default.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );	
		BUTTON_SetTextAlign(auto_LevelCmd_default.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );

		BUTTON_SetBkColor(auto_Leveling_cmd.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
		BUTTON_SetBkColor(auto_Leveling_cmd.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
		BUTTON_SetTextColor(auto_Leveling_cmd.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
		BUTTON_SetTextColor(auto_Leveling_cmd.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color);

        BUTTON_SetBkColor(z_speed_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
        BUTTON_SetBkColor(z_speed_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
        BUTTON_SetTextColor(z_speed_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
        BUTTON_SetTextColor(z_speed_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color); 
        
        BUTTON_SetBkColor(xy_speed_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
        BUTTON_SetBkColor(xy_speed_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
        BUTTON_SetTextColor(xy_speed_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
        BUTTON_SetTextColor(xy_speed_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color);
		
		BUTTON_SetBkColor(z_speed_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_bk_color);
		BUTTON_SetBkColor(z_speed_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_bk_color); 
		BUTTON_SetTextColor(z_speed_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_text_color);
		BUTTON_SetTextColor(z_speed_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_text_color);
		
		BUTTON_SetBkColor(xy_speed_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_bk_color);
		BUTTON_SetBkColor(xy_speed_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_bk_color); 
		BUTTON_SetTextColor(xy_speed_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_text_color);
		BUTTON_SetTextColor(xy_speed_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_text_color);
		
		BUTTON_SetBkColor(auto_LevelCmd_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_bk_color);
		BUTTON_SetBkColor(auto_LevelCmd_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_bk_color); 
		BUTTON_SetTextColor(auto_LevelCmd_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_text_color);
		BUTTON_SetTextColor(auto_LevelCmd_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_text_color);		

		memset(cmd_code,0,sizeof(cmd_code));
		SPI_FLASH_BufferRead((u8 *)cmd_code,BUTTON_AUTOLEVELING_ADDR,201);
        BUTTON_SetText(auto_Leveling_cmd.btnHandle,cmd_code);  

		memset(cmd_code,0,sizeof(cmd_code));
        sprintf(cmd_code,"%d",gCfgItems.leveling_z_speed);
        BUTTON_SetText(z_speed_value.btnHandle,cmd_code);  
		
        memset(cmd_code,0,sizeof(cmd_code));
        sprintf(cmd_code,"%d",gCfgItems.leveling_xy_speed);
        BUTTON_SetText(xy_speed_value.btnHandle,cmd_code);   

		button_next.btnHandle = BUTTON_CreateEx(320,270,70,40,hLevelingParaWnd,BUTTON_CF_SHOW,0,alloc_win_id());
		BUTTON_SetBmpFileName(button_next.btnHandle, "bmp_back70x40.bin",1); 
		BUTTON_SetBitmapEx(button_next.btnHandle, 0, &bmp_struct70X40,0, 0);

		BUTTON_SetBkColor(button_next.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_color);
		BUTTON_SetBkColor(button_next.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_color); 
		BUTTON_SetTextColor(button_next.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_textcolor);
		BUTTON_SetTextColor(button_next.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_textcolor);
		BUTTON_SetTextAlign(button_next.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );

		if(gCfgItems.multiple_language != 0)
		{
			BUTTON_SetText(z_speed_text.btnHandle,machine_menu.leveling_z_speed);
			BUTTON_SetText(xy_speed_text.btnHandle, machine_menu.leveling_xy_speed); 
			BUTTON_SetText(AutoLevelEnable_text.btnHandle, machine_menu.AutoLevelEnable);
			BUTTON_SetText(auto_LevelCmd_text.btnHandle, machine_menu.AutoLevelCmd);
			BUTTON_SetText(auto_LevelCmd_default.btnHandle, machine_menu.default_value);
			BUTTON_SetText(z_speed_default.btnHandle, machine_menu.default_value);
			BUTTON_SetText(xy_speed_default.btnHandle, machine_menu.default_value);
			BUTTON_SetText(button_next.btnHandle,machine_menu.next);
			 if(gCfgItems.leveling_mode == 1)
                BUTTON_SetText(AutoLevelEnable.btnHandle,machine_menu.enable);
            else
                BUTTON_SetText(AutoLevelEnable.btnHandle,machine_menu.disable);
		}



//	 AT24CXX_Read((uint16_t)EPR_MKSTOUCH,&mksCfg.mkstouch,1);
//        if(mksCfg.mkstouch==1)    
//           BUTTON_SetBmpFileName(BLtouchEnable.btnHandle, "bmp_enable.bin",1);
//        else
//           BUTTON_SetBmpFileName(BLtouchEnable.btnHandle, "bmp_disable.bin",1);
//        if(mksCfg.z_min_probe_pin_mode==1)
//           BUTTON_SetBmpFileName(ProbePort.btnHandle, "bmp_disable.bin",1);
//        else
//           BUTTON_SetBmpFileName(ProbePort.btnHandle, "bmp_enable.bin",1);
        
//        BUTTON_SetBmpFileName(ProbeXoffset_value.btnHandle, "bmp_value_blank.bin",1);
//        BUTTON_SetBmpFileName(ProbeXoffset_default.btnHandle, "bmp_default.bin",1); 
//        
//        BUTTON_SetBmpFileName(AutoLevelEnable_text.btnHandle,NULL,1);        
//        BUTTON_SetBmpFileName(BLtouchEnable_text.btnHandle,NULL,1);
//        BUTTON_SetBmpFileName(ProbePort_text.btnHandle,NULL,1);
//        BUTTON_SetBmpFileName(ProbeXoffset_text.btnHandle,NULL,1);
//        
//        BUTTON_SetBitmapEx(AutoLevelEnable.btnHandle,0,&bmp_struct90X30,0,5);
//        BUTTON_SetBitmapEx(BLtouchEnable.btnHandle,0,&bmp_struct90X30,0,5);
//        BUTTON_SetBitmapEx(ProbePort.btnHandle,0,&bmp_struct90X30,0,5);
//        BUTTON_SetBitmapEx(ProbeXoffset_value.btnHandle,0,&bmp_struct70X28,0,0); 
//        BUTTON_SetBitmapEx(ProbeXoffset_default.btnHandle,0,&bmp_struct70X28,0,0);
//        
//        BUTTON_SetTextAlign(AutoLevelEnable_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );
//        BUTTON_SetTextAlign(BLtouchEnable_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );
//        BUTTON_SetTextAlign(ProbePort_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );
//        BUTTON_SetTextAlign(ProbeXoffset_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );   
//
//        BUTTON_SetTextAlign(AutoLevelEnable.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
//        BUTTON_SetTextAlign(BLtouchEnable.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
//        BUTTON_SetTextAlign(ProbePort.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
//        BUTTON_SetTextAlign(ProbeXoffset_value.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER ); 
//        BUTTON_SetTextAlign(ProbeXoffset_default.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER ); 
//
//        button_next.btnHandle = BUTTON_CreateEx(320,270,70,40,hLevelingParaWnd,BUTTON_CF_SHOW,0,alloc_win_id());
//        BUTTON_SetBmpFileName(button_next.btnHandle, "bmp_next.bin",1); 
//    
//        BUTTON_SetBitmapEx(button_next.btnHandle, 0, &bmp_struct70X40,0, 0);
//
//        BUTTON_SetBkColor(ProbeXoffset_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
//        BUTTON_SetBkColor(ProbeXoffset_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
//        BUTTON_SetTextColor(ProbeXoffset_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
//        BUTTON_SetTextColor(ProbeXoffset_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color); 
//        
//        BUTTON_SetBkColor(ProbeXoffset_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_bk_color);
//        BUTTON_SetBkColor(ProbeXoffset_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_bk_color); 
//        BUTTON_SetTextColor(ProbeXoffset_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_text_color);
//        BUTTON_SetTextColor(ProbeXoffset_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_text_color); 
//
////        memset(cmd_code,0,sizeof(cmd_code));
////        sprintf(cmd_code,"%.1f",mksCfg.x_probe_offset_from_extruder);
////        BUTTON_SetText(ProbeXoffset_value.btnHandle,cmd_code);

    }
    else
    {

        z_high_text.btnHandle = BUTTON_CreateEx(10,50,240,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
        z_high_value.btnHandle = BUTTON_CreateEx(270,50+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
        z_high_default.btnHandle = BUTTON_CreateEx(370,50+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		BUTTON_SetBmpFileName(z_high_value.btnHandle, "bmp_value_blank.bin",1);
		BUTTON_SetBmpFileName(z_high_default.btnHandle, "bmp_default.bin",1); 
		BUTTON_SetBmpFileName(z_high_text.btnHandle,NULL,1);
        BUTTON_SetBitmapEx(z_high_value.btnHandle,0,&bmp_struct70X28,0,0);	
        BUTTON_SetBitmapEx(z_high_default.btnHandle,0,&bmp_struct70X28,0,0);	
        BUTTON_SetTextAlign(z_high_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER ); 
		BUTTON_SetTextAlign(z_high_value.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
		BUTTON_SetTextAlign(z_high_default.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );

		BUTTON_SetBkColor(z_high_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
		BUTTON_SetBkColor(z_high_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
		BUTTON_SetTextColor(z_high_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
		BUTTON_SetTextColor(z_high_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color); 

		BUTTON_SetBkColor(z_high_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_bk_color);
		BUTTON_SetBkColor(z_high_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_bk_color); 
		BUTTON_SetTextColor(z_high_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_text_color);
		BUTTON_SetTextColor(z_high_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_text_color);


		  memset(cmd_code,0,sizeof(cmd_code));
		  sprintf(cmd_code,"%d",gCfgItems.leveling_z_high);
		  BUTTON_SetText(z_high_value.btnHandle,cmd_code);	


//        ProbeYoffset_text.btnHandle = BUTTON_CreateEx(10,50,240,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//        ProbeYoffset_value.btnHandle = BUTTON_CreateEx(270,50+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//        ProbeYoffset_default.btnHandle = BUTTON_CreateEx(370,50+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//
//        ProbeZoffset_text.btnHandle = BUTTON_CreateEx(10,100,240,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//        ProbeZoffset_value.btnHandle = BUTTON_CreateEx(270,100+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//        ProbeZoffset_default.btnHandle = BUTTON_CreateEx(370,100+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//
//        ProbeXYSpeed_text.btnHandle = BUTTON_CreateEx(10,150,240,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//        ProbeXYSpeed_value.btnHandle = BUTTON_CreateEx(270,150+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//        ProbeXYSpeed_default.btnHandle = BUTTON_CreateEx(370,150+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//
//        ProbeZSpeed_text.btnHandle = BUTTON_CreateEx(10,200,240,40,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//        ProbeZSpeed_value.btnHandle = BUTTON_CreateEx(270,200+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//        ProbeZSpeed_default.btnHandle = BUTTON_CreateEx(370,200+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hLevelingParaWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
//
//        BUTTON_SetBmpFileName(ProbeYoffset_value.btnHandle, "bmp_value_blank.bin",1);
//        BUTTON_SetBmpFileName(ProbeZoffset_value.btnHandle, "bmp_value_blank.bin",1);
//        BUTTON_SetBmpFileName(ProbeXYSpeed_value.btnHandle, "bmp_value_blank.bin",1);
//        BUTTON_SetBmpFileName(ProbeZSpeed_value.btnHandle, "bmp_value_blank.bin",1);
//
//        BUTTON_SetBmpFileName(ProbeYoffset_default.btnHandle, "bmp_default.bin",1);
//        BUTTON_SetBmpFileName(ProbeZoffset_default.btnHandle, "bmp_default.bin",1);
//        BUTTON_SetBmpFileName(ProbeXYSpeed_default.btnHandle, "bmp_default.bin",1);
//        BUTTON_SetBmpFileName(ProbeZSpeed_default.btnHandle, "bmp_default.bin",1);
//        
//        BUTTON_SetBmpFileName(ProbeYoffset_text.btnHandle,NULL,1);
//        BUTTON_SetBmpFileName(ProbeZoffset_text.btnHandle,NULL,1);  
//        BUTTON_SetBmpFileName(ProbeXYSpeed_text.btnHandle,NULL,1);
//        BUTTON_SetBmpFileName(ProbeZSpeed_text.btnHandle,NULL,1);  
        
//        BUTTON_SetBitmapEx(ProbeYoffset_value.btnHandle,0,&bmp_struct70X28,0,0);
//        BUTTON_SetBitmapEx(ProbeZoffset_value.btnHandle,0,&bmp_struct70X28,0,0); 
//        BUTTON_SetBitmapEx(ProbeXYSpeed_value.btnHandle,0,&bmp_struct70X28,0,0);
//        BUTTON_SetBitmapEx(ProbeZSpeed_value.btnHandle,0,&bmp_struct70X28,0,0); 
//        
//        BUTTON_SetBitmapEx(ProbeYoffset_default.btnHandle,0,&bmp_struct70X28,0,0);
//        BUTTON_SetBitmapEx(ProbeZoffset_default.btnHandle,0,&bmp_struct70X28,0,0); 
//        BUTTON_SetBitmapEx(ProbeXYSpeed_default.btnHandle,0,&bmp_struct70X28,0,0);
//        BUTTON_SetBitmapEx(ProbeZSpeed_default.btnHandle,0,&bmp_struct70X28,0,0); 
//        
//        BUTTON_SetTextAlign(ProbeYoffset_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );
//        BUTTON_SetTextAlign(ProbeZoffset_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );   
//        BUTTON_SetTextAlign(ProbeXYSpeed_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );
//        BUTTON_SetTextAlign(ProbeZSpeed_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER ); 
//        
//        BUTTON_SetTextAlign(ProbeYoffset_value.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
//        BUTTON_SetTextAlign(ProbeZoffset_value.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );   
//        BUTTON_SetTextAlign(ProbeXYSpeed_value.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
//        BUTTON_SetTextAlign(ProbeZSpeed_value.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER ); 
//        BUTTON_SetTextAlign(ProbeYoffset_default.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
//        BUTTON_SetTextAlign(ProbeZoffset_default.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );   
//        BUTTON_SetTextAlign(ProbeXYSpeed_default.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
//        BUTTON_SetTextAlign(ProbeZSpeed_default.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );         
//
//        button_previous.btnHandle = BUTTON_CreateEx(320,270,70,40,hLevelingParaWnd,BUTTON_CF_SHOW,0,alloc_win_id());
//        BUTTON_SetBmpFileName(button_previous.btnHandle, "bmp_previous.bin",1); 
        
//        BUTTON_SetBitmapEx(button_previous.btnHandle, 0, &bmp_struct70X40,0, 0);
//
//        BUTTON_SetBkColor(ProbeYoffset_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
//        BUTTON_SetBkColor(ProbeYoffset_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
//        BUTTON_SetTextColor(ProbeYoffset_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
//        BUTTON_SetTextColor(ProbeYoffset_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color); 
//        
//        BUTTON_SetBkColor(ProbeYoffset_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_bk_color);
//        BUTTON_SetBkColor(ProbeYoffset_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_bk_color); 
//        BUTTON_SetTextColor(ProbeYoffset_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_text_color);
//        BUTTON_SetTextColor(ProbeYoffset_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_text_color); 
//
//        BUTTON_SetBkColor(ProbeZoffset_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
//        BUTTON_SetBkColor(ProbeZoffset_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
//        BUTTON_SetTextColor(ProbeZoffset_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
//        BUTTON_SetTextColor(ProbeZoffset_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color); 
//        
//        BUTTON_SetBkColor(ProbeZoffset_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_bk_color);
//        BUTTON_SetBkColor(ProbeZoffset_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_bk_color); 
//        BUTTON_SetTextColor(ProbeZoffset_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_text_color);
//        BUTTON_SetTextColor(ProbeZoffset_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_text_color); 
//
//        BUTTON_SetBkColor(ProbeXYSpeed_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
//        BUTTON_SetBkColor(ProbeXYSpeed_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
//        BUTTON_SetTextColor(ProbeXYSpeed_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
//        BUTTON_SetTextColor(ProbeXYSpeed_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color); 
//        
//        BUTTON_SetBkColor(ProbeXYSpeed_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_bk_color);
//        BUTTON_SetBkColor(ProbeXYSpeed_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_bk_color); 
//        BUTTON_SetTextColor(ProbeXYSpeed_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_text_color);
//        BUTTON_SetTextColor(ProbeXYSpeed_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_text_color); 
//
//        BUTTON_SetBkColor(ProbeZSpeed_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
//        BUTTON_SetBkColor(ProbeZSpeed_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
//        BUTTON_SetTextColor(ProbeZSpeed_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
//        BUTTON_SetTextColor(ProbeZSpeed_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color); 
//        
//        BUTTON_SetBkColor(ProbeZSpeed_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_bk_color);
//        BUTTON_SetBkColor(ProbeZSpeed_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_bk_color); 
//        BUTTON_SetTextColor(ProbeZSpeed_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_text_color);
//        BUTTON_SetTextColor(ProbeZSpeed_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_text_color); 

//        memset(cmd_code,0,sizeof(cmd_code));
//        sprintf(cmd_code,"%.1f",mksCfg.y_probe_offset_from_extruder);
//        BUTTON_SetText(ProbeYoffset_value.btnHandle,cmd_code);  
//        memset(cmd_code,0,sizeof(cmd_code));
//        sprintf(cmd_code,"%.1f",zprobe_zoffset);
//        BUTTON_SetText(ProbeZoffset_value.btnHandle,cmd_code);   
//
//        memset(cmd_code,0,sizeof(cmd_code));
//        sprintf(cmd_code,"%.1f",mksCfg.xy_probe_speed);
//        BUTTON_SetText(ProbeXYSpeed_value.btnHandle,cmd_code); 
//        memset(cmd_code,0,sizeof(cmd_code));
//        sprintf(cmd_code,"%.1f",mksCfg.z_probe_speed_fast);
//        BUTTON_SetText(ProbeZSpeed_value.btnHandle,cmd_code);       

		button_previous.btnHandle = BUTTON_CreateEx(320,270,70,40,hLevelingParaWnd,BUTTON_CF_SHOW,0,alloc_win_id());
		BUTTON_SetBmpFileName(button_previous.btnHandle, "bmp_previous.bin",1); 
		BUTTON_SetBitmapEx(button_previous.btnHandle, 0, &bmp_struct70X40,0, 0);

		BUTTON_SetBkColor(button_previous.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_color);
		BUTTON_SetBkColor(button_previous.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_color); 
		BUTTON_SetTextColor(button_previous.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_textcolor);
		BUTTON_SetTextColor(button_previous.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_textcolor);
		BUTTON_SetTextAlign(button_previous.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );

		if(gCfgItems.multiple_language != 0)
		{	
			BUTTON_SetText(z_high_text.btnHandle,machine_menu.leveling_z_high);
			BUTTON_SetText(z_high_default.btnHandle,machine_menu.default_value);
			BUTTON_SetText(button_previous.btnHandle,machine_menu.previous);
		}
    }

     button_back.btnHandle = BUTTON_CreateEx(400,270,70,40,hLevelingParaWnd,BUTTON_CF_SHOW,0,alloc_win_id());
     
     BUTTON_SetBmpFileName(button_back.btnHandle, "bmp_back70x40.bin",1);        
     BUTTON_SetBitmapEx(button_back.btnHandle, 0, &bmp_struct70X40,0, 0);
        
//     BUTTON_SetBkColor(button_next.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_color);
//     BUTTON_SetBkColor(button_next.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_color); 
//     BUTTON_SetTextColor(button_next.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_textcolor);
//     BUTTON_SetTextColor(button_next.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_textcolor);
//     BUTTON_SetBkColor(button_previous.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_color);
//     BUTTON_SetBkColor(button_previous.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_color);    
//     BUTTON_SetTextColor(button_previous.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_textcolor);
//     BUTTON_SetTextColor(button_previous.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_textcolor);    
     BUTTON_SetBkColor(button_back.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_color);
     BUTTON_SetBkColor(button_back.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_color); 
     BUTTON_SetTextColor(button_back.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_textcolor);
     BUTTON_SetTextColor(button_back.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_textcolor); 
     
//     BUTTON_SetTextAlign(button_next.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
//     BUTTON_SetTextAlign(button_previous.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
     BUTTON_SetTextAlign(button_back.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );

     if(gCfgItems.multiple_language != 0)
     {
        if(current_page != 1)
        {
//            BUTTON_SetText(AutoLevelEnable_text.btnHandle, machine_menu.AutoLevelEnable);
//            BUTTON_SetText(BLtouchEnable_text.btnHandle, machine_menu.BLtouchEnable);
//            BUTTON_SetText(ProbePort_text.btnHandle, machine_menu.ProbePort);
//            BUTTON_SetText(ProbeXoffset_text.btnHandle, machine_menu.ProbeXoffset);
//            BUTTON_SetText(button_next.btnHandle,machine_menu.next);
//
//            if(mksCfg.bed_leveling_method==8)
//                BUTTON_SetText(AutoLevelEnable.btnHandle,machine_menu.enable);
//            else
//                BUTTON_SetText(AutoLevelEnable.btnHandle,machine_menu.disable);
//
//            if(mksCfg.mkstouch==1)
//                BUTTON_SetText(BLtouchEnable.btnHandle,machine_menu.enable);
//            else
//                BUTTON_SetText(BLtouchEnable.btnHandle,machine_menu.disable); 
//            
//            if(mksCfg.z_min_probe_pin_mode==1)
//                BUTTON_SetText(ProbePort.btnHandle,machine_menu.z_min);
//            else
//                BUTTON_SetText(ProbePort.btnHandle,machine_menu.z_max);  
//
//            BUTTON_SetText(ProbeXoffset_default.btnHandle, machine_menu.default_value);

        }
        else
        {
//            BUTTON_SetText(ProbeYoffset_text.btnHandle,machine_menu.ProbeYoffset);
//            BUTTON_SetText(ProbeZoffset_text.btnHandle, machine_menu.ProbeZoffset); 
//            BUTTON_SetText(ProbeXYSpeed_text.btnHandle,machine_menu.ProbeXYspeed);
//            BUTTON_SetText(ProbeZSpeed_text.btnHandle, machine_menu.ProbeZspeed);            
//            BUTTON_SetText(button_previous.btnHandle,machine_menu.previous); 
//
//            BUTTON_SetText(ProbeYoffset_default.btnHandle, machine_menu.default_value);
//            BUTTON_SetText(ProbeZoffset_default.btnHandle, machine_menu.default_value);
//            BUTTON_SetText(ProbeXYSpeed_default.btnHandle, machine_menu.default_value);
//            BUTTON_SetText(ProbeZSpeed_default.btnHandle, machine_menu.default_value);
        }
	        BUTTON_SetText(button_back.btnHandle,common_menu.text_back);
              
     }

}


void Clear_LevelingPara()
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hLevelingParaWnd))
	{
		WM_DeleteWindow(hLevelingParaWnd);
	}
}





