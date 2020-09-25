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
*/

#include "ff.h"
#include "Repetier.h"
#include "ui.h"
//#include "mmc_sd.h"	//skyblue 2016-12-13
#include "spi_flash.h"
#include "pic_manager.h"
//#include "sdio_sdcard.h"
#include "sdio.h"       //skyblue 2016-12-13
#include "draw_pause_ui.h"   
#include "draw_operate.h"   
//#include "draw_ui.h"
#include "fatfs.h"      //skyblue 2016-12-13 
#include "usb_host.h"
//#include "others.h"

extern PRINT_TIME  print_time;
extern ApplicationTypeDef Appli_state;
ApplicationTypeDef Appli_state_old = APPLICATION_IDLE;

#ifndef SDSUPPORT
#define SDSUPPORT
#endif

#if SDSUPPORT


char tempLongFilename[LONG_FILENAME_LENGTH + 1];
char fullName[LONG_FILENAME_LENGTH * SD_MAX_FOLDER_DEPTH + SD_MAX_FOLDER_DEPTH + 1];

SDCard sd;

char *cfg_file = "1:/mks_config.txt";    
char *cfg_file_cur = "/config_cur.txt";
extern char cmd_code[201];

extern volatile uint8_t sd_operate_flag;
uint8_t  mksPrinterStatusFlag = MKS_PR_IDLE;
uint8_t Printer::flag1 = 0;

/*********************add********************************/
FATFS fs;
/**********************end*******************************/
#if 1
SDCard::SDCard()
{
    sdmode = 0;
    sdactive = false;
    usbactive = false;
    savetosd = false;
    Printer::setAutomount(false);
}
#endif
void SDCard::automount()
{
  if(usbactive == false)        //U盘没有挂载
  {
      if (SD_DET_IP != SDCARDDETECTINVERTED)    
    {
        if(sdactive || sdmode == 100)   // Card removed
        {
            //**Com::printFLN(PSTR("SD card removed"));
            FATFS_UnLinkDriver(SD_Path); 
            unmount();
        }
    }
    else
    {
        if(!sdactive)
        {
            //**Com::printFLN(PSTR("SD card inserted"));
            FATFS_LinkDriver(&SD_Driver, SD_Path);
            initsd();
        }
    }
  }
    
if (SD_DET_IP != SDCARDDETECTINVERTED)  //无SD卡,检测到U盘，挂载
  {
    if((usbactive == false)&& (Appli_state == APPLICATION_READY))
    {
        //**Com::printFLN(PSTR("USB inserted"));
        FATFS_LinkDriver(&USBH_Driver, USBH_Path);
      
        f_mount(&fs, (TCHAR const*)USBH_Path, 0);
        sdactive = true;
        usbactive = true;
        Appli_state = APPLICATION_IDLE;
      
    }
  } 
  
  if((Appli_state == APPLICATION_DISCONNECT) &&(usbactive == true))     //检测到U盘断开，卸载
  {
    unmount();
    usbactive = false;
    //**Com::printFLN(PSTR("USB removed"));
    FATFS_UnLinkDriver(USBH_Path);
  }
}

void SDCard::initsd()
{
  FRESULT mksMountState = FR_DISK_ERR;
    sdactive = false;

    if( SD_DET_IP != SDCARDDETECTINVERTED)
        return;

    mksMountState = f_mount(&fs, (TCHAR const*)SD_Path, 0);     //skyblue 2016-12-13
    sdactive = true;
}


void SDCard::mount()
{
    sdmode = false;
    initsd();
}

void SDCard::unmount()
{
    sdmode = false;
    sdactive = false;
    savetosd = false;
    Printer::setAutomount(false);
  //  Printer::setMenuMode(MENU_MODE_SD_MOUNTED+MENU_MODE_SD_PAUSED+MENU_MODE_SD_PRINTING,false);
#if UI_DISPLAY_TYPE!=0 && SDSUPPORT
    uid.cwd[0]='/';
    uid.cwd[1]=0;
    uid.folderLevel=0;
#endif
}
#if 0   //**
void SDCard::startPrint()
{
    if(!sdactive) return;
    sdmode = true;
    //Printer::setMenuMode(MENU_MODE_SD_PRINTING,true);
   // Printer::setMenuMode(MENU_MODE_SD_PAUSED,false);
   savecurFileName();
   mksPrinterStatusFlag = MKS_PR_WORKING;
}

void SDCard::pausePrint(bool intern)
{
    if(!sd.sdactive) return;
    sdmode = 2; // finish running line
    //Printer::setMenuMode(MENU_MODE_SD_PAUSED, true);
    if(intern) {
        Commands::waitUntilEndOfAllBuffers();
        sdmode = 0;
        Printer::MemoryPosition();
        Printer::moveToReal(IGNORE_COORDINATE, IGNORE_COORDINATE, IGNORE_COORDINATE,
                            Printer::memoryE - RETRACT_ON_PAUSE,
                            Printer::maxFeedrate[E_AXIS] / 2);
//#if DRIVE_SYSTEM == DELTA
          //Z轴上升
        PrintLine::moveRelativeDistanceInStepsReal(0,0,PAUSE_Z_RETRACT*Printer::axisStepsPerMM[Z_AXIS],0,Printer::maxFeedrate[Z_AXIS],true);  
        Commands::waitUntilEndOfAllMoves();

        if(CfgPrinterItems.cfg_drive_system == DELTA)
        {
          //移动X,Y
        Printer::moveToReal(0, 0.9 * EEPROM::deltaMaxRadius(), IGNORE_COORDINATE, IGNORE_COORDINATE, Printer::maxFeedrate[X_AXIS]);
        }
        else
        {
          //移动X,Y
        //Printer::moveToReal(Printer::xMin, Printer::yMin + Printer::yLength, IGNORE_COORDINATE, IGNORE_COORDINATE, Printer::maxFeedrate[X_AXIS]);
        Printer::moveToReal(Printer::xMin+20, Printer::yMin + 20, IGNORE_COORDINATE, IGNORE_COORDINATE, Printer::maxFeedrate[X_AXIS]);
        }
//#endif
        Commands::waitUntilEndOfAllMoves();
        Printer::lastCmdPos[X_AXIS] = Printer::currentPosition[X_AXIS];
        Printer::lastCmdPos[Y_AXIS] = Printer::currentPosition[Y_AXIS];
        Printer::lastCmdPos[Z_AXIS] = Printer::currentPosition[Z_AXIS];
        //GCode::executeFString(PSTR(PAUSE_START_COMMANDS));
        
        HAL::eprSetByte(EPR_SAV_FLAG,(uint8_t)PRINTER_PAUSE_REPRINT);        //暂停标志置位
        mksWriteToEpr();
    }
    
}

void SDCard::continuePrint(bool intern)
{
    if(!sd.sdactive) return;
    if(intern) {
       // GCode::executeFString(PSTR(PAUSE_END_COMMANDS));
        Printer::GoToMemoryPosition(true, true, false, false, Printer::maxFeedrate[X_AXIS]);
        Printer::GoToMemoryPosition(false, false, true, false, Printer::maxFeedrate[Z_AXIS] / 2.0f);
        Printer::GoToMemoryPosition(false, false, false, true, Printer::maxFeedrate[E_AXIS] / 2.0f);
    }
    //Printer::setMenuMode(MENU_MODE_SD_PAUSED, false);
    sdmode = 1;
    FALA_5V_CTRL = FALA_ON;    // 打开法拉电容
}
void SDCard::stopPrint()
{
    if(!sd.sdactive) return;
    sdmode = 0;
    //Printer::setMenuMode(MENU_MODE_SD_PRINTING,false);
    //Printer::setMenuMode(MENU_MODE_SD_PAUSED,false);
    //GCode::executeFString(PSTR(SD_RUN_ON_STOP));

          //Z轴上升
     PrintLine::moveRelativeDistanceInStepsReal(0,0,PAUSE_Z_RETRACT*Printer::axisStepsPerMM[Z_AXIS],0,Printer::maxFeedrate[Z_AXIS],true);  
     Commands::waitUntilEndOfAllMoves();

  	//xy归零
  	if(CfgPrinterItems.cfg_drive_system == DELTA)
  	{
  		Printer::homeAxis_delta(true,true,true);
  	}
	else
	{
		Printer::homeAxis(true,true,false);
	}
     
    if(SD_STOP_HEATER_AND_MOTORS_ON_STOP) {
        Commands::waitUntilEndOfAllMoves();
        Printer::kill(false);
	//关风扇
	Commands::setFanSpeed(0,1);
        
    }
  HAL::eprSetByte(EPR_SAV_FLAG,(uint8_t)PRINTER_NORMAL); //续打标志复位
  
}

void SDCard::operatePrint()
{
      
  
        if((sd_operate_flag & SD_PAUSE_BIT) == SD_PAUSE_BIT)
        {
          sd_operate_flag &=(uint8_t) ~(uint8_t)SD_PAUSE_BIT;
          mksPrinterStatusFlag = MKS_PR_PAUSE;
          pausePrint(true);
        }

        if((sd_operate_flag & SD_CONTINUE_BIT) == SD_CONTINUE_BIT)
        {
          if(mksPrinterStatusFlag == MKS_PR_REPRINT)//断电检测
          {
          	sd.mksContiuePrint();
			sd_operate_flag &=(uint8_t) ~(uint8_t)SD_CONTINUE_BIT;
          	mksPrinterStatusFlag = MKS_PR_WORKING;
			mksPrinterStatusFlag = MKS_PR_WORKING;
          }
		  else
		  {
          	sd_operate_flag &=(uint8_t) ~(uint8_t)SD_CONTINUE_BIT;
          	mksPrinterStatusFlag = MKS_PR_WORKING;
			//G92
		    Printer::currentPositionSteps[E_AXIS] = (long)((Printer::memoryE-2*RETRACT_ON_PAUSE) * Printer::axisStepsPerMM[E_AXIS]);
		    Printer::destinationSteps[E_AXIS] = Printer::currentPositionSteps[E_AXIS];
			
          	continuePrint(true);
		  }
        }
        

        if((sd_operate_flag & SD_STOP_BIT) == SD_STOP_BIT)
        {
          sd_operate_flag &=(uint8_t) ~(uint8_t)SD_STOP_BIT;
          mksPrinterStatusFlag = MKS_PR_STOP;
          stopPrint();
        }

}
#endif
void SDCard::writeCommand(GCode *code)
{
    unsigned int sum1=0,sum2=0; // for fletcher-16 checksum
    uint8_t buf[100];
    uint8_t p=2;
	uint32_t nbyte;
	int8_t writeError = 0;
   // file.writeError = false;
    int params = 128 | (code->params & ~1);
    *(int*)buf = params;
    if(code->isV2())   // Read G,M as 16 bit value
    {
        *(int*)&buf[p] = code->params2;
        p+=2;
        if(code->hasString())
            buf[p++] = strlen(code->text);
        if(code->hasM())
        {
            *(int*)&buf[p] = code->M;
            p+=2;
        }
        if(code->hasG())
        {
            *(int*)&buf[p]= code->G;
            p+=2;
        }
    }
    else
    {
        if(code->hasM())
        {
            buf[p++] = (uint8_t)code->M;
        }
        if(code->hasG())
        {
            buf[p++] = (uint8_t)code->G;
        }
    }
    if(code->hasX())
    {
        *(float*)&buf[p] = code->X;
        p+=4;
    }
    if(code->hasY())
    {
        *(float*)&buf[p] = code->Y;
        p+=4;
    }
    if(code->hasZ())
    {
        *(float*)&buf[p] = code->Z;
        p+=4;
    }
    if(code->hasE())
    {
        *(float*)&buf[p] = code->E;
        p+=4;
    }
    if(code->hasF())
    {
        *(float*)&buf[p] = code->F;
        p+=4;
    }
    if(code->hasT())
    {
        buf[p++] = code->T;
    }
    if(code->hasS())
    {
        *(long int*)&buf[p] = code->S;
        p+=4;
    }
    if(code->hasP())
    {
        *(long int*)&buf[p] = code->P;
        p+=4;
    }
    if(code->hasI())
    {
        *(float*)&buf[p] = code->I;
        p+=4;
    }
    if(code->hasJ())
    {
        *(float*)&buf[p] = code->J;
        p+=4;
    }
    if(code->hasString())   // read 16 uint8_t into string
    {
        char *sp = code->text;
        if(code->isV2())
        {
            uint8_t i = strlen(code->text);
            for(; i; i--) buf[p++] = *sp++;
        }
        else
        {
            for(uint8_t i=0; i<16; ++i) buf[p++] = *sp++;
        }
    }
    uint8_t *ptr = buf;
    uint8_t len = p;
    while (len)
    {
        uint8_t tlen = len > 21 ? 21 : len;
        len -= tlen;
        do
        {
            sum1 += *ptr++;
            if(sum1>=255) sum1-=255;
            sum2 += sum1;
            if(sum2>=255) sum2-=255;
        }
        while (--tlen);
    }
    buf[p++] = sum1;
    buf[p++] = sum2;
    if(params == 128)
    {
        //**Com::printErrorFLN(Com::tAPIDFinished);
    }
    else
        //file.write(buf,p);
        writeError = f_write(&curFile, buf, p, &nbyte);
	
    if (writeError)
    {
        //**Com::printFLN(Com::tErrorWritingToFile);
    }
}

