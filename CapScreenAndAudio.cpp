/**
 *最简单的基于FFmpeg的AVDevice例子（屏幕录制+声音采集），视频采用mpeg4编码，音频采用aac编码，并生成mp4文件
 *缪国凯 MK
 *821486004@qq.com
 */

#include "stdafx.h"

#ifdef	__cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/audio_fifo.h>
#include <SDL/SDL.h>

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




#undef USE_SDL
#define USE_SDL

extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }


#ifdef USE_SDL


//Output YUV420P   
//'1' Use Dshow   
//'0' Use GDIgrab  
#define USE_DSHOW 1

//Refresh Event  
#define SFM_REFRESH_EVENT  (SDL_USEREVENT + 1)  

#define SFM_BREAK_EVENT  (SDL_USEREVENT + 2)  
//Output YUV420P   
#define OUTPUT_YUV420P 0


int thread_exit = 0;


#endif

AVFormatContext	*pFormatCtx_Video = NULL, *pFormatCtx_Audio = NULL, *pFormatCtx_Out = NULL;
AVCodecContext	*pCodecCtx_Video;
AVCodec			*pCodec_Video;
AVFifoBuffer	*fifo_video = NULL;
AVAudioFifo		*fifo_audio = NULL;
int VideoIndex, AudioIndex;

CRITICAL_SECTION AudioSection, VideoSection;



SwsContext *img_convert_ctx;
int frame_size = 0;

uint8_t *picture_buf = NULL, *frame_buf = NULL;

bool bCap = true;
bool RecStat = false;

DWORD WINAPI ScreenCapThreadProc( LPVOID lpParam );
DWORD WINAPI AudioCapThreadProc( LPVOID lpParam );
DWORD WINAPI SetRecStat(LPVOID lpParam);

int OpenVideoCapture()
{
	int ret;
	AVInputFormat *ifmt=av_find_input_format("gdigrab");
	//这里可以加参数打开，例如可以指定采集帧率
	AVDictionary *options = NULL;

	printf("--James--[%s:%d]---\n",__FILE__,__LINE__);
	av_dict_set(&options, "framerate", "15", NULL);
	//av_dict_set(&options,"offset_x","20",0);
	//The distance from the top edge of the screen or desktop
	//av_dict_set(&options,"offset_y","40",0);
	//Video frame size. The default is to capture the full screen
	av_dict_set(&options,"video_size","1280x720",0);
	printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
	if( (ret = avformat_open_input(&pFormatCtx_Video, "desktop", ifmt, &options))!=0)
	//if ((ret = avformat_open_input(&pFormatCtx_Video, "Lenovo EasyCamera", ifmt, &options)) != 0)
	{
		printf("Couldn't open input stream.%d\n",ret);
		return -1;
	}
	if(avformat_find_stream_info(pFormatCtx_Video,NULL)<0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}
	if (pFormatCtx_Video->streams[0]->codec->codec_type != AVMEDIA_TYPE_VIDEO)
	{
		printf("Couldn't find video stream information.\n");
		return -1;
	}
	pCodecCtx_Video = pFormatCtx_Video->streams[0]->codec;
	pCodec_Video = avcodec_find_decoder(pCodecCtx_Video->codec_id);
	if(pCodec_Video == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}
	if(avcodec_open2(pCodecCtx_Video, pCodec_Video, NULL) < 0)
	{
		printf("Could not open codec.\n");
		return -1;
	}

	

	img_convert_ctx = sws_getContext(pCodecCtx_Video->width, pCodecCtx_Video->height, pCodecCtx_Video->pix_fmt, 
		pCodecCtx_Video->width, pCodecCtx_Video->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL); 

	frame_size = avpicture_get_size(pCodecCtx_Video->pix_fmt, pCodecCtx_Video->width, pCodecCtx_Video->height);
	//申请30帧缓存
	fifo_video = av_fifo_alloc(30 * avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx_Video->width, pCodecCtx_Video->height));

	printf("--James--[%s:%d]---End:%s--\n", __FILE__, __LINE__, __func__);
	return 0;
}

