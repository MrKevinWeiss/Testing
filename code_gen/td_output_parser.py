#!/usr/bin/env python3
"""This module generates output files based on typedefs."""


def _metadata_to_h_intro(metadata):
    intro_str = "/*\n"
    intro_str += " * Filename: {}.g\n".format(metadata["name"])
    intro_str += " * Author: {}\n".format(metadata["author"])
    intro_str += " * Revision: {}\n".format(metadata["revision"])
    intro_str += " */\n\n"
    intro_str += "#ifndef %s_H_\n" % (metadata["name"].upper())
    intro_str += "#define %s_H_\n" % (metadata["name"].upper())
    return intro_str


def _bitfield_to_c_struct(bf_element):
    bf_str = ""
    bf_str += "/* @brief {} */\n".format(bf_element["description"])
    bf_str += "typedef struct %s_TAG {\n" % (bf_element["type"])
    total_bits = 0
    for val in bf_element["bitfield"]:
        bf_str += "\t/* {} */\n".format(val["description"])
        bf_str += "\t{} {} : {};\n".format(bf_element["bit_type"],
                                           val["name"], val["bits"])
        total_bits += int(val["bits"])
    bf_str += "} %s;\n\n" % (bf_element["type"])

    if bf_element["size"] < ((float(total_bits)/8)):
        raise ValueError("Too many bits in %s for %s" %
                         (bf_element["type"], bf_element["bit_type"]))
    return bf_str


def _typedef_to_c_struct(typedef):
    c_str = ""
    for element in typedef["elements"]:
        if 'bitfield' in element:
            c_str += _bitfield_to_c_struct(element)
    c_str += "/* @brief {} */\n".format(typedef["description"])
    c_str += "typedef union %s_TAG {\n" % (typedef["name"])
    c_str += "\tstruct {\n"

    for element in typedef["elements"]:
        c_str += "\t\t/* {} */\n".format(element["description"])
        if 'array_size' in element:
            c_str += "\t\t{} {}[{}];\n".format(element["type"],
                                               element["name"],
                                               element["array_size"])
        else:
            c_str += "\t\t{} {};\n".format(element["type"], element["name"])
    c_str += "\t};\n"
    c_str += "\tuint8_t data8[{}];\n".format(typedef["size"])
    c_str += "} %s;\n" % (typedef["name"])
    return c_str


def parse_typedefs_to_h(typedefs, metadata=None):
    """Parses the typedef to a c header containing typedef structs."""
    td_str = ""
    if metadata is not None:
        td_str += _metadata_to_h_intro(metadata)
    td_str += "\n"
    td_str += "#include <stdint.h>\n"
    td_str += "\n"
    td_str += "#pragma pack(1)\n"
    for typedef in typedefs:
        td_str += _typedef_to_c_struct(typedef)
        td_str += "\n"
    td_str += "#pragma pack()\n"
    if metadata is not None:
        td_str += "#endif"
    return td_str


def main():
    """Tests parsing example typedef to c header."""
    from td_parser import parse_basic_typedefs
    from gen_helpers import TEST_T
    from gen_helpers import TEST_MD

    typedefs = parse_basic_typedefs(TEST_T)
    print(parse_typedefs_to_h(typedefs, TEST_MD))


if __name__ == "__main__":
    main()
