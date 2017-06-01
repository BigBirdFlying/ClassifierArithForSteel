#pragma once
#include "opencv2\core\core.hpp"
#include <vector>
using std::vector;
using cv::Mat;

struct Square
{
	int left;
	int right;
	int top;
	int bottom;
};
struct line_info
{
	cv::Vec8d   line_basic;
	Square		line_region;
	Square		search_region;
};
struct region_info
{
	Square group_line_region;
	Square group_search_region;
	vector<cv::Vec8d> inner_lines;
	double possibility;
	bool   independent;
};
struct region_mo
{
	int x;
	int y;
	int width;
	int height;
	int type;
};
struct block_info
{
	Mat block_img;
	unsigned int left;
	unsigned int right;
	unsigned int top;
	unsigned int bottom;
};
struct all_defects
{
	vector<cv::Rect> horizonal_dfts;
	vector<cv::Rect> vertical_dfts;
	vector<cv::Rect> spot_dfts;
	vector<cv::Rect> area_dfts;
	vector<cv::Rect> other_dfts;
};