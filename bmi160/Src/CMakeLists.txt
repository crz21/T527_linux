CMAKE_MINIMUM_REQUIRED(VERSION 3.25.1)

file(GLOB SRC_LIST "./*.c")
include_directories(../Inc)

ADD_EXECUTABLE(BMI160_TEST ${SRC_LIST})
target_link_libraries(BMI160_TEST m)