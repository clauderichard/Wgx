cmake_minimum_required(VERSION 3.6.0)
project(Wgx)
include_directories(
	${PROJECT_SOURCE_DIR}/src
)
file(GLOB SRCS
    "${PROJECT_SOURCE_DIR}/src/**/**/*"
    "${PROJECT_SOURCE_DIR}/src/**/*"
	"${PROJECT_SOURCE_DIR}/src/*"
)

file(GLOB PARSINGSRCS
"${PROJECT_SOURCE_DIR}/src/Parsing/*"
)
file(GLOB GENLANGSRCS
"${PROJECT_SOURCE_DIR}/src/Genlang/*.cpp"
"${PROJECT_SOURCE_DIR}/src/Gen/*.cpp"
)
file(GLOB WAVELANGSRCS
"${PROJECT_SOURCE_DIR}/src/Wavelang/*.cpp"
"${PROJECT_SOURCE_DIR}/src/Wave/*.cpp"
)
file(GLOB PLAYBACKSRCS
"${PROJECT_SOURCE_DIR}/src/Playback/*.cpp"
"${PROJECT_SOURCE_DIR}/src/Util/*.cpp"
"${PROJECT_SOURCE_DIR}/src/Common/*.cpp"
"${PROJECT_SOURCE_DIR}/src/main.cpp"
)

set(CMAKE_UNITY_BUILD ON)
add_executable(Wgx ${SRCS})

set_target_properties(Wgx PROPERTIES
    UNITY_BUILD_MODE GROUP
)
set_source_files_properties(${PARSINGSRCS}
    PROPERTIES UNITY_GROUP "parsingbucket"
)
set_source_files_properties(${GENLANGSRCS}
    PROPERTIES UNITY_GROUP "genlangbucket"
)
set_source_files_properties(${WAVELANGSRCS}
    PROPERTIES UNITY_GROUP "wavelangbucket"
)
set_source_files_properties(${PLAYBACKSRCS}
    PROPERTIES UNITY_GROUP "playbackbucket"
)


# Android mode
add_compile_definitions(
    SFML_SUPPORTED
    DEBUG_LOGS
    WGX_ROOTDIR="/storage/emulated/0/Cpp/Wgx/"
    WGX_WAVOUTPUTDIR="/storage/emulated/0/Music/WgxPlayground/"
    )
find_package(SFML COMPONENTS system network audio REQUIRED)
target_link_libraries(Wgx sfml-system sfml-network sfml-audio)
