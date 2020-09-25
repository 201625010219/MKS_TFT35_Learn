/********************   (C) COPYRIGHT 2014 www.makerbase.com.cn   ********************
 * 文件�?  ：mks_tft_fifo.h
 * 描述    ：fifo基本操作 init,push,pop,check
 * 作�?    : skyblue
**********************************************************************************/

#ifndef MKS_TFT_FIFO_H
#define MKS_TFT_FIFO_H

#ifdef __cplusplus
 extern "C" {
#endif

#define FIFO_NODE	15//10		//FIFO深度,节点�?�?
#define FIFO_SIZE	100		//FIFO宽度,节点长度


typedef enum
{
	fifo_empty = 0,		// 	FIFO �?
	fifo_full,			//	FIFO �?
	fifo_ok,
	fifo_err,										 
} FIFO_STATUS;

typedef struct _FIFO
{
    unsigned char       *head;		//指向FIFO的头节点
    unsigned char       *tail; 	//指向FIFO的尾节点
    FIFO_STATUS    	 status;	// FIFO 状�?
    unsigned char 	 buffer[FIFO_NODE][FIFO_SIZE];
    unsigned char       *p;		//
    unsigned char       count;			
}TFT_FIFO;





/***************************************************************************************
�?  数：initFIFO(TFT_FIFO *fifo)
输入量：fifo�?指向FIFO的指针；
        
返回值：
        
�?  �?�?
***************************************************************************************/
extern void initFIFO(TFT_FIFO *fifo);

/***************************************************************************************
�?  数：pushFIFO(TFT_FIFO *fifo,unsigned char *src);
输入量：fifo�?指向FIFO的指�?
        src�?指向数据源的指针,该数�?源必须以'\n'结束�?
返回值：fifo_full --表示FIFO已满，不�?写入�?
        fifo_ok   --表示成功写入,
        fifo_err	-- 表示数据过长或未�?'\n'结束
�?  �?：数�?必须�?'\n'结束�?
***************************************************************************************/
extern FIFO_STATUS pushFIFO(TFT_FIFO *fifo,unsigned char *src);

/***************************************************************************************
�?  数：popFIFO(TFT_FIFO *fifo,unsigned char *src);
输入量：fifo�?指向FIFO的指�?
        dst�?指向�?的buffer指针�?
返回值：fifo_empty --表示FIFO已空，不�?读，
		fifo_err  -- 表示�?读取�?'\n'�?
        fifo_ok   --表示读取成功,
�?  �?�?
***************************************************************************************/
extern FIFO_STATUS popFIFO(TFT_FIFO *fifo,unsigned char *dst);


/***************************************************************************************
�?  数：checkFIFO(TFT_FIFO *fifo);
输入量：fifo�?指向FIFO的指针；
        
返回值： fifo_empty --表示FIFO空，
		fifo_full --表示FIFO满，
        fifo_ok   --表示�?读写�?
        
�?  �?�?
***************************************************************************************/
extern FIFO_STATUS checkFIFO(TFT_FIFO *fifo);

#ifdef __cplusplus
}
#endif
#endif
