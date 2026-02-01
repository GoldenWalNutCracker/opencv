#include "NumberRecognizer.hpp"
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

namespace AutoAim {

    NumberRecognizer::NumberRecognizer() {
        // 初始化数字名称
        number_names_ = { "1", "2", "3", "4", "7" };

        // 创建默认模板
        createDefaultTemplates();
    }

    bool NumberRecognizer::loadTemplates(const std::string& template_dir) {
        templates_.clear();
        template_labels_.clear();

        // 加载数字模板
        std::vector<int> numbers = { 1, 2, 3, 4, 7 };

        for (int num : numbers) {
            std::string path = template_dir + "/" + std::to_string(num) + ".png";
            cv::Mat template_img = cv::imread(path, cv::IMREAD_GRAYSCALE);

            if (template_img.empty()) {
                std::cerr << "警告: 无法加载模板 " << path << std::endl;
                continue;
            }

            // 预处理模板
            cv::Mat processed;
            cv::threshold(template_img, processed, 100, 255, cv::THRESH_BINARY);
            cv::resize(processed, processed, cv::Size(32, 32));

            templates_.push_back(processed);
            template_labels_.push_back(num);
        }

        if (templates_.empty()) {
            std::cerr << "警告: 未加载任何模板，使用默认模板" << std::endl;
            createDefaultTemplates();
        }

        std::cout << "加载了 " << templates_.size() << " 个数字模板" << std::endl;
        return !templates_.empty();
    }

    int NumberRecognizer::recognize(const cv::Mat& roi) {
        if (roi.empty() || roi.cols < 10 || roi.rows < 10) {
            return -1;
        }

        // 预处理ROI
        cv::Mat processed_roi = preprocessNumberROI(roi);

        // 模板匹配
        auto result = templateMatch(processed_roi);

        // 置信度阈值
        if (result.second > 0.7) {  // 置信度阈值
            return result.first;
        }

        return -1;
    }

    cv::Mat NumberRecognizer::preprocessNumberROI(const cv::Mat& roi) {
        cv::Mat gray, binary, resized;

        // 转换为灰度图
        if (roi.channels() == 3) {
            cv::cvtColor(roi, gray, cv::COLOR_BGR2GRAY);
        }
        else {
            gray = roi.clone();
        }

        // 二值化
        cv::threshold(gray, binary, 100, 255, cv::THRESH_BINARY);

        // 去除小噪点
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
        cv::morphologyEx(binary, binary, cv::MORPH_OPEN, kernel);

        // 调整大小以匹配模板
        cv::resize(binary, resized, cv::Size(32, 32));

        return resized;
    }

    std::pair<int, double> NumberRecognizer::templateMatch(const cv::Mat& processed_roi) {
        int best_match = -1;
        double best_score = 0.0;

        for (size_t i = 0; i < templates_.size(); ++i) {
            cv::Mat result;

            // 模板匹配
            cv::matchTemplate(processed_roi, templates_[i], result, cv::TM_CCOEFF_NORMED);

            double min_val, max_val;
            cv::Point min_loc, max_loc;
            cv::minMaxLoc(result, &min_val, &max_val, &min_loc, &max_loc);

            if (max_val > best_score) {
                best_score = max_val;
                best_match = template_labels_[i];
            }
        }

        return { best_match, best_score };
    }

    std::string NumberRecognizer::getNumberName(int number) {
        if (number >= 1 && number <= 4) {
            return std::to_string(number);
        }
        else if (number == 7) {
            return "7";
        }
        return "Unknown";
    }

    void NumberRecognizer::createDefaultTemplates() {
        templates_.clear();
        template_labels_.clear();

        // 创建简单的数字模板
        std::vector<int> numbers = { 1, 2, 3, 4, 7 };

        for (int num : numbers) {
            cv::Mat template_img(32, 32, CV_8UC1, cv::Scalar(0));

            // 根据数字绘制简单形状
            switch (num) {
            case 1:
                cv::rectangle(template_img, cv::Rect(14, 5, 4, 22), cv::Scalar(255), -1);
                break;
            case 2:
                cv::line(template_img, cv::Point(5, 5), cv::Point(27, 5), cv::Scalar(255), 3);
                cv::line(template_img, cv::Point(27, 5), cv::Point(27, 15), cv::Scalar(255), 3);
                cv::line(template_img, cv::Point(27, 15), cv::Point(5, 15), cv::Scalar(255), 3);
                cv::line(template_img, cv::Point(5, 15), cv::Point(5, 27), cv::Scalar(255), 3);
                cv::line(template_img, cv::Point(5, 27), cv::Point(27, 27), cv::Scalar(255), 3);
                break;
            case 3:
                // 简单的3
                cv::line(template_img, cv::Point(5, 5), cv::Point(27, 5), cv::Scalar(255), 3);
                cv::line(template_img, cv::Point(27, 5), cv::Point(27, 27), cv::Scalar(255), 3);
                cv::line(template_img, cv::Point(5, 27), cv::Point(27, 27), cv::Scalar(255), 3);
                cv::line(template_img, cv::Point(5, 16), cv::Point(27, 16), cv::Scalar(255), 3);
                break;
            case 4:
                cv::line(template_img, cv::Point(5, 5), cv::Point(5, 27), cv::Scalar(255), 3);
                cv::line(template_img, cv::Point(5, 15), cv::Point(27, 15), cv::Scalar(255), 3);
                cv::line(template_img, cv::Point(27, 5), cv::Point(27, 27), cv::Scalar(255), 3);
                break;
            case 7:
                cv::line(template_img, cv::Point(5, 5), cv::Point(27, 5), cv::Scalar(255), 3);
                cv::line(template_img, cv::Point(27, 5), cv::Point(10, 27), cv::Scalar(255), 3);
                break;
            }

            templates_.push_back(template_img);
            template_labels_.push_back(num);
        }

        std::cout << "创建了 " << templates_.size() << " 个默认数字模板" << std::endl;
    }

} // namespace AutoAim