char *SDCard::createFilename(char *buffer,const dir_t &p)
{
    char *pos = buffer,*src = (char*)p.name;
    for (uint8_t i = 0; i < 11; i++,src++)
    {
        if (*src == ' ') continue;
        if (i == 8)
            *pos++ = '.';
        *pos++ = *src;
    }
    *pos = 0;
    return pos;
}

bool SDCard::showFilename(const uint8_t *name)
{
    if (*name == DIR_NAME_DELETED || *name == '.') return false;
    return true;
}

int8_t RFstricmp(const char* s1, const char* s2)
{
    while(*s1 && (tolower(*s1) == tolower(*s2)))
        s1++,s2++;
    return (const uint8_t)tolower(*s1)-(const uint8_t)tolower(*s2);
}

int8_t RFstrnicmp(const char* s1, const char* s2, size_t n)
{
    while(n--)
    {
        if(tolower(*s1)!=tolower(*s2))
            return (uint8_t)tolower(*s1) - (uint8_t)tolower(*s2);
        s1++;
        s2++;
    }
    return 0;
}

void SDCard::ls()
{
   // SdBaseFile file;

   //** Com::printFLN(Com::tBeginFileList);
   /* fat.chdir();

    file.openRoot(fat.vol());
    file.ls(0, 0);*/
      //get_file_list("1:");
    if ((SD_DET_IP == SDCARDDETECTINVERTED)&&(usbactive == false))
      get_file_list(SD_Path);
    else
      get_file_list(USBH_Path);
    //**Com::printFLN(Com::tEndFileList);
}

#if JSON_OUTPUT
void SDCard::lsJSON(const char *filename)
{
    SdBaseFile dir;
    fat.chdir();
    if (*filename == 0) {
        dir.openRoot(fat.vol());
    } else {
        if (!dir.open(fat.vwd(), filename, O_READ) || !dir.isDir()) {
            //**Com::printF(Com::tJSONErrorStart);
           //** Com::printF(Com::tFileOpenFailed);
            //**Com::printFLN(Com::tJSONErrorEnd);
            return;
        }
    }

    //**Com::printF(Com::tJSONDir);
    SDCard::printEscapeChars(filename);
    //**Com::printF(Com::tJSONFiles);
    dir.lsJSON();
    //*8Com::printFLN(Com::tJSONArrayEnd);
}

void SDCard::printEscapeChars(const char *s) {
    for (unsigned int i = 0; i < strlen(s); ++i) {
        switch (s[i]) {
            case '"':
            case '/':
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t':
            case '\\':
				//**Com::print('\\');
                break;
        }
		//**Com::print(s[i]);
    }
}

void SDCard::JSONFileInfo(const char* filename) {
    SdBaseFile targetFile;
    GCodeFileInfo *info,tmpInfo;
    if (strlen(filename) == 0)  {
        targetFile = file;
        info = &fileInfo;
    } else {
        if (!targetFile.open(fat.vwd(), filename, O_READ) || targetFile.isDir()) {
            //**Com::printF(Com::tJSONErrorStart);
            //**Com::printF(Com::tFileOpenFailed);
            //**Com::printFLN(Com::tJSONErrorEnd);
            return;
        }
		info = &tmpInfo;
        info->init(targetFile);
    }
    if (!targetFile.isOpen()) {
        //**Com::printF(Com::tJSONErrorStart);
        //**Com::printF(Com::tNotSDPrinting);
        //**Com::printFLN(Com::tJSONErrorEnd);
        return;
    }

    // {"err":0,"size":457574,"height":4.00,"layerHeight":0.25,"filament":[6556.3],"generatedBy":"Slic3r 1.1.7 on 2014-11-09 at 17:11:32"}
    /*Com::printF(Com::tJSONFileInfoStart);
    Com::print(info->fileSize);
    Com::printF(Com::tJSONFileInfoHeight);
    Com::print(info->objectHeight);
    Com::printF(Com::tJSONFileInfoLayerHeight);
    Com::print(info->layerHeight);
    Com::printF(Com::tJSONFileInfoFilament);
    Com::print(info->filamentNeeded);
    Com::printF(Com::tJSONFileInfoGeneratedBy);
    Com::print(info->generatedBy);
    Com::print('"');*/
    if (strlen(filename) == 0) {
        //**Com::printF(Com::tJSONFileInfoName);
        file.printName();
	    //**Com::print('"');
    }
	//**Com::print('}');
    //**Com::println();
};

#endif

bool SDCard::selectFile(char *filename, bool silent)
{
    SdBaseFile parent;
    char *oldP;
    boolean bFound;
    
  // char sdFileName[100];
  //  memset(sdFileName,0,sizeof(sdFileName));

	//char path[100] = "1:/";

	//tan 20160427
	if(sdmode)return false;
	
    if(!sdactive) return false;
    sdmode = false;

	//strcat(path, filename);

	//oldP = path;

    //file.close();
	f_close(&curFile);

   // parent = *fat.vwd();
   // if (file.open(&parent, filename, O_READ))
        //strcat(sdFileName,SD_Path);
        //strcat(sdFileName,filename);
	if(f_open(&curFile, (const TCHAR *)filename, FA_OPEN_EXISTING | FA_READ) == FR_OK)					
      {
     // if ((oldP = strrchr(path, '/')) != NULL)
     //     oldP++;
     // else
     //     oldP = path;

        if(!silent)
        {
          //  Com::printF(Com::tFileOpened, oldP);
            //Com::printFLN(Com::tSpaceSizeColon,file.fileSize());
            //**Com::printFLN(Com::tSpaceSizeColon,(uint32_t)sd.curFile.fsize);
        }
        sdpos = 0;
        //filesize = file.fileSize();
        filesize = curFile.fsize;
        //**Com::printFLN(Com::tFileSelected);
        return true;
    }
    else
    {
        if(!silent)
            //**Com::printFLN(Com::tFileOpenFailed);
        return false;
    }
}


void SDCard::printStatus()
{
    if(sdactive)
    {
        //**Com::printF(Com::tSDPrintingByte, sdpos);
        //**Com::printFLN(Com::tSlash, filesize);
    }
    else
    {
        //**Com::printFLN(Com::tNotSDPrinting);
    }
}

void SDCard::startWrite(char *filename)
{
 //char sdFileName[100];
  //memset(sdFileName,0,sizeof(sdFileName));
  
    if(!sdactive) return;
      f_close(&sd.curFile);
    sdmode = false;
    
    
    //fat.chdir();
	
    //if(!file.open(filename, O_CREAT | O_APPEND | O_WRITE | O_TRUNC))
        //strcat(sdFileName,SD_Path);
        //strcat(sdFileName,filename);
	if(f_open(&curFile, (const TCHAR *)filename, FA_CREATE_ALWAYS | FA_WRITE | FA_READ)  != FR_OK)
    {
        //**Com::printFLN(Com::tOpenFailedFile,filename);
    }
    else
    {
        UI_STATUS(UI_TEXT_UPLOADING);
        savetosd = true;
        //**Com::printFLN(Com::tWritingToFile,filename);
    }
}
void SDCard::finishWrite()
{
    if(!savetosd) return; // already closed or never opened
    //file.sync();
    //file.close();
    f_sync(&curFile);
	f_close(&curFile);
    savetosd = false;
    //**Com::printFLN(Com::tDoneSavingFile);
    //UI_CLEAR_STATUS;
}
void SDCard::deleteFile(char *filename)
{
    if(!sdactive) return;
    sdmode = false;
    //file.close();
    f_close(&curFile);
    /*if(fat.remove(filename))
    {
        Com::printFLN(Com::tFileDeleted);
    }
    else
    {
        if(fat.rmdir(filename))
            Com::printFLN(Com::tFileDeleted);
        else
            Com::printFLN(Com::tDeletionFailed);
    }*/
    if(f_unlink((const TCHAR *)filename) == FR_OK)
	{
        //**Com::printFLN(Com::tFileDeleted);
    }
	else
	{
		//**Com::printFLN(Com::tDeletionFailed);
	}
}
void SDCard::makeDirectory(char *filename)
{
    if(!sdactive) return;
    sdmode = false;
    //file.close();
    f_close(&curFile);
    //if(fat.mkdir(filename))
    if(f_mkdir((const TCHAR *)filename) == FR_OK)
    {
        //**Com::printFLN(Com::tDirectoryCreated);
    }
    else
    {
        //**Com::printFLN(Com::tCreationFailed);
    }
}

void SDCard::get_file_list(char *path)
{
	if( path == 0)
	{
		return;
	}

	Explore_Disk(path, 0);
	
	
}

uint8_t SDCard::Explore_Disk (char* path , uint8_t recu_level)
{

  FILINFO fno;
  DIR dir;
 // SD_CardInfo cardinfo;
  TCHAR *fn;
  char tmp[200];
  char Fstream[200];
	int local_offset;
	int file_offset = 0;
	FRESULT res;
	
  #if _USE_LFN
    static TCHAR lfn[_MAX_LFN + 1];
    fno.lfname = lfn;
    fno.lfsize = sizeof(lfn);
#endif

	if(path == 0)
		return 0;
/*
	if(path[0] == '0')               //skyblue 2016-12-13
		f_mount(0, &fs);
	else if(path[0] == '1')
		f_mount(1, &fs);
	else
		return;
*/	
        f_mount(&fs, (char *)path, 0);     //skyblue 2016-12-13
        
	for(;;)
	{
		local_offset = 0;
		
		if (f_opendir(&dir, (const TCHAR *)path) == FR_OK) 
	  	{

		    while(1)
		    {
				res = f_readdir(&dir, &fno);
				if (res != FR_OK || fno.fname[0] == 0) 
				{
					return;
				}
				if (fno.fname[0] == '.')
				{
					continue;
				}

				


				if(local_offset >= file_offset)
				{
					file_offset++;
					break;
				}

				local_offset++;
		    }
			
		    if ((fno.lfname[0] == 0) || (fno.lfname == 0))
				fn = fno.fname;
			else
				fn = fno.lfname;
				
		      	if((strstr((const char *)fn, ".gco")) || (strstr((const char *)fn, ".GCO")) || (fno.fattrib & AM_DIR))
				{
					  
					  tmp[0] = '\0';
					 // strcpy(tmp, path);
					 // strcat(tmp, "/");
					  strcat((char *)tmp, (char *)fn);

					memset(Fstream, 0, sizeof(Fstream));
					strcpy(Fstream, tmp);
					  if((fno.fattrib & AM_DIR)&&(recu_level <= 10))
				      {
				      //  Explore_Disk(tmp, recu_level + 1);
				      	
						
						strcat(Fstream, ".DIR\r\n");
						//send_to_wifi(Fstream, strlen(Fstream));
						//**Com::print(Fstream);
                     	//**Com::printF(Com::tSlash);
				      }
					  else
					  {					
						strcat(Fstream, "\r\n");
						//send_to_wifi(Fstream, strlen(Fstream));
						//**Com::print(Fstream);
                     	//**Com::printF(Com::tSlash);
					  }
				}
		  
		      

		     
		    }
				else
					break;
	
  	
	}
 return res;
}


