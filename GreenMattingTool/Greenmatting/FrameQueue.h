#pragma once
#include <mutex>
#include<opencv2\opencv.hpp>

//using namespace cv;

class FrameQueue
{
public:
	FrameQueue();
	~FrameQueue();
	std::deque<cv::Mat> m_queue;

	//返回一个消息指针
	const cv::Mat*	GetFrame(unsigned int index);
	cv::Mat*	 GetLastFrame();
	int		GetCount();
	void	RemoveFront();
	void	Add(const cv::Mat& frame);
	void	Reset();

};


//多线程同步队列
class FrameQueueAB
{
public:
	FrameQueueAB();
	virtual ~FrameQueueAB();
	void	Push(const cv::Mat& msg);
	 FrameQueue&	Swap();
	 FrameQueue&	Get();

protected:
	FrameQueue	m_queue[2];
	int		m_current;
	int		m_queueMax;
	std::mutex counter_mutex;
};
