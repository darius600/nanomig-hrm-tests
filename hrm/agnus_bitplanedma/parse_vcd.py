import re

with open('trace.vcd', 'r') as f:
    hard_start_sym = None
    for line in f:
        if '\' in line and ' hard_start ' in line:
            parts = line.split()
            hard_start_sym = parts[3]
        elif hard_start_sym and line.startswith('0' + hard_start_sym) or line.startswith('1' + hard_start_sym):
            print(line.strip())

