import difflib

file1_path = './Books/CPP_Trades.csv'
file2_path = './Books/Python_Trades.csv'

with open(file1_path, 'r', encoding='utf-8') as f1, open(file2_path, 'r', encoding='utf-8') as f2:
    f1_text = f1.readlines()
    f2_text = f2.readlines()

# Generate the difference
diff = difflib.unified_diff(
    f1_text, f2_text, 
    fromfile=file1_path, 
    tofile=file2_path, 
    lineterm=''
)

# Print the differences line by line
has_differences = False
for line in diff:
    print(line)
    has_differences = True

if not has_differences:
    print("No differences found.")

