#include "gui.h"
#include "button.h"
#include "draw_ui.h"
#include "at24cxx.h"

#include "spi_flash.h"
#include "pic_manager.h"
#include "wifi_module.h"


#ifndef GUI_FLASH
#define GUI_FLASH
#endif

static GUI_HWIN hWifiConfigWnd;
static BUTTON_STRUCT WifiMode_text,WifiName_text,Key_text,CloudEnable_text,CloudHost_text;
static BUTTON_STRUCT WifiMode_sel,WifiName_value,Key_value,CloudEnable,CloudHost_value;
static BUTTON_STRUCT port_number_text,port_number_value,port_number_default;

static BUTTON_STRUCT button_next,button_previous,button_back;

static uint8_t current_page;//0:上一页，1:下一页

extern int cfg_wifi_flag;
extern int cfg_cloud_flag;
static void cbWifiConfigWin(WM_MESSAGE * pMsg) {

    struct PressEvt *press_event;
    switch (pMsg->MsgId)
    {
    	case WM_PAINT:
			//GUI_SetColor(gCfgItems.state_background_color);
			//GUI_DrawRect(LCD_WIDTH/4+X_ADD, 0, LCD_WIDTH *3 / 4-X_INTERVAL, imgHeight /2 -15);
			GUI_SetColor(0xff5449);
			GUI_FillRect(10, 90, 470,90);
			GUI_FillRect(10, 140, 470,140);

			if(current_page != 1)
			{
//		            GUI_FillRect(10, 140, 470,140);
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
    				current_page  =0;
					cfg_wifi_flag = 1;
//					if (cfg_wifi_flag == 1)
//						wifi_init();
    				last_disp_state = WIFI_CONF_UI;
    				Clear_WifiConfig();
    				draw_return_ui();
    			}					
    			else if(pMsg->hWinSrc == WifiMode_sel.btnHandle)// 2:sta 1:ap
    			{
						if (gCfgItems.wifi_mode_sel == 1)
						{
							gCfgItems.wifi_mode_sel = 2;//sta
							gCfgItems.wifi_scan = 1;
							BUTTON_SetBmpFileName(WifiMode_sel.btnHandle, "bmp_disable.bin",1);							
							BUTTON_SetText(WifiMode_sel.btnHandle,machine_menu.WifiMode0);//
						}
						else 
						{
							gCfgItems.wifi_mode_sel = 1;//ap
							gCfgItems.wifi_scan  = 0;
							BUTTON_SetBmpFileName(WifiMode_sel.btnHandle, "bmp_enable.bin",1);
							BUTTON_SetText(WifiMode_sel.btnHandle,machine_menu.WifiMode1);
						}
					
						flash_reWriteInf((uint8_t *)&gCfgItems.wifi_mode_sel,WIFI_MODE_SEL_ADDR,1);	
//						cfg_wifi_flag = 1;
    			}
				else if(pMsg->hWinSrc == CloudEnable.btnHandle)
				{
					cfg_cloud_flag = 1;
					if (gCfgItems.cloud_enable == 1)
					{
						gCfgItems.cloud_enable = 0;
						BUTTON_SetBmpFileName(CloudEnable.btnHandle, "bmp_disable.bin",1);
						BUTTON_SetText(CloudEnable.btnHandle,machine_menu.disable);
					}
					else 
					{
						gCfgItems.cloud_enable = 1;
						BUTTON_SetBmpFileName(CloudEnable.btnHandle, "bmp_enable.bin",1);
						BUTTON_SetText(CloudEnable.btnHandle,machine_menu.enable);
					}
					HAL::AT24CXX_Write(BAK_CLOUD_FLAG_ADDR,(uint8_t *)&gCfgItems.cloud_enable,1);					
				}
    			else if(pMsg->hWinSrc == WifiName_value.btnHandle)
    			{
    				if (gCfgItems.wifi_mode_sel == 1)
    				{
    					last_disp_state = WIFI_CONF_UI;
	                    Clear_WifiConfig();
	                    valueType=wifi_name; 
	                    draw_Keyboard();   
    				}
    			}
    			else if(pMsg->hWinSrc == Key_value.btnHandle)
    			{
					if(gCfgItems.wifi_mode_sel == 1 )
					{
						last_disp_state = WIFI_CONF_UI;
	                    Clear_WifiConfig();
	                    valueType=wifi_key;
	                    draw_Keyboard();
					}       
    			} 
			else if(pMsg->hWinSrc == CloudHost_value.btnHandle)
    			{
    				last_disp_state = WIFI_CONF_UI;
	                   Clear_WifiConfig();
	                    valueType=wifi_host;
	                    draw_Keyboard();                      
    			}
    			else if(pMsg->hWinSrc == port_number_value.btnHandle)
    			{
    				last_disp_state = WIFI_CONF_UI;
	                   Clear_WifiConfig();
	                    value=wifi_port;
	                    draw_NumberKey();                                        
    			} 
			else if(pMsg->hWinSrc == port_number_default.btnHandle)
    			{
                 gCfgItems.cloud_port = 10086;
				memset(cmd_code,0,sizeof(cmd_code));
				sprintf(cmd_code,"%d",gCfgItems.cloud_port);
				 BUTTON_SetText(port_number_value.btnHandle, cmd_code);  
    			}
			else if(pMsg->hWinSrc == button_next.btnHandle)
	             {
	                    current_page = 1;
	                    last_disp_state = WIFI_CONF_UI;
	                    Clear_WifiConfig();
	                    draw_WifiConfig();
	              }
	              else if(pMsg->hWinSrc == button_previous.btnHandle)
	              {
	                    current_page = 0;
	                    last_disp_state = WIFI_CONF_UI;
	                    Clear_WifiConfig();
	                    draw_WifiConfig();
	              } 
    		}
    		break;
    		
    	default:
    		WM_DefaultProc(pMsg);
    	}
}


