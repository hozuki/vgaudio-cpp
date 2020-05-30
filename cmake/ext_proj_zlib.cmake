if (MSVC)
    # https://stackoverflow.com/a/44500079
    # https://stackoverflow.com/a/43363395
    # https://stackoverflow.com/a/37855224
    ExternalProject_Add(
            ext_proj_zlib
            PREFIX ${PROJECT_SOURCE_DIR}/ext/zlib/build
            SOURCE_DIR ${PROJECT_SOURCE_DIR}/ext/zlib
            BINARY_DIR ${PROJECT_SOURCE_DIR}/ext/zlib/build
            INSTALL_DIR ${PROJECT_SOURCE_DIR}/ext/zlib/build
            EXCLUDE_FROM_ALL TRUE

            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_SOURCE_DIR}/ext/zlib/build
            # Force removing CMAKE_BUILD_TYPE, or it will be unexpectedly truncated by the first semicolon.
            # E.g. passing "-DCMAKE_BUILD_TYPE:STRING=Debug;Release;MinSizeRel;RelWithDebInfo" (default value)
            #   will actually set CMAKE_BUILD_TYPE=Debug. Check `cmake -L -N .` to confirm.
            -DCMAKE_BUILD_TYPE:STRING=
            # Generate zlib1.dll (not zlib.dll)
            -DBUILD_SHARED_LIBS:BOOL=TRUE

            # Use external project's source dir (${CMAKE_CURRENT_SOURCE_DIR}), not this project's
            BUILD_IN_SOURCE 0
            # Only build once; manual cleanup required
            BUILD_ALWAYS 0

            BUILD_COMMAND
            cmake
            --build .
            --config Release
    )

    add_dependencies(vgaudio ext_proj_zlib)
    add_dependencies(vgaudio_static ext_proj_zlib)

    target_include_directories(
            vgaudio
            PUBLIC ${PROJECT_SOURCE_DIR}/ext/zlib/build/include
    )
    target_include_directories(
            vgaudio_static
            PUBLIC ${PROJECT_SOURCE_DIR}/ext/zlib/build/include
    )

    target_link_directories(
            vgaudio
            PUBLIC ${PROJECT_SOURCE_DIR}/ext/zlib/build/lib
    )
    target_link_directories(
            vgaudio_static
            PUBLIC ${PROJECT_SOURCE_DIR}/ext/zlib/build/lib
    )

    target_link_libraries(
            vgaudio
            PUBLIC zlibstatic
    )
    target_link_libraries(
            vgaudio_static
            PUBLIC zlibstatic
    )
else ()
    # List taken from FindZLIB
    # Perform static linking on Windows, dynamic linking otherwise because zlib is a system component on those OSs
    if (WIN32)
        set(ZLIB_NAMES zlibstatic z zlib zdll zlib1)
    else ()
        set(ZLIB_NAMES z zlib zdll zlib1 zlibstatic)
    endif ()

    find_library(ZLIB_LIBRARY_RELEASE NAMES ${ZLIB_NAMES})

    if (NOT ZLIB_LIBRARY_RELEASE)
        message(FATAL_ERROR "zlib is not found")
    endif ()

    target_link_libraries(
            vgaudio
            PUBLIC ${ZLIB_LIBRARY_RELEASE}
    )
    target_link_libraries(
            vgaudio_static
            PUBLIC ${ZLIB_LIBRARY_RELEASE}
    )
endif ()
