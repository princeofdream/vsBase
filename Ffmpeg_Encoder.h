#pragma once


#include "stdafx.h"

//#define __FFMPEG_ENCODER__

#if 0
#ifdef	__cplusplus
extern "C"
{
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavutil/audio_fifo.h"

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "avdevice.lib")
#pragma comment(lib, "avfilter.lib")

	//#pragma comment(lib, "avfilter.lib")
	//#pragma comment(lib, "postproc.lib")
	//#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")
#ifdef __cplusplus
};
#endif

AVFormatContext	*pFormatCtx_Video = NULL, *pFormatCtx_Audio = NULL, *pFormatCtx_Out = NULL;
AVCodecContext	*pCodecCtx_Video;
AVCodec			*pCodec_Video;
AVFifoBuffer	*fifo_video = NULL;
AVAudioFifo		*fifo_audio = NULL;
int VideoIndex, AudioIndex;

CRITICAL_SECTION AudioSection, VideoSection;


class Ffmpeg_Encoder
{
public:
	Ffmpeg_Encoder();
	~Ffmpeg_Encoder();

public:
	AVFrame *m_pRGBFrame;   //帧对象  
	AVFrame *m_pYUVFrame;   //帧对象  
	AVCodec *pCodecH264;    //编码器  
	AVCodecContext *c;      //编码器数据结构对象  
	uint8_t *yuv_buff;      //yuv图像数据区  
	uint8_t *rgb_buff;      //rgb图像数据区  
	SwsContext *scxt;       //图像格式转换对象  
	uint8_t *outbuf;        //编码出来视频数据缓存  
	int outbuf_size;        //编码输出数据去大小  
	int nDataLen;           //rgb图像数据区长度  
	int width;              //输出视频宽度  
	int height;             //输出视频高度  
//	avpacket pkt;            //数据包结构体

public:
	void Ffmpeg_Encoder_Init();//初始化  
//	void Ffmpeg_Encoder_Setpara(CodecID mycodeid, int vwidth, int vheight);//设置参数,第一个参数为编码器,第二个参数为压缩出来的视频的宽度，第三个视频则为其高度  
	void Ffmpeg_Encoder_Encode(FILE *file, uint8_t *data);//编码并写入数据到文件  
	void Ffmpeg_Encoder_Close();//关闭
};

#endif
