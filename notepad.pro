QT       += core gui widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG -= precompile_header

TARGET   = Notepad
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    finddialog.cpp \
    pluginmanager.cpp \
    spellchecker.cpp

HEADERS += \
    mainwindow.h \
    finddialog.h \
    pluginmanager.h \
    spellchecker.h \
    pybind_include.h

# ── Python 3.12 & pybind11 (via pip) ─────────────────────────────────────
win32 {
    PYTHON_DIR     = C:/Python312
    PYBIND11_DIR   = C:/Python312/Lib/site-packages/pybind11/include
    PYTHON_VERSION = 312

    INCLUDEPATH += $${PYTHON_DIR}/include
    INCLUDEPATH += $${PYBIND11_DIR}

    LIBS += -L$${PYTHON_DIR}/libs -lpython$${PYTHON_VERSION}

    QMAKE_CXXFLAGS         += /Zc:__cplusplus /permissive-
    QMAKE_CXXFLAGS_DEBUG   += /Zc:__cplusplus /permissive-
    QMAKE_CXXFLAGS_RELEASE += /Zc:__cplusplus /permissive-

    QMAKE_CXXFLAGS_DEBUG += /D "Py_ENABLE_SHARED"

    # Source dir where plugins folder actually lives (next to .pro file)
    SRC_DIR = D:/Notepad__/PyNotepad

    CONFIG(debug, debug|release) {
        DEST_DIR = $$OUT_PWD/debug
    } else {
        DEST_DIR = $$OUT_PWD/release
    }

    # Use SRC_DIR explicitly so xcopy finds the plugins folder
    QMAKE_POST_LINK += \
        $(MKDIR) \"$$shell_path($$DEST_DIR/plugins)\" $$escape_expand(\n\t) \
        xcopy /s /q /y /i \"$$shell_path($${SRC_DIR}/plugins)\" \"$$shell_path($$DEST_DIR/plugins)\"
}