void SDCard::ShowSDFiles(void)
	{
	  FILINFO fno;
	  DIR dir;
	  TCHAR *fn;
      const TCHAR gFileName[5] = {'.', 'g', 'c', 'o', '\0'};
      const TCHAR gFileNameCap[5] = {'.', 'G', 'C', 'O', '\0'};
	  TCHAR tmp[200];
	  int res;
	
  #if _USE_LFN
		static TCHAR lfn[_MAX_LFN + 1];
		fno.lfname = lfn;
		fno.lfsize = _MAX_LFN + 1;
#endif
	 
	
	 //f_mount(1, &fs);
          if ((SD_DET_IP == SDCARDDETECTINVERTED)&&(usbactive == false))                 
            f_mount(&fs, (TCHAR const*)SD_Path, 0);     //skyblue 2016-12-13
          else
            f_mount(&fs, (TCHAR const*)USBH_Path, 0);
	 if (f_opendir(&dir, gCurDir) == FR_OK) 
	  {
		Sd_file_cnt = 0;
			gcodeFileList.listVaild= 2;
		for (;;) 
		{
			res = f_readdir(&dir, &fno);
			if (res != FR_OK || fno.fname[0] == 0) 
			{
				gcodeFileList.listVaild= 2;
				break;
			}
			if ( fno.fname[0] == '.') 
				continue;
	
			if ((fno.lfname[0] == 0) || (fno.lfname == 0))
				fn = fno.fname;
			else
				fn = fno.lfname;
	
			/*	 if (fno.fattrib & AM_DIR) 
			{
			continue;
			} 
			else */
			if(Sd_file_cnt == Sd_file_offset)
			{
				//Sd_file_offset++;
				#if _LFN_UNICODE
				if((wcsstr((const wchar_t *)fn, (const wchar_t *)gFileName)) || (wcsstr((const wchar_t *)fn, (const wchar_t *)gFileNameCap)) || (fno.fattrib & AM_DIR))
				#else
					if((strstr(fn, ".gco")) || (strstr(fn, ".GCO")) || (fno.fattrib & AM_DIR))
				#endif
				{
					  //Sd_display_file_cnt++;
					  
					  tmp[0] = '\0';
					  strcpy(tmp, (char const*)gCurDir);
					  strcat(tmp, "/");
					  #if _LFN_UNICODE
					  wcscat((wchar_t *)tmp, (const wchar_t *)fn);
					  #else
					  strcat(tmp, fn);
					  #endif
	
					  gcodeFileList.listVaild= 1;
	
					if(fno.fattrib & AM_DIR)
					{
						gcodeFileList.fileAttr[gcodeFileList.index] = 1;
					}
					else
					{
						gcodeFileList.fileAttr[gcodeFileList.index] = 0;
					}
	
					#if _LFN_UNICODE
					wcscpy((wchar_t *)gcodeFileList.fileName[gcodeFileList.index], (const wchar_t *)tmp);
					#else
					strcpy((char *)gcodeFileList.fileName[gcodeFileList.index], (const char *)tmp);
					#endif
					gcodeFileList.index++;
					
					
				}
				else
				{
					gcodeFileList.listVaild= 0;
				}
				break;
			}
			
			Sd_file_cnt++;
			
			 
		}
#if 0
		while (f_readdir(&dirs, &finfo) == FR_OK)  
		{
		  if (finfo.fattrib & AM_ARC) 
		  {
			if(!finfo.fname[0]) 
			  break;		 
			  printf("\n\r file name is: %s\n",finfo.fname);
			  printf("\n\r file size is: %d ", finfo.fsize); 
	
			  if(File_type_Check( (u8 *)finfo.fname, "txt"))
			  { 
				BufferSet(buffer, 0, 100);
				res = f_open(&fsrc, finfo.fname, FA_OPEN_EXISTING | FA_READ);
				res = f_read(&fsrc, buffer, 100, &br);
				printf("\n\r file contex is: \n\r%s\n\r", buffer); 
				f_close(&fsrc); 							  
			  }
		  }
		  else
		  {
			printf("\n\r Path name is: %s", finfo.fname); 
			continue;//break;
		  }
		} 
		  res = f_open(&fsrc, "armjishu.txt", FA_CREATE_ALWAYS | FA_WRITE);
		  res = f_write(&fsrc, &armjishu, sizeof(armjishu), &bw);
		  f_close(&fsrc);
	#endif
	  
	 }
	 else
		 gcodeFileList.listVaild= 2;
	 
	}


int SDCard::ascii2dec(char *ascii, char width)
{
	int i = 0;
	int result = 0;

	if(ascii == 0)
		return 0;
	
	while(i < width)
	{		
		result = result << 4;
		
		if(*(ascii + i) >= '0' && *(ascii + i) <= '9')
			result += *(ascii + i) - '0';
		else if(*(ascii + i) >= 'a' && *(ascii + i) <= 'f')
			result += *(ascii + i) - 'a' + 0x0a;
		else if(*(ascii + i) >= 'A' && *(ascii + i) <= 'F')
			result += *(ascii + i) - 'A' + 0x0a;
		else
			return 0;
		
		i++;
	}
	return result;
}


