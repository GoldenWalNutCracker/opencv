#ifndef ARMOR_DETECTOR_HPP
#define ARMOR_DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include "Utils.hpp"

namespace AutoAim {

    class ArmorDetector {
    public:
        ArmorDetector();

        // 设置灯条检测器
        void setLightBarDetector(const LightBarDetector& detector);

        // 检测装甲板
        std::vector<Armor> detect(const cv::Mat& frame);

        // 灯条配对
        std::vector<Armor> pairLightBars(const std::vector<cv::RotatedRect>& light_bars);

        // 筛选装甲板
        std::vector<Armor> filterArmors(const std::vector<Armor>& armors);

    private:
        // 判断两个灯条是否可以配对
        bool canPair(const cv::RotatedRect& left, const cv::RotatedRect& right);

        // 计算装甲板矩形
        cv::Rect calculateArmorRect(const cv::RotatedRect& left, const cv::RotatedRect& right);

        // 判断是否为大装甲板
        bool isLargeArmor(const Armor& armor);

    private:
        LightBarDetector light_detector_;
        float max_height_ratio_;        // 最大高度比
        float max_angle_diff_;          // 最大角度差
        float max_distance_ratio_;      // 最大距离比
        float min_distance_ratio_;      // 最小距离比
    };

} // namespace AutoAim

#endif // ARMOR_DETECTOR_HPP