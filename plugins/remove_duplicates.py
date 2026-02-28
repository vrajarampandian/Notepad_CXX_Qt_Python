# Plugin: Remove Duplicates
seen = set()
unique = []
for line in input_text.splitlines():
    if line not in seen:
        seen.add(line)
        unique.append(line)
output_text = "\n".join(unique)
