from PIL import Image
import numpy as np
import sys

name = sys.argv[1]
outfile = sys.argv[3]
im = Image.open(sys.argv[2])

p = np.array(im)

height = p.shape[1]
width = p.shape[1]

p = p.reshape(-1, 4)[:, [0, 1, 2]].flatten()
p = bytearray(p)

array_string = ','.join('0x%02x'%i for i in p)
result = """#include "Files.h"
const char {name}[] = {{ {0} }};
const int {name}_width = {1};
const int {name}_height = {2};
""".format(array_string, height, width, name=name)

f = open(sys.argv[3], 'w')
f.write(result)
f.close
