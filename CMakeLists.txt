cmake_minimum_required(VERSION 3.11)

set(PRODUCT_NAME cretro)
string(TOUPPER ${PRODUCT_NAME} MSG_PREFIX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(
    ${PRODUCT_NAME}
    LANGUAGES C
)

set(INSTALL_PATH ~/.local/bin/${PRODUCT_NAME})

include_directories(src)
set(SOURCE_FILES
    src/cli.c
    src/cpu.c
    src/cretro.c
    src/lcd.c
    src/logging.c
    src/sound.c
    src/timer.c
)

add_executable(
    ${PRODUCT_NAME}
    ${SOURCE_FILES}
)
set_property(TARGET ${TARGET_NAME} PROPERTY C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

target_compile_options(${PRODUCT_NAME} PUBLIC
                   # Standard GCC warnings
                   -Wall
                   -Wextra
                   -Werror
                   -Wunused
                   -Wshadow
                   -Wcast-align
                   -Wconversion
                   -Wdouble-promotion
                   -fstack-protector -Wstack-protector
                   -Wredundant-decls
                   -Wpacked
                   -Wmissing-declarations
                   -Wmissing-prototypes
                   -Wstrict-prototypes
                   -Wwrite-strings
                   -Wcast-qual
                   -Wundef
                   -Wfloat-equal
                   -Wstrict-overflow=4
                   -Wswitch-enum
                   -Wswitch-default
                   -Wmissing-include-dirs
                   -Wformat=2)

if (DEFINED OPTIMIZE)
    message("[${UPPER_PRODUCT_NAME}] Using -O${OPTIMIZE}")
    target_compile_options(${PRODUCT_NAME} PUBLIC -O${OPTIMIZE})
endif ()

target_link_libraries(${PRODUCT_NAME} PUBLIC -lm -lSDL2 -pthread)

add_custom_target(install-bin COMMAND /bin/sh -c "cp ${PRODUCT_NAME} ${INSTALL_PATH}\; echo 'Installed ${PRODUCT_NAME} at ${INSTALL_PATH}'" DEPENDS ${PRODUCT_NAME})
add_custom_target(uninstall-bin COMMAND /bin/sh -c "if [[ -f ${INSTALL_PATH} ]]\; then rm ${INSTALL_PATH}\; echo 'Removing ${PRODUCT_NAME} from ${INSTALL_PATH}'\; else echo 'Nothing to uninstall.'\; fi")



################################################################################
###                                Unit Tests                                ###
################################################################################


if (${TEST})
    set(TEST_NAME ${PRODUCT_NAME}_test)

    set(TEST_FILES
        test/test_cpu.c
    )

    set(TEST_SOURCES ${TEST_FILES} ${SOURCE_FILES})
    list(REMOVE_ITEM TEST_SOURCES "src/cretro.c")
    add_executable(${TEST_NAME} ${TEST_SOURCES})

    if (${COVERAGE})
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "GNU")
            message("[${UPPER_PRODUCT_NAME}] Adding coverage flags")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 --coverage")
        else ()
            message(FATAL_ERROR "[${MSG_PREFIX}] GCC is required to output coverage reports")
        endif ()
    endif ()

    target_link_libraries(${TEST_NAME} PUBLIC -lm -lSDL2 -pthread -lcriterion)

    add_custom_target(test COMMAND ./${TEST_NAME} DEPENDS ${TEST_NAME})
endif ()

if (DEFINED SANITIZE)
    if (${SANITIZE} STREQUAL "valgrind")
        message("[${UPPER_PRODUCT_NAME}] Adding no compile flags for ${SANITIZE}")

        if (${TEST})
            add_custom_target(sanitize COMMAND valgrind --leak-check=yes --trace-children=yes ./${TEST_NAME} DEPENDS ${TEST_NAME})
        else ()
            add_custom_target(sanitize COMMAND valgrind --leak-check=yes --trace-children=yes ./${PRODUCT_NAME} DEPENDS ${PRODUCT_NAME})
        endif ()
    endif ()
endif ()
