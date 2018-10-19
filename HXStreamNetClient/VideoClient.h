#ifndef H_VIDEO_CLIENT
#define H_VIDEO_CLIENT

#include <stdlib.h>
#include <string>
using namespace std;

//#include "CLogger.h"
#include "VideoDecoder.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include <libavformat/avformat.h>

#ifdef __cplusplus
};
#endif



class VideoClient
{
public:
	VideoClient();
	~VideoClient();

	bool connect(string ip, int port);
	bool close();


	AVFrame* getFrame();
	void unrefFrame(AVFrame *frame);

private:
	//Logger *logger;

	void* sock_subscriber;
	VideoDecoder *decoder;

	string url;
};



#endif //H_VIDEO_CLIENT
