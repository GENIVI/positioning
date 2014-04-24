set(DLT_INCLUDE_DIRS /usr/include/dlt)
set(DLT_LIBRARIES dlt)
set(DLT_LIBRARY_DIRS /usr/lib)

find_package(PkgConfig)
pkg_check_modules(DLT REQUIRED automotive-dlt)

if(${DLT_FOUND})
    #message(STATUS "found and use automotive-dlt: version ${DLT_VERSION}")
else()
    message("missing DLT - check with 'pkg-config automotive-dlt --cflags-only-I'")
endif()
