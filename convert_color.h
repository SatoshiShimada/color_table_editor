#ifndef CONVERT_COLOR_H
#define CONVERT_COLOR_H

namespace cvtcolor {
	void rgb2hsv(const int r, const int g, const int b, int &h, int &s, int &v);
	void hsv2rgb(const int h, const int s, const int v, int &r, int &g, int &b);

	void rgb2ycbcr(const int r, const int g, const int b, int &y, int &cb, int &cr);
	void ycbcr2rgb(const int y, const int cb, const int cr, int &r, int &g, int &b);

	void ycbcr2hsv(const int y, const int cb, const int cr, int &h, int &s, int &v);
	void hsv2ycbcr(const int h, const int s, const int v, int &y, int &cb, int &cr);
};

#endif // CONVERT_COLOR_H

