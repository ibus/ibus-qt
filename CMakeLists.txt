cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# ======
# Project information
project(ibus-qt)
set(AUTHORS "Peng Huang")
set(MAINTAINER "Peng Huang <shawn.p.huang at gmail dot com>")
set(VERSION_MAJOR "1")
set(VERSION_MINOR "3")
set(VERSION_PATCH "2")
set(VERSION_SNAPDATE "")
set(LIB_VERSION "1.3.0")
set(LIB_SOVERSION "1")

# ======
# CPack configure
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
if(NOT VERSION_SNAPDATE)
    set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
else(NOT VERSION_SNAPDATE)
    set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH}.${VERSION_SNAPDATE})
endif(NOT VERSION_SNAPDATE)

set(CPACK_SET_DESTDIR "ON")
set(CPACK_INSTALL_PREFIX "/usr")
set(CPACK_GENERATOR "TGZ")
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/COPYING)
set(CPACK_SOURCE_IGNORE_FILES
    "/\\\\.git/"
    "\\\\.gitignore$"
    "/CMakeFiles/"
    "/_CPack_Packages/"
    "cmake_install.cmake"
    "CMakeCache.txt$"
    "install_manifest.txt$"
    "Makefile$"
    "Source\\\\.tar\\\\.gz$"
    "Linux\\\\.tar\\\\.gz$"
    "moc_.*\\\\.cxx$"
    "\\\\.so$"
    "\\\\.so\\\\.*"
    "ibus-test$"
    "/rpm/"
    "ibus-qt\\\\.spec$"
    "CPackSourceConfig\\\\.cmake$"
    "CPackConfig\\\\.cmake$"
    "docs/Doxyfile$"
    "docs/man"
    "docs/html"
    "build/"
)
include(CPack)

configure_file(ibus-qt.spec.in ibus-qt.spec)
configure_file(docs/Doxyfile.in docs/Doxyfile)

# ======
# Requires
# check X11
find_package(X11 REQUIRED)

# check qt
find_package(Qt4 4.5 COMPONENTS QtCore QtGui QtDBus QtXml REQUIRED)
include(${QT_USE_FILE})

# check dbus
find_package(PkgConfig REQUIRED)
find_package(ICU REQUIRED)
pkg_check_modules(DBUS REQUIRED dbus-1)
pkg_check_modules(IBUS REQUIRED ibus-1.0>=1.3.7)

# check doxygen
find_package(Doxygen REQUIRED)

# =====
# Define variables
if(NOT LIBDIR)
    if( $ENV{MACHTYPE} MATCHES "64")
        set(LIBDIR lib64)
    else()
        set(LIBDIR lib)
    endif()
endif(NOT LIBDIR)

if(NOT DOCDIR)
    set(DOCDIR "share/doc/ibus-qt-${CPACK_PACKAGE_VERSION}")
endif(NOT DOCDIR)

if(NOT MANDIR)
    set(MANDIR "share/man")
endif(NOT MANDIR)

# =====
# install
set(MAIN_DOCS
    AUTHORS
    README
    INSTALL)
install(FILES ${MAIN_DOCS}
    DESTINATION ${DOCDIR})



# ======
# sub dirs
add_subdirectory(src)
add_subdirectory(qtim)
add_subdirectory(test)
add_subdirectory(docs)

# ======
# targets
add_custom_target(version echo ${CPACK_PACKAGE_VERSION})
add_custom_target(gitclean git clean -f -d -X)
add_custom_target(rpm
    COMMAND make package_source
    COMMAND rpmbuild -bb
        --define "_sourcedir ${CMAKE_CURRENT_BINARY_DIR}"
        --define "_builddir ${CMAKE_CURRENT_BINARY_DIR}/rpm"
        --define "_srcrpmdir ${CMAKE_CURRENT_BINARY_DIR}/rpm"
        --define "_rpmdir ${CMAKE_CURRENT_BINARY_DIR}/rpm"
        --define "_specdir ${CMAKE_CURRENT_BINARY_DIR}/rpm"
        ibus-qt.spec
    SOURCES ibus-qt.spec
    )
add_custom_target(srpm
    COMMAND make package_source
    COMMAND rpmbuild -bs
        --define "_sourcedir ${CMAKE_CURRENT_BINARY_DIR}"
        --define "_builddir ${CMAKE_CURRENT_BINARY_DIR}/rpm"
        --define "_srcrpmdir ${CMAKE_CURRENT_BINARY_DIR}/rpm"
        --define "_rpmdir ${CMAKE_CURRENT_BINARY_DIR}/rpm"
        --define "_specdir ${CMAKE_CURRENT_BINARY_DIR}/rpm"
        ibus-qt.spec
    SOURCES ibus-qt.spec
    )
