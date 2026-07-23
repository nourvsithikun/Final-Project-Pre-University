cmake_minimum_required(VERSION 3.20)

project(TypingSpaceShooter VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

option(TYPING_HEADLESS_TESTS "Build core tests without the graphical raylib application" OFF)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if(NOT TYPING_HEADLESS_TESTS)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
        GIT_SHALLOW TRUE
    )
endif()

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)

if(TYPING_HEADLESS_TESTS)
    FetchContent_MakeAvailable(nlohmann_json)
else()
    FetchContent_MakeAvailable(raylib nlohmann_json)
endif()

set(PROJECT_SOURCES
    src/AssetManager.cpp
    src/Enemy.cpp
    src/EnemyModel.cpp
    src/Game.cpp
    src/Laser.cpp
    src/Leaderboard.cpp
    src/Particle.cpp
    src/Player.cpp
    src/ScoreManager.cpp
    src/Targeting.cpp
    src/WordManager.cpp
)

if(NOT TYPING_HEADLESS_TESTS)
    add_executable(${PROJECT_NAME} src/main.cpp ${PROJECT_SOURCES})
    target_include_directories(${PROJECT_NAME} PRIVATE include)
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib nlohmann_json::nlohmann_json)

    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    if(APPLE)
        set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE FALSE)
    endif()

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_SOURCE_DIR}/assets
                $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
        COMMENT "Copying game assets"
    )
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(CoreTests
        tests/CoreTests.cpp
        src/EnemyModel.cpp
        src/Leaderboard.cpp
        src/ScoreManager.cpp
        src/Targeting.cpp
        src/WordManager.cpp
    )
    target_include_directories(CoreTests PRIVATE include)
    target_link_libraries(CoreTests PRIVATE nlohmann_json::nlohmann_json)
    if(TYPING_HEADLESS_TESTS)
        target_include_directories(CoreTests PRIVATE tests/stubs)
    else()
        target_link_libraries(CoreTests PRIVATE raylib)
    endif()
    add_test(NAME core_logic COMMAND CoreTests)
    set_tests_properties(core_logic PROPERTIES WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()
