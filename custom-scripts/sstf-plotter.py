import re
import plotly.express as px
from typing import List

def main():
    file_name = "sstf-out.txt"
    blocks = read_and_parse_file(file_name)
    plot_blocks(blocks)

def read_and_parse_file(file_name: str) -> List[int]:
    blocks = []

    pattern = re.compile(r'\[SSTF\] dsp \w (\d+)')
    with open(file_name, 'r') as f:
        for line in f:
            match = pattern.search(line)
            if match:
                blocks.append(int(match.group(1)))
    
    return blocks

def plot_blocks(blocks: List[int]):
    print("Plotting... Please check the browser window.")
    fig = px.line(x=range(len(blocks)), y=blocks, title="SSTF Disk Scheduling Dispatches", labels={'x': 'Index', 'y': 'Block'})
    fig.show()

if __name__ == "__main__":
    main()