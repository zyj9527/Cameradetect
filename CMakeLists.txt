cmake_minimum_required(VERSION 2.8)
project(gstreamer)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/Modules)

find_package(GStreamer REQUIRED)
find_package(GLIB REQUIRED)
include_directories(${GSTREAMER_INCLUDE_DIRS} ${GLIB_INCLUDE_DIR} ${GLIBCONFIG_INCLUDE_DIR})
include_directories($ENV{OECORE_TARGET_SYSROOT}/usr/include/gio-unix-2.0)
include_directories($ENV{OECORE_TARGET_SYSROOT}/usr/include/libxml2)
include_directories(.)
message(GLIB_INCLUDE_DIR ${GLIB_INCLUDE_DIR})
aux_source_directory(. SOURCE_FILES )
add_executable(gstreamer ${SOURCE_FILES} )

target_link_libraries(gstreamer ${GSTREAMER_LIBRARIES} ${GLIB_LIBRARIES} libgobject-2.0.so  libgstvideo-1.0.so libpng.so libgio-2.0.so)