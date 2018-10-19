#include "VideoDecoder.h"



VideoDecoder::VideoDecoder()
{
	pFrame = av_frame_alloc();
	pPacket = av_packet_alloc();
	pFormatCtx = avformat_alloc_context();
}


VideoDecoder::~VideoDecoder()
{

}


bool VideoDecoder::init()
{
	pCodec = avcodec_find_decoder(AV_CODEC_ID_HEVC);
	pCodecCtx = avcodec_alloc_context3(pCodec);

	pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
	pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
		printf("Failed to open encoder! \n");
		return false;
	}

	parser = av_parser_init(pCodec->id);

	return true;
}


AVFrame* VideoDecoder::decode(AVPacket *pPacket)
{
	int ret;
	ret = avcodec_send_packet(pCodecCtx, pPacket);
	if (ret < 0) {
		if (AVERROR(EINVAL) == ret)
			printf("error EINVAL\n");

		fprintf(stderr, "Error submitting the packet to the decoder\n");
		return false;
	}

	ret = avcodec_receive_frame(pCodecCtx, pFrame);
	if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
		return false;
	else if (ret < 0) {
		fprintf(stderr, "Error during decoding\n");
		return false;
	}

	//av_frame_unref(pFrame);
	return pFrame;
}


AVFrame* VideoDecoder::decode(char *in_data, int in_size)
{
	pPacket->data = (uint8_t*)in_data;
	pPacket->size = in_size;

	AVFrame *frame = decode(pPacket);
	if (NULL == frame)
	{
		return NULL;
	}

	return frame;
}

