import json
import argparse
import pprint
import copy

types = {'uint8_t': 1, 'int8_t': 1, 'uint16_t': 2, 'int16_t': 2,
         'uint32_t': 4, 'int32_t': 4, 'uint64_t': 8, 'int64_t': 8}

primary_types = {'uint8_t': 1, 'int8_t': 1, 'uint16_t': 2, 'int16_t': 2,
                 'uint32_t': 4, 'int32_t': 4, 'uint64_t': 8, 'int64_t': 8}


def __format_name(name):
    name = name.replace('.', '_')
    name = name.replace('[', '')
    name = name.replace(']', '')
    return name


def __get_getter(record):
    str = "    def get_%s(self):\n" % __format_name(record['name'])
    str += "        return self.read_bytes(%d, %d)\n" % (record['offset'], record['size'])
    return str


def __get_setter(record):
    str = "    def set_%s(self, data=0):\n" % __format_name(record['name'])
    str += "        return self.write_bytes(%d, data, %d)\n" % (record['offset'], record['size'])
    return str


def get_if(map, name):
    str = "import base_if\n\n\n"
    str += "class %s(base_if.BaseIf):\n\n" % name

    for record in map:
        str += __get_getter(record)
        str += '\n'
        str += __get_setter(record)
        str += '\n'

    str += "    def get_command_list(self):\n"
    str += "        cmds = list()\n"
    for record in map:
        str += "        cmds.append(self.set_%s)\n" % __format_name(record['name'])
        str += "        cmds.append(self.get_%s)\n" % __format_name(record['name'])
    str += "        return cmds\n"
    return str


def get_mem_map(name, map, elements):
    for element in elements:

        if ("elements" in element):
            name += element["name"] + "."
            get_mem_map(name, map, element["elements"])
            name = name.replace(element["name"] + ".", "")
        elif ("array" in element):
            name += element["name"]
            for i, array_val in enumerate(element["array"]):
                name += "[%d]." % i
                get_mem_map(name, map, array_val["elements"])
                name = name.replace("[%d]." % i, "")
            name = name.replace(element["name"], "")
        elif ("bitfield" in element):
            map.append({"name": "", "size": -1, "offset": -1, "description": ""})
            map[-1]["size"] = element["size"]
            map[-1]["offset"] = element["offset"]
            map[-1]["name"] += name + element["name"]
            if ("description" in element):
                map[-1]["description"] = element["description"]
        elif (element["type"] in primary_types):
            map.append({"name": "", "size": -1, "offset": -1, "description": ""})
            map[-1]["size"] = element["size"]
            map[-1]["offset"] = element["offset"]
            map[-1]["name"] += name + element["name"]
            if ("description" in element):
                map[-1]["description"] = element["description"]
            if ("array_size" in element):
                map[-1]["name"] += "[%d]" % element["array_size"]
        else:
            raise ValueError("ERROR1: type not primary or contains elements")


def update_offsets(elements, offset):
    for element in elements:
        element.update({"offset": offset})
        if ("elements" in element):
            offset = update_offsets(element["elements"], offset)
        elif ("array" in element):
            for array_val in element["array"]:
                offset = update_offsets(array_val["elements"], offset)
        elif ("bitfield" in element):
            if (element["bit_type"] in primary_types):
                offset += primary_types[element["bit_type"]]
            else:
                raise ValueError("ERROR2: type not primary or contains elements")
        elif (element["type"] in primary_types):
            if ("array_size" in element):
                offset += primary_types[element["type"]]*element["array_size"]
            else:
                offset += primary_types[element["type"]]
        else:
            raise ValueError("ERROR1: type not primary or contains elements")
    return offset


def get_offsets(typedefs):
    for typedef in typedefs:
        offset = 0
        offset = update_offsets(typedef["elements"], offset)


