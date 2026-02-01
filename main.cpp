#include <iostream>
#include <opencv2/opencv.hpp>
#include "VideoProcessor.hpp"
#include "Utils.hpp"

int main(int argc, char** argv) {
    std::cout << "=== AutoAim 自动瞄准系统 ===" << std::endl;
    std::cout << "版本: 1.0" << std::endl;
    std::cout << "作者: [刘]" << std::endl;
    std::cout << std::endl;

    try {
        // 解析命令行参数
        AutoAim::Config config = AutoAim::Utils::parseArguments(argc, argv);

        // 创建视频处理器
        AutoAim::VideoProcessor processor(config);

        // 开始处理
        if (!config.input_path.empty()) {
            std::cout << "处理视频文件: " << config.input_path "C:\Users\24388\Desktop\test01.avi" << std::endl;
            processor.process();
        }
        else {
            std::cout << "处理摄像头: " << config.camera_id << std::endl;
            processor.processCamera();
        }

        std::cout << "处理完成!" << std::endl;

    }
    catch (const std::exception& e) {
        std::cerr << "错误: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}