FIL curFile_temp;
#if 0
void SDCard::get_sd_cfg(void)
{	
	uint32_t flash_inf_valid_flg;

	uint16_t t;

	uint8_t temp;
	char oldname[30]={0};
	char newname[30]={0};
	volatile long file_pos=0; 
	volatile uint16_t sd_read_cnt=0;	
	volatile uint16_t code_cnt = 0;

	FRESULT r;
//	FIL cfgFile;
	
	char file_str[1024];//?áè????t×?′ó?a1k?￡
	unsigned int byte_have_read;
	
	volatile char *tmp_index;

	
	uint32_t file_read_length = 0;
	uint32_t flg_length = 0;

	char *tmp1,*tmp2;
	//char invalid_char1[1024]={0};
	//char invalid_char2[1024]={0};
	
	//f_mount(1, &fs);
        
        f_mount(&fs, (TCHAR const*)SD_Path, 0);     //skyblue 2016-12-13
        strcat(oldname,SD_Path);
        strcat(oldname,cfg_file);
 	if (f_open(&curFile_temp, (const TCHAR *)oldname, FA_OPEN_EXISTING|FA_READ) == FR_OK)
	{
		//擦除4k
		//存储wifi和more菜单的信息
		//FSMC_NOR_EraseSector(SD_INF_ADDR);
        SPI_FLASH_SectorErase(SD_INF_ADDR);
		//while(1)
		while(curFile_temp.fsize > file_pos)
		{
		/*
			memset(file_str, 0, sizeof(file_str));
			memset(invalid_char1, 0, sizeof(invalid_char1));
			memset(invalid_char2, 0, sizeof(invalid_char2));
			f_lseek(&curFile_temp, file_read_length);
			
			r = f_read(&curFile_temp, file_str, 1024, (uint32_t *)&byte_have_read);
			if(file_str[byte_have_read-1] == '\n')
			{
				file_read_length += byte_have_read;
			}
			else
			{
				tmp1 = (char *)strrchr(&file_str[0],'>');
				if(tmp1)
				{
					strcpy(invalid_char1,tmp1);
					tmp2 = (char *)strrchr(invalid_char1,'\n');
					if(tmp2)
					{
						strcpy(invalid_char2,tmp2);
						flg_length = strlen(invalid_char2);
					}
					else
					{
						flg_length = strlen(invalid_char1);
					}
				}
				file_read_length += (byte_have_read - flg_length);
			}	
			*/
			memset(file_str, 0, sizeof(file_str));
			sd_read_cnt = 0;
			do
			{
				r = f_read(&curFile_temp, &file_str[sd_read_cnt], 1, &byte_have_read);
				sd_read_cnt++;
				file_pos++;
				if(curFile_temp.fsize <= file_pos)
				{
					//flash存储标志位。
					flash_inf_valid_flg = FLASH_INF_VALID_FLAG;
					SPI_FLASH_BufferWrite((uint8_t*)&flash_inf_valid_flg,FlASH_INF_VALID_ADDR,4);
					break;
				}
			}
			while((file_str[sd_read_cnt-1] != '\r')&&(file_str[sd_read_cnt-1] != '\n'));
			file_str[sd_read_cnt] = '\n';
			
			//if(r == FR_OK)
			{
					/*background_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_background_color");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_background_color");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						
						gCfgItems.background_color = ascii2dec(&cmd_code[2], 6);

						HAL::eprSetInt32(EPR_BACKGROUND_COLOR_CFG, gCfgItems.background_color);
					}

					/*title_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_title_color");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_title_color");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						gCfgItems.title_color = ascii2dec(&cmd_code[2], 6);

						HAL::eprSetInt32(EPR_TITLE_COLOR_CFG, gCfgItems.title_color);
					}

					/*state_background_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_state_background_color");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_state_background_color");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;

						gCfgItems.state_background_color = ascii2dec(&cmd_code[2], 6);

						HAL::eprSetInt32(EPR_STATE_BACKGROUND_COLOR_CFG,gCfgItems.state_background_color);
					}

					/*state_text_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_state_text_color");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_state_text_color");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;

						
						gCfgItems.state_text_color = ascii2dec(&cmd_code[2], 6);

						HAL::eprSetInt32(EPR_STATE_TEXT_COLOR_CFG, gCfgItems.state_text_color);
					}

					/*filename_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_filename_color");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_filename_color");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;

						
						gCfgItems.filename_color = ascii2dec(&cmd_code[2], 6);

						HAL::eprSetInt32(EPR_FILENAME_COLOR_CFG, gCfgItems.filename_color);
					}

					/*filename_background_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_filename_background_color");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_filename_background_color");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;

						gCfgItems.filename_background_color = ascii2dec(&cmd_code[2], 6);
						
						HAL::eprSetInt32(EPR_FILENAME_BACKGROUND_COLOE_CFG, gCfgItems.filename_background_color);
					}
					/*printingstate_words_background_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_printingstat_word_background_color");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_printingstat_word_background_color");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;

						
						gCfgItems.printingstate_word_background_color = ascii2dec(&cmd_code[2], 6);
						HAL::eprSetInt32(EPR_PRINTINGSTATE_TEXT_BACKGROUND_COLOR_CFG, gCfgItems.printingstate_word_background_color);

						
					}
					/*printingstate_background_color*/
					tmp_index = (char *)strstr(file_str, ">cfg_printingstat_word_color");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_printingstat_word_color");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;

						gCfgItems.printingstate_word_color = ascii2dec(&cmd_code[2], 6);

						HAL::eprSetInt32(EPR_PRINTINGSTATE_TEXT_COLOR_CFG, gCfgItems.printingstate_word_color);
					}
					/*?¨??°′?￥3d??ê?D§1?*/
					tmp_index = (char *)strstr(file_str,">cfg_BUTTON_3D_effects");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_BUTTON_3D_effects");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						gCfgItems.button_3d_effect_flag =  atoi(cmd_code);


						HAL::eprSetByte(EPR_BUTTON_3D_EFFECTS_CFG, gCfgItems.button_3d_effect_flag);
					}
					
					//ó???
					tmp_index = (char *)strstr(file_str, ">cfg_language_type");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_language_type");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						gCfgItems.language =  atoi(cmd_code);

						HAL::eprSetByte(EPR_LANGUAGE_TYPE_CFG, gCfgItems.language);
					}
					/*
					//2¨ì??ê
					tmp_index = (char *)strstr(file_str, "BAUDRATE");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen("BAUDRATE");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_baudrate = atoi(cmd_code);
						HAL::eprSetInt32(EPR_BAUDRATE_CFG,CfgPrinterItems.cfg_baudrate);
					}
					*/
					//?úDí?y?ˉ????
					tmp_index = (char *)strstr(file_str, ">DRIVE_SYSTEM");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">DRIVE_SYSTEM");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_drive_system = atoi(cmd_code);

						HAL::eprSetByte(EPR_DRIVE_SYSTEM_CFG,CfgPrinterItems.cfg_drive_system);	
					}
					//?·3??ú1???DààDí
					tmp_index = (char *)strstr(file_str, ">EXT0_TEMPSENSOR_TYPE");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">EXT0_TEMPSENSOR_TYPE");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_ext0_tempsensor_type = atoi(cmd_code);

						HAL::eprSetByte(EPR_EXT0_TEMPSENSOR_TYPE_CFG,CfgPrinterItems.cfg_ext0_tempsensor_type);	
						//HAL::AT24CXX_Read(EPR_EXT0_TEMPSENSOR_TYPE_CFG,(u8*)tmp_index,1);
					}
					//?·3??ú2???DààDí
					tmp_index = (char *)strstr(file_str, ">EXT1_TEMPSENSOR_TYPE");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">EXT1_TEMPSENSOR_TYPE");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_ext1_tempsensor_type = atoi(cmd_code);

						HAL::eprSetByte(EPR_EXT1_TEMPSENSOR_TYPE_CFG,CfgPrinterItems.cfg_ext1_tempsensor_type);		
					}
					//èè′2???DààDí
					tmp_index = (char *)strstr(file_str, ">HEATED_BED_SENSOR_TYPE");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">HEATED_BED_SENSOR_TYPE");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_heated_bed_sensor_type= atoi(cmd_code);

						HAL::eprSetByte(EPR_HEATED_BED_SENSOR_TYPE_CFG,CfgPrinterItems.cfg_heated_bed_sensor_type);
					}
					/*
					tmp_index = (char *)strstr(file_str, "AXIS_STEPS_PER_MM");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen("AXIS_STEPS_PER_MM");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_axis_steps_per_mm= atof(cmd_code);

						HAL::eprSetFloat(EPR_AXIS_STEPS_PER_MM_CFG,CfgPrinterItems.cfg_axis_steps_per_mm);
					}
					*/
					//X??mmμ???3??μ
					tmp_index = (char *)strstr(file_str, ">XAXIS_STEPS_PER_MM");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">XAXIS_STEPS_PER_MM");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_xaxis_steps_per_mm= atof(cmd_code);

						HAL::eprSetFloat(EPR_XAXIS_STEPS_PER_MM_CFG,CfgPrinterItems.cfg_xaxis_steps_per_mm);
					}
					//Y??mmμ???3??μ
					tmp_index = (char *)strstr(file_str, ">YAXIS_STEPS_PER_MM");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">YAXIS_STEPS_PER_MM");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_yaxis_steps_per_mm= atof(cmd_code);

						HAL::eprSetFloat(EPR_YAXIS_STEPS_PER_MM_CFG,CfgPrinterItems.cfg_yaxis_steps_per_mm);
					}
					//Z??mmμ???3??μ
					tmp_index = (char *)strstr(file_str, ">ZAXIS_STEPS_PER_MM");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">ZAXIS_STEPS_PER_MM");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_zaxis_steps_per_mm= atof(cmd_code);

						HAL::eprSetFloat(EPR_ZAXIS_STEPS_PER_MM_CFG,CfgPrinterItems.cfg_zaxis_steps_per_mm);		
					}
					//E0??mmμ???3??μ
					tmp_index = (char *)strstr(file_str, ">EXT0_STEPS_PER_MM");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">EXT0_STEPS_PER_MM");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_ext0_steps_per_mm= atof(cmd_code);

						HAL::eprSetFloat(EPR_EXT0_STEPS_PER_MM_CFG,CfgPrinterItems.cfg_ext0_steps_per_mm);	
					}
					//E1 脉冲
					tmp_index = (char *)strstr(file_str, ">EXT1_STEPS_PER_MM");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">EXT1_STEPS_PER_MM");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_ext1_steps_per_mm= atof(cmd_code);

						HAL::eprSetFloat(EPR_EXT1_STEPS_PER_MM_CFG,CfgPrinterItems.cfg_ext1_steps_per_mm);	
					}					


					//x?á×?′ó?ù?è
					tmp_index = (char *)strstr(file_str, ">MAX_FEEDRATE_X");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_FEEDRATE_X");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_feedrate_x = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_FEEDRATE_X_CFG,CfgPrinterItems.cfg_max_feedrate_x);	
					}
					//y?á×?′ó?ù?è
					tmp_index = (char *)strstr(file_str, ">MAX_FEEDRATE_Y");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_FEEDRATE_Y");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_feedrate_y = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_FEEDRATE_Y_CFG,CfgPrinterItems.cfg_max_feedrate_y);		
					}					
					//Z?á×?′ó?ù?è
					tmp_index = (char *)strstr(file_str, ">MAX_FEEDRATE_Z");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_FEEDRATE_Z");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_feedrate_z = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_FEEDRATE_Z_CFG,CfgPrinterItems.cfg_max_feedrate_z);	
					}		
					//E0×?′ó?ù?è
					tmp_index = (char *)strstr(file_str, ">MAX_FEEDRATE_EXT0");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_FEEDRATE_EXT0");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_feedrate_e0 = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_FEEDRATE_EXT0_CFG,CfgPrinterItems.cfg_max_feedrate_e0);		
					}	
					//X×?′ó?ó?ù?è
					//E1最大速度
					tmp_index = (char *)strstr(file_str, ">MAX_FEEDRATE_EXT1");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_FEEDRATE_EXT1");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_feedrate_e1 = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_FEEDRATE_EXT1_CFG,CfgPrinterItems.cfg_max_feedrate_e1);		
					}						


					tmp_index = (char *)strstr(file_str, ">MAX_ACCELERATION_UNITS_PER_SQ_SECOND_X");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_ACCELERATION_UNITS_PER_SQ_SECOND_X");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_acceleration_units_per_sq_second_x = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_ACCELERATION_UNITS_PER_SQ_SECOND_X_CFG,CfgPrinterItems.cfg_max_acceleration_units_per_sq_second_x);	
					}
					//Y×?′ó?ó?ù?è
					tmp_index = (char *)strstr(file_str, ">MAX_ACCELERATION_UNITS_PER_SQ_SECOND_Y");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_ACCELERATION_UNITS_PER_SQ_SECOND_Y");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_acceleration_units_per_sq_second_y = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_ACCELERATION_UNITS_PER_SQ_SECOND_Y_CFG,CfgPrinterItems.cfg_max_acceleration_units_per_sq_second_y);
					}
					//Z×?′ó?ó?ù?è
					tmp_index = (char *)strstr(file_str, ">MAX_ACCELERATION_UNITS_PER_SQ_SECOND_Z");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_ACCELERATION_UNITS_PER_SQ_SECOND_Z");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_acceleration_units_per_sq_second_z = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_ACCELERATION_UNITS_PER_SQ_SECOND_Z_CFG,CfgPrinterItems.cfg_max_acceleration_units_per_sq_second_z);
					}
					//E0×?′ó?ó?ù?è
					tmp_index = (char *)strstr(file_str, ">MAX_ACCELERATION_EXT0");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_ACCELERATION_EXT0");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_acceleration_e0= atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_ACCELERATION_EXT0_CFG,CfgPrinterItems.cfg_max_acceleration_e0);
					}
					//E1最大加速度
					tmp_index = (char *)strstr(file_str, ">MAX_ACCELERATION_EXT1");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_ACCELERATION_EXT1");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_acceleration_e1= atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_ACCELERATION_EXT1_CFG,CfgPrinterItems.cfg_max_acceleration_e1);
					}


					//X×?′ótravel?ó?ù?è
					tmp_index = (char *)strstr(file_str, ">MAX_TRAVEL_ACCELERATION_UNITS_PER_SQ_SECOND_X");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_TRAVEL_ACCELERATION_UNITS_PER_SQ_SECOND_X");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_travel_acceleration_units_per_sq_second_x = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_TRAVEL_ACCELERATION_UNITS_PER_SQ_SECOND_X_CFG,CfgPrinterItems.cfg_max_travel_acceleration_units_per_sq_second_x);	
					}
					//y×?′ótravel?ó?ù?è
					tmp_index = (char *)strstr(file_str, ">MAX_TRAVEL_ACCELERATION_UNITS_PER_SQ_SECOND_Y");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_TRAVEL_ACCELERATION_UNITS_PER_SQ_SECOND_Y");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_travel_acceleration_units_per_sq_second_y = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_TRAVEL_ACCELERATION_UNITS_PER_SQ_SECOND_Y_CFG,CfgPrinterItems.cfg_max_travel_acceleration_units_per_sq_second_y);	
					}	
					//Z×?′ótravel?ó?ù?è
					tmp_index = (char *)strstr(file_str, ">MAX_TRAVEL_ACCELERATION_UNITS_PER_SQ_SECOND_Z");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_TRAVEL_ACCELERATION_UNITS_PER_SQ_SECOND_Z");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_travel_acceleration_units_per_sq_second_z = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_TRAVEL_ACCELERATION_UNITS_PER_SQ_SECOND_Z_CFG,CfgPrinterItems.cfg_max_travel_acceleration_units_per_sq_second_z);	
					}
					//X1éá?ê±μ?1éá??ù?è
					tmp_index = (char *)strstr(file_str, ">HOMING_FEEDRATE_X");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">HOMING_FEEDRATE_X");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_homing_feedrate_x = atof(cmd_code);

						HAL::eprSetFloat(EPR_HOMING_FEEDRATE_X_CFG,CfgPrinterItems.cfg_homing_feedrate_x);		
					}
					//Y1éá?ê±μ?1éá??ù?è
					tmp_index = (char *)strstr(file_str, ">HOMING_FEEDRATE_Y");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">HOMING_FEEDRATE_Y");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_homing_feedrate_y = atof(cmd_code);

						HAL::eprSetFloat(EPR_HOMING_FEEDRATE_Y_CFG,CfgPrinterItems.cfg_homing_feedrate_y);	
					}
					//Z1éá?ê±μ?1éá??ù?è
					tmp_index = (char *)strstr(file_str, ">HOMING_FEEDRATE_Z");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">HOMING_FEEDRATE_Z");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_homing_feedrate_z = atof(cmd_code);

						HAL::eprSetFloat(EPR_HOMING_FEEDRATE_Z_CFG,CfgPrinterItems.cfg_homing_feedrate_z);	
					}
					//挤出头最低温度
					tmp_index = (char *)strstr(file_str, ">MIN_EXTRUDER_TEMP");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MIN_EXTRUDER_TEMP");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_ext_mintemp = atof(cmd_code);

						HAL::eprSetFloat(EPR_MIN_EXTRUDER_TEMP_CFG,CfgPrinterItems.cfg_ext_mintemp);	
					}
					//挤出头最高温度
					tmp_index = (char *)strstr(file_str, ">MAX_EXTRUDER_TEMP");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_EXTRUDER_TEMP");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_ext_maxtemp = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_EXTRUDER_TEMP_CFG,CfgPrinterItems.cfg_ext_maxtemp);		
					}
					//热板最高温度
					tmp_index = (char *)strstr(file_str, ">MAX_HEATED_BED_TEMP");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_HEATED_BED_TEMP");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_heated_bed_maxtemp = atof(cmd_code);

						HAL::eprSetFloat(EPR_MAX_HEATED_BED_TEMP_CFG,CfgPrinterItems.cfg_heated_bed_maxtemp);	
					}
					tmp_index = (char *)strstr(file_str, ">cfg_ui_set_maxtemp");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_ui_set_maxtemp");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_ui_set_maxtemp= atof(cmd_code);

						HAL::eprSetFloat(EPR_UI_SET_MAX_TEMPERATURE_CFG,CfgPrinterItems.cfg_ui_set_maxtemp);		
					}					
					tmp_index = (char *)strstr(file_str, ">cfg_ui_set_heated_bed_temp");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_ui_set_heated_bed_temp");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_ui_set_heated_bed_maxtemp= atof(cmd_code);

						HAL::eprSetFloat(EPR_UI_SET_HEAT_BED_TEMPERATURE_CFG,CfgPrinterItems.cfg_ui_set_heated_bed_maxtemp);	
					}	
					tmp_index = (char *)strstr(file_str, ">cfg_print_over_auto_close");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">cfg_print_over_auto_close");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						gCfgItems.print_finish_close_machine_flg = atoi(cmd_code);

						HAL::eprSetByte(EPR_PRINT_OVER_AUTO_CLOSE_CFG,gCfgItems.print_finish_close_machine_flg);	
					}


					//配置挤出头个数
					tmp_index = (char *)strstr(file_str, ">NUM_EXTRUDER");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">NUM_EXTRUDER");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_num_extruder = atoi(cmd_code);

						HAL::eprSetByte(EPR_NUM_EXTRUDER_CFG,CfgPrinterItems.cfg_num_extruder);	
					}
					//ê1?üèè′2
					tmp_index = (char *)strstr(file_str, ">HAVE_HEATED_BED");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">HAVE_HEATED_BED");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_have_heated_bed = atoi(cmd_code);

						HAL::eprSetByte(EPR_HAVE_HEATED_BED_CFG,CfgPrinterItems.cfg_have_heated_bed);	
					}
					//X2???μ??ú·??ò
					tmp_index = (char *)strstr(file_str, ">INVERT_X_DIR");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">INVERT_X_DIR");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						temp = atoi(cmd_code);
						if(temp == 1)
						{
						 	CfgPrinterItems.cfg_invert_x_dir = true;
						}
						else
						{
							CfgPrinterItems.cfg_invert_x_dir = false;
						}

						HAL::eprSetByte(EPR_INVERT_X_DIR_CFG,CfgPrinterItems.cfg_invert_x_dir);		
					}
					//Y2???μ??ú·??ò
					tmp_index = (char *)strstr(file_str, ">INVERT_Y_DIR");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">INVERT_Y_DIR");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						temp = atoi(cmd_code);
						if(temp == 1)
						{
							CfgPrinterItems.cfg_invert_y_dir = true;
						}
						else
						{
							CfgPrinterItems.cfg_invert_y_dir = false;
						}

						HAL::eprSetByte(EPR_INVERT_Y_DIR_CFG,CfgPrinterItems.cfg_invert_y_dir);	
					}
					//Z2???μ??ú·??ò
					tmp_index = (char *)strstr(file_str, ">INVERT_Z_DIR");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">INVERT_Z_DIR");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						temp = atoi(cmd_code);
						if(temp == 1)
						{
							CfgPrinterItems.cfg_invert_z_dir = true;
						}
						else
						{
							CfgPrinterItems.cfg_invert_z_dir = false;
						}

						HAL::eprSetByte(EPR_INVERT_Z_DIR_CFG,CfgPrinterItems.cfg_invert_z_dir);	
					}
					//E0?·3??ú2???μ??ú·??ò
					tmp_index = (char *)strstr(file_str, ">EXT0_INVERSE");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">EXT0_INVERSE");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						temp = atoi(cmd_code);
						if(temp == 1)
						{
							CfgPrinterItems.cfg_invert_e0_dir = true;
						}
						else
						{
							CfgPrinterItems.cfg_invert_e0_dir = false;
						}

						HAL::eprSetByte(EPR_EXT0_INVERSE_CFG,CfgPrinterItems.cfg_invert_e0_dir);		
					}
					//E1取反
					tmp_index = (char *)strstr(file_str, ">EXT1_INVERSE");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">EXT1_INVERSE");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						temp = atoi(cmd_code);
						if(temp == 1)
						{
							CfgPrinterItems.cfg_invert_e1_dir = true;
						}
						else
						{
							CfgPrinterItems.cfg_invert_e1_dir = false;
						}

						HAL::eprSetByte(EPR_EXT1_INVERSE_CFG,CfgPrinterItems.cfg_invert_e1_dir);		
					}	
					
					tmp_index = (char *)strstr(file_str, ">HOMING_ORDER");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">HOMING_ORDER");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_homing_order = atoi(cmd_code);

						HAL::eprSetByte(EPR_HOMING_ORDER_CFG,CfgPrinterItems.cfg_homing_order);		
					}				


					//X?á??á?ê±·??ò
					tmp_index = (char *)strstr(file_str, ">X_HOME_DIR");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">X_HOME_DIR");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_x_home_dir= atoi(cmd_code);

						HAL::eprSetByte(EPR_X_HOME_DIR_CFG,CfgPrinterItems.cfg_x_home_dir);		
					}
					//Y?á??á?ê±·??ò
					tmp_index = (char *)strstr(file_str, ">Y_HOME_DIR");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">Y_HOME_DIR");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_y_home_dir= atoi(cmd_code);

						HAL::eprSetByte(EPR_Y_HOME_DIR_CFG,CfgPrinterItems.cfg_y_home_dir);	
					}
					//Z?á??á?ê±·??ò
					tmp_index = (char *)strstr(file_str, ">Z_HOME_DIR");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">Z_HOME_DIR");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_z_home_dir= atoi(cmd_code);

						HAL::eprSetByte(EPR_Z_HOME_DIR_CFG,CfgPrinterItems.cfg_z_home_dir);	
					}

					//X·??ò×?′ó?μ
					tmp_index = (char *)strstr(file_str, ">X_MAX_LENGTH");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">X_MAX_LENGTH");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_x_max_length = atof(cmd_code);

						HAL::eprSetFloat(EPR_X_MAX_LENGTH_CFG,CfgPrinterItems.cfg_x_max_length);			
					}
					//Y·??ò×?′ó?μ
					tmp_index = (char *)strstr(file_str, ">Y_MAX_LENGTH");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">Y_MAX_LENGTH");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_y_max_length = atof(cmd_code);

						HAL::eprSetFloat(EPR_Y_MAX_LENGTH_CFG,CfgPrinterItems.cfg_y_max_length);	
					}		
					//Z·??ò×?′ó?μ
					tmp_index = (char *)strstr(file_str, ">Z_MAX_LENGTH");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">Z_MAX_LENGTH");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_z_max_length = atof(cmd_code);

						HAL::eprSetFloat(EPR_Z_MAX_LENGTH_CFG,CfgPrinterItems.cfg_z_max_length);	
					}

					//X·??ò×?D?????
					tmp_index = (char *)strstr(file_str, ">X_MIN_POS");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">X_MIN_POS");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_x_min_pos = atof(cmd_code);

						HAL::eprSetFloat(EPR_X_MIN_POS_CFG,CfgPrinterItems.cfg_x_min_pos);	
					}
					//Y·??ò×?D?????
					tmp_index = (char *)strstr(file_str, ">Y_MIN_POS");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">Y_MIN_POS");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_y_min_pos = atof(cmd_code);

						HAL::eprSetFloat(EPR_Y_MIN_POS_CFG,CfgPrinterItems.cfg_y_min_pos);	
					}

					//Z·??ò×?D?????
					tmp_index = (char *)strstr(file_str, ">Z_MIN_POS");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">Z_MIN_POS");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_z_min_pos = atof(cmd_code);

						HAL::eprSetFloat(EPR_Z_MIN_POS_CFG,CfgPrinterItems.cfg_z_min_pos);		
					}

					tmp_index = (char *)strstr(file_str, ">ENDSTOP_X_MIN_INVERTING");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">ENDSTOP_X_MIN_INVERTING");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_endstop_x_min_inverting = atoi(cmd_code);

						HAL::eprSetByte(EPR_ENDSTOP_X_MIN_INVERTING_CFG,CfgPrinterItems.cfg_endstop_x_min_inverting);		
					}	

					tmp_index = (char *)strstr(file_str, ">ENDSTOP_Y_MIN_INVERTING");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">ENDSTOP_Y_MIN_INVERTING");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_endstop_y_min_inverting = atoi(cmd_code);

						HAL::eprSetByte(EPR_ENDSTOP_Y_MIN_INVERTING_CFG,CfgPrinterItems.cfg_endstop_y_min_inverting);		
					}
					
					tmp_index = (char *)strstr(file_str, ">ENDSTOP_Z_MIN_INVERTING");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">ENDSTOP_Z_MIN_INVERTING");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_endstop_z_min_inverting = atoi(cmd_code);

						HAL::eprSetByte(EPR_ENDSTOP_Z_MIN_INVERTING_CFG,CfgPrinterItems.cfg_endstop_z_min_inverting);			
					}	

					tmp_index = (char *)strstr(file_str, ">ENDSTOP_X_MAX_INVERTING");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">ENDSTOP_X_MAX_INVERTING");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_endstop_x_max_inverting = atoi(cmd_code);

						HAL::eprSetByte(EPR_ENDSTOP_X_MAX_INVERTING_CFG,CfgPrinterItems.cfg_endstop_x_max_inverting);		
					}	

					tmp_index = (char *)strstr(file_str, ">ENDSTOP_Y_MAX_INVERTING");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">ENDSTOP_Y_MAX_INVERTING");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_endstop_y_max_inverting = atoi(cmd_code);

						HAL::eprSetByte(EPR_ENDSTOP_Y_MAX_INVERTING_CFG,CfgPrinterItems.cfg_endstop_y_max_inverting);	
					}
					
					tmp_index = (char *)strstr(file_str, ">ENDSTOP_Z_MAX_INVERTING");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">ENDSTOP_Z_MAX_INVERTING");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_endstop_z_max_inverting = atoi(cmd_code);

						HAL::eprSetByte(EPR_ENDSTOP_Z_MAX_INVERTING_CFG,CfgPrinterItems.cfg_endstop_z_max_inverting);	
					}	
					

					tmp_index = (char *)strstr(file_str, ">MIN_HARDWARE_ENDSTOP_X");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MIN_HARDWARE_ENDSTOP_X");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_min_hardware_endstop_x= atoi(cmd_code);

						HAL::eprSetByte(EPR_MIN_HARDWARE_ENDSTOP_X_CFG,CfgPrinterItems.cfg_min_hardware_endstop_x);		
					}	

					tmp_index = (char *)strstr(file_str, ">MIN_HARDWARE_ENDSTOP_Y");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MIN_HARDWARE_ENDSTOP_Y");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_min_hardware_endstop_y= atoi(cmd_code);

						HAL::eprSetByte(EPR_MIN_HARDWARE_ENDSTOP_Y_CFG,CfgPrinterItems.cfg_min_hardware_endstop_y);	
					}	

					tmp_index = (char *)strstr(file_str, ">MIN_HARDWARE_ENDSTOP_Z");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MIN_HARDWARE_ENDSTOP_Z");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_min_hardware_endstop_z= atoi(cmd_code);

						HAL::eprSetByte(EPR_MIN_HARDWARE_ENDSTOP_Z_CFG,CfgPrinterItems.cfg_min_hardware_endstop_z);		
					}	


					
					tmp_index = (char *)strstr(file_str, ">MAX_HARDWARE_ENDSTOP_X");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_HARDWARE_ENDSTOP_X");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_hardware_endstop_x= atoi(cmd_code);

						HAL::eprSetByte(EPR_MAX_HARDWARE_ENDSTOP_X_CFG,CfgPrinterItems.cfg_max_hardware_endstop_x);	
					}	

					tmp_index = (char *)strstr(file_str, ">MAX_HARDWARE_ENDSTOP_Y");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_HARDWARE_ENDSTOP_Y");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_hardware_endstop_y= atoi(cmd_code);

						HAL::eprSetByte(EPR_MAX_HARDWARE_ENDSTOP_Y_CFG,CfgPrinterItems.cfg_max_hardware_endstop_y);	
					}	

					tmp_index = (char *)strstr(file_str, ">MAX_HARDWARE_ENDSTOP_Z");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">MAX_HARDWARE_ENDSTOP_Z");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_max_hardware_endstop_z= atoi(cmd_code);

						HAL::eprSetByte(EPR_MAX_HARDWARE_ENDSTOP_Z_CFG,CfgPrinterItems.cfg_max_hardware_endstop_z);	
					}	

					tmp_index = (char *)strstr(file_str, ">min_software_endstop_x");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">min_software_endstop_x");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_min_software_endstop_x= atoi(cmd_code);

						HAL::eprSetByte(EPR_MIN_SOFTWARE_ENDSTOP_X_CFG,CfgPrinterItems.cfg_min_software_endstop_x);			
					}
					
					tmp_index = (char *)strstr(file_str, ">min_software_endstop_y");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">min_software_endstop_y");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_min_software_endstop_y= atoi(cmd_code);

						HAL::eprSetByte(EPR_MIN_SOFTWARE_ENDSTOP_Y_CFG,CfgPrinterItems.cfg_min_software_endstop_y);	
					}

					tmp_index = (char *)strstr(file_str, ">min_software_endstop_z");
					if(tmp_index != 0)
					{
						memset(cmd_code,0,sizeof(cmd_code));
						tmp_index += strlen(">min_software_endstop_z");
						while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
						{
							if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
							{
								cmd_code[code_cnt] = *tmp_index;
								code_cnt ++;
							}
							tmp_index++;
							if(code_cnt > 200)
							{
								break;
							}						
						}
						code_cnt = 0;
						CfgPrinterItems.cfg_min_software_endstop_z= atoi(cmd_code);

						HAL::eprSetByte(EPR_MIN_SOFTWARE_ENDSTOP_Z_CFG,CfgPrinterItems.cfg_min_software_endstop_z);	
					}

			tmp_index = (char *)strstr(file_str, ">max_software_endstop_x");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">max_software_endstop_x");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_max_software_endstop_x= atoi(cmd_code);
			
				HAL::eprSetByte(EPR_MAX_SOFTWARE_ENDSTOP_X_CFG,CfgPrinterItems.cfg_max_software_endstop_x);	
			}
			
			tmp_index = (char *)strstr(file_str, ">max_software_endstop_y");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">max_software_endstop_y");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_max_software_endstop_y= atoi(cmd_code);
			
				HAL::eprSetByte(EPR_MAX_SOFTWARE_ENDSTOP_Y_CFG,CfgPrinterItems.cfg_max_software_endstop_y);		
			}
			
			tmp_index = (char *)strstr(file_str, ">max_software_endstop_z");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">max_software_endstop_z");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_max_software_endstop_z= atoi(cmd_code);
			
				HAL::eprSetByte(EPR_MAX_SOFTWARE_ENDSTOP_Z_CFG,CfgPrinterItems.cfg_max_software_endstop_z);	
			}
			tmp_index = (char *)strstr(file_str, ">DELTA_MAX_RADIUS");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">DELTA_MAX_RADIUS");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_delta_max_radius = atof(cmd_code);
			
				HAL::eprSetFloat(EPR_DELTA_MAX_RADIUS_CFG,CfgPrinterItems.cfg_delta_max_radius);		
			}
			
			tmp_index = (char *)strstr(file_str, ">PRINTER_RADIUS");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">PRINTER_RADIUS");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_printer_radius = atof(cmd_code);
			
				HAL::eprSetFloat(EPR_PRINTER_RADIUS_CFG,CfgPrinterItems.cfg_printer_radius);		
			}
			
			tmp_index = (char *)strstr(file_str, ">DELTA_DIAGONAL_ROD");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">DELTA_DIAGONAL_ROD");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_delta_diagonal_rod= atof(cmd_code);
			
				HAL::eprSetFloat(EPR_DELTA_DIAGONAL_ROD_CFG,CfgPrinterItems.cfg_delta_diagonal_rod);		
			}
			
			tmp_index = (char *)strstr(file_str, ">DELTA_FLOOR_SAFETY_MARGIN_MM");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">DELTA_FLOOR_SAFETY_MARGIN_MM");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_delta_floor_safety_margin_mm = atof(cmd_code);
			
				HAL::eprSetFloat(EPR_DELTA_FLOOR_SAFETY_MARGIN_MM_CFG,CfgPrinterItems.cfg_delta_floor_safety_margin_mm);	
			}
			
			tmp_index = (char *)strstr(file_str, ">FEATURE_Z_PROBE");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">FEATURE_Z_PROBE");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_feature_z_probe= atoi(cmd_code);
			
				HAL::eprSetByte(EPR_FEATURE_Z_PROBE_CFG,CfgPrinterItems.cfg_feature_z_probe);		
			}

			tmp_index = (char *)strstr(file_str, ">BED_LEVELING_METHOD");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">BED_LEVELING_METHOD");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_bed_leveling_method = atoi(cmd_code);
			
				HAL::eprSetByte(EPR_BED_LEVELING_METHOD_CFG,CfgPrinterItems.cfg_bed_leveling_method);		
			}	

			tmp_index = (char *)strstr(file_str, ">Z_PROBE_HEIGHT");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">Z_PROBE_HEIGHT");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_z_probe_height= atof(cmd_code);
			
				HAL::eprSetFloat(EPR_Z_PROBE_HEIGHT_CFG,CfgPrinterItems.cfg_z_probe_height);		
			}	

			tmp_index = (char *)strstr(file_str, ">Z_PROBE_X1");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">Z_PROBE_X1");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_z_probe_x1= atof(cmd_code);
			
				HAL::eprSetFloat(EPR_Z_PROBE_X1_CFG,CfgPrinterItems.cfg_z_probe_x1);			
			}	

			tmp_index = (char *)strstr(file_str, ">Z_PROBE_Y1");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">Z_PROBE_Y1");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_z_probe_y1= atof(cmd_code);
			
				HAL::eprSetFloat(EPR_Z_PROBE_Y1_CFG,CfgPrinterItems.cfg_z_probe_y1);		
			}	

			tmp_index = (char *)strstr(file_str, ">Z_PROBE_X2");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">Z_PROBE_X2");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_z_probe_x2= atof(cmd_code);
			
				HAL::eprSetFloat(EPR_Z_PROBE_X2_CFG,CfgPrinterItems.cfg_z_probe_x2);
			}	

			tmp_index = (char *)strstr(file_str, ">Z_PROBE_Y2");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">Z_PROBE_Y2");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_z_probe_y2= atof(cmd_code);
			
				HAL::eprSetFloat(EPR_Z_PROBE_Y2_CFG,CfgPrinterItems.cfg_z_probe_y2);
			}	
			tmp_index = (char *)strstr(file_str, ">Z_PROBE_X3");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">Z_PROBE_X3");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_z_probe_x3= atof(cmd_code);
			
				HAL::eprSetFloat(EPR_Z_PROBE_X3_CFG,CfgPrinterItems.cfg_z_probe_x3);	
			}	

			tmp_index = (char *)strstr(file_str, ">Z_PROBE_Y3");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">Z_PROBE_Y3");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_z_probe_y3= atof(cmd_code);
			
				HAL::eprSetFloat(EPR_Z_PROBE_Y3_CFG,CfgPrinterItems.cfg_z_probe_y3);	
			}	
			
			tmp_index = (char *)strstr(file_str, ">END_EFFECTOR_HORIZONTAL_OFFSET");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">END_EFFECTOR_HORIZONTAL_OFFSET");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_end_effector_horizontal_offset = atof(cmd_code);
			
				HAL::eprSetFloat(EPR_END_EFFECTOR_HORIZONTAL_OFFSET_CFG,CfgPrinterItems.cfg_end_effector_horizontal_offset);	
			}
			
			tmp_index = (char *)strstr(file_str, ">CARRIAGE_HORIZONTAL_OFFSET");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">CARRIAGE_HORIZONTAL_OFFSET");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_carriage_horizontal_offset = atof(cmd_code);
			
				HAL::eprSetFloat(EPR_CARRIAGE_HORIZONTAL_OFFSET_CFG,CfgPrinterItems.cfg_carriage_horizontal_offset);	
			}	
			
			tmp_index = (char *)strstr(file_str, ">ROD_RADIUS");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">ROD_RADIUS");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_rod_radius= atof(cmd_code);
			
				HAL::eprSetFloat(EPR_ROD_RADIUS_CFG,CfgPrinterItems.cfg_rod_radius);	
			}	
			tmp_index = (char *)strstr(file_str, ">BED_LEVELING_GRID_SIZE");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">BED_LEVELING_GRID_SIZE");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_bed_leveling_grid_size = atof(cmd_code);
			
				HAL::eprSetFloat(EPR_BED_LEVELING_GRID_SIZE_CFG,CfgPrinterItems.cfg_bed_leveling_grid_size);	
			}	
			tmp_index = (char *)strstr(file_str, ">Z_PROBE_SPEED");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">Z_PROBE_SPEED");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_z_probe_speed = atof(cmd_code);
			
				HAL::eprSetFloat(EPR_Z_PROBE_SPEED_CFG,CfgPrinterItems.cfg_z_probe_speed);	
			}		
			
			tmp_index = (char *)strstr(file_str, ">Z_PROBE_XY_SPEED");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">Z_PROBE_XY_SPEED");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_z_probe_xy_speed = atof(cmd_code);
			
				HAL::eprSetFloat(EPR_Z_PROBE_XY_SPEED_CFG,CfgPrinterItems.cfg_z_probe_xy_speed);	
			}					
			tmp_index = (char *)strstr(file_str, ">moreitem_pic_cnt");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">moreitem_pic_cnt");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				gCfgItems.MoreItem_pic_cnt = atoi(cmd_code);
			
				HAL::eprSetByte(EPR_MOREITEM_PIC_CNT_CFG,gCfgItems.MoreItem_pic_cnt);	
			}	
			//读button1命令
			tmp_index = (char *)strstr(file_str, ">moreitem_button1_cmd:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">moreitem_button1_cmd:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt++;
					tmp_index++;
					if(code_cnt >= 200)
					{
						break;
					}
				}
				code_cnt = 0;

				//分别写入button的命令代码
				SPI_FLASH_BufferWrite((u8 *)cmd_code,BUTTON_CMD1_ADDR,(strlen(cmd_code)+1));				
			}
			//读button2命令
			tmp_index = (char *)strstr(file_str, ">moreitem_button2_cmd:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">moreitem_button2_cmd:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				//分别写入button的命令代码
				SPI_FLASH_BufferWrite((u8 *)cmd_code,BUTTON_CMD2_ADDR,(strlen(cmd_code)+1));					
			}
			//读button3命令
			tmp_index = (char *)strstr(file_str, ">moreitem_button3_cmd:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">moreitem_button3_cmd:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				//分别写入button的命令代码
				SPI_FLASH_BufferWrite((u8 *)cmd_code,BUTTON_CMD3_ADDR,(strlen(cmd_code)+1));					
			}
			//读button4命令
			tmp_index = (char *)strstr(file_str, ">moreitem_button4_cmd:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">moreitem_button4_cmd:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				//分别写入button的命令代码
				SPI_FLASH_BufferWrite((u8 *)cmd_code,BUTTON_CMD4_ADDR,(strlen(cmd_code)+1));					
			}	
			//读button5命令
			tmp_index = (char *)strstr(file_str, ">moreitem_button5_cmd:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">moreitem_button5_cmd:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				//分别写入button的命令代码
				SPI_FLASH_BufferWrite((u8 *)cmd_code,BUTTON_CMD5_ADDR,(strlen(cmd_code)+1));					
			}	
			//读button6命令
			tmp_index = (char *)strstr(file_str, ">moreitem_button6_cmd:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">moreitem_button6_cmd:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				//分别写入button的命令代码
				SPI_FLASH_BufferWrite((u8 *)cmd_code,BUTTON_CMD6_ADDR,(strlen(cmd_code)+1));					
			}	
			//读button7命令
			tmp_index = (char *)strstr(file_str, ">moreitem_button7_cmd:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">moreitem_button7_cmd:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{

					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				//分别写入button的命令代码
				SPI_FLASH_BufferWrite((u8 *)cmd_code,BUTTON_CMD7_ADDR,(strlen(cmd_code)+1));					
			}
			//功能按钮1是否显示设置
			tmp_index = (char *)strstr(file_str, ">setmenu_func1_display");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">setmenu_func1_display");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				gCfgItems.func_btn1_display_flag = atoi(cmd_code);
			
				HAL::eprSetByte(EPR_SETMENU_FUNC1_DISPLAY_FLAG_CFG,gCfgItems.func_btn1_display_flag);		
			}
			//功能按钮2是否显示设置
			tmp_index = (char *)strstr(file_str, ">setmenu_func2_display");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">setmenu_func2_display");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				gCfgItems.func_btn2_display_flag = atoi(cmd_code);
			
				HAL::eprSetByte(EPR_SETMENU_FUNC2_DISPLAY_FLAG_CFG,gCfgItems.func_btn2_display_flag);		
			}
			//功能按钮3是否显示设置
			tmp_index = (char *)strstr(file_str, ">setmenu_func3_display");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">setmenu_func3_display");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				gCfgItems.func_btn3_display_flag = atoi(cmd_code);
			
				HAL::eprSetByte(EPR_SETMENU_FUNC3_DISPLAY_FLAG_CFG,gCfgItems.func_btn3_display_flag);		
			}			

			
			//功能按钮1命令
			tmp_index = (char *)strstr(file_str, ">setmenu_func1:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">setmenu_func1:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{

					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				//分别写入button的命令代码
				SPI_FLASH_BufferWrite((u8 *)cmd_code,BUTTON_FUNCTION1_ADDR,(strlen(cmd_code)+1));					
			}
			//功能按钮2命令
			tmp_index = (char *)strstr(file_str, ">setmenu_func2:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">setmenu_func2:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{

					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				//分别写入button的命令代码
				SPI_FLASH_BufferWrite((u8 *)cmd_code,BUTTON_FUNCTION2_ADDR,(strlen(cmd_code)+1));					
			}
			//功能按钮3命令
			tmp_index = (char *)strstr(file_str, ">setmenu_func3:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">setmenu_func3:");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{

					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				//分别写入button的命令代码
				//**SPI_FLASH_BufferWrite((u8 *)cmd_code,BUTTON_FUNCTION3_ADDR,(strlen(cmd_code)+1));					
			}			

			
			/*
			tmp_index = (char *)strstr(file_str, ">cfg_Have_UPS");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_Have_UPS");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				gCfgItems.have_ups = atoi(cmd_code);
			
				HAL::eprSetByte(EPR_HAVE_UPS_CFG,gCfgItems.have_ups);		
			}
			*/
				tmp_index = (char *)strstr(file_str, ">STEPPER_HIGH_DELAY");
				if(tmp_index != 0)
				{
					memset(cmd_code,0,sizeof(cmd_code));
					tmp_index += strlen(">STEPPER_HIGH_DELAY");
					while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
					{
						if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
						{
							cmd_code[code_cnt] = *tmp_index;
							code_cnt++;
						}
						tmp_index++;
						if(code_cnt > 200)
						{
							break;
						}						
					}
					code_cnt = 0;
					CfgPrinterItems.cfg_stepper_high_delay = atoi(cmd_code);
				
					HAL::eprSetInt32(EPR_STEPPER_HIGH_DELAY_CFG,CfgPrinterItems.cfg_stepper_high_delay);	
				}				
				
			}
			//调平方式(0:手动调平;1:自动调平)
			tmp_index = (char *)strstr(file_str, ">cfg_leveling_mode");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_leveling_mode");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				gCfgItems.leveling_mode= atoi(cmd_code);
			
				HAL::eprSetByte(EPR_LEVELING_MODE_CFG,gCfgItems.leveling_mode);		
			}		
			//手动调平位置数
			tmp_index = (char *)strstr(file_str, ">cfg_point_number");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_point_number");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				gCfgItems.leveling_point_number = atoi(cmd_code);
			
				HAL::eprSetByte(EPR_LEVELING_POINT_NUMB_CFG,gCfgItems.leveling_point_number);		
			}		
			//坐标1		
			tmp_index = (char *)strstr(file_str, ">cfg_point1:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_point1:");
				while(((*tmp_index) != ',')&&((*tmp_index) != ' '))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 
				}
				code_cnt = 0;
				gCfgItems.leveling_point1_x = atoi(cmd_code);
				HAL::eprSetInt32(EPR_LEVELING_POINT1_X_CFG,gCfgItems.leveling_point1_x);		
				
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index++;
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.leveling_point1_y = atoi(cmd_code);
				HAL::eprSetInt32(EPR_LEVELING_POINT1_Y_CFG,gCfgItems.leveling_point1_y);	
				
			}		

			//坐标2	
			tmp_index = (char *)strstr(file_str, ">cfg_point2:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_point2:");
				while(((*tmp_index) != ',')&&((*tmp_index) != ' '))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 
				}
				code_cnt = 0;
				gCfgItems.leveling_point2_x = atoi(cmd_code);
				HAL::eprSetInt32(EPR_LEVELING_POINT2_X_CFG,gCfgItems.leveling_point2_x);		
				
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index++;
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.leveling_point2_y = atoi(cmd_code);
				HAL::eprSetInt32(EPR_LEVELING_POINT2_Y_CFG,gCfgItems.leveling_point2_y);	
				}
			//坐标3
			tmp_index = (char *)strstr(file_str, ">cfg_point3:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_point3:");
				while(((*tmp_index) != ',')&&((*tmp_index) != ' '))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 
				}
				code_cnt = 0;
				gCfgItems.leveling_point3_x = atoi(cmd_code);
				HAL::eprSetInt32(EPR_LEVELING_POINT3_X_CFG,gCfgItems.leveling_point3_x);		
				
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index++;
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.leveling_point3_y = atoi(cmd_code);
				HAL::eprSetInt32(EPR_LEVELING_POINT3_Y_CFG,gCfgItems.leveling_point3_y);	
				}		
			//坐标4	
			tmp_index = (char *)strstr(file_str, ">cfg_point4:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_point4:");
				while(((*tmp_index) != ',')&&((*tmp_index) != ' '))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 
				}
				code_cnt = 0;
				gCfgItems.leveling_point4_x = atoi(cmd_code);
				HAL::eprSetInt32(EPR_LEVELING_POINT4_X_CFG,gCfgItems.leveling_point4_x);		
				
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index++;
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.leveling_point4_y = atoi(cmd_code);
				HAL::eprSetInt32(EPR_LEVELING_POINT4_Y_CFG,gCfgItems.leveling_point4_y);	
				}			
			//坐标5
			tmp_index = (char *)strstr(file_str, ">cfg_point5:");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_point5:");
				while(((*tmp_index) != ',')&&((*tmp_index) != ' '))
				{
					cmd_code[code_cnt] = *tmp_index;
					code_cnt ++;
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 
				}
				code_cnt = 0;
				gCfgItems.leveling_point5_x = atoi(cmd_code);
				HAL::eprSetInt32(EPR_LEVELING_POINT5_X_CFG,gCfgItems.leveling_point5_x);		
				
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index++;
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt >= 20)
					{
						break;
					} 					
				}
				code_cnt = 0;
				gCfgItems.leveling_point5_y = atoi(cmd_code);
				HAL::eprSetInt32(EPR_LEVELING_POINT5_Y_CFG,gCfgItems.leveling_point5_y);	
				}			
			//手动调平Z轴移动速度
			tmp_index = (char *)strstr(file_str, ">cfg_leveling_z_speed");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_leveling_z_speed");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				gCfgItems.leveling_z_speed= atoi(cmd_code);
			
				HAL::eprSetInt32(EPR_LEVELING_Z_SPEED_CFG,gCfgItems.leveling_z_speed);		
			}		
			//手动调平XY轴移动速度
			tmp_index = (char *)strstr(file_str, ">cfg_leveling_xy_speed");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_leveling_xy_speed");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				gCfgItems.leveling_xy_speed= atoi(cmd_code);
			
				HAL::eprSetInt32(EPR_LEVELING_XY_SPEED_CFG,gCfgItems.leveling_xy_speed);		
			}	
			
			tmp_index = (char *)strstr(file_str, ">cfg_filament_in_out_step");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_filament_in_out_step");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				gCfgItems.filamentchange_step= atoi(cmd_code);
			
				HAL::eprSetByte(EPR_FILAMENT_IN_OUT_STEP,gCfgItems.filamentchange_step);		
			}					
			
			tmp_index = (char *)strstr(file_str, ">cfg_filament_change_speed");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_filament_change_speed");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				gCfgItems.filamentchange_speed= atoi(cmd_code);
			
				HAL::eprSetInt32(EPR_FILAMENT_CHANGE_SPEED_CFG,gCfgItems.filamentchange_speed);		
			}						

			tmp_index = (char *)strstr(file_str, ">cfg_filament_limit_temperature");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">cfg_filament_limit_temperature");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				gCfgItems.filament_limit_temper= atoi(cmd_code);
			
				HAL::eprSetInt32(EPR_FILAMENT_CHANGE_LIMIT_CFG,gCfgItems.filament_limit_temper);		
			}		
			tmp_index = (char *)strstr(file_str, ">X_ENABLE_ON");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">X_ENABLE_ON");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_x_enable_on = atoi(cmd_code);
			
				HAL::eprSetByte(EPR_X_ENABLE_ON_CFG,CfgPrinterItems.cfg_x_enable_on);		
			}				
			tmp_index = (char *)strstr(file_str, ">Y_ENABLE_ON");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">Y_ENABLE_ON");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_y_enable_on = atoi(cmd_code);
			
				HAL::eprSetByte(EPR_Y_ENABLE_ON_CFG,CfgPrinterItems.cfg_y_enable_on);		
			}			
			tmp_index = (char *)strstr(file_str, ">Z_ENABLE_ON");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">Z_ENABLE_ON");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_z_enable_on = atoi(cmd_code);
			
				HAL::eprSetByte(EPR_Z_ENABLE_ON_CFG,CfgPrinterItems.cfg_z_enable_on);		
			}				
			tmp_index = (char *)strstr(file_str, ">EXT0_ENABLE_ON");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">EXT0_ENABLE_ON");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_e0_enable_on = atoi(cmd_code);
			
				HAL::eprSetByte(EPR_E0_ENABLE_ON_CFG,CfgPrinterItems.cfg_e0_enable_on);		
			}			
			tmp_index = (char *)strstr(file_str, ">EXT1_ENABLE_ON");
			if(tmp_index != 0)
			{
				memset(cmd_code,0,sizeof(cmd_code));
				tmp_index += strlen(">EXT1_ENABLE_ON");
				while(((*tmp_index) != '\r')&&((*tmp_index) != '\n')&&((*tmp_index) != '#'))
				{
					if((*tmp_index != 0x20)&&(*tmp_index != 0x09))
					{
						cmd_code[code_cnt] = *tmp_index;
						code_cnt ++;
					}
					tmp_index++;
					if(code_cnt > 200)
					{
						break;
					}						
				}
				code_cnt = 0;
				CfgPrinterItems.cfg_e1_enable_on = atoi(cmd_code);
			
				HAL::eprSetByte(EPR_E1_ENABLE_ON_CFG,CfgPrinterItems.cfg_e1_enable_on);		
				}			
			//if(byte_have_read < 1024)
			//{
			//	//flash存储标志位。
			//	flash_inf_valid_flg = FLASH_INF_VALID_FLAG;
			//	SPI_FLASH_BufferWrite((uint8_t*)&flash_inf_valid_flg,FlASH_INF_VALID_ADDR,4);					
			//	break;
			//}
		}
		
		f_close(&curFile_temp);
                
                
                strcat(newname,SD_Path);
                strcat(newname,cfg_file_cur);
                
		r = f_unlink(newname);
		r = f_rename(oldname,newname);
		
	}
		

}

