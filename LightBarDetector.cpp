#include "LightBarDetector.hpp"
#include <iostream>

namespace AutoAim {

    LightBarDetector::LightBarDetector(const std::string& enemy_color)
        : enemy_color_(enemy_color),
        binary_threshold_(100),
        min_area_(50),
        max_area_(5000),
        min_aspect_ratio_(1.5),
        max_aspect_ratio_(15.0),
        min_angle_(0.0),
        max_angle_(60.0) {
    }

    void LightBarDetector::setEnemyColor(const std::string& color) {
        enemy_color_ = color;
    }

    void LightBarDetector::setThreshold(int binary_thresh, int area_thresh) {
        binary_threshold_ = binary_thresh;
        min_area_ = area_thresh;
    }

    std::vector<cv::RotatedRect> LightBarDetector::detect(const cv::Mat& frame) {
        // 预处理
        cv::Mat processed = preprocess(frame);

        // 颜色分割
        cv::Mat binary = colorSegmentation(processed);

        // 查找灯条
        return findLightBars(binary);
    }

    cv::Mat LightBarDetector::preprocess(const cv::Mat& frame) {
        cv::Mat processed;

        // 高斯模糊去噪
        cv::GaussianBlur(frame, processed, cv::Size(5, 5), 0);

        return processed;
    }

    cv::Mat LightBarDetector::colorSegmentation(const cv::Mat& frame) {
        cv::Mat hsv, binary;

        // 转换为HSV颜色空间
        cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);

        if (enemy_color_ == "red") {
            // 红色有两个范围（0-10和160-180）
            cv::Mat red1, red2;
            cv::inRange(hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), red1);
            cv::inRange(hsv, cv::Scalar(160, 100, 100), cv::Scalar(180, 255, 255), red2);
            binary = red1 | red2;
        }
        else if (enemy_color_ == "blue") {
            // 蓝色范围
            cv::inRange(hsv, cv::Scalar(100, 100, 100), cv::Scalar(130, 255, 255), binary);
        }
        else {
            // 默认使用红色
            cv::inRange(hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), binary);
        }

        // 形态学操作：先腐蚀后膨胀（开运算）
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::morphologyEx(binary, binary, cv::MORPH_OPEN, kernel);

        // 膨胀连接相近区域
        cv::dilate(binary, binary, kernel);

        return binary;
    }

    std::vector<cv::RotatedRect> LightBarDetector::findLightBars(const cv::Mat& binary) {
        std::vector<cv::RotatedRect> light_bars;

        // 查找轮廓
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(binary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        for (const auto& contour : contours) {
            // 轮廓面积
            double area = cv::contourArea(contour);
            if (area < min_area_ || area > max_area_) {
                continue;
            }

            // 最小外接旋转矩形
            cv::RotatedRect rect = cv::minAreaRect(contour);

            // 筛选灯条
            if (isValidLightBar(contour, rect)) {
                light_bars.push_back(rect);
            }
        }

        return light_bars;
    }

    bool LightBarDetector::isValidLightBar(const std::vector<cv::Point>& contour,
        const cv::RotatedRect& rect) {
        // 计算长宽比
        float width = rect.size.width;
        float height = rect.size.height;

        // 确保高度 > 宽度
        if (width > height) {
            float temp = width;
            width = height;
            height = temp;
        }

        float aspect_ratio = height / width;

        // 筛选条件
        if (aspect_ratio < min_aspect_ratio_ || aspect_ratio > max_aspect_ratio_) {
            return false;
        }

        // 角度筛选（灯条通常是近似垂直的）
        float angle = std::abs(rect.angle);
        if (angle > 90) {
            angle = 180 - angle;
        }

        if (angle < min_angle_ || angle > max_angle_) {
            return false;
        }

        // 轮廓面积与外接矩形面积比
        double rect_area = rect.size.area();
        double contour_area = cv::contourArea(contour);
        double fill_ratio = contour_area / rect_area;

        if (fill_ratio < 0.5) {
            return false;
        }

        return true;
    }

} // namespace AutoAim