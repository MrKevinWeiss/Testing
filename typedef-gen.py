import json
import argparse


types = {'uint8_t': 1, 'int8_t': 1, 'uint16_t': 2, 'int16_t': 2,
         'uint32_t': 4, 'int32_t': 4, 'uint64_t': 8, 'int64_t': 8}


def getBitfield(data):
    str = ""
    if ('description' in data):
        str += "/* %s */\n" % (data["description"])
    str += "typedef struct %s_TAG {\n" % (data["type"])
    total_bits = 0
    for val in data["bitfield"]:
        if ('description' in val):
            str += "\t/* %s */\n" % (data["description"])
        str += "\t%s %s : %s;\n" % (data["bit_type"], val["name"], val["bits"])
        total_bits += int(val["bits"])
    str += "} %s;" % (data["type"])

    if (types[data["bit_type"]] < ((total_bits/8) + 1)):
        raise ValueError("Too many bits in %s for %s" %
                         (data["type"], data["bit_type"]))
    types.update({data["type"]: types[data["bit_type"]]})
    return str


def getTypedef(data):
    str_list = list()
    str = ""
    if (u'description' in data):
        str += "/* %s */\n" % (data["description"])
    str += "typedef union %s_TAG {\n" % (data["name"])
    str += "\tstruct {\n"
    total_byte = 0
    for val in data["elements"]:
        if ('description' in val):
            str += "\t\t/* %s */\n" % (val["description"])
        if ('bitfield' in val):
            str_list.append(getBitfield(val))
        if (val["type"] not in types):
            raise ValueError("no %s type in %s" %
                             (val["type"], data["name"]))
        if ('array_size' in val):
            str += "\t\t%s %s[%d];\n" % (val["type"], val["name"],
                                         val["array_size"])
        else:
            str += "\t\t%s %s;\n" % (val["type"], val["name"])
        total_byte += types[val["type"]]

    if "total_size" in data:
        if (data["total_size"] < total_byte):
            raise ValueError("%d out of %d bytes in %s" %
                             (total_byte, data["total_size"], data["name"]))
        str += "\t\t/* reserve bytes */\n"
        str += "\t\tuint8_t res[%d];\n" % (data["total_size"] - total_byte)
        types.update({data["name"]: data["total_size"]})
    else:
        types.update({data["name"]: total_byte})
    str += "\t};\n"
    str += "\tuint8_t data8[%d];\n" % types[data["name"]]
    str += "} %s;\n" % (data["name"])

    str_list.append(str)
    return str_list


def getFullHeader(data):
    str = ""
    structs = list()
    for val in data["typedef"]["structs"]:
        structs.extend(getTypedef(val))

    str += "/*\n"
    str += " * Filename: %s\n" % (data["metadata"]["name"]+".h")
    str += " * Author: %s\n" % (data["metadata"]["author"])
    str += " * Revision: %s\n" % (data["metadata"]["revision"])
    str += "*/\n\n"
    str += "#ifndef %s_H_\n" % (data["metadata"]["name"].upper())
    str += "#define %s_H_\n" % (data["metadata"]["name"].upper())
    str += "\n"
    str += "#include <stdint.h>\n"
    str += "\n"
    str += "#pragma pack(1)\n"
    for typedef in structs:
        str += typedef
        str += "\n"
    str += "#pragma pack()\n"
    str += "#endif"
    return str


parser = argparse.ArgumentParser()
parser.add_argument("--file")
args = parser.parse_args()

f_name = 'bpt.json'
if args.file is not None:
    f_name = args.file

with open(f_name) as f:
    data = json.load(f)

str = getFullHeader(data)
print(str)
f = open((data["metadata"]["name"]+".h"), 'w')
f.write(str)
f.close()
