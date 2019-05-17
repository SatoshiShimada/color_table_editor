
#include <iostream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "color_table.h"
#include "detector_tool.h"

int main(int argc, char *argv[])
{
	cv::Mat img;
	img = cv::imread("test1.jpg");
	cv::Mat label_img(img.rows, img.cols, CV_16UC1);
	ColorTable color_table;
	color_table.loadColorTable(std::string("color_table.cnf"));
	color_table.apply(img, label_img);
	cv::Mat visualized_img(img.rows, img.cols, CV_8UC3);

	const unsigned short label_green = (1 << COLOR_GREEN);
	const unsigned short label_white = (1 << COLOR_WHITE);
	const unsigned short label_yellow = (1 << COLOR_BLACK);
	for(int y = 0; y < label_img.rows; y++) {
		for(int x = 0; x < label_img.cols; x++) {
			if(label_img.data[y * label_img.cols + x] | label_white) {
				visualized_img.data[(y * visualized_img.cols + x) * 3 + 0] = 255;
			} else if(label_img.data[y * label_img.cols + x] | label_green) {
				visualized_img.data[(y * visualized_img.cols + x) * 3 + 1] = 255;
			} else if(label_img.data[y * label_img.cols + x] | label_yellow) {
				visualized_img.data[(y * visualized_img.cols + x) * 3 + 2] = 255;
			}
		}
	}
	cv::imwrite("out.png", visualized_img);
	return 0;
}

