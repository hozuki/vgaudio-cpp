if (MSVC AND (MSVC_VERSION GREATER 1900 OR MSVC_VERSION EQUAL 1900))
    add_compile_options(/permissive-)
endif ()

target_compile_definitions(
        vgaudio
        PUBLIC __BUILDING_DLL__
)
target_compile_definitions(
        vgaudio_static
        PUBLIC __BUILDING_LIB__
)