void draw_WifiConfig()
{   
    WifiMode_text.btnHandle=0;
    WifiName_text.btnHandle=0;
    Key_text.btnHandle=0;
    CloudEnable_text.btnHandle=0;
    CloudHost_text.btnHandle=0;
	port_number_text.btnHandle = 0;

    WifiMode_sel.btnHandle=0;
    WifiName_value.btnHandle=0;
    Key_value.btnHandle=0;
    CloudEnable.btnHandle=0;
    CloudHost_value.btnHandle=0;

	port_number_value.btnHandle = 0;
	port_number_default.btnHandle = 0;

    button_back.btnHandle=0;

    button_next.btnHandle=0;
    button_previous.btnHandle=0;

    if(disp_state_stack._disp_state[disp_state_stack._disp_index] != WIFI_CONF_UI)
    {
        disp_state_stack._disp_index++;
        disp_state_stack._disp_state[disp_state_stack._disp_index] = WIFI_CONF_UI;
    }
    disp_state = WIFI_CONF_UI;
    
    GUI_SetBkColor(gCfgItems.background_color);
    GUI_SetColor(gCfgItems.title_color);
    GUI_Clear();
    
    GUI_DispStringAt(machine_menu.wifiConfTile, TITLE_XPOS, TITLE_YPOS);

    hWifiConfigWnd = WM_CreateWindow(0, 0, LCD_WIDTH, LCD_HEIGHT, WM_CF_SHOW, cbWifiConfigWin, 0);

    if(current_page != 1)
    {

		WifiMode_text.btnHandle = BUTTON_CreateEx(10,50,240,40,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
        WifiMode_sel.btnHandle = BUTTON_CreateEx(370,50,90,40,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

		WifiName_text.btnHandle = BUTTON_CreateEx(10,100,240,40,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		WifiName_value.btnHandle = BUTTON_CreateEx(370,100+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		
		Key_text.btnHandle = BUTTON_CreateEx(10,150,240,40,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		Key_value.btnHandle = BUTTON_CreateEx(370,150+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		
		CloudEnable_text.btnHandle = BUTTON_CreateEx(10,200,240,40,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		CloudEnable.btnHandle = BUTTON_CreateEx(370,200,90,40,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

		if (gCfgItems.wifi_mode_sel == 2)
			    BUTTON_SetBmpFileName(WifiMode_sel.btnHandle, "bmp_disable.bin",1);  
		else
			    BUTTON_SetBmpFileName(WifiMode_sel.btnHandle, "bmp_enable.bin",1);  

		if(gCfgItems.cloud_enable == 1)
				BUTTON_SetBmpFileName(CloudEnable.btnHandle, "bmp_enable.bin",1);
		else 
				BUTTON_SetBmpFileName(CloudEnable.btnHandle, "bmp_disable.bin",1);
     
	    BUTTON_SetBmpFileName(WifiName_value.btnHandle, "bmp_pic70x28_1.bin",1);
	    BUTTON_SetBmpFileName(Key_value.btnHandle, "bmp_pic70x28_1.bin",1);
	    
	    BUTTON_SetBmpFileName(WifiMode_text.btnHandle,NULL,1);        
	    BUTTON_SetBmpFileName(WifiName_text.btnHandle,NULL,1);
	    BUTTON_SetBmpFileName(Key_text.btnHandle,NULL,1);
	    BUTTON_SetBmpFileName(CloudEnable_text.btnHandle,NULL,1);

		BUTTON_SetBitmapEx(WifiMode_sel.btnHandle,0,&bmp_struct90X30,0,5); 	    
	    BUTTON_SetBitmapEx(WifiName_value.btnHandle,0,&bmp_struct70X28,0,0);
	    BUTTON_SetBitmapEx(Key_value.btnHandle,0,&bmp_struct70X28,0,0);
		BUTTON_SetBitmapEx(CloudEnable.btnHandle,0,&bmp_struct90X30,0,5); 

	    BUTTON_SetTextAlign(WifiMode_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );
	    BUTTON_SetTextAlign(WifiName_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );
	    BUTTON_SetTextAlign(Key_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );
	    BUTTON_SetTextAlign(CloudEnable_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );

	    BUTTON_SetTextAlign(WifiMode_sel.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
	    BUTTON_SetTextAlign(WifiName_value.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
	    BUTTON_SetTextAlign(Key_value.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
	    BUTTON_SetTextAlign(CloudEnable.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );

	    BUTTON_SetBkColor(WifiName_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
	    BUTTON_SetBkColor(WifiName_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
	    BUTTON_SetTextColor(WifiName_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
	    BUTTON_SetTextColor(WifiName_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color); 

	    BUTTON_SetBkColor(Key_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
	    BUTTON_SetBkColor(Key_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
	    BUTTON_SetTextColor(Key_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
	    BUTTON_SetTextColor(Key_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color); 


//	    BUTTON_SetBkColor(CloudEnable.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
//	    BUTTON_SetBkColor(CloudEnable.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
//	    BUTTON_SetTextColor(CloudEnable.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
//	    BUTTON_SetTextColor(CloudEnable.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color); 

//		BUTTON_SetBkColor(level_point4_y_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
//	    BUTTON_SetBkColor(level_point4_y_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
//	    BUTTON_SetTextColor(level_point4_y_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
//	    BUTTON_SetTextColor(level_point4_y_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color);

		BUTTON_SetBkColor(port_number_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
	    BUTTON_SetBkColor(port_number_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
	    BUTTON_SetTextColor(port_number_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
	    BUTTON_SetTextColor(port_number_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color);

		BUTTON_SetBkColor(port_number_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_bk_color);
		BUTTON_SetBkColor(port_number_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_bk_color); 
		BUTTON_SetTextColor(port_number_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_text_color);
		BUTTON_SetTextColor(port_number_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_text_color);

		memset(cmd_code,0,sizeof(cmd_code));			
		SPI_FLASH_BufferRead((u8*)cmd_code,WIFI_NAME_ADDR,200);
		BUTTON_SetText(WifiName_value.btnHandle,cmd_code);		

		memset(cmd_code,0,sizeof(cmd_code));
		SPI_FLASH_BufferRead((u8*)cmd_code,WIFI_KEYCODE_ADDR,200);
		BUTTON_SetText(Key_value.btnHandle,cmd_code);



		button_next.btnHandle = BUTTON_CreateEx(320,270,70,40,hWifiConfigWnd,BUTTON_CF_SHOW,0,alloc_win_id());
		BUTTON_SetBmpFileName(button_next.btnHandle, "bmp_pic70x40.bin",1); 
		BUTTON_SetBitmapEx(button_next.btnHandle, 0, &bmp_struct70X40,0, 0);

		BUTTON_SetBkColor(button_next.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_color);
		BUTTON_SetBkColor(button_next.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_color); 
		BUTTON_SetTextColor(button_next.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_textcolor);
		BUTTON_SetTextColor(button_next.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_textcolor);
		BUTTON_SetTextAlign(button_next.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
	     
	    if(gCfgItems.multiple_language != 0)
	    {
	        BUTTON_SetText(WifiMode_text.btnHandle, machine_menu.WifiMode);
	        BUTTON_SetText(WifiName_text.btnHandle, machine_menu.WifiName);
	        BUTTON_SetText(Key_text.btnHandle, machine_menu.WifiKey); 
	        BUTTON_SetText(CloudEnable_text.btnHandle, machine_menu.cloudEnable);  
		 	BUTTON_SetText(button_next.btnHandle,machine_menu.next);
			
			if(gCfgItems.wifi_mode_sel == 1)
				BUTTON_SetText(WifiMode_sel.btnHandle, machine_menu.WifiMode1);
			else 
				BUTTON_SetText(WifiMode_sel.btnHandle, machine_menu.WifiMode0);
			
			if (gCfgItems.cloud_enable == 1)
				BUTTON_SetText(CloudEnable.btnHandle, machine_menu.enable);
			else 	
		        BUTTON_SetText(CloudEnable.btnHandle, machine_menu.disable);
	   }
    }
    else
    {
		CloudHost_text.btnHandle = BUTTON_CreateEx(10,50,240,40,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
       	CloudHost_value.btnHandle = BUTTON_CreateEx(400,50+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

		port_number_text.btnHandle = BUTTON_CreateEx(10,100,240,40,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		port_number_value.btnHandle = BUTTON_CreateEx(320,100+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());
		port_number_default.btnHandle = BUTTON_CreateEx(400,100+5,VALUE_DEFAULT_X,VALUE_DEFAULT_Y,hWifiConfigWnd, BUTTON_CF_SHOW, 0, alloc_win_id());

	    BUTTON_SetBmpFileName(CloudHost_value.btnHandle, "bmp_pic70x28_1.bin",1);
	    BUTTON_SetBmpFileName(port_number_value.btnHandle, "bmp_pic70x28_1.bin",1); 
	    BUTTON_SetBmpFileName(port_number_default.btnHandle, "bmp_pic70x28_2.bin",1); 

		BUTTON_SetBmpFileName(CloudHost_text.btnHandle,NULL,1);
		BUTTON_SetBmpFileName(port_number_text.btnHandle,NULL,1);
		
	    BUTTON_SetBitmapEx(port_number_value.btnHandle,0,&bmp_struct70X28,0,0);//图片大小为70x28
	    BUTTON_SetBitmapEx(CloudHost_value.btnHandle,0,&bmp_struct70X28,0,0);//图片大小为70x28
	    BUTTON_SetBitmapEx(port_number_default.btnHandle,0,&bmp_struct70X28,0,0);//图片大小为70x28


	    BUTTON_SetBkColor(CloudHost_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
	    BUTTON_SetBkColor(CloudHost_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
	    BUTTON_SetTextColor(CloudHost_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
	    BUTTON_SetTextColor(CloudHost_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color); 

	    BUTTON_SetBkColor(port_number_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_bk_color);
	    BUTTON_SetBkColor(port_number_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_bk_color); 
	    BUTTON_SetTextColor(port_number_value.btnHandle, BUTTON_CI_PRESSED, gCfgItems.value_text_color);
	    BUTTON_SetTextColor(port_number_value.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.value_text_color); 


	    BUTTON_SetBkColor(port_number_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_bk_color);
	    BUTTON_SetBkColor(port_number_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_bk_color); 
	    BUTTON_SetTextColor(port_number_default.btnHandle, BUTTON_CI_PRESSED, gCfgItems.default_text_color);
	    BUTTON_SetTextColor(port_number_default.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.default_text_color); 

	    BUTTON_SetTextAlign(CloudHost_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );
	    BUTTON_SetTextAlign(CloudHost_value.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
	    BUTTON_SetTextAlign(port_number_text.btnHandle,GUI_TA_LEFT|GUI_TA_VCENTER );
	    BUTTON_SetTextAlign(port_number_value.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
	    BUTTON_SetTextAlign(port_number_default.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );

		
		memset(cmd_code,0,sizeof(cmd_code));
		strncpy(cmd_code, (char *)gCfgItems.cloud_hostUrl, 96);
		BUTTON_SetText(CloudHost_value.btnHandle,cmd_code);			

		memset(cmd_code,0,sizeof(cmd_code));
	    sprintf(cmd_code,"%d",gCfgItems.cloud_port);
	    BUTTON_SetText(port_number_value.btnHandle,cmd_code);


	    button_previous.btnHandle = BUTTON_CreateEx(320,270,70,40,hWifiConfigWnd,BUTTON_CF_SHOW,0,alloc_win_id());
		BUTTON_SetBmpFileName(button_previous.btnHandle, "bmp_pic70x40.bin",1); 

		BUTTON_SetBitmapEx(button_previous.btnHandle, 0, &bmp_struct70X40,0, 0);

		BUTTON_SetBkColor(button_previous.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_color);
		BUTTON_SetBkColor(button_previous.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_color); 
		BUTTON_SetTextColor(button_previous.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_textcolor);
		BUTTON_SetTextColor(button_previous.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_textcolor);

		BUTTON_SetTextAlign(button_previous.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
		

	    if(gCfgItems.multiple_language != 0)
	    {
	        BUTTON_SetText(CloudHost_text.btnHandle, machine_menu.cloudHost);
	        BUTTON_SetText(port_number_text.btnHandle, machine_menu.cloudPort);
	        BUTTON_SetText(port_number_default.btnHandle, machine_menu.cloudPort);			
	    	BUTTON_SetText(button_previous.btnHandle,machine_menu.previous); 
			BUTTON_SetText(port_number_default.btnHandle,machine_menu.default_value); 
	    }
    }

    button_back.btnHandle = BUTTON_CreateEx(400,270,70,40,hWifiConfigWnd,BUTTON_CF_SHOW,0,alloc_win_id());
    
    BUTTON_SetBmpFileName(button_back.btnHandle, "bmp_pic70x40.bin",1);        
    BUTTON_SetBitmapEx(button_back.btnHandle, 0, &bmp_struct70X40,0,0);
    BUTTON_SetTextAlign(button_back.btnHandle,GUI_TA_HCENTER|GUI_TA_VCENTER );
    
    BUTTON_SetBkColor(button_back.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_color);
    BUTTON_SetBkColor(button_back.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_color); 
    BUTTON_SetTextColor(button_back.btnHandle, BUTTON_CI_PRESSED, gCfgItems.back1_btn_textcolor);
    BUTTON_SetTextColor(button_back.btnHandle, BUTTON_CI_UNPRESSED, gCfgItems.back1_btn_textcolor); 
    
    BUTTON_SetText(button_back.btnHandle,common_menu.text_back);

}

void Clear_WifiConfig()	
{
	GUI_SetBkColor(gCfgItems.background_color);
	if(WM_IsWindow(hWifiConfigWnd))
	{
		WM_DeleteWindow(hWifiConfigWnd);
	}
}






