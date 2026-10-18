# make_evx.py
import struct
import sys

magic = b'EVX\0'

def main():
    raw_file = sys.argv[1]
    evx_file = sys.argv[2]

    code = open(raw_file, 'rb').read()
    code_size = len(code)
    entry_point = 0  # começo do código
    bss_size = 1024
    version = 1
    flags = 0
    stack_size = 8192
    heap_size = 16384

    header = struct.pack('<4sIIIIIII',
                         magic,
                         code_size,
                         entry_point,
                         bss_size,
                         version,
                         flags,
                         stack_size,
                         heap_size)

    with open(evx_file, 'wb') as f:
        f.write(header)
        f.write(code)

if __name__ == "__main__":
    main()
