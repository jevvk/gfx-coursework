import sys

name = sys.argv[1]
f = open(sys.argv[2], 'r')
outfile = sys.argv[3]

bs = bytearray(f.read())
array_string = ','.join('0x%02x'%i for i in bs)
result = "const char %s[] = { %s };" % (name, array_string)

f = open(sys.argv[3], 'w')
f.write(result)
f.close
