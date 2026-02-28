# Plugin: Pretty Print XML
# Formats and indents XML content with 4-space indentation.
# Works with both compact and already-formatted XML.
# Uses only Python standard library (no extra installs needed).
#
# Example input:
#   <root><person><name>John</name><age>30</age></person></root>
#
# Example output:
#   <?xml version="1.0" ?>
#   <root>
#       <person>
#           <name>John</name>
#           <age>30</age>
#       </person>
#   </root>

import xml.dom.minidom

text = input_text.strip()

if not text:
    output_text = input_text
else:
    try:
        dom    = xml.dom.minidom.parseString(text.encode('utf-8'))
        pretty = dom.toprettyxml(indent='    ', encoding=None)

        # minidom adds extra blank lines — remove them for clean output
        lines   = pretty.split('\n')
        cleaned = '\n'.join(line for line in lines if line.strip())

        output_text = cleaned

    except Exception as e:
        output_text = (
            f"[Pretty Print XML] The text is not valid XML.\n"
            f"Reason: {e}\n"
            f"{'=' * 40}\n"
            f"{input_text}"
        )