static char *dup_wchar_to_utf8(wchar_t *w)
{
	char *s = NULL;
	int l = WideCharToMultiByte(CP_UTF8, 0, w, -1, 0, 0, 0, 0);
	s = (char *) av_malloc(l);
	if (s)
		WideCharToMultiByte(CP_UTF8, 0, w, -1, s, l, 0, 0);
	printf("--James--[%s:%d]---End:%s--\n", __FILE__, __LINE__, __func__);
	return s;
}

int OpenAudioCapture()
{
	//查找输入方式
	AVInputFormat *pAudioInputFmt = av_find_input_format("dshow");
	
	//以Direct Show的方式打开设备，并将 输入方式 关联到格式上下文
	char * psDevName = dup_wchar_to_utf8(L"audio=virtual-audio-capturer");

	if (avformat_open_input(&pFormatCtx_Audio, psDevName, pAudioInputFmt,NULL) < 0)
	{
		printf("Couldn't open input stream.（无法打开音频输入流）\n");
		return -1;
	}

	if(avformat_find_stream_info(pFormatCtx_Audio,NULL)<0)  
		return -1; 
	
	if(pFormatCtx_Audio->streams[0]->codec->codec_type != AVMEDIA_TYPE_AUDIO)
	{
		printf("Couldn't find video stream information.（无法获取音频流信息）\n");
		return -1;
	}

	AVCodec *tmpCodec = avcodec_find_decoder(pFormatCtx_Audio->streams[0]->codec->codec_id);
	if(0 > avcodec_open2(pFormatCtx_Audio->streams[0]->codec, tmpCodec, NULL))
	{
		printf("can not find or open audio decoder!\n");
	}

	printf("--James--[%s:%d]---End:%s--\n", __FILE__, __LINE__, __func__);

	return 0;
}

