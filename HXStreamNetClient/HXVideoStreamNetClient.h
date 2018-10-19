#pragma once
#include "HXStreamNetClient.h"
#include <vector>
extern "C"
{
	#include "libswscale/swscale.h"
}

class VideoClient;

class HXVideoStreamNetClient :	public HXStreamNetClient
{
public:
	HXVideoStreamNetClient();
	virtual ~HXVideoStreamNetClient();

	virtual bool Connect(char* ip, int port) ;
	virtual bool Close();
	virtual bool GetStreamData(unsigned char **data, int& width, int& height, int& size);

private:
	int			m_width;
	int			m_height;

	VideoClient  *m_pVideoStreamClient;
	std::vector<uint8_t>	m_buffers;
	SwsContext	*m_swsContext;
};




