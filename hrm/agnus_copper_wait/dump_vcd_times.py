import sys
time = -1
for line in open('trace.vcd'):
    if line.startswith('#'):
        time = int(line[1:])
    if time >= 20 and time <= 28:
        print(line.strip())
