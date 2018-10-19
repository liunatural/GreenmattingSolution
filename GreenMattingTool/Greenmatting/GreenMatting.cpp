#pragma once
#include <windows.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>// Image processing methods for the CPU
#include <opencv2/imgcodecs.hpp>// Read images

// CUDA structures and methods
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudafilters.hpp>
#include "FrameQueue.h"

using namespace std;
struct BufferGPU		// Optimized CUDA versions
{
	cv::cuda::GpuMat frame, hsv, mask;
};

char * resultWin				= "�ϳ�Ч��ͼ";
bool GPU_Accelerate		= false;
bool stopRunning			= false;
BufferGPU						b;
FrameQueueAB				frameQueueAB;
cv::Mat							background;

cv::Mat replace_and_blend(cv::Mat &frame, cv::Mat &mask);
bool MakeGreenMatting(cv::Mat &source, cv::Mat &result);
bool MakeGreenMatting_GPU(cv::Mat &source, cv::Mat &result);

DWORD WINAPI DoGreenMatting(LPVOID lpParameter) 
{
	cv::Mat source, result;
	double timeConsum = 0;
	string msg = "Do green matting with GPU:";
	if (!GPU_Accelerate) msg = "Make green matting with CPU:";

	while (!stopRunning)
	{
		FrameQueue frameQueue = frameQueueAB.Swap();
		int count = frameQueue.GetCount();
		for (int i = 0; i < count; i++)
		{
			source = *frameQueue.GetFrame(i);
			timeConsum = (double)cv::getTickCount();
			if (GPU_Accelerate)
			{
				MakeGreenMatting_GPU(source, result);
			}
			else
			{
				MakeGreenMatting(source, result);
			}
			timeConsum = 1000 * ((double)cv::getTickCount() - timeConsum) / cv::getTickFrequency();
			cout << msg  << timeConsum << " milliseconds." << endl;

			imshow(resultWin, result);
		}
		
		char c = cv::waitKey(1);
		if (c == 27)//����ESC�˳�
		{
			stopRunning = true;
			break;
		}
	}
	
	return 0;
}

static void help()
{
	cout
		<< "\n--------------------------------------------------------------------------" << endl
		<< "Usage:" << endl
		<< "GreenMatting  <background image file Path>  <avi file path>" << endl
		<< "--------------------------------------------------------------------------" << endl
		<< endl;
}


int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		help();
		return -1;
	}

	cv::cuda::DeviceInfo _deviceInfo;
	GPU_Accelerate = _deviceInfo.isCompatible();
	if (!GPU_Accelerate)
	{
		std::cout << "Display device on this machine does *NOT* support GPU programing." << std::endl;
	}

	char* bgImagePath = argv[1];
	char* videoPath = argv[2];

	background = cv::imread(argv[1]);
	if (!background.data)
	{
		std::cout << "Could not open background file:" << bgImagePath << std::endl;
		return -1;
	}

	//������Ƶ
	cv::VideoCapture capture;
	capture.open(argv[2]);
	if (!capture.isOpened())
	{
		std::cout << "Could not open video file:" << videoPath << std::endl;
		return -1;
	}


	double bgWidth = background.cols;
	double bgHeight = background.rows;
	double videoWidth = capture.get(cv::CAP_PROP_FRAME_WIDTH);
	double videoHeight = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
	std::cout << "Background width:" << bgWidth << std::endl;
	std::cout << "Background height:" << bgHeight << std::endl;
	std::cout << "          Video width:" << videoWidth << std::endl;
	std::cout << "         Video height:" << videoHeight << std::endl;

	if (bgWidth < videoWidth || bgHeight < videoHeight)
	{
		cv::resize(background, background, cv::Size(videoWidth, videoHeight));
	}


	HANDLE hThread = 0; //�����߳̾��
	DWORD threadID = 0;
	hThread = CreateThread(NULL, 0, DoGreenMatting, NULL, 0, &threadID);

	if (hThread == NULL)
		return false;

	cv::namedWindow(resultWin, CV_WINDOW_AUTOSIZE);

	cv::Mat frame;//һ֡ͼ��
	while (!stopRunning && capture.read(frame))
	{
		
		frameQueueAB.Push(frame);
		char c = cv::waitKey(30);//��ʱ30ms
		if (c == 27)//����ESC�˳�
		{
			stopRunning = true;
			break;
		}
	}

	stopRunning = true;

	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);

	return 0;

}


