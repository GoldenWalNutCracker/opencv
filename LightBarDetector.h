// 装甲板识别1.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>
#include<opencv2/opencv.hpp>
#include<vector>
#include<string>
#ifndef LIGHT_BAR_DETECTOR_HPP
#define LIGHT_BAR_DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include "Utils.hpp"

namespace AutoAim {

    class LightBarDetector {
    public:
        // 构造函数
        explicit LightBarDetector(const std::string& enemy_color = "red");

        // 设置参数
        void setEnemyColor(const std::string& color);
        void setThreshold(int binary_thresh, int area_thresh);

        // 检测灯条
        std::vector<cv::RotatedRect> detect(const cv::Mat& frame);

        // 预处理
        cv::Mat preprocess(const cv::Mat& frame);

    private:
        // 颜色分割
        cv::Mat colorSegmentation(const cv::Mat& frame);

        // 轮廓检测与筛选
        std::vector<cv::RotatedRect> findLightBars(const cv::Mat& binary);

        // 筛选灯条轮廓
        bool isValidLightBar(const std::vector<cv::Point>& contour, const cv::RotatedRect& rect);

    private:
        std::string enemy_color_;      // 敌方颜色
        int binary_threshold_;         // 二值化阈值
        int min_area_;                 // 最小面积阈值
        int max_area_;                 // 最大面积阈值
        float min_aspect_ratio_;       // 最小长宽比
        float max_aspect_ratio_;       // 最大长宽比
        float min_angle_;              // 最小角度（绝对值）
        float max_angle_;              // 最大角度（绝对值）
    };

} // namespace AutoAim

#endif // LIGHT_BAR_DETECTOR_HPPTODO: 在此处引用程序需要的其他标头。
