#include "Utils.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cstring>

namespace AutoAim {

    // 解析命令行参数
    Config Utils::parseArguments(int argc, char** argv) {
        Config config;

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "--input" && i + 1 < argc) {
                config.input_path = argv[++i];
            }
            else if (arg == "--output" && i + 1 < argc) {
                config.output_path = argv[++i];
            }
            else if (arg == "--enemy_color" && i + 1 < argc) {
                std::string color = argv[++i];
                if (color == "red" || color == "blue") {
                    config.enemy_color = color;
                }
                else {
                    std::cerr << "警告: 颜色参数必须是 'red' 或 'blue'，使用默认值: red" << std::endl;
                }
            }
            else if (arg == "--camera" && i + 1 < argc) {
                config.camera_id = std::stoi(argv[++i]);
            }
            else if (arg == "--show") {
                config.show_result = true;
            }
            else if (arg == "--no-show") {
                config.show_result = false;
            }
            else if (arg == "--save") {
                config.save_result = true;
            }
            else if (arg == "--help" || arg == "-h") {
                std::cout << "使用方法: " << argv[0] << " [选项]" << std::endl;
                std::cout << "选项:" << std::endl;
                std::cout << "  --input <路径>         输入视频文件路径" << std::endl;
                std::cout << "  --output <路径>        输出视频文件路径 (默认: output.mp4)" << std::endl;
                std::cout << "  --enemy_color <颜色>   敌方颜色: red 或 blue (默认: red)" << std::endl;
                std::cout << "  --camera <ID>          摄像头ID (默认: 0)" << std::endl;
                std::cout << "  --show                 显示处理结果 (默认)" << std::endl;
                std::cout << "  --no-show              不显示处理结果" << std::endl;
                std::cout << "  --save                 保存处理结果视频" << std::endl;
                std::cout << "  --help, -h             显示此帮助信息" << std::endl;
                exit(0);
            }
        }

        return config;
    }

    // 颜色转换：BGR转HSV
    cv::Mat Utils::bgr2hsv(const cv::Mat& bgr_frame) {
        cv::Mat hsv_frame;
        cv::cvtColor(bgr_frame, hsv_frame, cv::COLOR_BGR2HSV);
        return hsv_frame;
    }

    // 颜色分割：根据颜色阈值提取掩码
    cv::Mat Utils::colorSegmentation(const cv::Mat& frame, const std::string& color) {
        cv::Mat hsv = bgr2hsv(frame);
        cv::Mat mask;

        if (color == "red") {
            // 红色有两个范围（0-10和160-180）
            cv::Mat mask1, mask2;
            cv::inRange(hsv, cv::Scalar(0, 100, 100), cv::Scalar(10, 255, 255), mask1);
            cv::inRange(hsv, cv::Scalar(160, 100, 100), cv::Scalar(180, 255, 255), mask2);
            mask = mask1 | mask2;
        }
        else if (color == "blue") {
            // 蓝色范围
            cv::inRange(hsv, cv::Scalar(100, 100, 100), cv::Scalar(130, 255, 255), mask);
        }
        else {
            // 默认使用灰度处理
            cv::cvtColor(frame, mask, cv::COLOR_BGR2GRAY);
            cv::threshold(mask, mask, 100, 255, cv::THRESH_BINARY);
        }

        return mask;
    }

    // 绘制装甲板
    void Utils::drawArmor(cv::Mat& frame, const Armor& armor) {
        // 绘制灯条
        drawLightBar(frame, armor.left_light);
        drawLightBar(frame, armor.right_light);

        // 绘制装甲板矩形
        cv::Scalar armor_color = armor.is_large ? cv::Scalar(0, 255, 255) : cv::Scalar(0, 165, 255); // 大装甲黄色，小装甲橙色
        cv::rectangle(frame, armor.bounding_rect, armor_color, 2);

        // 绘制装甲板中心点
        cv::Point center(armor.bounding_rect.x + armor.bounding_rect.width / 2,
            armor.bounding_rect.y + armor.bounding_rect.height / 2);
        cv::circle(frame, center, 3, cv::Scalar(255, 0, 0), -1);

        // 如果识别到数字，显示在装甲板上方
        if (armor.number != -1) {
            std::string text = std::to_string(armor.number);
            cv::Point text_pos(armor.bounding_rect.x, armor.bounding_rect.y - 5);
            cv::putText(frame, text, text_pos, cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
        }

        // 显示大小装甲板类型
        std::string size_text = armor.is_large ? "L" : "S";
        cv::Point size_pos(armor.bounding_rect.x + armor.bounding_rect.width - 15,
            armor.bounding_rect.y + 15);
        cv::putText(frame, size_text, size_pos, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 2);
    }

    // 绘制灯条
    void Utils::drawLightBar(cv::Mat& frame, const cv::RotatedRect& light) {
        // 获取旋转矩形的四个顶点
        cv::Point2f vertices[4];
        light.points(vertices);

        // 绘制灯条矩形
        for (int i = 0; i < 4; i++) {
            cv::line(frame, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
        }

        // 绘制灯条中心点
        cv::circle(frame, light.center, 2, cv::Scalar(255, 0, 0), -1);
    }

    // 获取当前时间戳
    std::string Utils::getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::tm tm_info;
#ifdef _WIN32
        localtime_s(&tm_info, &time_t_now);
#else
        localtime_r(&time_t_now, &tm_info);
#endif

        std::ostringstream oss;
        oss << std::put_time(&tm_info, "%Y-%m-%d %H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

        return oss.str();
    }

    // 打印调试信息
    void Utils::debugPrint(const std::string& message) {
        std::cout << "[" << getCurrentTime() << "] " << message << std::endl;
    }

    // 计算两点之间的距离
    double Utils::distance(const cv::Point2f& p1, const cv::Point2f& p2) {
        return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
    }

    // 获取旋转矩形的四个顶点
    std::vector<cv::Point2f> Utils::getRotatedRectVertices(const cv::RotatedRect& rect) {
        cv::Point2f vertices[4];
        rect.points(vertices);
        return std::vector<cv::Point2f>(vertices, vertices + 4);
    }

    // 安全地获取ROI区域
    cv::Mat Utils::getSafeROI(const cv::Mat& frame, const cv::Rect& roi) {
        // 确保ROI在图像范围内
        int x = clamp(roi.x, 0, frame.cols - 1);
        int y = clamp(roi.y, 0, frame.rows - 1);
        int width = clamp(roi.width, 1, frame.cols - x);
        int height = clamp(roi.height, 1, frame.rows - y);

        cv::Rect safe_roi(x, y, width, height);
        return frame(safe_roi).clone();
    }

    // 显示图像（调试用）
    void Utils::showImage(const std::string& window_name, const cv::Mat& image, int delay_ms) {
        if (!image.empty()) {
            cv::imshow(window_name, image);
            cv::waitKey(delay_ms);
        }
    }

    // 保存图像（调试用）
    void Utils::saveImage(const std::string& filename, const cv::Mat& image) {
        if (!image.empty()) {
            cv::imwrite(filename, image);
            debugPrint("保存图像: " + filename);
        }
    }

    // 检查文件是否存在
    bool Utils::fileExists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }

} // namespace AutoAim