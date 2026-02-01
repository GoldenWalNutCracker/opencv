# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#

#配置OpenCV环境
cmake_minimum_required (VERSION 3.8)

project (ArmorPlateRecognition1)
set(OpenCV_DIR "C: / Users / 24388 / Downloads / opencv/build" CACHE PATH "Path to OpenCV")


include_directories("C: / Users / 24388 / Downloads / opencv "${OpenCV_INCLUDE_DIRS}./include)

add_executable (ArmorPlateRecognition1
				 "ArmorPlateRecognition1.cpp" "ArmorPlateRecognition1.h")
				add_executable(main "main.cpp")

target_link_libraries(ArmorPlateRecognition1 ${OpenCV_LIBS})
# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ArmorPlateRecognition1")

# 包含子项目。
#add_subdirectory ()

