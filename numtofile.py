import re
import sys
import os

print("Starting python program...")
eval_pattern = re.compile(r'EVAL:\s*\[([^]]+)\]')
def extract_and_write_numbers(input_filename):
        with open(input_filename, 'r') as file:
            cpp_output = file.read()
        matches = eval_pattern.findall(cpp_output)
        for i, match in enumerate(matches):
            numbers = [int(num.strip()) for num in match.split()]
            base_filename, _ = os.path.splitext(input_filename)
            with open(f'{base_filename}_limb_{i + 1}.txt', 'a') as file:
                for num in numbers:
                    file.write(format(num,'x') + '\n')

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 numtofile.py <input_filename>")
        sys.exit(1)
    input_filename = sys.argv[1]
    extract_and_write_numbers(input_filename)

