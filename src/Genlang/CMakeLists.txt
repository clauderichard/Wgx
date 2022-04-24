cmake_minimum_required(VERSION 3.6.0)
project(GenlangUT)
include_directories(
	${PROJECT_SOURCE_DIR}/..
)
file(GLOB SRCS
    "${PROJECT_SOURCE_DIR}/../Parsing/*"
	"${PROJECT_SOURCE_DIR}/../Wave/*"
	"${PROJECT_SOURCE_DIR}/../Genlang/*.cpp"
	"${PROJECT_SOURCE_DIR}/../Gen/*.cpp"
	"${PROJECT_SOURCE_DIR}/Tests/*"
)

set(CMAKE_UNITY_BUILD ON)
add_executable(GenlangUT ${SRCS})

set_target_properties(GenlangUT PROPERTIES
    UNITY_BUILD_MODE BATCH
)

# Android mode
add_compile_definitions(
    DEBUG_LOGS
    )
