cmake_minimum_required(VERSION 3.16)
project(TlogViewer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Quick Qml Location Positioning QuickControls2)

qt_add_executable(tlog-viewer
    src/main.cpp
    src/TlogParser.cpp
    src/MessageListModel.cpp
    src/AppController.cpp
)

target_include_directories(tlog-viewer PRIVATE src)

qt_add_qml_module(tlog-viewer
    URI TlogViewer
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/WelcomePage.qml
        qml/MainPage.qml
        qml/MapView.qml
        qml/RawDataView.qml
        qml/TimelineControl.qml
)

target_link_libraries(tlog-viewer PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    Qt6::Location
    Qt6::Positioning
    Qt6::QuickControls2
)

set_target_properties(tlog-viewer PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

get_target_property(_qt_core_location Qt6::Core LOCATION)
get_filename_component(_qt_lib_dir "${_qt_core_location}" DIRECTORY)
set_target_properties(tlog-viewer PROPERTIES
    INSTALL_RPATH "${_qt_lib_dir}"
    BUILD_RPATH   "${_qt_lib_dir}"
)

include(CTest)
add_subdirectory(tests)
