cmake_minimum_required(VERSION 3.0)

project(waveshare)

file(GLOB CPP *.cpp)
file(GLOB HDR *.h)
find_package(Threads)
find_package(PkgConfig)

pkg_search_module(EVDEV REQUIRED libevdev)
pkg_search_module(UDEV REQUIRED libudev)

include_directories( ${EVDEV_INCLUDE_DIRS} ${UDEV_INCLUDE_DIRS} )
add_executable(waveshare ${CPP} ${HDR})
target_link_libraries(waveshare ${EVDEV_LIBRARIES} ${UDEV_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
install(
  TARGETS waveshare
  DESTINATION /usr/bin
)
