#ifndef UTILS_HPP
#define UTILS_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace AutoAim {

    // 参数结构体
    struct Config {
        std::string input_path;      // 输入视频路径
        std::string output_path;     // 输出视频路径
        std::string enemy_color;     // 敌方颜色: "red" 或 "blue"
        int camera_id;               // 摄像头ID
        bool show_result;            // 是否显示结果
        bool save_result;            // 是否保存结果
    };

    // 装甲板结构体
    struct Armor {
        cv::RotatedRect left_light;   // 左灯条
        cv::RotatedRect right_light;  // 右灯条
        cv::Rect bounding_rect;       // 装甲板矩形
        int number;                   // 识别到的数字
        double confidence;            // 置信度
        bool is_large;                // 是否为大装甲板

        Armor() : number(-1), confidence(0.0), is_large(false) {}
    };

    // 工具函数
    class Utils {
    public:
        // 解析命令行参数
        static Config parseArguments(int argc, char** argv);

        // 颜色转换：BGR转HSV
        static cv::Mat bgr2hsv(const cv::Mat& bgr_frame);

        // 颜色分割：根据颜色阈值提取掩码
        static cv::Mat colorSegmentation(const cv::Mat& frame, const std::string& color);

        // 绘制装甲板
        static void drawArmor(cv::Mat& frame, const Armor& armor);

        // 绘制灯条
        static void drawLightBar(cv::Mat& frame, const cv::RotatedRect& light);

        // 获取当前时间戳
        static std::string getCurrentTime();

        // 打印调试信息
        static void debugPrint(const std::string& message);
    };

} // namespace AutoAim

#endif // UTILS_HPP