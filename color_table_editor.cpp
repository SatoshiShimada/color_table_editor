
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "color_table.h"
#include "object_types.h"
#include "detector_tool.h"

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

void visualize(cv::Mat &label_img, cv::Mat &v_img, cv::Mat &edge_img)
{
	std::vector<int> field_edge(label_img.cols, label_img.rows);
	DetectorTool::detectFieldEdge(label_img, field_edge);
	DetectorTool::showFieldEdge(label_img, field_edge);
	std::vector<std::pair<int, int>> edge_lines = DetectorTool::detectLineOfFieldEdge(field_edge);
	DetectorTool::showLineOfFieldEdge(label_img, edge_lines);
	for(auto l : edge_lines) {
		cv::line(edge_img, cv::Point(l.first, field_edge[l.first]), cv::Point(l.second, field_edge[l.second]), cv::Scalar(0, 0, 255));
	}

	const unsigned short label_green = 1 << COLOR_GREEN;
	const unsigned short label_white = 1 << COLOR_WHITE;
	const unsigned short label_black = 1 << COLOR_BLACK;
	const unsigned short label_yellow = 1 << COLOR_YELLOW;
	const unsigned short label_blue = 1 << COLOR_BLUE;

	for(int y = 0; y < label_img.rows; y++) {
		for(int x = 0; x < label_img.cols; x++) {
			const unsigned short label = label_img.data[y * label_img.cols + x];
			v_img.data[(y * v_img.cols + x) * 3 + 0] = 0;
			v_img.data[(y * v_img.cols + x) * 3 + 1] = 0;
			v_img.data[(y * v_img.cols + x) * 3 + 2] = 0;
			if(label & label_green) {
				v_img.data[(y * v_img.cols + x) * 3 + 1] = 255;
			}
			if(label & label_white) {
				v_img.data[(y * v_img.cols + x) * 3 + 0] = 255;
				v_img.data[(y * v_img.cols + x) * 3 + 1] = 255;
				v_img.data[(y * v_img.cols + x) * 3 + 2] = 255;
			}
			if(label & label_black) {
				v_img.data[(y * v_img.cols + x) * 3 + 0] = 255;
			}
			if(label & label_blue) {
				v_img.data[(y * v_img.cols + x) * 3 + 0] = 0;
				v_img.data[(y * v_img.cols + x) * 3 + 1] = 0;
				v_img.data[(y * v_img.cols + x) * 3 + 2] = 0;
			}
		}
	}
	for(int y = 0; y < label_img.rows; y++) {
		for(int x = 0; x < label_img.cols; x++) {
			const unsigned short label = label_img.data[y * label_img.cols + x];
			if(label & label_yellow) {
				v_img.data[(y * v_img.cols + x) * 3 + 0] = 0;
				v_img.data[(y * v_img.cols + x) * 3 + 1] = 0;
				v_img.data[(y * v_img.cols + x) * 3 + 2] = 255;
			}
		}
	}

	cv::Mat brightness;
	cv::cvtColor(edge_img, brightness, CV_BGR2YCrCb);
	for(int y = 1; y < edge_img.rows - 1; y++) {
		for(int x = 0; x < edge_img.cols; x++) {
			edge_img.data[(y * edge_img.cols + x) * 3 + 0] = (brightness.data[((y - 1) * brightness.cols + x) * 3 + 0] - brightness.data[((y + 1) * brightness.cols + x) * 3 + 0]) / 2 + 128;
			edge_img.data[(y * edge_img.cols + x) * 3 + 1] = 128;
			edge_img.data[(y * edge_img.cols + x) * 3 + 2] = 128;
		}
	}
	cv::Mat tmp_img;
	cv::cvtColor(edge_img, tmp_img, CV_YCrCb2BGR);
	edge_img = tmp_img;
}

void loadImageFileList(std::string filename, std::vector<std::string> &list)
{
	std::ifstream fin(filename.c_str());
	if(!fin) {
		return;
	}

	list.clear();
	std::string line;
	while(std::getline(fin, line)) {
		if(line.size() > 0) {
			list.push_back(line);
		}
	}
}

