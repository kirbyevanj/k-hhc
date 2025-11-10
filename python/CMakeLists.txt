cmake_minimum_required(VERSION 3.20)

# Find Python and pybind11
find_package(Python COMPONENTS Interpreter Development REQUIRED)
find_package(pybind11 REQUIRED)

# Create the Python module
pybind11_add_module(k_hhc hhc_python.cpp)

# Link against the header-only library
target_link_libraries(k_hhc PRIVATE k-hhc)

# Set C++ standard
target_compile_features(k_hhc PRIVATE cxx_std_23)

# Set output name (removes any lib prefix)
set_target_properties(k_hhc PROPERTIES 
    OUTPUT_NAME "k_hhc"
    PREFIX ""
)

# Install the Python module
install(TARGETS k_hhc
    LIBRARY DESTINATION ${Python_SITEARCH}
    RUNTIME DESTINATION ${Python_SITEARCH}
    ARCHIVE DESTINATION ${Python_SITEARCH}
)
