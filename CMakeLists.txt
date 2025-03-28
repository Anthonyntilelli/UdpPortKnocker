cmake_minimum_required(VERSION 3.20)

project(
    UdpKnocker
    VERSION 1.0
    DESCRIPTION "Udp Port Knocker"
    HOMEPAGE_URL "https://github.com/Anthonyntilelli/UdpPortKnocker"
    LANGUAGES CXX
)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(main1 "src/main.cpp")

# set{MY_VAR "This is my value"}
# message("Comment goes here: ${MY_VAR}")

# include (CTest)
# enable_testing()

add_subdirectory(src)