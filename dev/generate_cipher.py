#!/usr/bin/env python3

import os.path
import argparse

CIPHER_KEY = 0x30
CIPHER_TEMPLATE_H = f"""#define PLAIN_MESSAGE (decrypt(message, sizeof(message)))

const char* decrypt(char data[], unsigned int size)
{{{{
    for (int i = 0; i < size; i++) {{{{
        data[i] ^= {hex(CIPHER_KEY)};
    }}}}

    return data;
}}}}

char message[] = {{{{ {{0}} }}}};"""

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("file", help="file to be encrypted")
    args = parser.parse_args()

    if not os.path.isfile(args.file):
        print(f"error: `{args.file}` does not exist.")
        exit(1)

    with open(args.file, "r") as fin:
        content = fin.read()

        chars = []
        for ch in content:
            chars.append(hex(ord(ch) ^ CIPHER_KEY))
        chars.append(hex(CIPHER_KEY)) # NULL terminator

        print(CIPHER_TEMPLATE_H.format(', '.join(chars)))

if __name__ == "__main__":
    main()
