#include <iostream>
#include <algorithm>
#include <fstream>
#include <new>
#include <cmath>

#include "color_table.h"
#include "convert_color.h"

ColorTable::ColorTable() : color_table_size(0x01000000), table(NULL)
{
	try {
		table = new unsigned short[color_table_size];
		this->resetColor();
	} catch(std::bad_alloc &ba) {
		std::cerr << "memory allocation error in Vision/ColorTable\n" << ba.what() << std::endl;
		throw 0;
	}
}

ColorTable::~ColorTable()
{
	delete[] table;
}

inline int ColorTable::getIndex(const int y, const int cb, const int cr)
{
	return cr + cb * 255 + y * 255 * 255;
}

void ColorTable::loadColorTable(std::string filename)
{
	std::ifstream ifs(filename, std::ifstream::binary);
	if(!ifs) {
		std::cerr << "Failed open color table file" << std::endl;;
		throw 0;
	}
	ifs.read(reinterpret_cast<char *>(table), sizeof(unsigned short) * color_table_size);
}

void ColorTable::saveColorTable(std::string filename)
{
	std::ofstream ofs(filename, std::ofstream::binary);
	if(!ofs) {
		std::cerr << "Failed open color table file" << std::endl;;
		throw 0;
	}
	ofs.write(reinterpret_cast<char *>(table), sizeof(unsigned short) * color_table_size);
}

void ColorTable::setColor(int y, int cb, int cr, int flag, int margin)
{
	const unsigned short object_type = 1 << flag;
	int h, s, v;
	cvtcolor::ycbcr2hsv(y, cb, cr, h, s, v);
	const int s_start = std::max<int>(s - margin,   0);
	const int s_end   = std::min<int>(s + margin, 255);
	const int v_start = std::max<int>(v - margin,   0);
	const int v_end   = std::min<int>(v + margin, 255);
	for(int si = s_start; si < s_end + 1; si++) {
		for(int vi = v_start; vi < v_end + 1; vi++) {
			int yi, cbi, cri;
			cvtcolor::hsv2ycbcr(h, si, vi, yi, cbi, cri);
			table[getIndex(yi, cbi, cri)] |= object_type;
		}
	}
	table[getIndex(y, cb, cr)] |= object_type;
}

void ColorTable::clearColor(int y, int cb, int cr, int flag, int margin)
{
	const unsigned short object_type = 1 << flag;
	int h, s, v;
	cvtcolor::ycbcr2hsv(y, cb, cr, h, s, v);
	const int s_start = std::max<int>(s - margin,   0);
	const int s_end   = std::min<int>(s + margin, 255);
	const int v_start = std::max<int>(v - margin,   0);
	const int v_end   = std::min<int>(v + margin, 255);
	for(int si = s_start; si < s_end + 1; si++) {
		for(int vi = v_start; vi < v_end + 1; vi++) {
			int yi, cbi, cri;
			cvtcolor::hsv2ycbcr(h, si, vi, yi, cbi, cri);
			table[getIndex(yi, cbi, cri)] &= ~object_type;
		}
	}
	table[getIndex(y, cb, cr)] &= ~object_type;
}

void ColorTable::resetColor(int flag)
{
	const unsigned short object_type = 1 << flag;
	for(int i = 0; i < color_table_size; i++) {
		table[i] &= ~object_type;
	}
}

void ColorTable::resetColor(void)
{
	for(int i = 0; i < color_table_size; i++) {
		table[i] = 0;
	}
}

bool ColorTable::checkColor(int y, int cb, int cr, int flag)
{
	// unused?
	return false;
}

inline unsigned short ColorTable::getLabel(const int y, const int cb, const int cr)
{
	return table[getIndex(y, cb, cr)];
}

void ColorTable::apply(cv::Mat &img, cv::Mat &label_img)
{
	if(img.rows != label_img.rows || img.cols != label_img.cols) {
		throw 0;
	}
	const int width = img.cols;
	const int height = img.rows;
	for(int h = 0; h < height; h++) {
		for(int w = 0; w < width; w++) {
			/* OpenCV image: YCrCb */
			/* ColorTable: YCbCr */
			const int y  = img.data[(h * width + w) * img.channels() + 0];
			const int cr = img.data[(h * width + w) * img.channels() + 1];
			const int cb = img.data[(h * width + w) * img.channels() + 2];
			label_img.data[(h * width + w) * label_img.channels()] = getLabel(y, cb, cr);
		}
	}
}

