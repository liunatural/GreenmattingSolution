#include "HXVideoStreamNetClient.h"
#include "VideoClient.h"

HXVideoStreamNetClient::HXVideoStreamNetClient()
{
	m_pVideoStreamClient = new VideoClient();

	m_swsContext = NULL;
	m_width = 0;
	m_width = 0;

}

HXVideoStreamNetClient::~HXVideoStreamNetClient()
{
	if (m_pVideoStreamClient)
	{
		delete m_pVideoStreamClient;
	}
	
	if (m_swsContext)
	{
		sws_freeContext(m_swsContext);
	}
}

bool HXVideoStreamNetClient::Connect(char* ip, int port)
{
	bool ret =  m_pVideoStreamClient->connect(ip, port);
	return ret;
}

bool HXVideoStreamNetClient::Close()
{
	return m_pVideoStreamClient->close();
}

bool HXVideoStreamNetClient::GetStreamData(unsigned char **data, int& width, int& height, int& size)
{
	AVFrame* pFrame = m_pVideoStreamClient->getFrame();

	if (pFrame)
	{
		if (m_width != pFrame->width)
		{
			m_width = pFrame->width;
		}

		if (m_height != pFrame->height)
		{
			m_height = pFrame->height;
		}

		height = m_height;
		width = m_width;
		size = m_width *m_height * 3;

		if (m_buffers.size() != size)
		{
			m_buffers.resize(size);
		}

		//m_buffers.clear();
		
		if (NULL == m_swsContext)
		{
			//YUV色彩空间转换成RGB色彩空间
			m_swsContext = sws_getContext(m_width, m_height, AV_PIX_FMT_YUV420P, 
				m_width, m_height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);
		}

		if (NULL != m_swsContext)
		{
			int out_stride[4] = { 0 };
			out_stride[0] = m_width * 3;

			uint8_t* dst[4];
			dst[0] = &m_buffers.at(0);
			int ret = sws_scale(m_swsContext, pFrame->data, pFrame->linesize, 0, m_height, dst, out_stride);

			*data = &m_buffers.at(0);

			m_pVideoStreamClient->unrefFrame(pFrame);

			return true;
		}

	}

	return false;
}
