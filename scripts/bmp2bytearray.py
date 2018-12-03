from PIL import Image
import numpy as np
import sys

name = sys.argv[1]
outfile = sys.argv[3]
im = Image.open(sys.argv[2])

p = np.array(im).reshape(-1, 4)[:, [0, 1, 2]].flatten()
p = bytearray(p)

array_string = ','.join('0x%02x'%i for i in p)
result = "const char %s[] = { %s };" % (name, array_string)

f = open(sys.argv[3], 'w')
f.write(result)
f.close