bool MakeGreenMatting(cv::Mat &source, cv::Mat &result)
{
	cv::Mat hsv, mask;

	//��ÿһ֡��ͼ��ת����hsv�ռ�
	cv::cvtColor(source, hsv, cv::COLOR_BGR2HSV);

	//��Ļ����ɫ��Χ�����������mask��
	cv::inRange(hsv, cv::Scalar(35, 43, 46), cv::Scalar(155, 255, 255), mask);

	//��mask������̬ѧ����
	//����һ���ṹ
	cv::Mat k = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));

	//��mask������̬ѧ�ղ���
	cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, k);

	cv::erode(mask, mask, k);

	//��˹ģ��
	cv::GaussianBlur(mask, mask, cv::Size(3, 3), 0, 0);

	result = replace_and_blend(source, mask);

	return true;
}


bool MakeGreenMatting_GPU(cv::Mat &source, cv::Mat &result)
{

	b.frame.upload(source);

	//��ÿһ֡��ͼ��ת����hsv�ռ�
	cv::cuda::cvtColor(b.frame, b.hsv, cv::COLOR_BGR2HSV);

	cv::Mat hsv(b.hsv);
	cv::Mat mask;
	//��Ļ����ɫ��Χ�����������mask��
	//cv::inRange(hsv, cv::Scalar(35, 43, 46), cv::Scalar(155, 255, 255), mask);
	cv::inRange(hsv, cv::Scalar(35, 43,46), cv::Scalar(95, 255, 255), mask);
	b.hsv.upload(hsv);
	b.mask.upload(mask);
	//��mask������̬ѧ����
	//����һ���ṹ
	cv::Mat k = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));

	//��mask������̬ѧ�ղ���
	cv::Ptr<cv::cuda::Filter> closeFilter = cv::cuda::createMorphologyFilter(cv::MORPH_CLOSE, b.mask.type(), k);
	closeFilter->apply(b.mask, b.mask);

	//��mask���и�ʴ����
	cv::Ptr<cv::cuda::Filter> erodeFilter = cv::cuda::createMorphologyFilter(cv::MORPH_ERODE, b.mask.type(), k);
	erodeFilter->apply(b.mask, b.mask);


	//��˹ģ��
	cv::Ptr<cv::cuda::Filter> gaussFilter = cv::cuda::createGaussianFilter(b.mask.type(), b.mask.type(), cv::Size(3, 3), 0);
	gaussFilter->apply(b.mask, b.mask);


	cv::Mat frame(b.frame);
	b.mask.download(mask);
	result = replace_and_blend(frame, mask);


	return true;
}

//����Ƶ��ÿһ֡��ͼ����д���
cv::Mat replace_and_blend(cv::Mat &frame, cv::Mat &mask)

{
	//����һ�Ž��ͼ
	cv::Mat result = cv::Mat(frame.size(), frame.type());
	//ͼ��ĸ� �� ��ͨ����
	int height = result.rows;
	int width = result.cols;
	int channels = result.channels();

	// replace and blend
	int m = 0;//mask������ֵ
	double wt = 0;//�ںϵı���
	int r = 0, g = 0, b = 0;//���������
	int r1 = 0, g1 = 0, b1 = 0;
	int r2 = 0, g2 = 0, b2 = 0;
	for (int i = 0; i < height; i++)

	{
		//����ÿһ�� ÿһ֡ͼ���ָ�룬maskͼ���ָ�룬���ű���ͼ��ָ��,���ͼ��ָ��
		uchar *pbg = background.ptr<uchar>(i);
		uchar *pframe = frame.ptr<uchar>(i);
		uchar *pmask = mask.ptr<uchar>(i);
		uchar *presult = result.ptr<uchar>(i);

		for (int j = 0; j < width; j++)
		{
			m = *pmask++;//��ȡmask������ֵ
			if (m == 255)//����Ǳ����Ļ�
			{
				//��������ͨ���ĸ�ֵ
				*presult++ = *pbg++;
				*presult++ = *pbg++;
				*presult++ = *pbg++;
				pframe += 3;//��frame��ͼ������ص�ͨ��Ҳ�ƶ���������һ��
			}
			else if (m == 0)//�����ǰ���Ļ�
			{

				//��������ͨ���ĸ�ֵ
				*presult++ = *pframe++;
				*presult++ = *pframe++;
				*presult++ = *pframe++;
				pbg += 3;//��frame��ͼ������ص�ͨ��Ҳ�ƶ���������һ��

			}
			else
			{
				//����ͼÿ�����ص�����ͨ��
				b1 = *pbg++;
				g1 = *pbg++;
				r1 = *pbg++;

				//ÿһ֡ÿһ�����ص�����ͨ��
				b2 = *pframe++;
				g2 = *pframe++;
				r2 = *pframe++;

				// Ȩ��
				wt = m / 255.0;

				// ���
				b = b1*wt + b2*(1.0 - wt);
				g = g1*wt + g2*(1.0 - wt);
				r = r1*wt + r2*(1.0 - wt);

				*presult++ = b;
				*presult++ = g;
				*presult++ = r;
			}
		}
	}
	return result;

}
