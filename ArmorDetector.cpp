#include "ArmorDetector.hpp"
#include <algorithm>
#include <cmath>

namespace AutoAim {

    ArmorDetector::ArmorDetector()
        : max_height_ratio_(2.0),
        max_angle_diff_(20.0),
        max_distance_ratio_(4.0),
        min_distance_ratio_(0.5) {
    }

    void ArmorDetector::setLightBarDetector(const LightBarDetector& detector) {
        light_detector_ = detector;
    }

    std::vector<Armor> ArmorDetector::detect(const cv::Mat& frame) {
        // 检测灯条
        std::vector<cv::RotatedRect> light_bars = light_detector_.detect(frame);

        // 灯条配对
        std::vector<Armor> armors = pairLightBars(light_bars);

        // 筛选装甲板
        return filterArmors(armors);
    }

    std::vector<Armor> ArmorDetector::pairLightBars(const std::vector<cv::RotatedRect>& light_bars) {
        std::vector<Armor> armors;

        if (light_bars.size() < 2) {
            return armors;
        }

        // 对灯条按x坐标排序
        std::vector<cv::RotatedRect> sorted_bars = light_bars;
        std::sort(sorted_bars.begin(), sorted_bars.end(),
            [](const cv::RotatedRect& a, const cv::RotatedRect& b) {
                return a.center.x < b.center.x;
            });

        // 尝试配对相邻的灯条
        for (size_t i = 0; i < sorted_bars.size(); ++i) {
            for (size_t j = i + 1; j < sorted_bars.size(); ++j) {
                if (canPair(sorted_bars[i], sorted_bars[j])) {
                    Armor armor;
                    armor.left_light = sorted_bars[i];
                    armor.right_light = sorted_bars[j];
                    armor.bounding_rect = calculateArmorRect(sorted_bars[i], sorted_bars[j]);
                    armor.is_large = isLargeArmor(armor);
                    armors.push_back(armor);
                }
            }
        }

        return armors;
    }

    std::vector<Armor> ArmorDetector::filterArmors(const std::vector<Armor>& armors) {
        std::vector<Armor> filtered;

        for (const auto& armor : armors) {
            // 过滤过小的装甲板
            if (armor.bounding_rect.area() < 100) {
                continue;
            }

            // 过滤过大的装甲板
            if (armor.bounding_rect.area() > 10000) {
                continue;
            }

            filtered.push_back(armor);
        }

        return filtered;
    }

    bool ArmorDetector::canPair(const cv::RotatedRect& left, const cv::RotatedRect& right) {
        // 高度比
        float left_height = left.size.height;
        float right_height = right.size.height;
        float height_ratio = std::max(left_height, right_height) /
            std::min(left_height, right_height);

        if (height_ratio > max_height_ratio_) {
            return false;
        }

        // 角度差
        float left_angle = std::abs(left.angle);
        float right_angle = std::abs(right.angle);

        if (left_angle > 90) left_angle = 180 - left_angle;
        if (right_angle > 90) right_angle = 180 - right_angle;

        float angle_diff = std::abs(left_angle - right_angle);
        if (angle_diff > max_angle_diff_) {
            return false;
        }

        // 距离和高度比
        float avg_height = (left_height + right_height) / 2.0;
        float distance = std::abs(right.center.x - left.center.x);
        float distance_ratio = distance / avg_height;

        if (distance_ratio < min_distance_ratio_ || distance_ratio > max_distance_ratio_) {
            return false;
        }

        // y坐标差
        float y_diff = std::abs(right.center.y - left.center.y);
        if (y_diff > avg_height) {
            return false;
        }

        return true;
    }

    cv::Rect ArmorDetector::calculateArmorRect(const cv::RotatedRect& left,
        const cv::RotatedRect& right) {
        // 计算装甲板的边界框
        float x1 = std::min(left.center.x - left.size.width / 2,
            right.center.x - right.size.width / 2);
        float x2 = std::max(left.center.x + left.size.width / 2,
            right.center.x + right.size.width / 2);
        float y1 = std::min(left.center.y - left.size.height / 2,
            right.center.y - right.size.height / 2);
        float y2 = std::max(left.center.y + left.size.height / 2,
            right.center.y + right.size.height / 2);

        // 扩展边界
        float expand = 0.2;
        float width = x2 - x1;
        float height = y2 - y1;

        x1 -= width * expand / 2;
        x2 += width * expand / 2;
        y1 -= height * expand / 2;
        y2 += height * expand / 2;

        // 确保在图像范围内
        x1 = std::max(0.0f, x1);
        y1 = std::max(0.0f, y1);
        x2 = std::min(1920.0f, x2);  // 假设最大宽度
        y2 = std::min(1080.0f, y2);  // 假设最大高度

        return cv::Rect(x1, y1, x2 - x1, y2 - y1);
    }

    bool ArmorDetector::isLargeArmor(const Armor& armor) {
        // 根据灯条间距判断大小装甲板
        float distance = std::abs(armor.right_light.center.x - armor.left_light.center.x);
        float avg_height = (armor.left_light.size.height + armor.right_light.size.height) / 2.0;

        // 小装甲板：距离/高度比约2.5，大装甲板：约4.5
        float ratio = distance / avg_height;
        return ratio > 3.5;  // 大于3.5认为是大装甲板
    }

} // namespace AutoAim