# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#

#配置OpenCV环境
cmake_minimum_required(VERSION 3.8)
project(ArmorPlateRecognition1)


set(OpenCV_DIR "C:/opencv/build" CACHE PATH "Path to OpenCV")


find_package(OpenCV REQUIRED)

include_directories(
	include
	ArmorDetector
	LightBarDetector	
	${OpenCV_INCLUDE_DIRS})

add_executable (${PROJECT_NAME}  main.cpp
				 ArmorPlateRecognition1.cpp  LightBarDetector.cpp)

target_link_libraries(ArmorPlateRecognition1 ${OpenCV_LIBS})
# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ArmorPlateRecognition1")

# 包含子项目。
#add_subdirectory ()


