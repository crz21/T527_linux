CMAKE_MINIMUM_REQUIRED(VERSION 3.25.1)

# 目标工程
project(temp_file)

# 查找指定路径下的所有源文件
# aux_source_directory(路径 宏名)

# 编译其他目录下的文件，如math
# add_subdirectory(math)

# 执行文件temp_file 源文件temp.c
add_executable(temp_file temp.c)