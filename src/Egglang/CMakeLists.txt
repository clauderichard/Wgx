cmake_minimum_required(VERSION 3.6.0)
project(EgglangUT)
include_directories(
	${PROJECT_SOURCE_DIR}/..
)
file(GLOB SRCS
    "${PROJECT_SOURCE_DIR}/../Parsing/*"
	"${PROJECT_SOURCE_DIR}/../Wave/*"
	"${PROJECT_SOURCE_DIR}/../Egglang/*.cpp"
	"${PROJECT_SOURCE_DIR}/../Egg/*.cpp"
	"${PROJECT_SOURCE_DIR}/Tests/*"
)

set(CMAKE_UNITY_BUILD ON)
add_executable(EgglangUT ${SRCS})

set_target_properties(EgglangUT PROPERTIES
    UNITY_BUILD_MODE BATCH
)

# Android mode
add_compile_definitions(
    DEBUG_LOGS
    )
