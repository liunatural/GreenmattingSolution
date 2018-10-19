#pragma once

#ifdef HXSTREAMNETCLIENT_EXPORTS
#define HXSTREAMNETCLIENT_API __declspec(dllexport)
#else
#define HXSTREAMNETCLIENT_API __declspec(dllimport)
#endif



class HXStreamNetClient
{
public:
	HXStreamNetClient() {};
	virtual ~HXStreamNetClient() {};
	//������������
	virtual bool Connect(char* ip, int port) = 0;
	//�ر�����������������
	virtual bool Close() = 0;
	//��ȡ������
	virtual bool GetStreamData(unsigned char **data, int& width, int& height, int& size) = 0;
};

//������Ƶ������ͻ��˶���
HXSTREAMNETCLIENT_API HXStreamNetClient*  CreateVideoStreamNetClientService();