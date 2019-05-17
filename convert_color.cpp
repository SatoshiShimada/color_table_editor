#include <algorithm>
#include "convert_color.h"

/*
 * ref: http://opencv.jp/opencv-2.1/cpp/miscellaneous_image_transformations.html
 */
namespace cvtcolor {

void rgb2hsv(const int r, const int g, const int b, int &h, int &s, int &v)
{
	/*
	 * range
	 * h: 0 - 180
	 * s: 0 - 255
	 * v: 0 - 255
	 */
	const double double_r = r / 255.0;
	const double double_g = g / 255.0;
	const double double_b = b / 255.0;
	const double double_v = std::max<double>(double_r, std::max<double>(double_g, double_b));
	const double min_ = std::min<double>(double_r, std::min<double>(double_g, double_b));
	double double_s;
	if(double_v == 0.0) {
		double_s = 0.0;
	} else {
		double_s = (double_v - min_) / double_v;
	}
	double double_h;
	if(double_r == double_g && double_r == double_b) {
		double_h = 0.0;
	} else if(double_v == double_r) {
		double_h = (60.0 * (double_g - double_b)) / (double_v - min_);
	} else if(double_v == double_g) {
		double_h = (60.0 * (double_b - double_r)) / (double_v - min_) + 120.0;
	} else if(double_v == double_b) {
		double_h = (60.0 * (double_r - double_g)) / (double_v - min_) + 240.0;
	}
	if(double_h < 0.0)
		double_h += 360.0;
	/* scaling (to fit to 0 to 255) */
	h = static_cast<int>(double_h / 2.0); // scaling: 0-360 to 0-180
	s = static_cast<int>(double_s * 255.0);
	v = static_cast<int>(double_v * 255.0);
	h = std::max<int>(0, std::min<int>(180, h));
	s = std::max<int>(0, std::min<int>(255, s));
	v = std::max<int>(0, std::min<int>(255, v));
}

void hsv2rgb(const int in_h, const int s, const int v, int &r, int &g, int &b)
{
	/*
	 * range
	 * r: 0 - 255
	 * g: 0 - 255
	 * b: 0 - 255
	 */
	const int h = in_h * 2; // scaling: 0-180 to 0-360
	const int max_ = v;
	const int min_ = static_cast<int>(max_ - ((s / 255.0) * max_));
	if(h >= 0 && h <= 60) {
		r = max_;
		g = static_cast<int>((h / 60.0) * (max_ - min_) + min_);
		b = min_;
	} else if(h > 60 && h <= 120) {
		r = static_cast<int>(((120.0 / h) / 60.0) * (max_ - min_) + min_);
		g = max_;
		b = min_;
	} else if(h > 120 && h <= 180) {
		r = min_;
		g = max_;
		b = static_cast<int>(((h - 120.0) / 60.0) * (max_ - min_) + min_);
	} else if(h > 180 && h <= 240) {
		r = min_;
		g = static_cast<int>(((240.0 - h) / 60.0) * (max_ - min_) + min_);
		b = max_;
	} else if(h > 240 && h <= 300) {
		r = static_cast<int>(((h - 240.0) / 60.0) * (max_ - min_) + min_);
		g = min_;
		b = max_;
	} else if(h > 300 && h <= 360) {
		r = max_;
		g = min_;
		b = static_cast<int>(((360.0 - h) / 60.0) * (max_ - min_) + min_);
	}
	r = std::max<int>(0, std::min<int>(255, r));
	g = std::max<int>(0, std::min<int>(255, g));
	b = std::max<int>(0, std::min<int>(255, b));
}

void rgb2ycbcr(const int r, const int g, const int b, int &y, int &cb, int &cr)
{
	const int delta = 128;
	y = static_cast<int>(0.299 * r + 0.587 * g + 0.114 * b);
	cr = static_cast<int>((r - y) * 0.713 + delta);
	cb = static_cast<int>((b - y) * 0.564 + delta);
}

void ycbcr2rgb(const int y, const int cb, const int cr, int &r, int &g, int &b)
{
	const int delta = 128;
	r = static_cast<int>(y + 1.403 * (cr - delta));
	g = static_cast<int>(y - 0.344 * (cr - delta) - 0.714 * (cb - delta));
	b = static_cast<int>(y + 1.773 * (cb - delta));
}


void ycbcr2hsv(const int y, const int cb, const int cr, int &h, int &s, int &v)
{
	int r, g, b;
	ycbcr2rgb(y, cb, cr, r, g, b);
	rgb2hsv(r, g, b, h, s, v);
}

void hsv2ycbcr(const int h, const int s, const int v, int &y, int &cb, int &cr)
{
	int r, g, b;
	hsv2rgb(h, s, v, r, g, b);
	rgb2ycbcr(r, g, b, y, cb, cr);
}

};

