if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CYGWIN OR MINGW)
    set(GNU_COMPILER 1)
else ()
    set(GNU_COMPILER 0)
endif ()

if (${GNU_COMPILER})
    add_custom_command(
            TARGET vgaudio
            POST_BUILD
            COMMAND ${CMAKE_STRIP} -s $<TARGET_FILE:vgaudio>
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    add_custom_command(
            TARGET test_misc
            POST_BUILD
            COMMAND ${CMAKE_STRIP} -s $<TARGET_FILE:test_misc>
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    add_custom_command(
            TARGET test_deflate
            POST_BUILD
            COMMAND ${CMAKE_STRIP} -s $<TARGET_FILE:test_deflate>
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    add_custom_command(
            TARGET test_wave_io
            POST_BUILD
            COMMAND ${CMAKE_STRIP} -s $<TARGET_FILE:test_wave_io>
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    add_custom_command(
            TARGET hcaenc
            POST_BUILD
            COMMAND ${CMAKE_STRIP} -s $<TARGET_FILE:hcaenc>
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    add_custom_command(
            TARGET hcadec
            POST_BUILD
            COMMAND ${CMAKE_STRIP} -s $<TARGET_FILE:hcadec>
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
endif ()
