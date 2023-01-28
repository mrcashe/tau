# Script file for sed(1).
# Edit ctags output.

s+^!++              # Filter out pseudo-tags
s+^.*/\^ *++        # Filter out "/^ "
s+\$\/\;\"++        # Filter out "$/;\""
