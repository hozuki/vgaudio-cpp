if (VGAUDIO_APPS_DYNAMIC_LINKING)
    set(VGAUDIO_APPS_LINK_LIBRARY vgaudio)
else ()
    set(VGAUDIO_APPS_LINK_LIBRARY vgaudio_static)
endif ()

target_link_libraries(
        test_misc
        PUBLIC ${VGAUDIO_APPS_LINK_LIBRARY}
)
target_link_libraries(
        test_deflate
        PUBLIC ${VGAUDIO_APPS_LINK_LIBRARY}
)
target_link_libraries(
        test_wave_io
        PUBLIC ${VGAUDIO_APPS_LINK_LIBRARY}
)
target_link_libraries(
        hcaenc
        PUBLIC ${VGAUDIO_APPS_LINK_LIBRARY}
)
target_link_libraries(
        hcadec
        PUBLIC ${VGAUDIO_APPS_LINK_LIBRARY}
)

if (MSVC OR MINGW OR CYGWIN)
    target_link_libraries(
            vgaudio_static
            PUBLIC ws2_32
    )
    target_link_libraries(
            vgaudio
            PUBLIC ws2_32
    )
endif ()
