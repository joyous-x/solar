#ifndef __OPENCV_H__
#define __OPENCV_H__


#ifdef _DEBUG
#pragma comment(lib, "opencv_imgproc440d.lib")
#pragma comment(lib, "opencv_imgcodecs440d.lib")
#pragma comment(lib, "opencv_highgui440d.lib")
#pragma comment(lib, "opencv_core440d.lib")
#else
#pragma comment(lib, "opencv_imgproc440.lib")
#pragma comment(lib, "opencv_imgcodecs440.lib")
#pragma comment(lib, "opencv_highgui440.lib")
#pragma comment(lib, "opencv_core440.lib")
#endif


#endif //> __OPENCV_H__