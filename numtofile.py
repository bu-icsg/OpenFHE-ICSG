import re

print("Starting python program...")
eval_pattern = re.compile(r'EVAL:\s*\[([^]]+)\]')
def extract_and_write_numbers(output):
        matches = eval_pattern.findall(output)
        for i, match in enumerate(matches):
            numbers = [int(num.strip()) for num in match.split()]
            with open(f'output_{i + 1}.txt', 'a') as file:
                for num in numbers:
                    file.write(hex(num) + '\n')


with open('output.txt', 'r') as file:
    cpp_output = file.read()
extract_and_write_numbers(cpp_output)

