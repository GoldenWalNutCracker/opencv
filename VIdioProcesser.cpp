#include "VideoProcessor.hpp"
#include <iostream>
#include <chrono>

namespace AutoAim {

    VideoProcessor::VideoProcessor(const Config& config)
        : config_(config), frame_count_(0), total_time_(0.0) {

        // 初始化视频捕获
        if (!config_.input_path.empty()) {
            cap_.open(config_.input_path);
        }
        else {
            cap_.open(config_.camera_id);
        }

        if (!cap_.isOpened()) {
            throw std::runtime_error("无法打开视频源!");
        }

        // 初始化视频写入器（如果需要保存）
        if (config_.save_result && !config_.output_path.empty()) {
            int codec = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
            double fps = cap_.get(cv::CAP_PROP_FPS);
            cv::Size frame_size(
                cap_.get(cv::CAP_PROP_FRAME_WIDTH),
                cap_.get(cv::CAP_PROP_FRAME_HEIGHT)
            );

            writer_.open(config_.output_path, codec, fps, frame_size);
            if (!writer_.isOpened()) {
                std::cerr << "警告: 无法创建输出视频文件" << std::endl;
            }
        }

        // 初始化数字识别器
        number_recognizer_.loadTemplates("data/templates");
    }

    void VideoProcessor::process() {
        cv::Mat frame;
        int frame_num = 0;

        auto start_time = std::chrono::high_resolution_clock::now();

        while (true) {
            cap_ >> frame;
            if (frame.empty()) {
                break;
            }

            frame_num++;
            std::cout << "处理帧: " << frame_num << "\r" << std::flush;

            // 处理当前帧
            auto frame_start = std::chrono::high_resolution_clock::now();
            cv::Mat result = processFrame(frame);
            auto frame_end = std::chrono::high_resolution_clock::now();

            double frame_time = std::chrono::duration<double>(frame_end - frame_start).count();
            total_time_ += frame_time;

            // 显示结果
            if (config_.show_result) {
                displayStats(result, frame_num, 1.0 / frame_time);
                cv::imshow("AutoAim - 自动瞄准系统", result);

                if (cv::waitKey(1) == 27) {  // ESC键退出
                    break;
                }
            }

            // 保存结果
            if (config_.save_result && writer_.isOpened()) {
                writer_.write(result);
            }
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        double total_elapsed = std::chrono::duration<double>(end_time - start_time).count();

        std::cout << "\n处理完成!" << std::endl;
        std::cout << "总帧数: " << frame_num << std::endl;
        std::cout << "总时间: " << total_elapsed << " 秒" << std::endl;
        std::cout << "平均帧率: " << frame_num / total_elapsed << " FPS" << std::endl;
    }

    void VideoProcessor::processCamera() {
        cv::Mat frame;
        int frame_num = 0;

        std::cout << "按ESC键退出摄像头采集..." << std::endl;

        while (true) {
            cap_ >> frame;
            if (frame.empty()) {
                break;
            }

            frame_num++;

            // 处理当前帧
            cv::Mat result = processFrame(frame);
            displayStats(result, frame_num, 0);

            // 显示结果
            cv::imshow("AutoAim - 摄像头模式", result);

            if (cv::waitKey(1) == 27) {  // ESC键退出
                break;
            }
        }
    }

    cv::Mat VideoProcessor::processFrame(const cv::Mat& frame) {
        cv::Mat result = frame.clone();

        try {
            // 检测装甲板
            std::vector<Armor> armors = armor_detector_.detect(frame);

            // 对每个装甲板进行数字识别
            for (auto& armor : armors) {
                // 提取ROI
                cv::Mat roi = frame(armor.bounding_rect);

                // 数字识别
                armor.number = number_recognizer_.recognize(roi);

                // 绘制结果
                Utils::drawArmor(result, armor);

                // 在装甲板上方显示识别结果
                if (armor.number != -1) {
                    std::string text = "Num: " + std::to_string(armor.number);
                    cv::putText(result, text,
                        cv::Point(armor.bounding_rect.x, armor.bounding_rect.y - 10),
                        cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
                }

                // 显示大小装甲板信息
                std::string size_text = armor.is_large ? "Large" : "Small";
                cv::putText(result, size_text,
                    cv::Point(armor.bounding_rect.x, armor.bounding_rect.y - 30),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 1);
            }

            // 如果没有检测到装甲板，显示提示信息
            if (armors.empty()) {
                cv::putText(result, "No armor detected",
                    cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX,
                    1.0, cv::Scalar(0, 0, 255), 2);
            }

        }
        catch (const std::exception& e) {
            std::cerr << "处理帧时出错: " << e.what() << std::endl;
        }

        return result;
    }

    void VideoProcessor::saveFrame(const cv::Mat& frame) {
        if (writer_.isOpened()) {
            writer_.write(frame);
        }
    }

    void VideoProcessor::displayStats(cv::Mat& frame, int frame_count, double fps) {
        // 显示帧号
        std::string frame_text = "Frame: " + std::to_string(frame_count);
        cv::putText(frame, frame_text, cv::Point(10, 30),
            cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);

        // 显示FPS
        if (fps > 0) {
            std::string fps_text = "FPS: " + std::to_string(static_cast<int>(fps));
            cv::putText(frame, fps_text, cv::Point(10, 60),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);
        }

        // 显示敌方颜色
        std::string color_text = "Enemy: " + config_.enemy_color;
        cv::putText(frame, color_text, cv::Point(10, 90),
            cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 255), 2);

        // 显示当前时间
        std::string time_text = "Time: " + Utils::getCurrentTime();
        cv::putText(frame, time_text, cv::Point(10, 120),
            cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(200, 200, 200), 1);
    }

} // namespace AutoAim