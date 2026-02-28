# Plugin: Auto Correct Spelling
# Scans the entire document and auto-corrects misspelled words.
# Uses pyspellchecker library.
# Install: pip install pyspellchecker

from spellchecker import SpellChecker
import re

spell = SpellChecker()

def correct_word(match):
    word      = match.group()
    # Preserve leading/trailing punctuation
    stripped  = word.strip(".,!?;:\"'()[]{}")
    corrected = spell.correction(stripped)
    if corrected and corrected != stripped.lower():
        # Try to preserve original casing
        if stripped.isupper():
            corrected = corrected.upper()
        elif stripped[0].isupper():
            corrected = corrected.capitalize()
        return word.replace(stripped, corrected)
    return word

# Normalize line endings
text  = input_text.replace('\r\n', '\n').replace('\r', '\n')
lines = text.split('\n')

corrected_lines = []
for line in lines:
    # Correct each word in the line
    corrected_line = re.sub(r"\b[a-zA-Z]+\b", correct_word, line)
    corrected_lines.append(corrected_line)

output_text = '\n'.join(corrected_lines)
