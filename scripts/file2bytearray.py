import sys

name = sys.argv[1]
f = open(sys.argv[2], 'r')
outfile = sys.argv[3]

bs = bytearray(f.read())
array_string = ','.join('0x%02x'%i for i in bs)
result = """#include "Files.h"
const char {name}[] = {{ {0} }};
const int {name}_length = {1};
""".format(array_string, len(bs), name=name)

f = open(sys.argv[3], 'w')
f.write(result)
f.close
