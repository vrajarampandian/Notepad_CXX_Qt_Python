# Qt Notepad

A multi-tab text editor built with **C++ / Qt 6**, embedded **Python plugins** and **real-time spell check**.
---

## Features
- **Multi-tab** editing — open, close (`Ctrl+W`), drag to reorder
- **Find & Replace** with case-sensitive and whole-word options
- **Real-time spell check** — red underline + right-click suggestions
- **Python plugin system** — drop a `.py` file in `plugins/`, no rebuild needed
- Line & column status bar, word wrap, font picker

## Built-in Plugins
| Plugin | What it does |
|---|---|
| `word_count.py` | Character / word / line count |
| `remove_duplicates.py` | Remove duplicate lines |
| `format_JSON.py` | Pretty-print JSON |
| `text_to_JSON.py` | Plain text → JSON array |
| `pretty_print_XML.py` | Pretty-print XML |
| `autocorrect_spelling.py` | Auto-correct entire document |

## Requirements
| Tool | Version |
|---|---|
| Qt | 6.x (MSVC kit) |
| Visual Studio | 2022 |
| Python | 3.12 |

## Setup

```powershell
# 1. Install Python dependencies
python -m pip install pybind11 pyspellchecker

# 2. Open notepad.pro in Qt Creator
# 3. Build → Clean All → Run qmake → Ctrl+R

# 4. Copy runtime files next to Notepad.exe (first time only)
#    C:\Python312\python312.dll
#    C:\Python312\Lib\
#    C:\Python312\DLLs\
```

## Writing a Plugin

```python
# plugins/sort_lines.py
lines = input_text.replace('\r\n', '\n').split('\n')
lines.sort()
output_text = '\n'.join(lines)
```

Drop it in `plugins/` → click **Plugins → Reload Plugins**. Done.

## Shortcuts
`Ctrl+N` New tab · `Ctrl+O` Open · `Ctrl+S` Save · `Ctrl+W` Close tab · `Ctrl+F` Find

## License
MIT
