cmake_minimum_required(VERSION 3.6.0)
project(Wgx)
include_directories(
	${PROJECT_SOURCE_DIR}/src
)
file(GLOB SRCS
    "${PROJECT_SOURCE_DIR}/src/**/*"
	"${PROJECT_SOURCE_DIR}/src/*"
)


set(CMAKE_UNITY_BUILD ON)
add_executable(Wgx ${SRCS})

set_target_properties(Wgx PROPERTIES
    UNITY_BUILD_MODE BATCH
)

if (EXISTS "${PROJECT_SOURCE_DIR}/../../../Claude")
    # Desktop mode
    add_compile_definitions(
        WGX_ROOTDIR="C:/Claude/Cpp/Wgx/"
        WGX_WAVOUTPUTDIR="C:/Claude/foobar2000/Music/WgxPlayground/"
        )
else()
    # Android mode
    add_compile_definitions(
        SFML_SUPPORTED
        DEBUG_LOGS
        WGX_ROOTDIR="/storage/emulated/0/Cpp/Wgx/"
        WGX_WAVOUTPUTDIR="/storage/emulated/0/Music/WgxPlayground/"
        )
    find_package(SFML COMPONENTS system network audio REQUIRED)
    target_link_libraries(Wgx sfml-system sfml-network sfml-audio)
endif()

