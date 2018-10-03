#include "ImageManagerIns.h"

double red_l = 46;
double  red_h = 255;
double green_l = 43;
double green_h = 255;
double blue_l = 35;
double blue_h = 95;


/// Global Variables
char* trackWindowTitle = "������ɫ����";
char* RedLow = "Red Low";
char* RedHigh = "Red High";
char* GreenLow = "Green Low";
char* GreenHigh = "Green High";
char* BlueLow = "Blue Low";
char* BlueHigh = "Blue High";

void on_trackbar(int, void*)
{
	red_l = cv::getTrackbarPos(RedLow, trackWindowTitle);
	red_h = cv::getTrackbarPos(RedHigh, trackWindowTitle);

	green_l = cv::getTrackbarPos(GreenLow, trackWindowTitle);
	green_h = cv::getTrackbarPos(GreenHigh, trackWindowTitle);

	blue_l = cv::getTrackbarPos(BlueLow, trackWindowTitle);
	blue_h = cv::getTrackbarPos(BlueHigh, trackWindowTitle);

}




struct BufferGPU		// Optimized CUDA versions
{
	cv::cuda::GpuMat frame, hsv, mask, t1, t2;

	cv::cuda::GpuMat buf;
};

BufferGPU b;


ImageManagerIns::ImageManagerIns()
{
	m_capture = 0;

	cv::namedWindow(trackWindowTitle, 0);
	cv::createTrackbar(RedLow, trackWindowTitle, 0, 255, on_trackbar);
	cv::createTrackbar(RedHigh, trackWindowTitle, 0, 255, on_trackbar);

	cv::createTrackbar(GreenLow, trackWindowTitle, 0, 255, on_trackbar);
	cv::createTrackbar(GreenHigh, trackWindowTitle, 0, 255, on_trackbar);

	cv::createTrackbar(BlueLow, trackWindowTitle, 0, 255, on_trackbar);
	cv::createTrackbar(BlueHigh, trackWindowTitle, 0, 255, on_trackbar);

	cvSetTrackbarPos(RedLow, trackWindowTitle, 46);
	cvSetTrackbarPos(RedHigh, trackWindowTitle, 242);

	cvSetTrackbarPos(GreenLow, trackWindowTitle, 43);
	cvSetTrackbarPos(GreenHigh, trackWindowTitle, 255);

	cvSetTrackbarPos(BlueLow, trackWindowTitle, 35);
	cvSetTrackbarPos(BlueHigh, trackWindowTitle, 95);
}


ImageManagerIns::~ImageManagerIns()
{
	Destroy();
}


bool ImageManagerIns::OpenCamera(int imageWidth, int imageHeight, int deviceId)
{
	Destroy();
	m_capture = new cv::VideoCapture();
	if (!m_capture->open(0))
		return false;
	if (!m_capture->isOpened())
		return false;
	m_capture->set(cv::CAP_PROP_FRAME_WIDTH, imageWidth);
	m_capture->set(cv::CAP_PROP_FRAME_HEIGHT, imageHeight);
	m_videoWidth = m_capture->get(cv::CAP_PROP_FRAME_WIDTH);
	m_videoHeight = m_capture->get(cv::CAP_PROP_FRAME_HEIGHT);

	return true;

}

bool ImageManagerIns::GetPixels(void* result, int width, int height)
{
	int depth = m_image.elemSize1();
	cv::Size size = m_image.size();
	if (width > size.width)size.width = width;
	if (height > size.height)size.height = height;
	int channels = m_image.channels();
	if (depth == sizeof(float)) {
		for (int y = 0; y < size.height; y++) {
			for (int x = 0; x < size.width; x++) {
				float* rgb = m_image.ptr<float>(y, x);
				float* dst = (float*)result + (y*width + x);
				dst[0] = rgb[0];
			}
		}
	}
	else if (depth == sizeof(unsigned char)) {
		for (int x = 0; x < size.width; x++) {
			for (int y = 0; y < size.height; y++) {
				unsigned char* rgb = m_image.ptr<unsigned char>(y, x);
				unsigned char* dst = (unsigned char*)result + (y*width + x)*channels;
				for (int i = 0; i < channels; i++) {
					dst[i] = rgb[i];
				}
			}
		}
	}
	else if (depth == sizeof(unsigned short)) {
		for (int x = 0; x < size.width; x++) {
			for (int y = 0; y < size.height; y++) {
				unsigned short* rgb = m_image.ptr<unsigned short>(y, x);
				unsigned short* dst = (unsigned short*)result + (y*width + x)*channels;
				dst[0] = rgb[0];
				dst[1] = rgb[1];
			}
		}
	}
	else
		return false;

	return true;
}