#endif

#ifdef GLENN_DEBUG
void SDCard::writeToFile()
{
  size_t nbyte;
  char szName[10];

  strcpy(szName, "Testing\r\n");
  nbyte = file.write(szName, strlen(szName));
  /*Com::print("L=");
  Com::print((long)nbyte);
  Com::println();*/
}

#endif
/*----------------------暂停-断电-续打 功能函数-------------------------------*/
#if 0   //**
void SDCard::saveSdpos()
{
  if(!sdmode) return;
  *(sdposbufP++) = sdpos;
  if(sdposbufP > (&sdposbuf[SDPOS_BUF_LEN-1]))
    sdposbufP=&sdposbuf[0];
}
void SDCard::savecurFileName()        //保存文件名
{
  for(int i=0;i<SAVE_CURFILENAME_LEN;i++)       //清除旧文件名
      HAL::eprSetByte(EPR_SAV_curFileName+i,0);

  for(int i=0;i<SAVE_CURFILENAME_LEN;i++)
      HAL::eprSetByte(EPR_SAV_curFileName+i,(uint8_t)curFileName[i]);
  
  sdposbufP=&sdposbuf[0];
  FALA_5V_CTRL = FALA_ON;    // 打开法拉电容
}


void SDCard::printStatusCheck()        //检查是否续打
{
  volatile int flag;

  flag =0;
  if(flag)      //测试使用
    HAL::eprSetByte(EPR_SAV_FLAG,(uint8_t)PRINTER_NORMAL);    //续打标志复位
  
  
  if(HAL::eprGetByte(EPR_SAV_FLAG)!=PRINTER_NORMAL)      //需要续打
    mksContiuePrint();
  
}


