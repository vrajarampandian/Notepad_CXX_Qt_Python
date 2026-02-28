# Plugin: Format JSON
import json
try:
    output_text = json.dumps(json.loads(input_text), indent=4, ensure_ascii=False)
except json.JSONDecodeError as e:
    output_text = f"[JSON Error] {e}\n\n--- Original ---\n{input_text}"
