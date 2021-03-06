
set (
	SOURCES
	core/array2d_test.cpp
	core/sliding_window_view_test.cpp
	image/image_reader_test.cpp
	image/dft_test.cpp
	image/image_converter_test.cpp
	image/convolve_test.cpp
	image/edge_detection_test.cpp
	test_runner.cpp
)

find_package(GTest REQUIRED)
enable_testing()

add_executable(${PROJECT_NAME}_test ${SOURCES})

target_include_directories(
    ${PROJECT_NAME}_test
    PRIVATE "${PROJECT_SOURCE_DIR}/lib/")

target_link_libraries(
    ${PROJECT_NAME}_test
    ${PROJECT_NAME}
    GTest::GTest
    GTest::Main
    Threads::Threads
)

add_test(AllTestsInMain ${PROJECT_NAME}_test)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/TestResults)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/TestData/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/TestData)
