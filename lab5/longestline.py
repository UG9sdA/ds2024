def longest_line_in_file(file):
    longest_line = ""
    with open(file, 'r') as file:
        for line in file:
            if len(line.strip()) > len(longest_path):
                longest_line = line
    return longest_line

if __name__ == '__main__':
    file_path = input("Enter the file's path: ")  
    longest_line = longest_line_in_file(file_path)
    print(f"The longest line in {file_path} is: {longest_line}")