int OpenOutPut()
{
	AVStream *pVideoStream = NULL, *pAudioStream = NULL;
	const char *outFileName = "Rec.mp4";
	avformat_alloc_output_context2(&pFormatCtx_Out, NULL, NULL, outFileName);

	if (pFormatCtx_Video->streams[0]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
	{
		AVCodecContext *videoCodecCtx;
		VideoIndex = 0;
		pVideoStream = avformat_new_stream(pFormatCtx_Out, NULL);

		if (!pVideoStream)
		{
			printf("can not new stream for output!\n");
			return -1;
		}

		//set codec context param
		pVideoStream->codec->codec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
		pVideoStream->codec->height = pFormatCtx_Video->streams[0]->codec->height;
		pVideoStream->codec->width = pFormatCtx_Video->streams[0]->codec->width;
		
		pVideoStream->codec->time_base = pFormatCtx_Video->streams[0]->codec->time_base;
		pVideoStream->codec->sample_aspect_ratio = pFormatCtx_Video->streams[0]->codec->sample_aspect_ratio;
		// take first format from list of supported formats
		pVideoStream->codec->pix_fmt = pFormatCtx_Out->streams[VideoIndex]->codec->codec->pix_fmts[0];

		//open encoder
		if (!pVideoStream->codec->codec)
		{
			printf("can not find the encoder!\n");
			return -1;
		}

		if (pFormatCtx_Out->oformat->flags & AVFMT_GLOBALHEADER)
			pVideoStream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

		if ((avcodec_open2(pVideoStream->codec, pVideoStream->codec->codec, NULL)) < 0)
		{
			printf("can not open the encoder\n");
			return -1;
		}
	}

	if(pFormatCtx_Audio->streams[0]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
	{
		AVCodecContext *pOutputCodecCtx;
		AudioIndex = 1;
		pAudioStream = avformat_new_stream(pFormatCtx_Out, NULL);

		pAudioStream->codec->codec = avcodec_find_encoder(pFormatCtx_Out->oformat->audio_codec);

		pOutputCodecCtx = pAudioStream->codec;

		pOutputCodecCtx->sample_rate = pFormatCtx_Audio->streams[0]->codec->sample_rate;
		pOutputCodecCtx->channel_layout = pFormatCtx_Out->streams[0]->codec->channel_layout;
		pOutputCodecCtx->channels = av_get_channel_layout_nb_channels(pAudioStream->codec->channel_layout);
		if(pOutputCodecCtx->channel_layout == 0)
		{
			pOutputCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
			pOutputCodecCtx->channels = av_get_channel_layout_nb_channels(pOutputCodecCtx->channel_layout);

		}
		pOutputCodecCtx->sample_fmt = pAudioStream->codec->codec->sample_fmts[0];
		AVRational time_base={1, pAudioStream->codec->sample_rate};
		pAudioStream->time_base = time_base;
		//audioCodecCtx->time_base = time_base;

		pOutputCodecCtx->codec_tag = 0;  
		if (pFormatCtx_Out->oformat->flags & AVFMT_GLOBALHEADER)  
			pOutputCodecCtx->flags |= CODEC_FLAG_GLOBAL_HEADER;

		if (avcodec_open2(pOutputCodecCtx, pOutputCodecCtx->codec, 0) < 0)
		{
			//编码器打开失败，退出程序
			return -1;
		}
	}

	if (!(pFormatCtx_Out->oformat->flags & AVFMT_NOFILE))
	{
		if(avio_open(&pFormatCtx_Out->pb, outFileName, AVIO_FLAG_WRITE) < 0)
		{
			printf("can not open output file handle!\n");
			return -1;
		}
	}

	if(avformat_write_header(pFormatCtx_Out, NULL) < 0)
	{
		printf("can not write the header of the output file!\n");
		return -1;
	}

	printf("--James--[%s:%d]---End:%s--\n", __FILE__, __LINE__, __func__);
	return 0;
}

DWORD WINAPI ScreenCapThreadProc( LPVOID lpParam )
{
	AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	int got_picture;
	AVFrame	*pFrame;
	pFrame=avcodec_alloc_frame();

	printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
	AVFrame *picture = avcodec_alloc_frame();
	int size = avpicture_get_size(pFormatCtx_Out->streams[VideoIndex]->codec->pix_fmt, 
		pFormatCtx_Out->streams[VideoIndex]->codec->width, pFormatCtx_Out->streams[VideoIndex]->codec->height);
	picture_buf = new uint8_t[size];

	avpicture_fill((AVPicture *)picture, picture_buf, 
		pFormatCtx_Out->streams[VideoIndex]->codec->pix_fmt, 
		pFormatCtx_Out->streams[VideoIndex]->codec->width, 
		pFormatCtx_Out->streams[VideoIndex]->codec->height);

	FILE *p = NULL;
	p = fopen("proc_test.yuv", "wb+");

	int height = pFormatCtx_Out->streams[VideoIndex]->codec->height;
	int width = pFormatCtx_Out->streams[VideoIndex]->codec->width;
	int y_size=height*width;
	while(bCap)
	{
		if (av_read_frame(pFormatCtx_Video, packet) < 0)
		{
			continue;
		}
		if(packet->stream_index == 0)
		{
			if (avcodec_decode_video2(pCodecCtx_Video, pFrame, &got_picture, packet) < 0)
			{
				printf("Decode Error.（解码错误）\n");
				continue;
			}
			if (got_picture)
			{
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, 
					pFormatCtx_Out->streams[VideoIndex]->codec->height, picture->data, picture->linesize);
				
				if (av_fifo_space(fifo_video) >= size)
				{
					EnterCriticalSection(&VideoSection);					
					av_fifo_generic_write(fifo_video, picture->data[0], y_size, NULL);
					av_fifo_generic_write(fifo_video, picture->data[1], y_size/4, NULL);
					av_fifo_generic_write(fifo_video, picture->data[2], y_size/4, NULL);
					LeaveCriticalSection(&VideoSection);
				}
			}
		}
	}
	av_frame_free(&pFrame);
	av_frame_free(&picture);
	delete[] picture_buf;
	printf("--James--[%s:%d]---End:%s--\n", __FILE__, __LINE__, __func__);
	return 0;
}

DWORD WINAPI AudioCapThreadProc( LPVOID lpParam )
{
	AVPacket pkt;
	AVFrame *frame;
	frame = av_frame_alloc();
	int gotframe;
	while(bCap)
	{
		if(av_read_frame(pFormatCtx_Audio,&pkt) < 0)
		{
			continue;
		}
		
		if (avcodec_decode_audio4(pFormatCtx_Audio->streams[0]->codec, frame, &gotframe, &pkt) < 0)
		{
			av_frame_free(&frame);
			printf("can not decoder a frame");
			break;
		}
		av_free_packet(&pkt);

		if (!gotframe)
		{
			continue;//没有获取到数据，继续下一次
		}

		//printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
		if (NULL == fifo_audio)
		{
			fifo_audio = av_audio_fifo_alloc(pFormatCtx_Audio->streams[0]->codec->sample_fmt, 
				pFormatCtx_Audio->streams[0]->codec->channels, 30 * frame->nb_samples);
		}

		int buf_space = av_audio_fifo_space(fifo_audio);
		if (av_audio_fifo_space(fifo_audio) >= frame->nb_samples)
		{
			EnterCriticalSection(&AudioSection);
			av_audio_fifo_write(fifo_audio, (void **)frame->data, frame->nb_samples);
			LeaveCriticalSection(&AudioSection);
		}
	}
	av_frame_free(&frame);
	printf("--James--[%s:%d]---End:%s--\n", __FILE__, __LINE__,__func__);
	return 0;
}


DWORD WINAPI SetRecStat(LPVOID lpParam)
{
#if 0
	int m_count = 0;
	while(m_count <= 10)
	{
		++m_count;
		printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
		Sleep(1000);
	}
	RecStat = true;
	printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
#else
	RecStat = true;
#endif
	return 0;
}





#ifdef USE_SDL
int sfp_refresh_thread(void *opaque)
{
	while (thread_exit == 0) {
		SDL_Event event;
		event.type = SFM_REFRESH_EVENT;
		SDL_PushEvent(&event);
		SDL_Delay(40);
	}
	return 0;
}

//Show Dshow Device
void show_dshow_device() {
	AVFormatContext *pFormatCtx = avformat_alloc_context();
	AVDictionary* options = NULL;
	av_dict_set(&options, "list_devices", "true", 0);
	AVInputFormat *iformat = av_find_input_format("dshow");
	printf("========Device Info=============\n");
	avformat_open_input(&pFormatCtx, "video=dummy", iformat, &options);
	printf("================================\n");
}

//Show AVFoundation Device
void show_avfoundation_device() {
	AVFormatContext *pFormatCtx = avformat_alloc_context();
	AVDictionary* options = NULL;
	av_dict_set(&options, "list_devices", "true", 0);
	AVInputFormat *iformat = av_find_input_format("avfoundation");
	printf("==AVFoundation Device Info===\n");
	avformat_open_input(&pFormatCtx, "", iformat, &options);
	printf("=============================\n");
}



int Start_SDL_Rec(int argc, char* argv[])
{

	AVFormatContext	*pFormatCtx;
	int				i, videoindex;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;

	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();

	//Open File
	//char filepath[]="src01_480x272_22.h265";
	//avformat_open_input(&pFormatCtx,filepath,NULL,NULL)

	//Register Device
	avdevice_register_all();
	//Windows
#ifdef _WIN32
#if USE_DSHOW
	//Use dshow
	//
	//Need to Install screen-capture-recorder
	//screen-capture-recorder
	//Website: http://sourceforge.net/projects/screencapturer/
	//
	AVInputFormat *ifmt = av_find_input_format("dshow");
	if (avformat_open_input(&pFormatCtx, "video=screen-capture-recorder", ifmt, NULL) != 0) {
		printf("Couldn't open input stream.\n");
		return -1;
	}
#else
	//Use gdigrab
	AVDictionary* options = NULL;
	//Set some options
	//grabbing frame rate
	//av_dict_set(&options,"framerate","5",0);
	//The distance from the left edge of the screen or desktop
	//av_dict_set(&options,"offset_x","20",0);
	//The distance from the top edge of the screen or desktop
	//av_dict_set(&options,"offset_y","40",0);
	//Video frame size. The default is to capture the full screen
	//av_dict_set(&options,"video_size","640x480",0);
	AVInputFormat *ifmt = av_find_input_format("gdigrab");
	if (avformat_open_input(&pFormatCtx, "desktop", ifmt, &options) != 0) {
		printf("Couldn't open input stream.\n");
		return -1;
	}

#endif
#elif defined linux
	//Linux
	AVDictionary* options = NULL;
	//Set some options
	//grabbing frame rate
	//av_dict_set(&options,"framerate","5",0);
	//Make the grabbed area follow the mouse
	//av_dict_set(&options,"follow_mouse","centered",0);
	//Video frame size. The default is to capture the full screen
	//av_dict_set(&options,"video_size","640x480",0);
	AVInputFormat *ifmt = av_find_input_format("x11grab");
	//Grab at position 10,20
	if (avformat_open_input(&pFormatCtx, ":0.0+10,20", ifmt, &options) != 0) {
		printf("Couldn't open input stream.\n");
		return -1;
	}
#else
	show_avfoundation_device();
	//Mac
	AVInputFormat *ifmt = av_find_input_format("avfoundation");
	//Avfoundation
	//[video]:[audio]
	if (avformat_open_input(&pFormatCtx, "1", ifmt, NULL) != 0) {
		printf("Couldn't open input stream.\n");
		return -1;
	}
#endif

	if (avformat_find_stream_info(pFormatCtx, NULL)<0)
	{
		printf("Couldn't find stream information.\n");
		return -1;
	}
	videoindex = -1;
	for (i = 0; i<pFormatCtx->nb_streams; i++)
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			break;
		}
	if (videoindex == -1)
	{
		printf("Didn't find a video stream.\n");
		return -1;
	}
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		printf("Codec not found.\n");
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL)<0)
	{
		printf("Could not open codec.\n");
		return -1;
	}
	AVFrame	*pFrame, *pFrameYUV;
	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	//uint8_t *out_buffer=(uint8_t *)av_malloc(avpicture_get_size(PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	//avpicture_fill((AVPicture *)pFrameYUV, out_buffer, PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	//SDL----------------------------
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -1;
	}
	int screen_w = 640, screen_h = 480;
	const SDL_VideoInfo *vi = SDL_GetVideoInfo();
	//Half of the Desktop's width and height.
	screen_w = vi->current_w / 2;
	screen_h = vi->current_h / 2;
	SDL_Surface *screen;
	screen = SDL_SetVideoMode(screen_w, screen_h, 0, 0);

	if (!screen) {
		printf("SDL: could not set video mode - exiting:%s\n", SDL_GetError());
		return -1;
	}
	SDL_Overlay *bmp;
	bmp = SDL_CreateYUVOverlay(pCodecCtx->width, pCodecCtx->height, SDL_YV12_OVERLAY, screen);
	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = screen_w;
	rect.h = screen_h;
	//SDL End------------------------
	int ret, got_picture;

	AVPacket *packet = (AVPacket *)av_malloc(sizeof(AVPacket));