bool ImageManagerIns::UpdateFromCamera(ImageManager* camera)
{
	if (!camera)
		return false;
	ImageManagerIns* s = static_cast<ImageManagerIns*>(camera);
	if (!s->m_capture || !s->m_capture->isOpened())
		return false;
	(*s->m_capture) >> m_image;

	return true;
}


bool ImageManagerIns::MakeGreenMatting(cv::Mat &source, cv::Mat &result)
{

	b.frame.upload(source);

	//��ÿһ֡��ͼ��ת����hsv�ռ�
	cv::cuda::cvtColor(b.frame, b.hsv, cv::COLOR_BGR2HSV);

	cv::Mat hsv(b.hsv);
	cv::Mat mask;
	//��Ļ����ɫ��Χ�����������mask��
	//cv::inRange(hsv, cv::Scalar(35, 43, 46), cv::Scalar(155, 255, 255), mask);
	//cv::inRange(hsv, cv::Scalar(35, 43,46), cv::Scalar(95, 255, 255), mask);
	cv::inRange(hsv, cv::Scalar(blue_l, green_l, red_l), cv::Scalar(blue_h, green_h, red_h), mask);
	b.hsv.upload(hsv);
	b.mask.upload(mask);
	//��mask������̬ѧ����
	//����һ���ṹ
	cv::Mat k = getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3), cv::Point(-1, -1));

	//��mask������̬ѧ�ղ���
	//cv::morphologyEx(b.mask, b.mask, cv::MORPH_CLOSE, k);
	cv::Ptr<cv::cuda::Filter> closeFilter = cv::cuda::createMorphologyFilter(cv::MORPH_CLOSE, b.mask.type(), k);
	closeFilter->apply(b.mask, b.mask);

	//��mask���и�ʴ����
	//cv::erode(b.mask, b.mask, k);
	cv::Ptr<cv::cuda::Filter> erodeFilter = cv::cuda::createMorphologyFilter(cv::MORPH_ERODE, b.mask.type(), k);
	erodeFilter->apply(b.mask, b.mask);


	//��˹ģ��
	//cv::GaussianBlur(b.mask, b.mask, cv::Size(3, 3), 0, 0);
	cv::Ptr<cv::cuda::Filter> gaussFilter = cv::cuda::createGaussianFilter(b.mask.type(), b.mask.type(), cv::Size(3, 3), 0);
	gaussFilter->apply(b.mask, b.mask);


	cv::Mat frame(b.frame);
	b.mask.download(mask);
	result = replace_and_blend(frame, mask);


	return true;
}


bool ImageManagerIns::MakeGreenMatting()
{
	return MakeGreenMatting(m_image, m_image);
}

ImageManager::float2 ImageManagerIns::ImageSize()
{
	if (m_image.empty())
	{
		return ImageManager::float2(0, 0);
	}

	return ImageManager::float2(m_image.size().width, m_image.size().height);

}


ImageManager::float2 ImageManagerIns::VideoImageSize()
{
	return ImageManager::float2(m_videoWidth, m_videoHeight);
}


void ImageManagerIns::Destroy()
{
	if (m_capture)
		m_capture->release();
	m_capture = 0;
	if (!m_image.empty())
		m_image.release();
}

//����Ƶ��ÿһ֡��ͼ����д���
cv::Mat ImageManagerIns::replace_and_blend(cv::Mat &frame, cv::Mat &mask)
{
	//����һ�Ž��ͼ
	cv::Mat result = cv::Mat(frame.size(), frame.type());

	//ͼ��ĸ߿���ͨ����
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
		uchar *pframe = frame.ptr<uchar>(i);			//֡ͼ��ָ��
		uchar *pmask = mask.ptr<uchar>(i);				// maskͼ��ָ��
		uchar *presult = result.ptr<uchar>(i);				// ���ͼָ��

		for (int j = 0; j < width; j++)
		{
			m = *pmask++;//��ȡmask������ֵ
			if (m == 0)//�����ǰ���Ļ�
			{
				//��������ͨ���ĸ�ֵ
				*presult++ = *pframe++;
				*presult++ = *pframe++;
				*presult++ = *pframe++;
			}
			else
			{
				//��������ͨ���ĸ�ֵ
				*presult++ = 0;
				*presult++ = 255;
				*presult++ = 0;

				pframe += 3;//��frame��ͼ������ص�ͨ��Ҳ�ƶ���������һ��

			}
		}
	}
	return result;
}
