# Plugin: Word Count
lines = input_text.splitlines()
words = input_text.split()
chars = len(input_text)

summary = (
    "\n\n--- Word Count Summary ---\n"
    f"Characters : {chars}\n"
    f"Words      : {len(words)}\n"
    f"Lines      : {len(lines)}\n"
    "--------------------------"
)
output_text = input_text + summary
