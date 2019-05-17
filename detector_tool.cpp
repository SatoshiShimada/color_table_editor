#include <algorithm>

#include "detector_tool.h"

static void medianFilter(std::vector<int> &field_edge)
{
	std::vector<int> edge(field_edge);
	const int width = field_edge.size();
	constexpr int filter_width = 3; // only odd number
	constexpr int half_filter_width = filter_width / 2;
	constexpr int median_index = filter_width / 2 + 1;
	std::vector<int> value(filter_width, 0);
	for(int x = half_filter_width; x < width - half_filter_width; x++) {
		for(int xi = 0; xi < filter_width; xi++)
			value[xi] = edge[x + xi - half_filter_width];
		std::sort(value.begin(), value.end());
		field_edge[x] = value[median_index];
	}
	field_edge[0] = field_edge[1];
	field_edge[width - 1] = field_edge[width - 2];
}

namespace DetectorTool {

void eraseWhiteLineOutsideField(cv::Mat &labeling_image, std::vector<int> field_edge,  const int exempt_green_count)
{
	const unsigned short label_white = (1 << COLOR_WHITE);
	const int w = labeling_image.cols;
	const int h = labeling_image.rows;
	constexpr int erase_margin = 10;
	for(int x = 0; x < w; x++) {
		const int edge_height = std::min(h, field_edge[x] + erase_margin);
		for(int y = 0; y < edge_height; y++) {
			labeling_image.data[y * w + x] &= ~label_white;
		}
	}
}

void eraseWhiteLineOutsideField(cv::Mat &labeling_image, const int exempt_green_count)
{
	const unsigned short label_green = (1 << COLOR_GREEN);
	const unsigned short label_white = (1 << COLOR_WHITE);
	const int w = labeling_image.cols;
	const int h = labeling_image.rows;
	for(int x = 0; x < w; x++) {
		int green_count = 0;
		for(int y = 0; y < h; y++) {
			auto &label = labeling_image.data[y * w + x];
			if(label & label_green) {
				green_count++;
				if(green_count > exempt_green_count)
					break;
			}
			label &= ~label_white;
		}
	}
}

void detectFieldEdge(cv::Mat &labeling_image, std::vector<int> &field_edge)
{
	const int width = labeling_image.cols;
	const int height = labeling_image.rows;
	const unsigned short label_green = (1 << COLOR_GREEN);
	const unsigned short label_white = (1 << COLOR_WHITE);// | (1 << COLOR_BALL);
	const unsigned short label_black = (1 << COLOR_BLACK);
	for(int x = 0; x < width; x++) {
		bool found_green = false;
		constexpr int min_value = 20;
		int max_value = min_value;
		int max_value_index = height - 1;
		int edge_score = 0;
		for(int y = height - 1; y >= 0; y--) {
			unsigned short label = labeling_image.data[y * width + x];
			if(!found_green) {
				if(label & label_green || label & label_black)
					found_green = true;
			} else {
				if(label & label_green)
					edge_score += y / (height / 4) + 1;
				else if(label & label_white)
					edge_score += +1;
				else
					edge_score += -2;
				if(edge_score > max_value) {
					max_value = edge_score;
					max_value_index = y;
				}
			}
		}
		field_edge[x] = max_value_index;
	}
	medianFilter(field_edge);
}

void detectObstacle(std::vector<int> &field_edge, std::vector<object_pos> &objects)
{
	const int width = field_edge.size();
	std::vector<int> obstacle_pixel(field_edge);

	// gift wrapping algorithm
	for(int x = 0; x < width - 1;) {
		double max_slope = -100.0;
		int max_slope_index = x + 1;
		int start_y = field_edge[x];
		for(int xi = x + 1; xi < width; xi++) {
			double slope = -static_cast<double>(field_edge[xi] - start_y) / (xi - x);
			if(slope >= max_slope) {
				max_slope = slope;
				max_slope_index = xi;
			}
		}
		for(int xi = x; xi < max_slope_index; xi++) {
			double new_field_edge = -max_slope * (xi - x) + start_y;
			obstacle_pixel[xi] = field_edge[xi] - new_field_edge;
			field_edge[xi] = new_field_edge;
		}
		x = max_slope_index;
	}
	constexpr int min_area = 40 * 8;
	constexpr int min_width = 5 * 8;
	constexpr int min_height = 6 * 8;
	int area = 0;
	int area_max_height = 0;
	int area_max_height_index = 0;
	int area_start_x = 0;
	for(int x = 0; x < width; x++) {
		const int obstacle_height = obstacle_pixel[x];
		if(obstacle_height > min_height) {
			area += obstacle_height;
			if(obstacle_height > area_max_height) {
				area_max_height = obstacle_height;
				area_max_height_index = x;
			}
		} else {
			if((area > min_area) && ((x - area_start_x) > min_width)) {
				const int x = area_max_height_index;
				const int y = field_edge[x] + obstacle_height;
				object_pos obj(x, y, OBJECT_ROBOT, OWNERSHIP_THEIRS); // Obstacles are enemy robots
				objects.push_back(obj);
			}
			area = 0;
			area_start_x = x;
			area_max_height = 0;
		}
	}
}

void showFieldEdge(cv::Mat &labeling_image, std::vector<int> &field_edge)
{
	const unsigned short color_yellow = 1 << COLOR_YELLOW;
	for(int x = 0; x < labeling_image.cols; x++) {
		const int margin = 2;
		const int edge_ym = std::max<int>(field_edge[x] - margin, 0);
		const int edge_yp = std::min<int>(field_edge[x] + margin, labeling_image.rows - 2);
		for(int edge_y = edge_ym; edge_y < edge_yp + 1; edge_y++) {
			labeling_image.data[edge_y * labeling_image.cols + x] |= color_yellow;
		}
	}
}

void detectWhiteLine(cv::Mat &labeling_image, std::vector<object_pos> &white_line)
{
	const unsigned short color_white = 1 << COLOR_WHITE;
	for(int h = 0; h < labeling_image.rows; h++) {
		for(int w = 0; w < labeling_image.cols; w++) {
			unsigned short label = labeling_image.data[h * labeling_image.cols + w];
			if(label & color_white) {
				object_pos pos(w, h);
				white_line.push_back(pos);
			}
		}
	}
}

};

