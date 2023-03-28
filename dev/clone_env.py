import os
import random
import string
import argparse

def random_char():
    """Generate a random alphabetic character"""
    return random.choice(string.ascii_letters)


def damage_file(file_path, source_dir, output_dir):
    """Damage a file by replacing alphanumeric characters with random letters and numbers"""
    filename = os.path.basename(file_path)
    output_path = os.path.join(output_dir, os.path.relpath(file_path, source_dir))
    output_dirname = os.path.dirname(output_path)
    os.makedirs(output_dirname, exist_ok=True)
    with open(file_path, 'r') as f:
        content = f.read()
    damaged_content = ''
    for c in content:
        if c.isalnum():
            damaged_content += random_char()
        else:
            damaged_content += c
    with open(output_path, 'w') as f:
        f.write(damaged_content)


def damage_directory(directory, output_dir):
    """Recursively damage all files in a directory"""
    for root, dirs, files in os.walk(directory):
        # Skip hidden directories
        dirs[:] = [d for d in dirs if not d.startswith('.')]
        for file in files:
            if not file.startswith('.'):
                file_path = os.path.join(root, file)
                damage_file(file_path, directory, output_dir)


def main():
    parser = argparse.ArgumentParser(description='Copy files from a source directory to an output directory and damage them')
    parser.add_argument('source_dir', metavar='source_dir', type=str, help='path to the source directory')
    parser.add_argument('output_dir', metavar='output_dir', type=str, help='path to the output directory')
    args = parser.parse_args()

    source_dir = args.source_dir
    output_dir = args.output_dir

    damage_directory(source_dir, output_dir)

if __name__ == '__main__':
    main()
