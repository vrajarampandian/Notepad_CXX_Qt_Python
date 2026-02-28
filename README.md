# \# Qt Notepad

# 

# A modern multi-tab text editor built with \*\*C++ / Qt 6\*\* featuring an embedded \*\*Python plugin system\*\* and \*\*real-time spell checking\*\*.

# 

# ---

# 

# \## Screenshots

# 

# > File → New opens a new tab. Each tab is independent with its own spell checker.

# 

# ---

# 

# \## Features

# 

# \### Editor

# \- Multi-tab editing — open multiple files simultaneously

# \- Tabs are closable (`Ctrl+W`) and draggable to reorder

# \- Unsaved changes indicator (`\*` prefix on tab title)

# \- Full path shown as tab tooltip on hover

# \- Word wrap toggle

# \- Custom font selection (applies to all tabs)

# \- Line \& column display in status bar

# 

# \### File Operations

# \- New (`Ctrl+N`) — opens a new empty tab

# \- Open (`Ctrl+O`) — opens file(s); detects if already open and switches to existing tab

# \- Save (`Ctrl+S`) / Save As (`Ctrl+Shift+S`)

# \- Close Tab (`Ctrl+W`) — prompts to save if unsaved changes exist

# \- Exit — checks all tabs for unsaved changes before closing

# 

# \### Edit

# \- Undo / Redo (`Ctrl+Z` / `Ctrl+Y`)

# \- Cut / Copy / Paste / Select All

# \- Find \& Replace (`Ctrl+F`) with case-sensitive and whole-word options

# 

# \### Spell Check (Tools menu)

# \- Real-time red wavy underline on misspelled words

# \- \*\*Debounced\*\* — waits 600ms after typing before checking (keeps typing fast)

# \- \*\*Word cache\*\* — each word is sent to Python only once per session

# \- Right-click a misspelled word to see suggestions and auto-replace

# \- Toggle on/off via \*\*Tools → Spell Check\*\*

# \- Applies independently to every tab

# 

# \### Python Plugin System (Plugins menu)

# \- Plugins are plain `.py` files dropped in the `plugins/` folder

# \- No recompilation needed — click \*\*Reload Plugins\*\* to pick up new ones

# \- Each plugin receives `input\_text` (editor content) and sets `output\_text`

# 

# ---

# 

# \## Built-in Plugins

# 

# | Plugin | Description |

# |---|---|

# | `word\_count.py` | Appends character, word and line count summary |

# | `remove\_duplicates.py` | Removes duplicate lines, preserves order |

# | `format\_JSON.py` | Pretty-prints JSON with 4-space indentation |

# | `text\_to\_JSON.py` | Converts plain text lines into a JSON array |

# | `pretty\_print\_XML.py` | Pretty-prints XML with 4-space indentation |

# | `autocorrect\_spelling.py` | Auto-corrects misspelled words in entire document |

# 

# ---

# 

# \## Requirements

# 

# | Tool | Version |

# |---|---|

# | Qt | 6.x (MSVC kit) |

# | Visual Studio | 2022 |

# | Python | 3.12 |

# | pybind11 | 3.0+ (via pip) |

# | pyspellchecker | latest (via pip) |

# 

# ---

# 

# \## Setup \& Build

# 

# \### Step 1 — Install Python packages

# ```powershell

# C:\\Python312\\python.exe -m pip install pybind11

# C:\\Python312\\python.exe -m pip install pyspellchecker

# ```

# 

# \### Step 2 — Verify pybind11 include path

# ```powershell

# C:\\Python312\\python.exe -c "import pybind11; print(pybind11.get\_include())"

# \# Should print: C:\\Python312\\Lib\\site-packages\\pybind11\\include

# ```

# 

# \### Step 3 — Open project in Qt Creator

# 1\. Open `notepad.pro` in Qt Creator

# 2\. Select the \*\*Desktop Qt 6.x MSVC2022 64bit\*\* kit

# 3\. \*\*Build → Clean All\*\*

# 4\. \*\*Build → Run qmake\*\*

# 5\. Press `Ctrl+R` to build and run

# 

# \### Step 4 — Copy runtime files (first time only)

# Copy these into your `release/` and `debug/` build folders next to `Notepad.exe`:

# ```

# C:\\Python312\\python312.dll   →   release\\python312.dll

# C:\\Python312\\Lib\\           →   release\\Lib\\

# C:\\Python312\\DLLs\\          →   release\\DLLs\\

# ```

# > The `plugins/` folder is copied automatically by the build step.

# 

# ---

# 

# \## Project Structure

# 

# ```

# notepad/

# ├── notepad.pro              # Qt project file

# ├── main.cpp                 # Entry point

# ├── mainwindow.h/.cpp        # Main window — tabs, menus, file ops

# ├── finddialog.h/.cpp        # Find \& Replace dialog

# ├── pluginmanager.h/.cpp     # Python plugin loader \& runner

# ├── spellchecker.h/.cpp      # Real-time spell checker (QSyntaxHighlighter)

# ├── pybind\_include.h         # MSVC-safe pybind11 include wrapper

# ├── pybind11/                # pybind11 headers (pip-installed)

# └── plugins/

# &nbsp;   ├── word\_count.py

# &nbsp;   ├── remove\_duplicates.py

# &nbsp;   ├── format\_JSON.py

# &nbsp;   ├── text\_to\_JSON.py

# &nbsp;   ├── pretty\_print\_XML.py

# &nbsp;   └── autocorrect\_spelling.py

# ```

# 

# ---

# 

# \## Writing Your Own Plugin

# 

# Create a `.py` file in the `plugins/` folder. Use `input\_text` to read the

# editor content and set `output\_text` with your result.

# 

# ```python

# \# plugins/sort\_lines.py

# \# Sorts all lines alphabetically

# 

# lines = input\_text.replace('\\r\\n', '\\n').split('\\n')

# lines.sort()

# output\_text = '\\n'.join(lines)

# ```

# 

# Click \*\*Plugins → Reload Plugins\*\* in the app to pick it up — no recompilation needed.

# 

# ---

# 

# \## Keyboard Shortcuts

# 

# | Action | Shortcut |

# |---|---|

# | New Tab | `Ctrl+N` |

# | Open | `Ctrl+O` |

# | Save | `Ctrl+S` |

# | Save As | `Ctrl+Shift+S` |

# | Close Tab | `Ctrl+W` |

# | Undo | `Ctrl+Z` |

# | Redo | `Ctrl+Y` |

# | Cut | `Ctrl+X` |

# | Copy | `Ctrl+C` |

# | Paste | `Ctrl+V` |

# | Select All | `Ctrl+A` |

# | Find \& Replace | `Ctrl+F` |

# | Exit | `Alt+F4` |

# 

# ---

# 

# \## Troubleshooting

# 

# \*\*`python312.dll` not found on startup\*\*

# → Copy `C:\\Python312\\python312.dll` next to `Notepad.exe`

# 

# \*\*Plugins menu shows "No plugins found"\*\*

# → Make sure the `plugins/` folder exists next to `Notepad.exe`

# 

# \*\*Spell check not working\*\*

# → Run `python -m pip install pyspellchecker` and rebuild

# 

# \*\*LNK2038 runtime mismatch error\*\*

# → Make sure `pybind\_include.h` is the \*\*very first\*\* `#include` in every `.cpp` file

# 

# \*\*`object.h` syntax error on build\*\*

# → Make sure you're using pybind11 installed via pip (`pip install pybind11`), not a git clone

# 

# ---

# 

# \## License

# 

# MIT License — free to use and modify.