void SDCard::mksWriteToEpr()        //暂停、断电时保存数据，以备断电后续打
{
  	TemperatureController *act0_save = tempController[0];
	TemperatureController *act1_save = tempController[1];

  
  if(HAL::eprGetByte(EPR_SAV_FLAG)==PRINTER_PWRDN_REPRINT)      //直接断电时保存的文件位置，需回退n个G指令
    mksCalculateSdpos();
  
  HAL::eprSetInt32(EPR_SAV_sdpos,(int32_t)sdpos);
  
  HAL::eprSetInt16(EPR_SAV_targetTmpCEtr,(int16_t)act0_save->targetTemperatureC);//打印头温度
  HAL::eprSetInt16(EPR_SAV_targetTmpCEtr_e1,(int16_t)act1_save->targetTemperatureC);
  
  //delta机型归零，不需要保存当前位置
  //直接断电，不需要保存当前位置
  if(CfgPrinterItems.cfg_drive_system != DELTA && HAL::eprGetByte(EPR_SAV_FLAG)!=PRINTER_PWRDN_REPRINT)  
  {
  HAL::eprSetFloat(EPR_SAV_currentPositionX,Printer::currentPosition[X_AXIS]);
  HAL::eprSetFloat(EPR_SAV_currentPositionY,Printer::currentPosition[Y_AXIS]);
  HAL::eprSetFloat(EPR_SAV_currentPositionZ,Printer::currentPosition[Z_AXIS]);
  }
  
  if(HAL::eprGetByte(EPR_SAV_FLAG)==PRINTER_PAUSE_REPRINT)      //直接断电时，不需要保存X,Y,E
  {
    HAL::eprSetFloat(EPR_SAV_memoryX,Printer::memoryX);
    HAL::eprSetFloat(EPR_SAV_memoryY,Printer::memoryY);
    HAL::eprSetFloat(EPR_SAV_memoryE,Printer::memoryE);
  }
  
  HAL::eprSetFloat(EPR_SAV_memoryZ,Printer::memoryZ);
  
  
  
  if(CfgPrinterItems.cfg_have_heated_bed == 1)
    HAL::eprSetByte(EPR_SAV_targetTmpCBed,(uint8_t)heatedBedController.targetTemperatureC);
  
  HAL::eprSetInt16(EPR_SAV_memoryF,(int16_t)Printer::memoryF);
  
  HAL::eprSetByte(EPR_SAV_fanSpeed,Printer::fanSpeed); //风扇
  // 打印时间保寸
  HAL::eprSetInt16(EPR_SAV_hour,(int16_t)print_time.hours);
  HAL::eprSetByte(EPR_SAV_minute,print_time.minutes);
  //HAL::eprSetByte(EPR_SAV_second,print_time.seconds);  
  
}

