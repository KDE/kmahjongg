# - Try to find the libkmahjongg library
# Once done this will define
#
#  LIBKMAHJONGG_FOUND - system has the libkmahjongg library
#  LIBKMAHJONGG_INCLUDE_DIR - the libkmahjongg include directory
#  LIBKMAHJONGG_LIBRARY - Link this to use the libkmahjongg library
#
include(CheckLibraryExists)

if (LIBKMAHJONGG_INCLUDE_DIR AND LIBKMAHJONGG_LIBRARY)
    # in cache already
    set(LIBKMAHJONGG_FOUND TRUE)

else (LIBKMAHJONGG_INCLUDE_DIR AND LIBKMAHJONGG_LIBRARY)
    # reset vars
    set(LIBKMAHJONGG_INCLUDE_DIR)
    set(LIBKMAHJONGG_LIBRARY)

    find_path(LIBKMAHJONGG_INCLUDE_DIR kstandardgameaction.h
        ${INCLUDE_INSTALL_DIR} ${KDE4_INCLUDE_DIR} ${GNUWIN32_DIR}/include)
    set(LIBKMAHJONGG_INCLUDE_DIR ${LIBKMAHJONGG_INCLUDE_DIR})

    set(CMAKE_LIBRARY_PATH ${LIB_INSTALL_DIR})
    find_library(LIBKMAHJONGG_LIBRARY NAMES kmahjongglib
        PATHS ${LIB_INSTALL_DIR} ${KDE4_LIB_DIR} ${GNUWIN32_DIR}/lib)
    set(LIBKMAHJONGG_LIBRARY ${LIBKMAHJONGG_LIBRARY})

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(LIBKMAHJONGG DEFAULT_MSG LIBKMAHJONGG_INCLUDE_DIR LIBKMAHJONGG_LIBRARY)

    mark_as_advanced(LIBKMAHJONGG_INCLUDE_DIR LIBKMAHJONGG_LIBRARY)
endif(LIBKMAHJONGG_INCLUDE_DIR AND LIBKMAHJONGG_LIBRARY)
