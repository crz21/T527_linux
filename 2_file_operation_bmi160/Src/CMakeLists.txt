CMAKE_MINIMUM_REQUIRED(VERSION 3.25.1)

# 由普通字符和/或通配字符组成的字符串，用于匹配文件路径
file(GLOB SRC_LIST "./*.c")

# 从当前目录下的Inc添加头文件
include_directories(../Inc)

# 生成执行文件
ADD_EXECUTABLE(BMI160_TEST ${SRC_LIST})

# 调用math库
target_link_libraries(BMI160_TEST m)