#if OUTPUT_YUV420P 
	FILE *fp_yuv = fopen("output.yuv", "wb+");
#endif  

	struct SwsContext *img_convert_ctx;
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	//------------------------------
	SDL_Thread *video_tid = SDL_CreateThread(sfp_refresh_thread, NULL);
	//
	SDL_WM_SetCaption("Simplest FFmpeg Grab Desktop", NULL);
	//Event Loop
	SDL_Event event;

	for (;;) {
		//Wait
		SDL_WaitEvent(&event);
		if (event.type == SFM_REFRESH_EVENT) {
			//------------------------------
			if (av_read_frame(pFormatCtx, packet) >= 0) {
				if (packet->stream_index == videoindex) {
					ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
					if (ret < 0) {
						printf("Decode Error.\n");
						return -1;
					}
					if (got_picture) {
						SDL_LockYUVOverlay(bmp);
						pFrameYUV->data[0] = bmp->pixels[0];
						pFrameYUV->data[1] = bmp->pixels[2];
						pFrameYUV->data[2] = bmp->pixels[1];
						pFrameYUV->linesize[0] = bmp->pitches[0];
						pFrameYUV->linesize[1] = bmp->pitches[2];
						pFrameYUV->linesize[2] = bmp->pitches[1];
						sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);

#if OUTPUT_YUV420P  
						int y_size = pCodecCtx->width*pCodecCtx->height;
						fwrite(pFrameYUV->data[0], 1, y_size, fp_yuv);    //Y   
						fwrite(pFrameYUV->data[1], 1, y_size / 4, fp_yuv);  //U  
						fwrite(pFrameYUV->data[2], 1, y_size / 4, fp_yuv);  //V  
#endif  
						SDL_UnlockYUVOverlay(bmp);

						SDL_DisplayYUVOverlay(bmp, &rect);

					}
				}
				av_free_packet(packet);
			}
			else {
				//Exit Thread
				thread_exit = 1;
				break;
			}
			if (RecStat)
			{
				thread_exit = 1;
				break;
			}
		}
		else if (event.type == SDL_QUIT) {
			thread_exit = 1;
			break;
		}

	}


	sws_freeContext(img_convert_ctx);

