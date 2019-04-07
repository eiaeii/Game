#工程名字
project(Base)

#cmake最低版本
cmake_minimum_required(VERSION 3.9.2)

#设置执行文件输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../../Bin/Server)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../../Bin/Server)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/../../Bin/Server)

#设置静态库输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../Lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../Lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/../Lib)

#设置动态库输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/../Lib)

add_definitions(-DUNICODE -D_UNICODE)

if (MSVC)
	add_compile_options ("/W4")
	add_compile_options ("/WX")
	add_compile_options ("/std:c++17")
	add_compile_options ("/MP")
else ()
	add_compile_options ("-Wall")
	add_compile_options ("-std=c++17")
	add_compile_options ("-o3")
	add_compile_options ("-fPIC")
endif ()

#包含路径
include_directories(
./Include
../Base/Include
../Base/Include/Net
../CommonDefine
)

#源文件路径
aux_source_directory(./Src DIR_SRCS)
aux_source_directory(./Src/Net DIR_SRCS)

#生成静态库
add_library(Base STATIC ${DIR_SRCS})
