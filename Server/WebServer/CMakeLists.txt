#工程名字
project(WebServer)

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
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/../../Bin/Server)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/../../Bin/Server)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/../../Bin/Server)

add_definitions(-DUNICODE -D_UNICODE)

#包含路径
include_directories(
./Include
../Base/Include
../Base/Include/Net
../ExportDefine
../Base/Include/LuaJIT
../Base/Include/ZeroMQ
)

#设置库目录
link_directories(../Lib)

#源文件路径
aux_source_directory(./Src DIR_SRCS)
aux_source_directory(../Common/Src DIR_COMMON)
aux_source_directory(./WebServer/Src DIR_WebServer)

if (MSVC)
	add_compile_options ("/W4")
	add_compile_options ("/WX")
	add_compile_options ("/std:c++17")
	add_compile_options ("/MP")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")

	#附加库
	link_libraries(Base buildvm lua51 luajit)
else ()
	add_compile_options ("-Wall")
	add_compile_options ("-std=c++17")
	add_compile_options ("-o3")
	
	#附加库
	link_libraries(Base stdc++ pthread stdc++fs)
	#luajit
	link_libraries(luajit m dl)
	#luajit
	link_libraries(libzmq m dl)
endif ()

#生成可执行程序
add_executable(WebServer ${DIR_SRCS} ${DIR_COMMON} ${DIR_WebServer})
