#include "FrameQueue.h"

using namespace cv;

FrameQueue::FrameQueue()
{
}

FrameQueue::~FrameQueue()
{
}

const Mat*	FrameQueue::GetFrame(unsigned int index)
{
	if (index >= m_queue.size())
		return 0;
	return &m_queue[index];
}


Mat*	FrameQueue::GetLastFrame()
{
	if ( m_queue.size() == 0)
		return 0;
	return &m_queue.back();
}


//返回消息包数量
int		FrameQueue::GetCount() 
{
	return (int)m_queue.size();
}
void	FrameQueue::RemoveFront()
{

	m_queue.pop_front();
}
void	FrameQueue::Add(const Mat& frame)
{
	m_queue.push_back(frame);
}
void	FrameQueue::Reset()
{
	m_queue.clear();
}



/////////////////////////////////////////////////////////////////////////////////

FrameQueueAB::FrameQueueAB()
{
	m_current = 0;
	m_queueMax = 3;
}

FrameQueueAB::~FrameQueueAB()
{

}

void FrameQueueAB::Push(const Mat& msg)
{
	std::unique_lock<std::mutex> lck(counter_mutex);

	m_queue[m_current].Add(msg);

	if (m_queue[m_current].GetCount() >= m_queueMax)
		m_queue[m_current].RemoveFront();
}

FrameQueue& FrameQueueAB::Swap()
{
	std::unique_lock<std::mutex> lck(counter_mutex);

	int old = m_current;
	m_current = (m_current + 1) % 2;
	m_queue[m_current].Reset();
	return m_queue[old];
}

FrameQueue& FrameQueueAB::Get()
{
	return m_current == 0 ? m_queue[1] : m_queue[0];
}



