// cilab.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "common/similarity.h"

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

int main()
{
    // abdomen-1140720_960_720.jpg, abdomen-1140722_960_720.jpg
    // abendstimmung-817716_960_720.jpg abendstimmung-817715_960_720.jpg
    auto imgA = cv::imread("D:/Workspace/ai_proc/img/pixabay/a380-66217_960_720.jpg", cv::IMREAD_COLOR);
    auto imgB = cv::imread("D:/Workspace/ai_proc/img/pixabay/a380-1010801_960_720.jpg", cv::IMREAD_COLOR);
    CiSimilarity ciSimilarity;
    {
        auto sim = ciSimilarity.similarity_by_hist(&imgA, &imgB);
        std::cout << "similarity_by_hist = " << sim << std::endl;
    } 
    {
        auto sim = ciSimilarity.similarity_by_aHash(&imgA, &imgB);
        std::cout << "similarity_by_aHash = " << sim << std::endl;
    }
    {
        auto sim = ciSimilarity.similarity_by_pHash(&imgA, &imgB);
        std::cout << "similarity_by_pHash = " << sim << std::endl;
    }
    {
        auto sim = ciSimilarity.similarity_by_dHash(&imgA, &imgB);
        std::cout << "similarity_by_dHash = " << sim << std::endl;
    }
    {
        auto sim = ciSimilarity.similarity_by_cosin(&imgA, &imgB);
        std::cout << "similarity_by_cosin = " << sim << std::endl;
    }
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
