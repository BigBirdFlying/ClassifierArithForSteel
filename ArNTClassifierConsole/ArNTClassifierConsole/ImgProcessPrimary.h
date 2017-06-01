#pragma once

class ImgProcessPrimary
{
public:
	ImgProcessPrimary(void);
	~ImgProcessPrimary(void);

	static void GetSobelFeat(cv::Mat srcimg,vector<float>& feat);
	static void GetLBPFeat(cv::Mat srcimg,vector<float>& feat);
};