void SDCard::mksReadFrEpr()        //暂停、断电后读取数据,恢复续打
{
	TemperatureController *act0_read = tempController[0];
	TemperatureController *act1_read = tempController[1];

  Printer::currentPosition[X_AXIS] = HAL::eprGetFloat(EPR_SAV_currentPositionX);
  Printer::currentPosition[Y_AXIS] = HAL::eprGetFloat(EPR_SAV_currentPositionY);
  Printer::currentPosition[Z_AXIS] = HAL::eprGetFloat(EPR_SAV_currentPositionZ);
 
  Printer::memoryX = HAL::eprGetFloat(EPR_SAV_memoryX);
  Printer::memoryY = HAL::eprGetFloat(EPR_SAV_memoryY);
  Printer::memoryZ = HAL::eprGetFloat(EPR_SAV_memoryZ);

  Printer::memoryE = HAL::eprGetFloat(EPR_SAV_memoryE);
  Printer::memoryF = (float)HAL::eprGetInt16(EPR_SAV_memoryF);
  
  if(CfgPrinterItems.cfg_have_heated_bed == 1)
    heatedBedController.targetTemperatureC = (float)HAL::eprGetByte(EPR_SAV_targetTmpCBed);
  
  act0_read->targetTemperatureC = (float)HAL::eprGetInt16(EPR_SAV_targetTmpCEtr);
  act1_read->targetTemperatureC = (float)HAL::eprGetInt16(EPR_SAV_targetTmpCEtr_e1);
  
  Printer::fanSpeed = HAL::eprGetByte(EPR_SAV_fanSpeed);
  
  sdpos = (uint32_t)HAL::eprGetInt32(EPR_SAV_sdpos);

}

