cmake_minimum_required(VERSION 3.6.0)
project(WavelangUT)
include_directories(
	${PROJECT_SOURCE_DIR}/../..
)
file(GLOB SRCS
    "${PROJECT_SOURCE_DIR}/*"
    "${PROJECT_SOURCE_DIR}/../*.cpp"
    "${PROJECT_SOURCE_DIR}/../../Parsing/*"
	"${PROJECT_SOURCE_DIR}/../../Wave/*"
)

set(CMAKE_UNITY_BUILD ON)
add_executable(WavelangUT ${SRCS})

set_target_properties(WavelangUT PROPERTIES
    UNITY_BUILD_MODE BATCH
)

add_compile_definitions(
    DEBUG_LOGS
    )
