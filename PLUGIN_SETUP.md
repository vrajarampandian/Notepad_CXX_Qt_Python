# Qt Notepad + Python Plugin Setup Guide

## Project Structure

    notepad/
    ├── notepad.pro
    ├── main.cpp
    ├── mainwindow.h
    ├── mainwindow.cpp
    ├── finddialog.h
    ├── finddialog.cpp
    ├── pluginmanager.h
    ├── pluginmanager.cpp
    ├── pybind11/              ← clone this (step 1)
    └── plugins/
        ├── word_count.py
        ├── remove_duplicates.py
        └── format_JSON.py

## Step 1 — Get pybind11

Open PowerShell in your project folder and run:

    git clone https://github.com/pybind/pybind11.git

## Step 2 — Verify Python 3.12 path in notepad.pro

    PYTHON_DIR     = C:/Python312
    PYTHON_VERSION = 312

To confirm your Python path, run in PowerShell:
    python -c "import sys; print(sys.prefix)"

## Step 3 — Build in Qt Creator

    1. Open notepad.pro in Qt Creator
    2. Build → Clean All
    3. Build → Run qmake
    4. Press Ctrl+R

## Step 4 — Copy plugins folder next to the .exe

After building, your output folder looks like:

    build-.../release/
    ├── Notepad.exe
    └── plugins/
        ├── word_count.py
        ├── remove_duplicates.py
        └── format_JSON.py

## Using Plugins

Click the Plugins menu in the app to see all available plugins.
Click any plugin to run it on the current document text.
Click "Reload Plugins" to pick up newly added plugin files.

## Writing Your Own Plugin

Create a .py file in the plugins/ folder.
Use input_text to read editor content.
Set output_text with your result.

Example — sort_lines.py:
    lines = input_text.splitlines()
    lines.sort()
    output_text = "\n".join(lines)
