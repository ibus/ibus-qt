include_directories(
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR}
    ${DBUS_INCLUDE_DIRS}
)

link_directories(
    ${DBUS_LIBRARY_DIRS}
)

set(
    ibus_qt_SRCS
    qibusobject.cpp
    qibusserializable.cpp
    qibusattribute.cpp
    qibusattrlist.cpp
    qibusproperty.cpp
    qibusproplist.cpp
    qibustext.cpp
    qibuslookuptable.cpp
    qibusenginedesc.cpp
    qibuscomponent.cpp
    qibusibusproxy.cpp
    qibusdbusproxy.cpp
    qibusinputcontextproxy.cpp
    qibusbus.cpp
    qibusinputcontext.cpp
    qibusobservedpath.cpp
    qibusengineadaptor.cpp
    qibusengine.cpp
    qibusfactoryadaptor.cpp
    qibusenginefactory.cpp
    qibusconfigadaptor.cpp
    qibusconfigproxy.cpp
    qibusconfigservice.cpp
    qibusconfig.cpp
)

set(
    ibus_qt_HDRS
    qibusobject.h
    qibusserializable.h
    qibusattribute.h
    qibusattrlist.h
    qibusproperty.h
    qibusproplist.h
    qibustext.h
    qibuslookuptable.h
    qibusenginedesc.h
    qibuscomponent.h
    qibusibusproxy.h
    qibusdbusproxy.h
    qibusinputcontextproxy.h
    qibusbus.h
    qibusinputcontext.h
    qibusobservedpath.h
    qibusengineadaptor.h
    qibusengine.h
    qibusfactoryadaptor.h
    qibusenginefactory.h
    qibusconfigadaptor.h
    qibusconfigproxy.h
    qibusconfigservice.h
    qibusconfig.h
)

# QT4_ADD_DBUS_INTERFACE(
#     ibus_qt_SRCS
#     ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/org.freedesktop.IBus.xml
#     basename
# )
# 
# QT4_AUTOMOC(${ibus_qt_SRCS})
QT4_WRAP_CPP(
    ibus_qt_MOC_SRCS
    ${ibus_qt_HDRS}
)

add_library(
    ibus-qt
    SHARED
    ${ibus_qt_SRCS}
    ${ibus_qt_MOC_SRCS}
)

set_target_properties(
    ibus-qt PROPERTIES
    VERSION ${LIB_VERSION}
    SOVERSION ${LIB_SOVERSION}
)

target_link_libraries(
    ibus-qt
    debug ${QT_QTCORE_LIBRARY_DEBUG}
    debug ${QT_QTGUI_LIBRARY_DEBUG}
    debug ${QT_QTDBUS_LIBRARY_DEBUG}
    debug ${QT_QTXML_LIBRARY_DEBUG}
    optimized ${QT_QTCORE_LIBRARY_RELEASE}
    optimized ${QT_QTGUI_LIBRARY_RELEASE}
    optimized ${QT_QTDBUS_LIBRARY_RELEASE}
    optimized ${QT_QTXML_LIBRARY_RELEASE}
    ${DBUS_LIBRARIES}
    ${X11_LIBRARIES}
)

install(TARGETS ibus-qt
    LIBRARY DESTINATION ${LIBDIR})
install(FILES ${ibus_qt_HDRS}
    DESTINATION include/ibus-qt COMPONENT Devel)

add_executable(
    ibus-test
    ibus.cpp
)

target_link_libraries(
    ibus-test
    ibus-qt
)

# install(TARGETS ibus-test
#     RUNTIME DESTINATION bin)
