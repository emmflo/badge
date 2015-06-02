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

string = """#include <avr/pgmspace.h>
#include <avr/io.h>
#include \"font.h\"

typedef uint16_t   u16;

extern const u16 font[] PROGMEM = \n{\n"""
for num in range(96):
    temp_string = ""
    for i in range(char_height):
        for j in range(char_width):
            if lines[(char_height*(num//16)+i)*width+(num%16)*char_width+j] == "1":
                temp_string += "1"
                print("\u25AE", end="")
            else:
                temp_string += "0"
                print(" ", end="")
        print("")
    print("")
    string += "\t0b"
    string += temp_string[::-1]
    string += ",\n"
string += "};\n"
print(string)

string += """extern const u16 font_extend[] PROGMEM = \n{\n"""
for num in range(96, 192):
    temp_string = ""
    for i in range(char_height):
        for j in range(char_width):
            if lines[(char_height*(num//16)+i)*width+(num%16)*char_width+j] == "1":
                temp_string += "1"
                print("\u25AE", end="")
            else:
                temp_string += "0"
                print(" ", end="")
        print("")
    print("")
    string += "\t0b"
    string += temp_string[::-1]
    string += ",\n"
string += "};\n"
print(string)


c = open("font.c", 'w')
c.write(string)
c.close()
f.close()
