#pragma once
#include <windows.h>
#include <opencv2\opencv.hpp>
#include <opencv2\core\core.hpp>
#include <opencv2\nonfree\features2d.hpp> // 这是surf用的
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp> // Hough在这里
#include <opencv2\gpu\gpu.hpp>

#include <iostream>
#include <fstream>
#include <vector> 
#include <stdio.h>
#include <math.h>
#include <io.h>
#include <windows.h>
#include <string>
#include <sstream>	//字符串串来串去用
using namespace std;
//using namespace cv;
#define NTRAINING_SAMPLES   100         // Number of training samples per class
#define FRAC_LINEAR_SEP     0.9f        // Fraction of samples which compose the linear separable part
#define MAX_CLUSTERS (8)  


#define PI 3.1415926


