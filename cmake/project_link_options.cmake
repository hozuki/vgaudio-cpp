if (CYGWIN OR MINGW)
    # TODO: does this even make any difference?
    add_link_options(--exclude-all-symbols)
endif ()