#if OUTPUT_YUV420P 
	fclose(fp_yuv);
#endif 

	SDL_Quit();

	//av_free(out_buffer);
	av_free(pFrameYUV);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}

#endif


int Start_Rec(int argc, _TCHAR* argv[])
{
	int ret;
	av_register_all();
	avdevice_register_all();
	if (OpenVideoCapture() < 0)
	{
		return -1;
	}
	if (OpenAudioCapture() < 0)
	{
		return -1;
	}
	if (OpenOutPut() < 0)
	{
		return -1;
	}

	InitializeCriticalSection(&VideoSection);
	InitializeCriticalSection(&AudioSection);

	AVFrame *picture = av_frame_alloc();
	int size = avpicture_get_size(pFormatCtx_Out->streams[VideoIndex]->codec->pix_fmt,
		pFormatCtx_Out->streams[VideoIndex]->codec->width, pFormatCtx_Out->streams[VideoIndex]->codec->height);
	picture_buf = new uint8_t[size];

	avpicture_fill((AVPicture *)picture, picture_buf,
		pFormatCtx_Out->streams[VideoIndex]->codec->pix_fmt,
		pFormatCtx_Out->streams[VideoIndex]->codec->width,
		pFormatCtx_Out->streams[VideoIndex]->codec->height);



	//star cap screen thread
	printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
	CreateThread(NULL, 0, ScreenCapThreadProc, 0, 0, NULL);
	//star cap audio thread
	CreateThread(NULL, 0, AudioCapThreadProc, 0, 0, NULL);
	//CreateThread(NULL, 0, SetRecStat, 0, 0, NULL);
	printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
	int64_t cur_pts_v = 0, cur_pts_a = 0;
	int VideoFrameIndex = 0, AudioFrameIndex = 0;

#if 1
	while (1)
	{
#if 0
		if (_kbhit() != 0 && bCap)
#else
		if (RecStat && bCap)
#endif
		{
			bCap = false;
			RecStat = false;
			printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
			Sleep(2000);//简单的用sleep等待采集线程关闭
		}
		if (fifo_audio && fifo_video)
		{
			int sizeAudio = av_audio_fifo_size(fifo_audio);
			int sizeVideo = av_fifo_size(fifo_video);
			/* Cant not print here */
			//printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
			//缓存数据写完就结束循环
			if (av_audio_fifo_size(fifo_audio) <= pFormatCtx_Out->streams[AudioIndex]->codec->frame_size &&
				av_fifo_size(fifo_video) <= frame_size && !bCap)
			{
				printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
				break;
			}
		}

		if (av_compare_ts(cur_pts_v, pFormatCtx_Out->streams[VideoIndex]->time_base,
			cur_pts_a, pFormatCtx_Out->streams[AudioIndex]->time_base) <= 0)
		{
			//			printf("--James--[%s:%d]---avfifo size:%d, frame_size:%d.\n", __FILE__, __LINE__, av_fifo_size(fifo_video) , frame_size);
			//read data from fifo
			if (av_fifo_size(fifo_video) < frame_size && !bCap)
			{
				/*Can not print here*/
				//printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
				cur_pts_v = 0x7fffffffffffffff;
			}
			if (av_fifo_size(fifo_video) >= size /*&& RecStat == FALSE*/)
			{
				//				printf("--James--[%s:%d]---av_fifo_size(fifo_video):%d,%d\n", __FILE__, __LINE__,av_fifo_size(fifo_video),size);
				EnterCriticalSection(&VideoSection);
				printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
				//picture_buf = (uint8_t*)malloc(size);
				//memset(picture_buf, 0x0, size);
				av_fifo_generic_read(fifo_video, picture_buf, size, NULL);
				printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
				LeaveCriticalSection(&VideoSection);
				printf("--James--[%s:%d]---\n", __FILE__, __LINE__);

				avpicture_fill((AVPicture *)picture, picture_buf,
					pFormatCtx_Out->streams[VideoIndex]->codec->pix_fmt,
					pFormatCtx_Out->streams[VideoIndex]->codec->width,
					pFormatCtx_Out->streams[VideoIndex]->codec->height);

				//pts = n * (（1 / timbase）/ fps);
				picture->pts = VideoFrameIndex * ((pFormatCtx_Video->streams[0]->time_base.den / pFormatCtx_Video->streams[0]->time_base.num) / 15);

				int got_picture = 0;
				AVPacket pkt;
				av_init_packet(&pkt);

				pkt.data = NULL;
				pkt.size = 0;
				int ret = avcodec_encode_video2(pFormatCtx_Out->streams[VideoIndex]->codec, &pkt, picture, &got_picture);
				if (ret < 0)
				{
					printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
					//编码错误,不理会此帧
					continue;
				}

				if (got_picture == 1)
				{
					//printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
					pkt.stream_index = VideoIndex;
					pkt.pts = av_rescale_q_rnd(pkt.pts, pFormatCtx_Video->streams[0]->time_base,
						pFormatCtx_Out->streams[VideoIndex]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
					pkt.dts = av_rescale_q_rnd(pkt.dts, pFormatCtx_Video->streams[0]->time_base,
						pFormatCtx_Out->streams[VideoIndex]->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

					pkt.duration = ((pFormatCtx_Out->streams[0]->time_base.den / pFormatCtx_Out->streams[0]->time_base.num) / 15);

					cur_pts_v = pkt.pts;

					ret = av_interleaved_write_frame(pFormatCtx_Out, &pkt);
					//delete[] pkt.data;
					av_free_packet(&pkt);
				}
				//free(picture_buf);
				VideoFrameIndex++;
			}
		}
		else
		{
			printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
			if (NULL == fifo_audio)
			{
				//printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
				continue;//还未初始化fifo
			}
			if (av_audio_fifo_size(fifo_audio) < pFormatCtx_Out->streams[AudioIndex]->codec->frame_size && !bCap)
			{
				//printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
				cur_pts_a = 0x7fffffffffffffff;
			}
			if (av_audio_fifo_size(fifo_audio) >=
				(pFormatCtx_Out->streams[AudioIndex]->codec->frame_size > 0 ? pFormatCtx_Out->streams[AudioIndex]->codec->frame_size : 1024))
			{
				/* Can not print here*/
				AVFrame *frame;
				frame = av_frame_alloc();
				frame->nb_samples = pFormatCtx_Out->streams[AudioIndex]->codec->frame_size>0 ? pFormatCtx_Out->streams[AudioIndex]->codec->frame_size : 1024;
				frame->channel_layout = pFormatCtx_Out->streams[AudioIndex]->codec->channel_layout;
				frame->format = pFormatCtx_Out->streams[AudioIndex]->codec->sample_fmt;
				frame->sample_rate = pFormatCtx_Out->streams[AudioIndex]->codec->sample_rate;
				av_frame_get_buffer(frame, 0);

				EnterCriticalSection(&AudioSection);
				av_audio_fifo_read(fifo_audio, (void **)frame->data,
					(pFormatCtx_Out->streams[AudioIndex]->codec->frame_size > 0 ? pFormatCtx_Out->streams[AudioIndex]->codec->frame_size : 1024));
				LeaveCriticalSection(&AudioSection);

				if (pFormatCtx_Out->streams[0]->codec->sample_fmt != pFormatCtx_Audio->streams[AudioIndex]->codec->sample_fmt
					|| pFormatCtx_Out->streams[0]->codec->channels != pFormatCtx_Audio->streams[AudioIndex]->codec->channels
					|| pFormatCtx_Out->streams[0]->codec->sample_rate != pFormatCtx_Audio->streams[AudioIndex]->codec->sample_rate)
				{
					//Must not add debug here
					//printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
					//如果输入和输出的音频格式不一样 需要重采样，这里是一样的就没做
				}

				AVPacket pkt_out;
				av_init_packet(&pkt_out);
				int got_picture = -1;
				pkt_out.data = NULL;
				pkt_out.size = 0;

				frame->pts = AudioFrameIndex * pFormatCtx_Out->streams[AudioIndex]->codec->frame_size;
				if (avcodec_encode_audio2(pFormatCtx_Out->streams[AudioIndex]->codec, &pkt_out, frame, &got_picture) < 0)
				{
					printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
					printf("can not decoder a frame");
				}
				av_frame_free(&frame);
				if (got_picture)
				{
					printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
					pkt_out.stream_index = AudioIndex;
					pkt_out.pts = AudioFrameIndex * pFormatCtx_Out->streams[AudioIndex]->codec->frame_size;
					pkt_out.dts = AudioFrameIndex * pFormatCtx_Out->streams[AudioIndex]->codec->frame_size;
					pkt_out.duration = pFormatCtx_Out->streams[AudioIndex]->codec->frame_size;

					cur_pts_a = pkt_out.pts;

					printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
					int ret = av_interleaved_write_frame(pFormatCtx_Out, &pkt_out);
					av_free_packet(&pkt_out);
				}
				AudioFrameIndex++;
			}
		}
	}

	printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
	av_write_trailer(pFormatCtx_Out);
	printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
	avio_close(pFormatCtx_Out->pb);
	avformat_free_context(pFormatCtx_Out);

	printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
	if (pFormatCtx_Video != NULL)
	{
		printf("--James--[%s:%d]---\n", __FILE__, __LINE__);
		avformat_free_context(pFormatCtx_Video);
		pFormatCtx_Video = NULL;
	}
	if (pFormatCtx_Audio != NULL)
	{
		avformat_free_context(pFormatCtx_Audio);
		pFormatCtx_Audio = NULL;
	}
#else
#endif
	printf("--James--[%s:%d]---End:%s--\n", __FILE__, __LINE__, __func__);
	return 0;
}

