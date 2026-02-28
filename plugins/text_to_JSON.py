# Plugin: Text to JSON
# Converts plain text lines into a JSON array of strings.
# Each line becomes one element in the array.
#
# Input:
#   Hai Hai
#   Hai fdfg fghfgnh
#
# Output:
#   [
#       "Hai Hai",
#       "Hai fdfg fghfgnh"
#   ]

import json

lines = [line.rstrip() for line in input_text.replace('\r\n','\n').split('\n') if line.strip()]
output_text = json.dumps(lines, indent=4, ensure_ascii=False)
