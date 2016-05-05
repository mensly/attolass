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



SPRITE_HEADER_TEMPLATE = """extern const uint8_t res_sprite_{0}_data[];
#define res_sprite_{0}_width 8
#define res_sprite_{0}_height 8
"""
# Process res/sprite/*.png into C code
def create_sprite(name, file, flip=False):
    (width, height, pixels, metadata) = png.Reader(filename=file).asRGBA8()
    data = [item for sublist in pixels for item in sublist][3::4]
    resource = "PROGMEM const uint8_t res_sprite_{0}_data[] = ".format(name) + "{"
    data_lines = []
    for x in xrange(width):
        line = "\n    0b"
        bits = 0
        for y in xrange(height - 1, -1, -1):
            if bits == 8:
                line += ", 0b"
                bits = 0
            if data[y * width + x]:
                line += "1"
            else:
                line += "0"
            bits += 1
        line += ("0" * (8 - bits))
        line += ", "
        data_lines += (line,)
    if flip:
        data_lines = reversed(data_lines)
    for line in data_lines:
        resource += line
    resource += "\n};\n"
    return resource

sprite_dir = os.path.join(dir, 'res/sprite')
sprite_header = ""
sprite_data = ""
for sprite_file in os.listdir(sprite_dir):
    if sprite_file.endswith('.png'):
        # Create sprite
        sprite_name = os.path.splitext(sprite_file)[0]
        sprite_header += SPRITE_HEADER_TEMPLATE.format(sprite_name) + "\n"
        sprite_data += create_sprite(sprite_name, os.path.join(sprite_dir, sprite_file)) + "\n"
        if sprite_name.endswith('right'):
            # Create flipped sprite
            sprite_name = sprite_name.replace('right', 'left')
            sprite_header += SPRITE_HEADER_TEMPLATE.format(sprite_name) + "\n"
            sprite_data += create_sprite(sprite_name, os.path.join(sprite_dir, sprite_file), True) + "\n"

replace_region('resources.h', 'res_sprite', sprite_header)
replace_region('resources.c', 'res_sprite', sprite_data)

#print sprite_header
#print sprite_data

class Section:
    def __init__(self, bitmap, count=1):
        self.bitmap = bitmap
        self.count = count

    def __str__(self):
        resource = "    {0}&MASK_SECTION,".format(self.count)
        bitmap = "{0:064b}".format(self.bitmap)
        search_ch = '0'
        swap_index = 0
        try:
            while True:
                swap_index = bitmap.index(search_ch, swap_index)
                resource += "{0}|FLAG_BLOCK,".format(swap_index)
                if search_ch == '0':
                    search_ch = '1'
                else:
                    search_ch = '0'
        except ValueError:
            pass
        return resource

LEVEL_HEADER_TEMPLATE = "extern const level_t res_level_{0}[];"
# Process res/level/*.png into C code
def create_level(name, file):
    (width, height, pixels, metadata) = png.Reader(filename=file).asRGBA8()
    if height <> 64: raise Exception("levels must be 64 pixels high")
    # Convert image into list of sections (each column can be represented as a 64 bit int)
    pixels = [row[3::4] for row in pixels]
    resource =  "PROGMEM const level_t res_level_{0}[] = ".format(name) + "{\n"
    section = None
    for x in xrange(width):
        bitmap = 0
        for y in xrange(height):
            if pixels[y][x]:
                bitmap += (1 << y)
        if section and section.bitmap == bitmap and section.count < 127:
            section.count += 1
        else:
            if section:
                resource += str(section) + "\n"
            section = Section(bitmap)
    if section:
        resource += str(section) + "\n"
    resource += "0 };\n"
    return resource

level_dir = os.path.join(dir, 'res/level')
level_header = ""
level_data = ""
for level_file in os.listdir(level_dir):
    if level_file.endswith('.png'):
        # Create level
        level_name = os.path.splitext(level_file)[0]
        level_header += LEVEL_HEADER_TEMPLATE.format(level_name) + "\n"
        level_data += create_level(level_name, os.path.join(level_dir, level_file)) + "\n"

replace_region('resources.h', 'res_level', level_header)
replace_region('resources.c', 'res_level', level_data)
#print level_header
#print level_data

