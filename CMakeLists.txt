cmake_minimum_required(VERSION 3.16)
project(WhiteBearGreenWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output binary to /bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find SDL2 packages (prefer config-mode, fall back to pkg-config when needed)
find_package(SDL2 REQUIRED)
find_package(SDL2_image QUIET)
find_package(SDL2_mixer QUIET)
find_package(SDL2_ttf QUIET)

# Find nlohmann_json for JSON parsing
find_package(nlohmann_json REQUIRED)

find_package(PkgConfig QUIET)

# SDL2_image fallback to pkg-config
if(NOT TARGET SDL2_image::SDL2_image)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
        set(SDL2_IMAGE_INCLUDE_DIRS ${SDL2_IMAGE_INCLUDE_DIRS})
        set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARIES})
    else()
        message(FATAL_ERROR "SDL2_image not found; install libsdl2-image-dev or provide SDL2_imageConfig.cmake")
    endif()
else()
    set(SDL2_IMAGE_TARGET SDL2_image::SDL2_image)
endif()

# SDL2_mixer fallback to pkg-config
if(NOT TARGET SDL2_mixer::SDL2_mixer)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
        set(SDL2_MIXER_INCLUDE_DIRS ${SDL2_MIXER_INCLUDE_DIRS})
        set(SDL2_MIXER_LIBRARIES ${SDL2_MIXER_LIBRARIES})
    else()
        message(FATAL_ERROR "SDL2_mixer not found; install libsdl2-mixer-dev or provide SDL2_mixerConfig.cmake")
    endif()
else()
    set(SDL2_MIXER_TARGET SDL2_mixer::SDL2_mixer)
endif()

# SDL2_ttf fallback to pkg-config
if(NOT TARGET SDL2_ttf::SDL2_ttf)
    if(PkgConfig_FOUND)
        pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
        set(SDL2_TTF_INCLUDE_DIRS ${SDL2_TTF_INCLUDE_DIRS})
        set(SDL2_TTF_LIBRARIES ${SDL2_TTF_LIBRARIES})
    else()
        message(FATAL_ERROR "SDL2_ttf not found; install libsdl2-ttf-dev or provide SDL2_ttfConfig.cmake")
    endif()
else()
    set(SDL2_TTF_TARGET SDL2_ttf::SDL2_ttf)
endif()

file(GLOB_RECURSE SRC_FILES
    src/*.cpp
)

add_executable(polar_bear ${SRC_FILES})

# On macOS with Homebrew, SDL2 include dirs point directly to SDL2 headers (e.g., /opt/homebrew/include/SDL2)
# We need to also add the parent directory so that #include <SDL2/SDL.h> works
# This maintains compatibility with Linux where includes are typically /usr/include
if(APPLE)
    # For imported targets, we need to get the include directories from the target properties
    # For pkg-config, we use the variables directly
    set(ALL_SDL2_INCLUDES "")

    # SDL2 base
    if(TARGET SDL2::SDL2)
        get_target_property(_sdl2_inc SDL2::SDL2 INTERFACE_INCLUDE_DIRECTORIES)
        if(_sdl2_inc)
            list(APPEND ALL_SDL2_INCLUDES ${_sdl2_inc})
        endif()
    endif()
    list(APPEND ALL_SDL2_INCLUDES ${SDL2_INCLUDE_DIRS})

    # SDL2_image
    if(TARGET SDL2_image::SDL2_image)
        get_target_property(_img_inc SDL2_image::SDL2_image INTERFACE_INCLUDE_DIRECTORIES)
        if(_img_inc)
            list(APPEND ALL_SDL2_INCLUDES ${_img_inc})
        endif()
    endif()
    list(APPEND ALL_SDL2_INCLUDES ${SDL2_IMAGE_INCLUDE_DIRS})

    # SDL2_mixer
    if(TARGET SDL2_mixer::SDL2_mixer)
        get_target_property(_mix_inc SDL2_mixer::SDL2_mixer INTERFACE_INCLUDE_DIRECTORIES)
        if(_mix_inc)
            list(APPEND ALL_SDL2_INCLUDES ${_mix_inc})
        endif()
    endif()
    list(APPEND ALL_SDL2_INCLUDES ${SDL2_MIXER_INCLUDE_DIRS})

    # SDL2_ttf
    if(TARGET SDL2_ttf::SDL2_ttf)
        get_target_property(_ttf_inc SDL2_ttf::SDL2_ttf INTERFACE_INCLUDE_DIRECTORIES)
        if(_ttf_inc)
            list(APPEND ALL_SDL2_INCLUDES ${_ttf_inc})
        endif()
    endif()
    list(APPEND ALL_SDL2_INCLUDES ${SDL2_TTF_INCLUDE_DIRS})

    # Remove empty entries and duplicates
    list(REMOVE_DUPLICATES ALL_SDL2_INCLUDES)
    list(FILTER ALL_SDL2_INCLUDES EXCLUDE REGEX "^$")
    list(FILTER ALL_SDL2_INCLUDES EXCLUDE REGEX ".*-NOTFOUND$")

    # Get parent directories for SDL2-style includes
    foreach(inc_dir ${ALL_SDL2_INCLUDES})
        get_filename_component(parent_dir "${inc_dir}" DIRECTORY)
        if(parent_dir)
            list(APPEND SDL2_PARENT_DIRS "${parent_dir}")
        endif()
    endforeach()
    if(SDL2_PARENT_DIRS)
        list(REMOVE_DUPLICATES SDL2_PARENT_DIRS)
    endif()
endif()

target_include_directories(polar_bear PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
    ${SDL2_PARENT_DIRS}
    src
)

target_link_libraries(polar_bear PRIVATE SDL2::SDL2)

if(TARGET SDL2_image::SDL2_image)
    target_link_libraries(polar_bear PRIVATE SDL2_image::SDL2_image)
else()
    target_link_libraries(polar_bear PRIVATE ${SDL2_IMAGE_LIBRARIES})
endif()

if(TARGET SDL2_mixer::SDL2_mixer)
    target_link_libraries(polar_bear PRIVATE SDL2_mixer::SDL2_mixer)
else()
    target_link_libraries(polar_bear PRIVATE ${SDL2_MIXER_LIBRARIES})
endif()

if(TARGET SDL2_ttf::SDL2_ttf)
    target_link_libraries(polar_bear PRIVATE SDL2_ttf::SDL2_ttf)
else()
    target_link_libraries(polar_bear PRIVATE ${SDL2_TTF_LIBRARIES})
endif()

# Link nlohmann_json
target_link_libraries(polar_bear PRIVATE nlohmann_json::nlohmann_json)


# macOS: needed to find dylibs at runtime
if(APPLE)
    set_target_properties(polar_bear PROPERTIES
        INSTALL_RPATH "@executable_path"
    )
endif()
