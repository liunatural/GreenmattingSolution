#include "VideoClient.h"
#include "zmq.h"
#include "VideoDecoder.h"

VideoClient::VideoClient()
{
	decoder = new VideoDecoder();
	bool r = decoder->init();
	if (!r) {
		return;  
	}
}


VideoClient::~VideoClient()
{
	if (decoder)
	{
		delete decoder;
		decoder = NULL;
	}
}


bool VideoClient::connect(string ip, int port)
{
	void * ctx_subscriber = zmq_ctx_new();
	if (!ctx_subscriber)
	{
		return false;
	}
	sock_subscriber = zmq_socket(ctx_subscriber, ZMQ_SUB);
	if (!sock_subscriber)
	{
		return false;
	}
	int queue_length = 1;
	zmq_setsockopt(sock_subscriber, ZMQ_RCVHWM, &queue_length, sizeof(queue_length));

	//const char *identityString = "VideoClient_127.0.0.1";
	//zmq_setsockopt(sock_subscriber, ZMQ_IDENTITY, identityString, strlen(identityString));

	int iRcvTimeout = 3000;//设置zmq的接收超时时间为3秒 
	zmq_setsockopt(sock_subscriber, ZMQ_RCVTIMEO, &iRcvTimeout, sizeof(iRcvTimeout));

	int iLingerTime = 100;//socket关闭设置停留时间 
	zmq_setsockopt(sock_subscriber, ZMQ_LINGER, &iLingerTime, sizeof(iLingerTime));


	url = "tcp://" + ip + ":" + to_string(port);

	int ret = zmq_connect(sock_subscriber, url.c_str());
	if (ret == -1)
	{
		return false;
	}
	zmq_setsockopt(sock_subscriber, ZMQ_SUBSCRIBE, "", 0);

	return true;
}


bool VideoClient::close()
{
	bool ret = false;
	int iRet = -1;
	
	if (sock_subscriber)
	{
		iRet = zmq_close(sock_subscriber);
	}
	assert(iRet == 0);
	if (iRet == 0)
	{
		ret = true;
	}
		
	return ret;

}

AVFrame* VideoClient::getFrame()
{
	zmq_msg_t message;
	zmq_msg_init(&message);
	int size = zmq_msg_recv(&message, sock_subscriber, 0);
	if (size == -1) {
		return NULL;
	}

	char *data = (char*)malloc(size + 1);
	memcpy(data, zmq_msg_data(&message), size);
	data[size] = 0;

	AVFrame *frame = decoder->decode(data, size);

	free(data);
	zmq_msg_close(&message);

	return frame;
}


void VideoClient::unrefFrame(AVFrame *frame)
{
	if (!frame)
		return;
	av_frame_unref(frame);
}
