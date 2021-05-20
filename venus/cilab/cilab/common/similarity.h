#pragma once

#include <opencv2/opencv.hpp>

class CiSimilarity {

public:
	float similarity_by_hist(const cv::Mat* imgBgrA, const cv::Mat* imgBgrB, cv::Size resizeTo = cv::Size(64, 64)) {
		cv::Mat imgGrayResizedA, imgGrayResizedB;
		cv::resize(*imgBgrA, imgGrayResizedA, resizeTo);
		cv::resize(*imgBgrB, imgGrayResizedB, resizeTo);
		cv::cvtColor(imgGrayResizedA, imgGrayResizedA, cv::COLOR_BGR2GRAY);
		cv::cvtColor(imgGrayResizedB, imgGrayResizedB, cv::COLOR_BGR2GRAY);

		const int channels = 0;
		const int histDims = 1;
		const int histSize = 256;
		float grayRanges[2] = { 0, 255 };
		const float* histRanges[1] = { grayRanges };

		cv::Mat histA;
		cv::calcHist(&imgGrayResizedA, 1, &channels, cv::Mat(), histA, histDims, &histSize, histRanges, true, false);
		cv::Mat histB;
		cv::calcHist(&imgGrayResizedB, 1, &channels, cv::Mat(), histB, histDims, &histSize, histRanges, true, false);

		assert(histA.cols == 1);
		assert(histB.cols == 1);
		assert(histA.rows == histB.rows);

		auto similarity = 0.0f;
		for (int i = 0; i < histA.rows; i++) {
			auto a = histA.at<float>(i, 0);
			auto b = histB.at<float>(i, 0);
			similarity += a == b ? 1 : (1 - std::abs(a - b) / std::max(a, b));
		}
		similarity /= (histSize - 1);
		return similarity;
	}

	//
	// aHash, 均值哈希, 算法简单但受均值影响大。如果对图像进行伽马校正或者进行直方图均值化都会影响均值，从而影响哈希值的计算
	// 
	float similarity_by_aHash(const cv::Mat* imgBgrA, const cv::Mat* imgBgrB, cv::Size resizeTo = cv::Size(8, 8)) {
		cv::Mat hashA, hashB;
		_calc_aHash(imgBgrA, hashA, resizeTo);
		_calc_aHash(imgBgrB, hashB, resizeTo);
		return _distance_hamming(hashA, hashB);
	}

	//
	// pHash, 感知哈希算法, Phash哈希算法过于严格，更适合搜索缩略图，为了获得更精确的结果可以选择pHash，它采用的是DCT（离散余弦变换）来降低频率的方法。
	// 
	float similarity_by_pHash(const cv::Mat* imgBgrA, const cv::Mat* imgBgrB, cv::Size resizeTo = cv::Size(8, 8)) {
		cv::Mat hashA, hashB;
		_calc_pHash(imgBgrA, hashA, resizeTo);
		_calc_pHash(imgBgrB, hashB, resizeTo);
		return _distance_hamming(hashA, hashB);
	}

	//
	// dHash, 差异值哈希, dHash相比pHash:速度要快的多，相比aHash:dHash在效率几乎相同的情况下的效果要更好，它是基于渐变实现的。
	// 
	float similarity_by_dHash(const cv::Mat* imgBgrA, const cv::Mat* imgBgrB, cv::Size resizeTo = cv::Size(8, 8)) {
		cv::Mat hashA, hashB;
		_calc_dHash(imgBgrA, hashA, resizeTo);
		_calc_dHash(imgBgrB, hashB, resizeTo);
		return _distance_hamming(hashA, hashB);
	}

