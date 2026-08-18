import sys

def parse_vcd(file_path):
    signals = {}
    signal_names = {}
    
    with open(file_path, 'r') as f:
        lines = f.readlines()
        
    in_def = True
    time = 0
    values = {}
    
    for line in lines:
        line = line.strip()
        if not line: continue
        
        if in_def:
            if line.startswith('$var'):
                parts = line.split()
                if len(parts) >= 5:
                    size = int(parts[2])
                    code = parts[3]
                    name = parts[4]
                    if name in ['clk', 'copper_state', 'reqdma', 'dma_ack', 'enable', 'bus_ena', 'clk_ena', 'bus_blk', 'ackdma', 'hpos', 'strobe', 'ir1', 'ir2', 'data_in', 'illegalreg', 'skip_flag']:
                        signals[code] = name
                        values[name] = 'x'
            elif line.startswith('$enddefinitions'):
                in_def = False
        else:
            if line.startswith('#'):
                time = int(line[1:])
            else:
                val = ''
                code = ''
                if line[0] in ['0', '1', 'x', 'z']:
                    val = line[0]
                    code = line[1:]
                elif line[0] == 'b':
                    parts = line.split()
                    val = parts[0][1:]
                    if len(parts) > 1:
                        code = parts[1]
                
                if code in signals:
                    name = signals[code]
                    values[name] = val
                    if name == 'clk' and val == '1': # print on posedge
                        out = [f"{time}"]
                        for k in ['hpos', 'clk_ena', 'bus_ena', 'enable', 'reqdma', 'strobe', 'copper_state', 'ir1', 'ir2', 'data_in', 'dma_ack', 'illegalreg', 'skip_flag']:
                            out.append(f"{k}={values.get(k, 'x')}")
                        print(" ".join(out))

if __name__ == '__main__':
    parse_vcd(sys.argv[1])
