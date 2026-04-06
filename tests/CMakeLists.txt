cmake_minimum_required(VERSION 3.16)

find_package(Qt6 REQUIRED COMPONENTS Core Test Positioning)

enable_testing()

set(SRC "${CMAKE_SOURCE_DIR}/src")

# ── tst_tlogparser ────────────────────────────────────────────────────────────
qt_add_executable(tst_tlogparser
    tst_tlogparser.cpp
    ${SRC}/TlogParser.cpp
)
target_include_directories(tst_tlogparser PRIVATE ${SRC} helpers)
target_link_libraries(tst_tlogparser PRIVATE Qt6::Core Qt6::Test)
add_test(NAME TlogParser COMMAND tst_tlogparser)

# ── tst_messagelistmodel ──────────────────────────────────────────────────────
qt_add_executable(tst_messagelistmodel
    tst_messagelistmodel.cpp
    ${SRC}/MessageListModel.cpp
)
target_include_directories(tst_messagelistmodel PRIVATE ${SRC})
target_link_libraries(tst_messagelistmodel PRIVATE Qt6::Core Qt6::Test)
add_test(NAME MessageListModel COMMAND tst_messagelistmodel)

# ── tst_appcontroller ─────────────────────────────────────────────────────────
qt_add_executable(tst_appcontroller
    tst_appcontroller.cpp
    ${SRC}/AppController.cpp
    ${SRC}/TlogParser.cpp
    ${SRC}/MessageListModel.cpp
)
target_include_directories(tst_appcontroller PRIVATE ${SRC} helpers)
target_link_libraries(tst_appcontroller PRIVATE Qt6::Core Qt6::Test Qt6::Positioning)
add_test(NAME AppController COMMAND tst_appcontroller)
