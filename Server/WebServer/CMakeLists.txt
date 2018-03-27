#工程名字
project(WebServer)

#cmake最低版本
cmake_minimum_required(VERSION 3.9.2)

#设置执行文件输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../../Bin/Server)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../../Bin/Server)

#设置静态库输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../Lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../Lib)

#设置动态库输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../Lib)

add_definitions(-DUNICODE -D_UNICODE)

if (MSVC)
	add_compile_options ("/W4")
	add_compile_options ("/WX")
	add_compile_options ("/std:c++17")
	add_compile_options ("/MP")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
else ()
	add_compile_options ("-Wall")
	add_compile_options ("-std=c++17")
	add_compile_options ("-o3")
endif ()

#包含路径
include_directories(./Include ../Base/Include ../Common ../CommonDefine ../ExportDefine)

#设置库目录
link_directories(../Lib)

#附加库
link_libraries(Base buildvm lua51 luajit)

#源文件路径
aux_source_directory(./Src DIR_SRCS)

#生成可执行程序
add_executable(WebServer ${DIR_SRCS})
