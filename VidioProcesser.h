#ifndef VIDEO_PROCESSOR_HPP
#define VIDEO_PROCESSOR_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include "ArmorDetector.hpp"
#include "NumberRecognizer.hpp"

namespace AutoAim {

    class VideoProcessor {
    public:
        VideoProcessor(const Config& config);

        // 处理视频
        void process();

        // 处理摄像头
        void processCamera();

        // 处理单帧
        cv::Mat processFrame(const cv::Mat& frame);

        // 保存结果
        void saveFrame(const cv::Mat& frame);

        // 显示统计信息
        void displayStats(cv::Mat& frame, int frame_count, double fps);

    private:
        Config config_;
        cv::VideoCapture cap_;
        cv::VideoWriter writer_;
        ArmorDetector armor_detector_;
        NumberRecognizer number_recognizer_;
        int frame_count_;
        double total_time_;
    };

} // namespace AutoAim

#endif // VIDEO_PROCESSOR_HPP