cmake_minimum_required(VERSION 3.2)
project(psvkanuefb)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(WEBKIT2 REQUIRED webkit2gtk-4.0)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
include_directories(${GTK3_INCLUDE_DIRS} ${WEBKIT2_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${WEBKIT2_LIBRARY_DIRS} ${SQLITE3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER} ${WEBKIT2_CFLAGS_OTHER})

set(SOURCE_FILES
    calendar.c
    calendar.h
    event.c
    event.h
    efb.c
    efb.h
    main.c)

add_executable(psvkanuefb ${SOURCE_FILES})

target_link_libraries(psvkanuefb ${GTK3_LIBRARIES} ${WEBKIT2_LIBRARIES} ${SQLITE3_LIBRARIES})