int main(int argc, char *argv[])
{
	std::cout <<
		"+-------------------------+" << std::endl <<
		"Usage:" << std::endl <<
		"l: load color table" << std::endl <<
		"s: save color tablel" << std::endl <<
		"r: reset color table" << std::endl <<
		"n: next image" << std::endl <<
		"p: previous image" << std::endl <<
		"0-9: set color margin" << std::endl <<
		"g: select label green" << std::endl <<
		"w: select label white" << std::endl <<
		"b: select label black" << std::endl <<
		"ESC: exit" << std::endl <<
		"+-------------------------+" << std::endl <<
		std::endl;

	std::vector<std::string> img_names;
	loadImageFileList(std::string("image.list"), img_names);

	mouseParam mouseEvent;

	cv::Mat img;
	img = cv::imread(img_names[0].c_str());
	cv::Mat label_img(img.rows, img.cols, CV_16UC1);
	cv::Mat vis_img(img.rows, img.cols, CV_8UC3);
	cv::Mat edge_img(img.rows, img.cols, CV_8UC3);
	edge_img = img;
	cv::Mat ycrcb_img;
	cv::cvtColor(img, ycrcb_img, CV_BGR2YCrCb);
	cv::imshow("captured image", img);
	cv::imshow("result image", vis_img);
	cv::imshow("edge image", edge_img);

	ColorTable color_table;

	cv::setMouseCallback("captured image", callBackFunc, &mouseEvent);
	bool mouse_l_clicked = false;
	bool mouse_r_clicked = false;
	unsigned short label = COLOR_GREEN;
	int margin = 1;
	int img_index = 0;
	while(true) {
		const char pressed_key = static_cast<char>(cv::waitKey(20));
		if(pressed_key == 'n') {
			// next image
			if(++img_index >= img_names.size())
				img_index = 0;
			img = cv::imread(img_names[img_index].c_str());
			edge_img = cv::imread(img_names[img_index].c_str());
			label_img = cv::Mat(img.rows, img.cols, CV_16UC1);
			vis_img = cv::Mat(img.rows, img.cols, CV_8UC3);
			cv::cvtColor(img, ycrcb_img, CV_BGR2YCrCb);
			color_table.apply(ycrcb_img, label_img);
			visualize(label_img, vis_img, edge_img);
			cv::imshow("captured image", img);
			cv::imshow("result image", vis_img);
			cv::imshow("edge image", edge_img);
			std::cout << "loaded image: " << img_names[img_index] << std::endl;
		} else if(pressed_key == 'p') {
			// previous image
			if(--img_index < 0)
				img_index = img_names.size() - 1;
			img = cv::imread(img_names[img_index].c_str());
			edge_img = cv::imread(img_names[img_index].c_str());
			label_img = cv::Mat(img.rows, img.cols, CV_16UC1);
			vis_img = cv::Mat(img.rows, img.cols, CV_8UC3);
			cv::cvtColor(img, ycrcb_img, CV_BGR2YCrCb);
			color_table.apply(ycrcb_img, label_img);
			visualize(label_img, vis_img, edge_img);
			cv::imshow("captured image", img);
			cv::imshow("result image", vis_img);
			cv::imshow("edge image", edge_img);
			std::cout << "loaded image: " << img_names[img_index] << std::endl;
		} else if(pressed_key == 'l') {
			// load
			color_table.loadColorTable(std::string("color_table.cnf"));
			color_table.apply(ycrcb_img, label_img);
			visualize(label_img, vis_img, edge_img);
			cv::imshow("result image", vis_img);
			cv::imshow("edge image", edge_img);
			std::cout << "color table loaded" << std::endl;
		} else if(pressed_key == 's') {
			// save
			color_table.saveColorTable(std::string("color_table.cnf"));
			std::cout << "color table saved" << std::endl;
		} else if(pressed_key == 'r') {
			color_table.resetColor();
			color_table.apply(ycrcb_img, label_img);
			visualize(label_img, vis_img, edge_img);
			cv::imshow("result image", vis_img);
			cv::imshow("edge image", edge_img);
			std::cout << "reset color table" << std::endl;
		} else if(pressed_key == 'g') {
			// label green
			label = COLOR_GREEN;
			std::cout << "color green is selected" << std::endl;
		} else if(pressed_key == 'w') {
			// label white
			label = COLOR_WHITE;
			std::cout << "color white is selected" << std::endl;
		} else if(pressed_key == 'b') {
			// black
			label = COLOR_BLACK;
			std::cout << "color black is selected" << std::endl;
		} else if(pressed_key >= '0' && pressed_key <= '9') {
			// set margin
			margin = pressed_key - '0';
			std::cout << "color margin is " << margin << std::endl;
		} else if(pressed_key == 27) {
			// ESC is exit
			break;
		}
		if(mouseEvent.event == cv::EVENT_LBUTTONDOWN) {
			mouse_l_clicked = true;
			const int mx = mouseEvent.x;
			const int my = mouseEvent.y;
			unsigned char y = ycrcb_img.data[(my * ycrcb_img.cols + mx) * 3 + 0];
			unsigned char cr = ycrcb_img.data[(my * ycrcb_img.cols + mx) * 3 + 1];
			unsigned char cb = ycrcb_img.data[(my * ycrcb_img.cols + mx) * 3 + 2];
			color_table.setColor(y, cb, cr, label, margin);
			color_table.apply(ycrcb_img, label_img);
			visualize(label_img, vis_img, edge_img);
			cv::imshow("result image", vis_img);
			cv::imshow("edge image", edge_img);
		} else if(mouseEvent.event == cv::EVENT_MOUSEMOVE) {
			if(mouse_l_clicked) {
				const int mx = mouseEvent.x;
				const int my = mouseEvent.y;
				unsigned char y = ycrcb_img.data[(my * ycrcb_img.cols + mx) * 3 + 0];
				unsigned char cr = ycrcb_img.data[(my * ycrcb_img.cols + mx) * 3 + 1];
				unsigned char cb = ycrcb_img.data[(my * ycrcb_img.cols + mx) * 3 + 2];
				color_table.setColor(y, cb, cr, label, margin);
				color_table.apply(ycrcb_img, label_img);
				visualize(label_img, vis_img, edge_img);
				cv::imshow("result image", vis_img);
				cv::imshow("edge image", edge_img);
			} else if(mouse_r_clicked) {
				const int mx = mouseEvent.x;
				const int my = mouseEvent.y;
				unsigned char y = ycrcb_img.data[(my * ycrcb_img.cols + mx) * 3 + 0];
				unsigned char cr = ycrcb_img.data[(my * ycrcb_img.cols + mx) * 3 + 1];
				unsigned char cb = ycrcb_img.data[(my * ycrcb_img.cols + mx) * 3 + 2];
				color_table.clearColor(y, cb, cr, label, margin);
				color_table.apply(ycrcb_img, label_img);
				visualize(label_img, vis_img, edge_img);
				cv::imshow("result image", vis_img);
				cv::imshow("edge image", edge_img);
			}
		} else if(mouseEvent.event == cv::EVENT_LBUTTONUP) {
			mouse_l_clicked = false;
		} else if(mouseEvent.event == cv::EVENT_RBUTTONDOWN) {
			mouse_r_clicked = true;
			const int mx = mouseEvent.x;
			const int my = mouseEvent.y;
			unsigned char y = ycrcb_img.data[(my * ycrcb_img.cols + mx) * 3 + 0];
			unsigned char cr = ycrcb_img.data[(my * ycrcb_img.cols + mx) * 3 + 1];
			unsigned char cb = ycrcb_img.data[(my * ycrcb_img.cols + mx) * 3 + 2];
			color_table.clearColor(y, cb, cr, label, margin);
			color_table.apply(ycrcb_img, label_img);
			visualize(label_img, vis_img, edge_img);
			cv::imshow("result image", vis_img);
			cv::imshow("edge image", edge_img);
		} else if(mouseEvent.event == cv::EVENT_RBUTTONUP) {
			mouse_r_clicked = false;
		} else if(mouseEvent.event == cv::EVENT_MBUTTONDOWN) {
		}
	}

	return 0;
}

