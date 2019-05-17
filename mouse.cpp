
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

struct mouseParam {
	int x;
	int y;
	int event;
	int flags;
};

void callBackFunc(int event_type, int x, int y, int flags, void* userdata)
{
	mouseParam *ptr = static_cast<mouseParam *>(userdata);
	ptr->x = x;
	ptr->y = y;
	ptr->event = event_type;
	ptr->flags = flags;
}

int main(int argc, char *argv[])
{
	mouseParam mouseEvent;
	cv::Mat img;
	img = cv::imread("test1.jpg");
	cv::imshow("captured image", img);

	cv::setMouseCallback("captured image", callBackFunc, &mouseEvent);
	while(true) {
		cv::waitKey(20);
		if(mouseEvent.event == cv::EVENT_LBUTTONDOWN) {
			mouseEvent.x;
		} else if(mouseEvent.event == cv::EVENT_RBUTTONDOWN) {
			mouseEvent.y;
		} else if(mouseEvent.event == cv::EVENT_MBUTTONDOWN) {
			break;
		}
	}

	return 0;
}