def get_sizes(typedefs):
    type_sizes = dict()
    type_sizes.update(primary_types)
    for typedef in typedefs:
        total_byte = 0
        for element in typedef["elements"]:
            if ('bitfield' in element):
                total_byte += primary_types[element["bit_type"]]
                element.update({"size": primary_types[element["bit_type"]]})
            elif ('array_size' in element):
                total_byte += type_sizes[element["type"]] * element["array_size"]
                element.update({"size": type_sizes[element["type"]] * element["array_size"]})
            else:
                total_byte += type_sizes[element["type"]]
                element.update({"size": type_sizes[element["type"]]})

        if "total_size" in typedef:
            if (typedef["total_size"] < total_byte):
                raise ValueError("%d out of %d bytes in %s" %
                                 (total_byte, typedef["total_size"], typedef["name"]))
            type_sizes.update({typedef["name"]: typedef["total_size"]})
            typedef.update({"size": typedef["total_size"]})
            if (typedef["total_size"] is not total_byte):
                typedef["elements"].append({"name": "res", "type": "uint8_t", "array_size": typedef["total_size"] - total_byte, "size": typedef["total_size"] - total_byte, "description": "Reserved bytes"})
        else:
            type_sizes.update({typedef["name"]: total_byte})
            typedef.update({"size": total_byte})


def get_type_elements(typename, typedefs):
    for typedef in typedefs:
        if typename == typedef["name"]:
            elements = copy.deepcopy(typedef["elements"])
            return {"elements": elements, "size": typedef["size"]}
    raise ValueError("Cannot find %s" % (typename))


def expand_typedefs(typedefs):
    total_typedefs = list()
    for typedef in typedefs:
        for element in typedef["elements"]:
            if (element["type"] not in primary_types):
                if ("bitfield" not in element):
                    if ("array_size" in element):
                        element_array = list()
                        single_copy = (get_type_elements(element["type"], total_typedefs))
                        for i in range(0, element["array_size"]):
                            element_array.append(copy.deepcopy(single_copy))
                        element.update({"array": element_array})
                    else:
                        element.update(get_type_elements(element["type"], total_typedefs))
        total_typedefs.append(typedef)
    return total_typedefs


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

    if (types[data["bit_type"]] < ((float(total_bits)/8))):
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
            total_byte += types[val["type"]] * val["array_size"]
        else:
            str += "\t\t%s %s;\n" % (val["type"], val["name"])
            total_byte += types[val["type"]]
    if "total_size" in data:
        if (data["total_size"] < total_byte):
            raise ValueError("%d out of %d bytes in %s" %
                             (total_byte, data["total_size"], data["name"]))
        if (data["total_size"] is not total_byte):
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
    for val in data["typedefs"]:
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

get_sizes(data["typedefs"])
x = expand_typedefs(data["typedefs"])
get_offsets(x)

mem_map = list()
get_mem_map("", mem_map, x[8]["elements"])

f = open((data["metadata"]["name"]+".h"), 'w')
f.write(str)
f.close()
f = open((data["metadata"]["name"]+".json"), 'w')
f.write(json.dumps(x))
f.close()

f = open((data["metadata"]["name"]+".csv"), 'w')
str = "Name,Size,Offset,Description,Access\n"
for line in mem_map:
    str += "%s,%d,%d,%s," % (line["name"], line["size"], line["offset"], line["description"])
f.write(str)
f.close()

f = open(("app_access.c"), 'w')
str = "#include \"app_access.h\"\n\n"
str += "const uint8_t REG_ACCESS[] = { \n"
size = 0
for line in mem_map:
    for access_byte in range(line["size"]):
        if (access_byte != 0):
            str += ", "
        str += "0x%02X" % 0x22
        size += 1
    str += ",/* %s */\n" % line["name"]
str += "/* total size %d */\n" % size
f.write(str)
f.close()

str = get_if(mem_map, "BptIf")
f = open(("bpt_if.py"), 'w')
f.write(str)
f.close()
