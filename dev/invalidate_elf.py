import io
import sys
from elftools.elf.elffile import ELFFile

def process_file(filename):
    with open(filename, 'rb+') as f:
        elffile = ELFFile(f)
        text_section = elffile.get_section_by_name(".text")
        text_section_offset = text_section.header['sh_offset']
        text_section_size = text_section.header['sh_size']

        f.seek(text_section_offset, io.SEEK_SET)
        f.write(bytearray(text_section_size))

if __name__ == '__main__':
    for filename in sys.argv[1:]:
        process_file(filename)
