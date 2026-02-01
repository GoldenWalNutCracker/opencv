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

        // 构造函数，设置默认值
        Config() :
            input_path(""),
            output_path("output.mp4"),
            enemy_color("red"),
            camera_id(0),
            show_result(true),
            save_result(false) {
        }
    };

    // 装甲板结构体
    struct Armor {
        cv::RotatedRect left_light;   // 左灯条
        cv::RotatedRect right_light;  // 右灯条
        cv::Rect bounding_rect;       // 装甲板矩形
        int number;                   // 识别到的数字
        double confidence;            // 置信度
        bool is_large;                // 是否为大装甲板

        // 构造函数
        Armor() : number(-1), confidence(0.0), is_large(false) {}
    };

    // 工具函数类
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

        // 限制值在范围内
        template<typename T>
        static T clamp(T value, T min_val, T max_val) {
            return (value < min_val) ? min_val : ((value > max_val) ? max_val : value);
        }

        // 计算两点之间的距离
        static double distance(const cv::Point2f& p1, const cv::Point2f& p2);

        // 获取旋转矩形的四个顶点
        static std::vector<cv::Point2f> getRotatedRectVertices(const cv::RotatedRect& rect);

        // 安全地获取ROI区域
        static cv::Mat getSafeROI(const cv::Mat& frame, const cv::Rect& roi);

        // 显示图像（调试用）
        static void showImage(const std::string& window_name, const cv::Mat& image, int delay_ms = 1);

        // 保存图像（调试用）
        static void saveImage(const std::string& filename, const cv::Mat& image);

        // 检查文件是否存在
        static bool fileExists(const std::string& filename);
    };

} // namespace AutoAim

#endif // UTILS_HPP