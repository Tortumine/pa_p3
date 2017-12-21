cmake_minimum_required(VERSION 3.8)
project(pa_p3)

set(CMAKE_C_STANDARD 11)

set(COMIC_SOURCE_FILES
        src/Comic.c
        src/Comic.h
        src/PNM.c
        src/PNM.h
        src/mainComic.c)

set(SEAMCARVING_SOURCE_FILES
        src/SeamCarving.c
        src/SeamCarving.h
        src/PNM.c
        src/PNM.h
        src/mainSeamCarving.c)

set(SOURCE_FILES
        src/SeamCarving.c
        src/SeamCarving.h
        src/Comic.c
        src/Comic.h
        src/PNM.c
        src/PNM.h
        src/mainComic.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --pedantic -Wall -Wextra -Wmissing-prototypes")

add_executable(mainComic ${SOURCE_FILES})
add_executable(mainSeamCarving ${SEAMCARVING_SOURCE_FILES})
