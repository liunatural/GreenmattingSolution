#ifndef H_VIDEO_DECODER
#define H_VIDEO_DECODER

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


#ifdef __cplusplus
extern "C"
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/parseutils.h>

#ifdef __cplusplus
};
#endif




class VideoDecoder
{
private:
	AVFormatContext* pFormatCtx;
	AVCodecContext* pCodecCtx;
	AVCodec* pCodec;
	AVPacket *pPacket;
	AVFrame* pFrame;
	AVCodecParserContext *parser = NULL;

public:
	VideoDecoder();
	~VideoDecoder();
	bool init();

	AVFrame* decode(AVPacket *pPacket);
	AVFrame* decode(char *in_data, int in_size);
	//bool decode(char *in_data, int in_size, YUVData **out_yuvData);
};


#endif /*H_VIDEO_DECODER*/