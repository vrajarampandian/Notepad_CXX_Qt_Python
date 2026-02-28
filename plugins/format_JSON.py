# Plugin: Format JSON
# Pretty-prints valid JSON with 4-space indentation.
# If the text is not valid JSON, output is left unchanged
# and a clear message is shown at the top.

import json

text = input_text.strip()

if not text:
    output_text = input_text
else:
    try:
        parsed      = json.loads(text)
        output_text = json.dumps(parsed, indent=4, ensure_ascii=False)
    except json.JSONDecodeError as e:
        # Leave the original text untouched, show error at top
        output_text = (
            f"[Format JSON] The text is not valid JSON.\n"
            f"Reason: {e}\n"
            f"{'='*40}\n"
            f"{input_text}"
        )
