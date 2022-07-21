
# if(UNIX AND NOT APPLE)
#  set(MAYBE_RT_LIBRARY -lrt)
# endif()
# ${MAYBE_RT_LIBRARY}

cmake_minimum_required(VERSION 3.1)

project(boost-dev C CXX)

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type specified, defaulting to Release")
  set(CMAKE_BUILD_TYPE Release)
endif()

set(BOOST_COMPONENTS date_time chrono filesystem iostreams program_options regex system thread unit_test_framework)

find_package(Boost 1.54 REQUIRED COMPONENTS ${BOOST_COMPONENTS})

message(STATUS Boost_VERSION=> ${Boost_VERSION})
message(STATUS Boost_FOUND=> ${Boost_FOUND})
message(STATUS Boost_INCLUDE_DIRS=> ${Boost_INCLUDE_DIRS})
message(STATUS Boost_LIBRARY_DIRS=> ${Boost_LIBRARY_DIRS})

include_directories(SYSTEM ${Boost_INCLUDE_DIRS})

set(BOOST_ALL_LIBRARIES 
   ${Boost_DATE_TIME_LIBRARY}
   ${Boost_CHRONO_LIBRARY}
   ${Boost_FILESYSTEM_LIBRARY}
   ${Boost_IOSTREAMS_LIBRARY}
   ${Boost_THREAD_LIBRARY}
   ${Boost_SYSTEM_LIBRARY}
   ${Boost_ZLIB_LIBRARY}
   ${Boost_REGEX_LIBRARY})


add_executable(csv_parser src/csv_parser.cpp)
target_link_libraries(csv_parser ${BOOST_ALL_LIBRARIES})
