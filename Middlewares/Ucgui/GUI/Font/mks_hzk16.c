/*
****************************************************** 


		 File Name   : hzk16.C
		 Compiler    : 
		 Author      : Liu_xf
		 Version     : V1.0
		 Date        : 2011-3-28 11:25:54
		 Description : 
		    ucguiµÄÖÐÎÄ×Ö¿â£¬Óëuc¹¤¾ßÉú³ÉµÄ×Ö¿âÎÄ¼þ²»Í¬µÄÊÇ£¬¿ÉÒÔ½«
		    ´óÈÝÁ¿µÄºº×ÖÊý×é´æÈëµ½Íâ²¿µÄFALSHÀï
		    µ±È»Õâ¸öÒ²ÊÇÓÉuc¹¤¾ßÉú³ÉµÄÎÄ¼þÐÞ¸Ä¶øÀ´µÄ¡£
*******************************************************
		 Structure : 
		 
		 History:

*******************************************************
*/

#include "..\core\GUI.H"
#ifndef GUI_FLASH
   #define GUI_FLASH
#endif
extern GUI_FLASH const GUI_FONT GUI_FontHZ16;

/*
GUI_FLASH const GUI_CHARINFO GUI_FontHZ16_CharInfo[] = {
{ 10, 10, 1, (void GUI_FLASH *)0},  // ×Ö·ûÔÚFLASHÖÐµÄÆ«ÒÆÁ¿
{ 16, 16, 2, (void GUI_FLASH *)3840}, //ºº×ÖÔÚFLASHÖÐµÄÆ«ÒÆÁ¿
};
*/

GUI_FLASH const GUI_CHARINFO GUI_FontHZ16_CharInfo[] = {
{ 8, 8, 1, (void GUI_FLASH *)0},	//FLASHÀïÃ»ÓÐ´æ×Ö·û£¬ÕâÀïÎª0
{ 16, 16, 2, (void GUI_FLASH *)0x600000}, //GUI_FontHZ16_Flash_BaseAddrÔÚGUI.hÖÐ¶¨Òå
{ 16, 12, 2, (void GUI_FLASH *)0x600000}, //GUI_FontHZ16_Flash_BaseAddrÔÚGUI.hÖÐ¶¨Òå
};

//ºº×ÖºÍ×Ö·ûË÷Òý±í//////////////////////////////////

GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8140= {
      0x8140,
      0x81fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)0 
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8240= {
      0x8240,
      0x82fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8140
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8340= {
      0x8340,
      0x83fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8240
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8440= {
      0x8440,
      0x84fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8340
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8540= {
      0x8540,
      0x85fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8440
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8640= {
      0x8640,
      0x86fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8540
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8740= {
      0x8740,
      0x87fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8640
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8840= {
      0x8840,
      0x88fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8740
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8940= {
      0x8940,
      0x89fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8840
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8a40= {
      0x8a40,
      0x8afe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8940
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8b40= {
      0x8b40,
      0x8bfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8a40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8c40= {
      0x8c40,
      0x8cfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8b40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8d40= {
      0x8d40,
      0x8dfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8c40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8e40= {
      0x8e40,
      0x8efe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8d40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop8f40= {
      0x8f40,
      0x8ffe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8e40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9040= {
      0x9040,
      0x90fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop8f40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9140= {
      0x9140,
      0x91fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9040
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9240= {
      0x9240,
      0x92fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9140
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9340= {
      0x9340,
      0x93fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9240
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9440= {
      0x9440,
      0x94fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9340
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9540= {
      0x9540,
      0x95fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9440
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9640= {
      0x9640,
      0x96fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9540
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9740= {
      0x9740,
      0x97fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9640
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9840= {
      0x9840,
      0x98fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9740
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9940= {
      0x9940,
      0x99fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9840
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9a40= {
      0x9a40,
      0x9afe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9940
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9b40= {
      0x9b40,
      0x9bfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9a40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9c40= {
      0x9c40,
      0x9cfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9b40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9d40= {
      0x9d40,
      0x9dfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9c40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9e40= {
      0x9e40,
      0x9efe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9d40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prop9f40= {
      0x9f40,
      0x9ffe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9e40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propa040= {
      0xa040,
      0xa0fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prop9f40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propa140= {
      0xa140,
      0xa1fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa040
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propa240= {
      0xa240,
      0xa2fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa140
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propa340= {
      0xa340,
      0xa3fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa240
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propa440= {
      0xa440,
      0xa4fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa340
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propa540= {
      0xa540,
      0xa5fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa440
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propa640= {
      0xa640,
      0xa6fe,
      &GUI_FontHZ16_CharInfo[2],
      (void *)&GUI_FontHZ16_Propa540
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propa740= {
      0xa740,
      0xa7fe,
      &GUI_FontHZ16_CharInfo[2],
      (void *)&GUI_FontHZ16_Propa640
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propa840= {
      0xa840,
      0xa89e,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa740
};

GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propa8a0= {
      0xa8a0,
      0xa8c0,
      &GUI_FontHZ16_CharInfo[2],
      (void *)&GUI_FontHZ16_Propa840
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propa8c1= {
      0xa8c1,
      0xa8fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa8a0
};

GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propa940= {
      0xa940,
      0xa9fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa840
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propaa40= {
      0xaa40,
      0xaafe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa940
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propab40= {
      0xab40,
      0xabfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propaa40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propac40= {
      0xac40,
      0xacfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propab40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propad40= {
      0xad40,
      0xadfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propac40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propae40= {
      0xae40,
      0xaefe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propad40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propaf40= {
      0xaf40,
      0xaffe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propae40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propb040= {
      0xb040,
      0xb0fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propaf40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propb140= {
      0xb140,
      0xb1fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb040
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propb240= {
      0xb240,
      0xb2fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb140
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propb340= {
      0xb340,
      0xb3fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb240
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propb440= {
      0xb440,
      0xb4fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb340
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propb540= {
      0xb540,
      0xb5fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb440
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propb640= {
      0xb640,
      0xb6fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb540
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propb740= {
      0xb740,
      0xb7fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb640
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propb840= {
      0xb840,
      0xb8fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb740
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propb940= {
      0xb940,
      0xb9fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb840
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propba40= {
      0xba40,
      0xbafe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb940
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propbb40= {
      0xbb40,
      0xbbfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propba40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propbc40= {
      0xbc40,
      0xbcfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propbb40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propbd40= {
      0xbd40,
      0xbdfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propbc40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propbe40= {
      0xbe40,
      0xbefe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propbd40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propbf40= {
      0xbf40,
      0xbffe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propbe40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propc040= {
      0xc040,
      0xc0fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propbf40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propc140= {
      0xc140,
      0xc1fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc040
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propc240= {
      0xc240,
      0xc2fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc140
};

GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propc340= {
      0xc340,
      0xc3fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc240
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propc440= {
      0xc440,
      0xc4fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc340
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propc540= {
      0xc540,
      0xc5fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc440
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propc640= {
      0xc640,
      0xc6fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc540
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propc740= {
      0xc740,
      0xc7fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc640
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propc840= {
      0xc840,
      0xc8fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc740
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propc940= {
      0xc940,
      0xc9fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc840
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propca40= {
      0xca40,
      0xcafe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc940
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propcb40= {
      0xcb40,
      0xcbfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propca40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propcc40= {
      0xcc40,
      0xccfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propcb40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propcd40= {
      0xcd40,
      0xcdfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propcc40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propce40= {
      0xce40,
      0xcefe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propcd40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propcf40= {
      0xcf40,
      0xcffe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propce40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propd040= {
      0xd040,
      0xd0fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propcf40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propd140= {
      0xd140,
      0xd1fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd040
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propd240= {
      0xd240,
      0xd2fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd140
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propd340= {
      0xd340,
      0xd3fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd240
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propd440= {
      0xd440,
      0xd4fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd340
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propd540= {
      0xd540,
      0xd5fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd440
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propd640= {
      0xd640,
      0xd6fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd540
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propd740= {
      0xd740,
      0xd7fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd640
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propd840= {
      0xd840,
      0xd8fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd740
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propd940= {
      0xd940,
      0xd9fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd840
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propda40= {
      0xda40,
      0xdafe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd940
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propdb40= {
      0xdb40,
      0xdbfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propda40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propdc40= {
      0xdc40,
      0xdcfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propdb40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propdd40= {
      0xdd40,
      0xddfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propdc40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propde40= {
      0xde40,
      0xdefe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propdd40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propdf40= {
      0xdf40,
      0xdffe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propde40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prope040= {
      0xe040,
      0xe0fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propdf40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prope140= {
      0xe140,
      0xe1fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope040
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prope240= {
      0xe240,
      0xe2fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope140
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prope340= {
      0xe340,
      0xe3fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope240
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prope440= {
      0xe440,
      0xe4fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope340
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prope540= {
      0xe540,
      0xe5fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope440
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prope640= {
      0xe640,
      0xe6fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope540
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prope740= {
      0xe740,
      0xe7fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope640
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prope840= {
      0xe840,
      0xe8fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope740
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Prope940= {
      0xe940,
      0xe9fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope840
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propea40= {
      0xea40,
      0xeafe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope940
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propeb40= {
      0xeb40,
      0xebfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propea40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propec40= {
      0xec40,
      0xecfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propeb40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Proped40= {
      0xed40,
      0xedfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propec40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propee40= {
      0xee40,
      0xeefe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Proped40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propef40= {
      0xef40,
      0xeffe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propee40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propf040= {
      0xf040,
      0xf0fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propef40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propf140= {
      0xf140,
      0xf1fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf040
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propf240= {
      0xf240,
      0xf2fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf140
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propf340= {
      0xf340,
      0xf3fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf240
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propf440= {
      0xf440,
      0xf4fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf340
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propf540= {
      0xf540,
      0xf5fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf440
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propf640= {
      0xf640,
      0xf6fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf540
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propf740= {
      0xf740,
      0xf7fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf640
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propf840= {
      0xf840,
      0xf8fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf740
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propf940= {
      0xf940,
      0xf9fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf840
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propfa40= {
      0xfa40,
      0xfafe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf940
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propfb40= {
      0xfb40,
      0xfbfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propfa40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propfc40= {
      0xfc40,
      0xfcfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propfb40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propfd40= {
      0xfd40,
      0xfdfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propfc40
};
GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propfe40= {
      0xfe40,
      0xfefe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propfd40
};
//ASC×Ö·û
/* --*/ 
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Prop0020= {
      0x0020,
      0x007f,
      &GUI_FontHZ16_CharInfo[0],
      (void *)&GUI_FontHZ16_Propfe40 
};

GUI_FLASH const GUI_FONT GUI_FontHZ16 = {
      GUI_FONTTYPE_PROP_SJIS, 
      16, 
      16, 
      1,  
      1,  
      (void GUI_FLASH *)&GUI_FontHZ16_Prop0020
};


#if 0

/*÷¡--÷þ*/

GUI_FLASH const GUI_FONT_PROP GUI_FontHZ16_Propf7a1= {
      0xf7a1,
      0xf7fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)0 
};
/*ö¡--öþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propf6a1= {
      0xf6a1,
      0xf6fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf7a1 
};
/*õ¡--õþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propf5a1= {
      0xf5a1,
      0xf5fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf6a1 
};
/*ô¡--ôþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propf4a1= {
      0xf4a1,
      0xf4fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf5a1 
};
/*ó¡--óþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propf3a1= {
      0xf3a1,
      0xf3fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf4a1 
};
/*ò¡--òþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propf2a1= {
      0xf2a1,
      0xf2fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf3a1 
};
/*ñ¡--ñþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propf1a1= {
      0xf1a1,
      0xf1fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf2a1 
};
/*ð¡--ðþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propf0a1= {
      0xf0a1,
      0xf0fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf1a1 
};
/*ï¡--ïþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propefa1= {
      0xefa1,
      0xeffe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propf0a1 
};
/*î¡--îþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propeea1= {
      0xeea1,
      0xeefe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propefa1 
};
/*í¡--íþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propeda1= {
      0xeda1,
      0xedfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propeea1 
};
/*ì¡--ìþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propeca1= {
      0xeca1,
      0xecfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propeda1 
};
/*ë¡--ëþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propeba1= {
      0xeba1,
      0xebfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propeca1 
};
/*ê¡--êþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propeaa1= {
      0xeaa1,
      0xeafe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propeba1 
};
/*é¡--éþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Prope9a1= {
      0xe9a1,
      0xe9fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propeaa1 
};
/*è¡--èþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Prope8a1= {
      0xe8a1,
      0xe8fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope9a1 
};
/*ç¡--çþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Prope7a1= {
      0xe7a1,
      0xe7fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope8a1 
};
/*æ¡--æþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Prope6a1= {
      0xe6a1,
      0xe6fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope7a1 
};
/*å¡--åþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Prope5a1= {
      0xe5a1,
      0xe5fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope6a1 
};
/*ä¡--äþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Prope4a1= {
      0xe4a1,
      0xe4fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope5a1 
};
/*ã¡--ãþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Prope3a1= {
      0xe3a1,
      0xe3fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope4a1 
};
/*â¡--âþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Prope2a1= {
      0xe2a1,
      0xe2fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope3a1 
};
/*á¡--áþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Prope1a1= {
      0xe1a1,
      0xe1fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope2a1 
};
/*à¡--àþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Prope0a1= {
      0xe0a1,
      0xe0fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope1a1 
};
/*ß¡--ßþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propdfa1= {
      0xdfa1,
      0xdffe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Prope0a1 
};
/*Þ¡--Þþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propdea1= {
      0xdea1,
      0xdefe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propdfa1 
};
/*Ý¡--Ýþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propdda1= {
      0xdda1,
      0xddfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propdea1 
};
/*Ü¡--Üþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propdca1= {
      0xdca1,
      0xdcfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propdda1 
};
/*Û¡--Ûþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propdba1= {
      0xdba1,
      0xdbfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propdca1 
};
/*Ú¡--Úþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propdaa1= {
      0xdaa1,
      0xdafe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propdba1 
};
/*Ù¡--Ùþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propd9a1= {
      0xd9a1,
      0xd9fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propdaa1 
};
/*Ø¡--Øþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propd8a1= {
      0xd8a1,
      0xd8fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd9a1 
};
/*×¡--×þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propd7a1= {
      0xd7a1,
      0xd7fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd8a1 
};
/*Ö¡--Öþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propd6a1= {
      0xd6a1,
      0xd6fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd7a1 
};
/*Õ¡--Õþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propd5a1= {
      0xd5a1,
      0xd5fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd6a1 
};
/*Ô¡--Ôþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propd4a1= {
      0xd4a1,
      0xd4fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd5a1 
};
/*Ó¡--Óþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propd3a1= {
      0xd3a1,
      0xd3fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd4a1 
};
/*Ò¡--Òþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propd2a1= {
      0xd2a1,
      0xd2fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd3a1 
};
/*Ñ¡--Ñþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propd1a1= {
      0xd1a1,
      0xd1fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd2a1 
};
/*Ð¡--Ðþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propd0a1= {
      0xd0a1,
      0xd0fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd1a1 
};
/*Ï¡--Ïþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propcfa1= {
      0xcfa1,
      0xcffe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propd0a1 
};
/*Î¡--Îþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propcea1= {
      0xcea1,
      0xcefe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propcfa1 
};
/*Í¡--Íþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propcda1= {
      0xcda1,
      0xcdfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propcea1 
};
/*Ì¡--Ìþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propcca1= {
      0xcca1,
      0xccfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propcda1 
};
/*Ë¡--Ëþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propcba1= {
      0xcba1,
      0xcbfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propcca1 
};
/*Ê¡--Êþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propcaa1= {
      0xcaa1,
      0xcafe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propcba1 
};
/*É¡--Éþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propc9a1= {
      0xc9a1,
      0xc9fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propcaa1 
};
/*È¡--Èþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propc8a1= {
      0xc8a1,
      0xc8fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc9a1 
};
/*Ç¡--Çþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propc7a1= {
      0xc7a1,
      0xc7fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc8a1 
};
/*Æ¡--Æþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propc6a1= {
      0xc6a1,
      0xc6fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc7a1 
};
/*Å¡--Åþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propc5a1= {
      0xc5a1,
      0xc5fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc6a1 
};
/*Ä¡--Äþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propc4a1= {
      0xc4a1,
      0xc4fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc5a1 
};
/*Ã¡--Ãþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propc3a1= {
      0xc3a1,
      0xc3fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc4a1 
};
/*Â¡--Âþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propc2a1= {
      0xc2a1,
      0xc2fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc3a1 
};
/*Á¡--Áþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propc1a1= {
      0xc1a1,
      0xc1fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc2a1 
};
/*À¡--Àþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propc0a1= {
      0xc0a1,
      0xc0fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc1a1 
};
/*¿¡--¿þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propbfa1= {
      0xbfa1,
      0xbffe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propc0a1 
};
/*¾¡--¾þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propbea1= {
      0xbea1,
      0xbefe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propbfa1 
};
/*½¡--½þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propbda1= {
      0xbda1,
      0xbdfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propbea1 
};
/*¼¡--¼þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propbca1= {
      0xbca1,
      0xbcfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propbda1 
};
/*»¡--»þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propbba1= {
      0xbba1,
      0xbbfe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propbca1 
};
/*º¡--ºþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propbaa1= {
      0xbaa1,
      0xbafe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propbba1 
};
/*¹¡--¹þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propb9a1= {
      0xb9a1,
      0xb9fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propbaa1 
};
/*¸¡--¸þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propb8a1= {
      0xb8a1,
      0xb8fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb9a1 
};
/*·¡--·þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propb7a1= {
      0xb7a1,
      0xb7fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb8a1 
};
/*¶¡--¶þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propb6a1= {
      0xb6a1,
      0xb6fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb7a1 
};
/*µ¡--µþ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propb5a1= {
      0xb5a1,
      0xb5fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb6a1 
};
/*´¡--´þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propb4a1= {
      0xb4a1,
      0xb4fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb5a1 
};
/*³¡--³þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propb3a1= {
      0xb3a1,
      0xb3fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb4a1 
};
/*²¡--²þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propb2a1= {
      0xb2a1,
      0xb2fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb3a1 
};
/*±¡--±þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propb1a1= {
      0xb1a1,
      0xb1fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb2a1 
};
/*°¡--°þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propb0a1= {
      0xb0a1,
      0xb0fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb1a1 
};
/*©¡--©þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propa9a1= {
      0xa9a1,
      0xa9fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propb0a1 
};
/*¨¡--¨þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propa8a1= {
      0xa8a1,
      0xa8fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa9a1 
};
/*§¡--§þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propa7a1= {
      0xa7a1,
      0xa7fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa8a1 
};
/*¦¡--¦þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propa6a1= {
      0xa6a1,
      0xa6fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa7a1 
};
/*¥¡--¥þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propa5a1= {
      0xa5a1,
      0xa5fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa6a1 
};
/*¤¡--¤þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propa4a1= {
      0xa4a1,
      0xa4fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa5a1 
};
/*£¡--£þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propa3a1= {
      0xa3a1,
      0xa3fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa4a1 
};
/*¢¡--¢þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propa2a1= {
      0xa2a1,
      0xa2fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa3a1 
};
/*¡¡--¡þ*/
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Propa1a1= {
      0xa1a1,
      0xa1fe,
      &GUI_FontHZ16_CharInfo[1],
      (void *)&GUI_FontHZ16_Propa2a1 
};
//ASC×Ö·û
/* --*/ 
GUI_FLASH  const GUI_FONT_PROP GUI_FontHZ16_Prop0020= {
      0x0020,
      0x007f,
      &GUI_FontHZ16_CharInfo[0],
      (void *)&GUI_FontHZ16_Propa1a1 
};
GUI_FLASH const GUI_FONT GUI_FontHZ16 = {
      GUI_FONTTYPE_PROP_SJIS, 
      16, 
      16, 
      1,  
      1,  
      (void GUI_FLASH *)&GUI_FontHZ16_Prop0020
};
#endif