void SDCard::mksContiuePrint()        //暂停、断电后续打
{   
    //加热打打印头和热床
    GCode code;
    if(CfgPrinterItems.cfg_have_heated_bed == 1)
    {
      code.params=1026; code.M=140; code.S=(int32_t)heatedBedController.targetTemperatureC;      //M140
      Commands::executeGCode(&code);
    }
      code.params=1026; code.M=109; code.S=(int32_t)Extruder::current->tempControl.targetTemperatureC;      //M109
      Commands::executeGCode(&code);
    if(CfgPrinterItems.cfg_have_heated_bed == 1)
    {
      code.params=1026; code.M=190; code.S=(int32_t)heatedBedController.targetTemperatureC;      //M190
      Commands::executeGCode(&code);
    } 
    
    Commands::setFanSpeed(Printer::fanSpeed);    //设置风扇

	
    //各类机型继续打印处理
    switch(CfgPrinterItems.cfg_drive_system)
    {
      case DELTA:
        deltaContinuePrint();
        break;
      case CARTESIAN:
        CartesianContinuePrint();
        break;
        
      default:
        CartesianContinuePrint();
      break;
        
    }
    //mksPrinterStatusFlag = MKS_PR_REPRINT;

}

void SDCard::mksContiuePrint_interface()        //续打提示界面
{
  for(int i=0;i<SAVE_CURFILENAME_LEN;i++)       //读取文件名
    curFileName[i] = (char)HAL::eprGetByte(EPR_SAV_curFileName+i);
  
  sdposbufP=&sdposbuf[0];
  
  if (selectFile(curFileName, false))   //打开续打的文件
  {
  #if 0
    mksReadFrEpr();

    f_lseek(&curFile,sdpos);
    
    if(HAL::eprGetByte(EPR_SAV_FLAG)==PRINTER_PWRDN_REPRINT)    //恢复X,Y,E坐标
      mksRecoverXYE();
    
    //加热打打印头和热床
    GCode code;
    if(CfgPrinterItems.cfg_have_heated_bed == 1)
    {
      code.params=1026; code.M=140; code.S=(int32_t)heatedBedController.targetTemperatureC;      //M140
      Commands::executeGCode(&code);
    }
      code.params=1026; code.M=109; code.S=(int32_t)Extruder::current->tempControl.targetTemperatureC;      //M109
      Commands::executeGCode(&code);
    if(CfgPrinterItems.cfg_have_heated_bed == 1)
    {
      code.params=1026; code.M=190; code.S=(int32_t)heatedBedController.targetTemperatureC;      //M190
      Commands::executeGCode(&code);
    } 
    
    Commands::setFanSpeed(Printer::fanSpeed);    //设置风扇
    
    //各类机型继续打印处理
    switch(CfgPrinterItems.cfg_drive_system)
    {
      case DELTA:
        deltaContinuePrint();
        break;
      case CARTESIAN:
        CartesianContinuePrint();
        break;
        
      default:
        CartesianContinuePrint();
      break;
        
    }
    mksPrinterStatusFlag = MKS_PR_REPRINT;
    #endif

	mksReadFrEpr();
	
    f_lseek(&curFile,sdpos);
  
    if(HAL::eprGetByte(EPR_SAV_FLAG)==PRINTER_PWRDN_REPRINT)    //恢复X,Y,E坐标
      mksRecoverXYE();
	
	mksPrinterStatusFlag = MKS_PR_REPRINT;
	
	disp_state_stack._disp_index = 0;
	memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
	disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINTING_UI;
	draw_pause();
  }
  else  //打开文件错误
  {
  //tan 20160826
  /*
    if(true) //用户放弃续打
    {
      HAL::eprSetByte(EPR_SAV_FLAG,(uint8_t)PRINTER_NORMAL);    //续打标志复位
    }
      else //用户重新拷贝文件
      {
        ;
      }
      */

	
	disp_state_stack._disp_index = 0;
	memset(disp_state_stack._disp_state, 0, sizeof(disp_state_stack._disp_state));
	disp_state_stack._disp_state[disp_state_stack._disp_index] = PRINT_READY_UI;
	draw_dialog(DIALOG_TYPE_REPRINT_NO_FILE);
  }
  
}

void SDCard::deltaContinuePrint()       //delta 机型暂停，断电续打处理
{
        //坐标归位  
        Printer::currentPositionSteps[E_AXIS] = (long)((Printer::memoryE-2*RETRACT_ON_PAUSE) * Printer::axisStepsPerMM[E_AXIS]);     //设置E长度
        Printer::destinationSteps[E_AXIS] = Printer::currentPositionSteps[E_AXIS];
        Printer::homeAxis_delta(true,true,true);                                                                         //X,Y,Z 归零
        Commands::waitUntilEndOfAllMoves();
        //Z轴先移动 到打印点上方 PAUSE_Z_RETRACT 位置
        float zpos =(float)(Printer::memoryZ + PAUSE_Z_RETRACT -Printer::currentPosition[Z_AXIS]);
        PrintLine::moveRelativeDistanceInStepsReal(0,0,(int32_t)(zpos*Printer::axisStepsPerMM[Z_AXIS]),0,Printer::homingFeedrate[Z_AXIS],true); //Z轴先下降,否则X,Y不能移动 
        Commands::waitUntilEndOfAllMoves();
        //继续打印
        
        Printer::currentPosition[Z_AXIS]= Printer::memoryZ;//保存Z,防止在同一层连续断电，
        
        continuePrint(true);
}


void SDCard::CartesianContinuePrint()   //cartesian 机型暂停，断电续打处理
{
  if(HAL::eprGetByte(EPR_SAV_FLAG)==PRINTER_PAUSE_REPRINT) //暂停
  {
    //G92
    Printer::currentPositionSteps[E_AXIS] = (long)((Printer::memoryE-2*RETRACT_ON_PAUSE) * Printer::axisStepsPerMM[E_AXIS]);
    Printer::destinationSteps[E_AXIS] = Printer::currentPositionSteps[E_AXIS];
    
    Printer::currentPositionSteps[X_AXIS] = (long)(Printer::currentPosition[X_AXIS] * Printer::axisStepsPerMM[X_AXIS]);
    Printer::currentPositionSteps[Y_AXIS] = (long)(Printer::currentPosition[Y_AXIS] * Printer::axisStepsPerMM[Y_AXIS]);
    Printer::currentPositionSteps[Z_AXIS] = (long)(Printer::currentPosition[Z_AXIS] * Printer::axisStepsPerMM[Z_AXIS]);
  
    //继续打印
    Printer::currentPosition[Z_AXIS]= Printer::memoryZ;//保存Z,防止在同一层连续断电，
    
    continuePrint(true);
    
  }
  
  if(HAL::eprGetByte(EPR_SAV_FLAG)==PRINTER_PWRDN_REPRINT) //直接断电
  {
        //设置E相對位置 G92 E 
        Printer::currentPositionSteps[E_AXIS] = (long)((Printer::memoryE-2*RETRACT_ON_PAUSE) * Printer::axisStepsPerMM[E_AXIS]);     
        Printer::destinationSteps[E_AXIS] = Printer::currentPositionSteps[E_AXIS];
        // 设置Z相對位置 G92 Z 
        Printer::currentPositionSteps[Z_AXIS] = (long)((Printer::memoryZ) * Printer::axisStepsPerMM[Z_AXIS]);     //设置Z相對位置
        Printer::destinationSteps[Z_AXIS] = Printer::currentPositionSteps[Z_AXIS];
        // Z軸上升PAUSE_Z_RETRACT
        PrintLine::moveRelativeDistanceInStepsReal(0,0,(int32_t)(PAUSE_Z_RETRACT*Printer::axisStepsPerMM[Z_AXIS]),0,Printer::homingFeedrate[Z_AXIS],true); //Z轴上升PAUSE_Z_RETRACT
        

        //X,Y 归零
        Printer::homeAxis(true,true,false);             
        Commands::waitUntilEndOfAllMoves();

        //继续打印
        
        Printer::currentPosition[Z_AXIS]= Printer::memoryZ;//保存Z,防止在同一层连续断电，
        
        continuePrint(true);

    
  }    

}


void SDCard::mksRecoverXYE()
{
  __ASM volatile("cpsid i");            //禁止中断
  uint32_t sdposbak = sdpos;
  uint32_t sdposold = sdpos;
  GCode *code;
  sdmode = 1;
  while(1)
  {
    GCode::commandsBuffered[GCode::bufferReadIndex].X =100000001; GCode::commandsBuffered[GCode::bufferReadIndex].Y =100000001; GCode::commandsBuffered[GCode::bufferReadIndex].E =100000001;
    GCode::readFromSerial();
    code = GCode::peekCurrentCommand();
    if(code)
    {
      code->popCurrentCommand();
      if(sdposold != sdposbak)  sdposold = sdposbak;
      
      sdposbak = sdpos;
        
      if(((long)code->X) < 100000000 && ((long)code->Y) < 100000000 && ((long)code->E) < 100000000 )
      {
        Printer::memoryX=code->X;
        Printer::memoryY=code->Y;
        Printer::memoryE=code->E;
        break;
      }
      
    }
  }
  sdmode = 0;
  memset(GCode::commandReceiving,0,MAX_CMD_SIZE);
  memset(GCode::commandsBuffered,0,GCODE_BUFFER_SIZE*sizeof(GCode));
  GCode::bufferReadIndex=0;
  GCode::bufferWriteIndex=0;
  GCode::commandsReceivingWritePosition=0;
  GCode::bufferLength=0;
  
  sdpos = sdposold;
  f_lseek(&curFile,sdpos);
  
   __ASM volatile("cpsie i");    //允许中断
}
void SDCard::mksCalculateSdpos()
{
  int i;
  uint8_t w=0;
  uint8_t offset=0;
  uint8_t count = PrintLine::getLinesCount();
  PrintLine *p= &PrintLine::lines[PrintLine::linesPos];
  
  for(i=0;i<count;i++)
  {
    if(p->flags == FLAG_WARMUP) w++;
    if(++p > &PrintLine::lines[PRINTLINE_CACHE_SIZE-1])   p=&PrintLine::lines[0];   
  }
  
  
    switch(CfgPrinterItems.cfg_drive_system)
    {
      case DELTA:
        offset=(count-w+2)/3-1;
        break;
      case CARTESIAN:
        offset=count-w+2;
        break;
      default:
        offset=count-w+2;
      break;
        
    }
    
  for(i=0;i<offset;i++)        // 
  {
    sdposbufP--;
    if(sdposbufP < (&sdposbuf[0]))   sdposbufP = &sdposbuf[SDPOS_BUF_LEN-1];
  }
  sdpos = *sdposbufP;
}

void SDCard::mksUpdateXYZE()
{
  if(mksPrinterStatusFlag != MKS_PR_PAUSE)      return;
  
  if( fabs(Printer::currentPosition[X_AXIS] - HAL::eprGetFloat(EPR_SAV_currentPositionX)) > 0.01 )  HAL::eprSetFloat(EPR_SAV_currentPositionX,Printer::currentPosition[X_AXIS]);
  if( fabs(Printer::currentPosition[Y_AXIS] - HAL::eprGetFloat(EPR_SAV_currentPositionY)) > 0.01 )  HAL::eprSetFloat(EPR_SAV_currentPositionY,Printer::currentPosition[Y_AXIS]);
  if( fabs(Printer::currentPosition[Z_AXIS] - HAL::eprGetFloat(EPR_SAV_currentPositionZ)) > 0.01 )  HAL::eprSetFloat(EPR_SAV_currentPositionZ,Printer::currentPosition[Z_AXIS]);
  
  //UI.cpp 需要增加emov_distance，并做处理
  if( fabs(Printer::memoryE - HAL::eprGetFloat(EPR_SAV_memoryE)) > 0.01 )  HAL::eprSetFloat(EPR_SAV_memoryE,Printer::memoryE);
  
}
#endif  //**
void SDCard::mksFalaInit()
{
        SET_OUTPUT(58);         //PA0   EXT3V3
        SET_OUTPUT(86);         //PF10  FALA

        FALA_5V_CTRL = FALA_ON;
        
        EXT_3V3_CTRL = EXT3V3_ON;
    
}

/*
void SDCard::mksFastPause()
{
  InterruptProtectedBlock noInts;
  PrintLine::linesWritePos = 0;           
  PrintLine::linesCount = 0;      
  PrintLine::linesPos = 0;    
  
  memset(PrintLine::lines,0,PRINTLINE_CACHE_SIZE*sizeof(PrintLine));
  
  mksCalculateSdpos();
  f_lseek(&curFile,sdpos);
  
}
*/
/*----------------------暂停-断电-续打 功能函数-------------------------------*/


#endif

