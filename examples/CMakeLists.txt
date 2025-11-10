cmake_minimum_required(VERSION 3.20)

# Example executables
add_executable(hhc_encode_example encode_example.cpp)
target_link_libraries(hhc_encode_example PRIVATE k-hhc)
target_compile_features(hhc_encode_example PRIVATE cxx_std_23)

add_executable(hhc_decode_example decode_example.cpp)
target_link_libraries(hhc_decode_example PRIVATE k-hhc)
target_compile_features(hhc_decode_example PRIVATE cxx_std_23)

add_executable(hhc_assert_example assert_example.cpp)
target_link_libraries(hhc_assert_example PRIVATE k-hhc)
target_compile_features(hhc_assert_example PRIVATE cxx_std_23)

