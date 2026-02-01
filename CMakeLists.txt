cmake_minimum_required(VERSION 3.10)
project(AutoAim)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置OpenCV路径
set(OpenCV_DIR "C:/opencv/build/x64/vc16/lib" CACHE PATH "OpenCV installation path")
find_package(OpenCV REQUIRED 
    COMPONENTS core imgproc highgui videoio calib3d
)

# 打印OpenCV信息
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")

# 添加可执行文件
add_executable(auto_aim
    src/main.cpp
    src/ArmorDetector.cpp
    src/LightBarDetector.cpp
    src/NumberRecognizer.cpp
    src/VideoProcessor.cpp
)

# 包含头文件目录
target_include_directories(auto_aim PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

# 链接OpenCV库
target_link_libraries(auto_aim PRIVATE ${OpenCV_LIBS})

# 设置输出目录
set_target_properties(auto_aim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
# 编译选项
if(MSVC)
    target_compile_options(auto_aim PRIVATE /W3)
else()
    target_compile_options(auto_aim PRIVATE -Wall -Wextra)

endif()
