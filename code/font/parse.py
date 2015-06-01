char_width = 3
char_height = 5

f = open("ASCII_FONT_ascii.pbm", 'r')
file_format = f.readline()[:-1]
f.readline() #should be the comment
width, height = [int(x) for x in f.readline()[:-1].split()]
lines = ""
line = f.readline().rstrip('\n')
while line != '':
    lines = lines + line
    line = f.readline().rstrip('\n')

print(file_format)
print(width)
print(height)
print(lines)

string = """#include \"font.h\"\n
extern u8 font[] PROGMEM = \n{\n"""
for num in range(96):
    string += "\t0b"
    for i in range(char_height):
        for j in range(char_width):
            if lines[(char_height*(num//16)+i)*width+(num%16)*char_width+j] == "1":
                string += "1"
                print("\u25AE", end="")
            else:
                string += "0"
                print(" ", end="")
        print("")
    print("")
    string += ",\n"
string += "}\n"
print(string)

c = open("font.c", 'w')
c.write(string)
c.close()
h = open("font.h", 'w')
h.write("""#ifndef FONT_H
#define FONT_H

extern u8 font[];
#endif""")
h.close()
f.close()
