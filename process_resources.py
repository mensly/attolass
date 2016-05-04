#!/usr/bin/env python
import os
import png

dir = os.path.dirname(__file__)

def replace_region(filename, region, content):
    filename = os.path.join(dir, filename)
    with open(filename) as f:
        existing = f.readlines()
    start_region = "#pragma region {}\n".format(region)
    end_region = "#pragma endregion\n"
    try:
        start_line = existing.index(start_region)
        end_line = existing.index(end_region, start_line)
    except ValueError:
        new_file_content = "".join(existing)
        new_file_content += start_line
        new_file_content += content
        new_file_content += end_line
    else:
        new_file_content = "".join(existing[:start_line + 1])
        new_file_content += content
        new_file_content += "".join(existing[end_line:])
    with open(filename, 'w') as f:
        f.write(new_file_content)



HEADER_TEMPLATE = """extern const uint8_t res_sprite_{0}_data[];
#define res_sprite_{0}_width 8
#define res_sprite_{0}_height 8
"""
# Process res/sprite/*.png into C code
def create_sprite(name, file):
    (width, height, pixels, metadata) = png.Reader(filename=file).asRGBA8()
    data = [item for sublist in pixels for item in sublist][3::4]
    resource = "PROGMEM const uint8_t res_sprite_{0}_data[] = ".format(name) + "{"
    for x in xrange(width):
        resource += "\n    0b"
        bits = 0
        for y in xrange(height - 1, -1, -1):
            if bits == 8:
                resource += ", 0b"
                bits = 0
            if data[y * width + x]:
                resource += "1"
            else:
                resource += "0"
            bits += 1
        resource += ("0" * (8 - bits))
        resource += ", "
    resource += "\n};\n"
    return resource

sprite_dir = os.path.join(dir, 'res/sprite')
sprite_header = ""
sprite_data = ""
for sprite_file in os.listdir(sprite_dir):
    if sprite_file.endswith('.png'):
        sprite_name = os.path.splitext(sprite_file)[0]
        sprite_header += HEADER_TEMPLATE.format(sprite_name) + "\n"
        sprite_data += create_sprite(sprite_name, os.path.join(sprite_dir, sprite_file)) + "\n"

replace_region('resources.h', 'res_sprite', sprite_header)
replace_region('resources.c', 'res_sprite', sprite_data)

#print sprite_header
#print sprite_data

# TODO: Process res/level/*.png into C code
# TODO: Insert code into res_level region of resources.c/resources.h
