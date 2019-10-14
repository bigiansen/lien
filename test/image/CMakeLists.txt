SET(LIEN_TEST_IMAGE_SOURCES
	src/image_ops.cpp
	src/x86/image_ops.cpp

	src/benchmarks/image_ops.cpp	

    src/main.cpp
)

add_executable(lien_test_image ${LIEN_TEST_IMAGE_SOURCES})
target_link_libraries(lien_test_image lien_image Catch2::Catch2)