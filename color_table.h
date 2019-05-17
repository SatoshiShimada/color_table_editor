#ifndef COLOR_TABLE_H
#define COLOR_TABLE_H

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>

class ColorTable
{
public:
	ColorTable();
	~ColorTable();
	void loadColorTable(std::string);
	void saveColorTable(std::string);
	void clearColor(int, int, int, int, int=0);
	void setColor(int, int, int, int, int=0);
	void resetColor(void);
	void resetColor(int);
	bool checkColor(int, int, int, int);
	void apply(cv::Mat &, cv::Mat &);
private:
	const int color_table_size;
	unsigned short *table;
	int getIndex(const int, const int, const int);
	unsigned short getLabel(const int, const int, const int);
};

#endif // COLOR_TABLE_H

