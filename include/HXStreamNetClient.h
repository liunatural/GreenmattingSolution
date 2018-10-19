#pragma once

#ifdef HXSTREAMNETCLIENT_EXPORTS
#define HXSTREAMNETCLIENT_API __declspec(dllexport)
#else
#define HXSTREAMNETCLIENT_API __declspec(dllimport)
#endif

//封装zmq网络客户端，用户连接绿幕服务器，接收视频流

class HXStreamNetClient
{
public:
	HXStreamNetClient() {};
	virtual ~HXStreamNetClient() {};
	//连接流服务器
	virtual bool Connect(char* ip, int port) = 0;
	//关闭与流服务器的连接
	virtual bool Close() = 0;
	//获取流数据
	virtual bool GetStreamData(unsigned char **data, int& width, int& height, int& size) = 0;
};

//创建视频流服务客户端对象
HXSTREAMNETCLIENT_API HXStreamNetClient*  CreateVideoStreamNetClientService();