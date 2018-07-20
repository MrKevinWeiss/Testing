#!/usr/bin/env python3
"""This module generates output files based on memory maps."""
from gen_helpers import to_camel_case, to_underscore_case


def parse_mem_map_to_csv(mem_map, name_delimiter='.'):
    """Parses a memory map to a csv table string."""
    csv_str = ','.join(sorted(mem_map[0].keys()))
    for record in mem_map:
        csv_str += '\n'
        name = name_delimiter.join(record['name'])
        for key, value in sorted(record.items()):
            if key is 'name':
                csv_str += name
            else:
                csv_str += str(value).replace(',', '","')
            csv_str += ','
    return csv_str


def _record_to_getter(record, fxn_call_list=None):
    fxn_str = 'get_{}'.format('_'.join(record['name']))
    getter_str = "    def {}(self):\n".format(fxn_str)
    getter_str += "    \"\"\"{}\"\"\"\n".format(record['description'])
    if record['is_bitfield']:
        getter_str += "        return self.read_bits"
        getter_str += "({}, {}, {})\n".format(record['offset'],
                                              record['bit_offset'],
                                              record['bits'])
    else:
        getter_str += "        return self.read_bytes"
        getter_str += "({}, {})\n".format(record['offset'],
                                          record['total_size'])
    if isinstance(fxn_call_list, list):
        fxn_call_list.append(fxn_str)
    return getter_str


def _record_to_setter(record, fxn_call_list=None):
    fxn_str = 'set_{}'.format('_'.join(record['name']))
    setter_str = "    def {}(self, data={}):\n".format(fxn_str,
                                                       record['default'])
    setter_str += "    \"\"\"{}\"\"\"\n".format(record['description'])
    if record['is_bitfield']:
        setter_str += "        return self.write_bits"
        setter_str += "({}, {}, {}, data)\n".format(record['offset'],
                                                    record['bit_offset'],
                                                    record['bits'])
    else:
        setter_str += "        return self.write_bytes"
        setter_str += "({}, data, {})\n".format(record['offset'],
                                                record['total_size'])
    if isinstance(fxn_call_list, list):
        fxn_call_list.append(fxn_str)
    return setter_str


def parse_mem_map_to_if(mem_map, class_name, parent_module=None):
    """Parses a memory map to a python interface to a .py string."""
    if_str = ""
    if parent_module is not None:
        if_str += "from {} ".format(to_underscore_case(parent_module))
        if_str += "import {}\n\n\n".format(to_camel_case(parent_module))
    if_str += "class {}".format(to_camel_case(class_name))
    if parent_module is not None:
        if_str += "({})".format(to_camel_case(parent_module))
    if_str += ':\n'
    fxn_call_list = []
    for record in mem_map:
        if_str += _record_to_getter(record, fxn_call_list)
        if_str += '\n'
        if record['access'] != 0:
            if_str += _record_to_setter(record, fxn_call_list)
            if_str += '\n'

    if_str += "    def get_command_list(self):\n"
    if_str += "    \"\"\"A list of all possible commands\"\"\"\n"
    if_str += "        cmds = list()\n"
    for fxn_call in fxn_call_list:
        if_str += "        cmds.append(self.{})\n".format(fxn_call)
    if_str += "        return cmds\n"
    return if_str


def parse_mem_map_to_access_c(mem_map):
    """Parses access registers based on memory map to a .c string."""
    a_str = "#include \"app_access.h\"\n\n"
    a_str += "const uint8_t REG_ACCESS[] = { \n"
    size = 0
    for record in mem_map:
        for access_byte in range(record["total_size"]):
            if access_byte != 0:
                a_str += ", "
            a_str += "0x%02X" % record["access"]
            size += 1
        if record != mem_map[-1]:
            a_str += ","
        a_str += " /* {} */\n".format('_'.join(record["name"]))
    a_str = a_str.rstrip(',')
    a_str += "/* total size %d */\n};" % size
    return a_str


def main():
    """Tests parsing each output based on example typedef."""
    from td_parser import parse_basic_typedefs
    from mm_parser import parse_typedefs_to_mem_map
    from gen_helpers import TEST_T

    typedefs = parse_basic_typedefs(TEST_T)
    mem_map = parse_typedefs_to_mem_map(typedefs)
    mem_map[0]['default'] = 6

    base_if = parse_mem_map_to_if(mem_map, 'base_if')
    test_if = parse_mem_map_to_if(mem_map, 'test_if', 'base_if')
    access_c = parse_mem_map_to_access_c(mem_map)

    print(base_if)
    print(test_if)
    print(access_c)


if __name__ == "__main__":
    main()