	//
	// cosin, 余弦相似度, 把图片表示成一个向量，通过计算向量之间的余弦距离来表征两张图片的相似度。
	// 
	float similarity_by_cosin(const cv::Mat* imgBgrA, const cv::Mat* imgBgrB, cv::Size resizeTo = cv::Size(64, 64)) {
		cv::Mat imgGrayResizedA, imgGrayResizedB;
		cv::resize(*imgBgrA, imgGrayResizedA, resizeTo, cv::INTER_CUBIC);
		cv::cvtColor(imgGrayResizedA, imgGrayResizedA, cv::COLOR_BGR2GRAY);
		cv::resize(*imgBgrB, imgGrayResizedB, resizeTo, cv::INTER_CUBIC);
		cv::cvtColor(imgGrayResizedB, imgGrayResizedB, cv::COLOR_BGR2GRAY);

		cv::normalize(imgGrayResizedA, imgGrayResizedA, 1, 0, cv::NORM_L2, CV_32F);
		cv::normalize(imgGrayResizedB, imgGrayResizedB, 1, 0, cv::NORM_L2, CV_32F);
		return imgGrayResizedA.dot(imgGrayResizedB);
	}
private:
	//
	// args: matA and matB must be composed of 0 or 1 only
	// 
	float _distance_hamming(const cv::Mat& matA, const cv::Mat& matB) {
		cv::Mat xorRst;
		cv::bitwise_xor(matA, matB, xorRst);
		int nonZeroCnt = cv::countNonZero(xorRst);
		return (float)nonZeroCnt / ((double)xorRst.cols * xorRst.rows);
	}

	int _calc_hash(const cv::Mat imgGray, cv::Mat& imgHash) {
		if (imgHash.empty()) {
			imgHash = cv::Mat(imgGray.size(), CV_8U);
		}
		assert(imgGray.cols == imgHash.cols && imgGray.rows == imgHash.rows);
		double avgGray = cv::sum(imgGray)[0] / ((double)imgGray.cols * imgGray.rows);
		for (int i = 0; i < imgGray.rows; i++) {
			auto imgRow = imgGray.ptr<uint8_t>(i);
			auto hashRow = imgHash.ptr<uint8_t>(i);
			for (int j = 0; j < imgGray.cols; j++) {
				auto val = *(imgRow + j);
				*(hashRow + j) = val > avgGray ? 1 : 0;
			}
		}
		return 0;
	}

	int _calc_aHash(const cv::Mat* imgBgrA, cv::Mat& aHash, cv::Size resizeTo = cv::Size(8, 8)) {
		cv::Mat& imgGrayResized = aHash;
		cv::resize(*imgBgrA, imgGrayResized, resizeTo, cv::INTER_CUBIC);
		cv::cvtColor(imgGrayResized, imgGrayResized, cv::COLOR_BGR2GRAY);
		_calc_hash(imgGrayResized, aHash);
		return 0;
	}

	int _calc_pHash(const cv::Mat* imgBgrA, cv::Mat& pHash, cv::Size resizeTo = cv::Size(32, 32)) {
		cv::Mat& imgGrayResized = pHash;
		cv::resize(*imgBgrA, imgGrayResized, resizeTo, cv::INTER_CUBIC);
		cv::cvtColor(imgGrayResized, imgGrayResized, cv::COLOR_BGR2GRAY);
		
		cv::Mat dctRst;
		imgGrayResized.assignTo(dctRst, CV_32F);

		cv::dct(dctRst, dctRst);
		cv::Mat dctRstRoi = dctRst(cv::Rect(0, 0, 8, 8));
		_calc_hash(dctRstRoi, pHash);
		return 0;
	}

	int _calc_dHash(const cv::Mat* imgBgrA, cv::Mat& dHash, cv::Size resizeTo = cv::Size(9, 8)) {
		cv::Mat& imgGrayResized = dHash;
		cv::resize(*imgBgrA, imgGrayResized, resizeTo, cv::INTER_CUBIC);
		cv::cvtColor(imgGrayResized, imgGrayResized, cv::COLOR_BGR2GRAY);
		for (int i = 0; i < imgGrayResized.rows; i++) {
			auto imgRow = imgGrayResized.ptr<uint8_t>(i);
			for (int j = 1; j < imgGrayResized.cols; j++) {
				auto valL = *(imgRow + j - 1);
				auto valR = *(imgRow + j);
				*(imgRow + j - 1) = valL > valR ? 1 : 0;
			}
		}
		dHash = dHash(cv::Rect(0, 0, resizeTo.width - 1, resizeTo.height));
		return 0;
	}
};

