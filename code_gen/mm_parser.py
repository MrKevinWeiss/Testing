#!/usr/bin/env python3
"""Parses and manages memory maps from typedefs"""
import copy
from gen_helpers import PRIM_TYPES


def _copy_elements(typename, typedefs):
    for typedef in typedefs:
        if typename == typedef["name"]:
            return {'elements': copy.deepcopy(typedef["elements"])}
    raise ValueError("Cannot find {}".format(typename))


def _copy_typedef_elements(element, expanded_typedefs):
    e_type = element["type"]
    if "array_size" in element:
        a_size = element["array_size"]
        element['array'] = [_copy_elements(e_type, expanded_typedefs)
                            for i in range(a_size)]
    else:
        element.update(_copy_elements(e_type, expanded_typedefs))


def _expand_typedefs(typedefs):
    expanded_typedefs = []
    for typedef in typedefs:
        for element in typedef["elements"]:
            if element["type"] not in PRIM_TYPES:
                if "bitfield" not in element:
                    _copy_typedef_elements(element, expanded_typedefs)
        expanded_typedefs.append(typedef)
    return expanded_typedefs


def _update_offsets(elements, offset=0):
    for element in elements:
        element["offset"] = offset
        if "elements" in element:
            offset = _update_offsets(element["elements"], offset)
        elif "array" in element:
            for array_val in element["array"]:
                offset = _update_offsets(array_val["elements"], offset)
        elif "array_size" in element:
            offset += element["size"]*element["array_size"]
        else:
            offset += element["size"]
    return offset


def _expand_mem_map(typedefs, mem_map=None):

    exp_typedefs = _expand_typedefs(typedefs)
    if mem_map is None:
        mem_map = exp_typedefs[-1]
    else:
        mem_map = next(itm for itm in exp_typedefs if itm["name"] == "mem_map")
    _update_offsets(mem_map['elements'])
    return mem_map


def _element_to_bitfield_record(name, element):

    bitfields = []
    offset = 0
    for bit_info in element["bitfield"]:
        bitfield = {}
        bitfield["type"] = element["bit_type"]
        bitfield["size"] = element["size"]
        bitfield["total_size"] = element["size"]
        bitfield["offset"] = element["offset"]
        bitfield["description"] = bit_info["description"]
        bitfield["access"] = element["access"]
        bitfield["default"] = element["default"]
        name.append(bit_info["name"])
        bitfield["name"] = copy.deepcopy(name)
        name.pop()
        bitfield["bits"] = bit_info["bits"]
        offset += bit_info["bits"]
        bitfield["bit_offset"] = offset
        bitfield["is_bitfield"] = True
        bitfields.append(bitfield)
    return bitfields


def _element_to_mem_map_record(name, element, mem_map):
    mem_map.append({})
    name.append(element["name"])
    if "array_size" in element:
        mem_map[-1]["total_size"] = element["size"] * element["array_size"]
        name.append("%d" % element["array_size"])
    else:
        mem_map[-1]["total_size"] = element["size"]
    mem_map[-1]["size"] = element["size"]
    mem_map[-1]["name"] = copy.deepcopy(name)
    mem_map[-1]["type"] = element["type"]
    mem_map[-1]["offset"] = element["offset"]
    mem_map[-1]["description"] = element["description"]
    mem_map[-1]["access"] = element["access"]
    mem_map[-1]["default"] = element["default"]
    mem_map[-1]["bits"] = element["size"] * 8
    mem_map[-1]["bit_offset"] = 0
    mem_map[-1]["is_bitfield"] = False
    if 'bitfield' in element:
        mem_map.extend(_element_to_bitfield_record(name, element))
    if "array_size" in element:
        name.pop()
    name.pop()


def _parse_elements_to_mem_map(elements, mem_map=None, name=None):
    if mem_map is None:
        mem_map = []
    if name is None:
        name = []
    for element in elements:
        if "elements" in element:
            name.append(element["name"])
            _parse_elements_to_mem_map(element["elements"], mem_map, name)
            name.pop()
        elif "array" in element:
            name.append(element["name"])
            for i, array_val in enumerate(element["array"]):
                name.append("%d" % i)
                _parse_elements_to_mem_map(array_val["elements"],
                                           mem_map, name)
                name.pop()
            name.pop()
        else:
            _element_to_mem_map_record(name, element, mem_map)
    return mem_map


def parse_typedefs_to_mem_map(typedefs, mem_map=None):
    """Parses a selected (or the last) typedef to a memory map"""
    mem_map_expanded = _expand_mem_map(typedefs, mem_map=None)
    mem_map = _parse_elements_to_mem_map(mem_map_expanded['elements'])
    return mem_map


def import_mem_map_values(mem_map, saved_mem_map,
                          type_names=None):
    """Imports type_name values from saved memory maps"""
    if type_names is None:
        type_names = ['access', 'default', 'description']
    for saved_record in saved_mem_map:
        for record in mem_map:
            if record['name'] == saved_record['name']:
                for type_name in type_names:
                    record[type_name] = saved_record[type_name]


def main():
    """Tests the parsing and updating saved values from the memory map."""
    from pprint import pprint
    from gen_helpers import TEST_T
    import td_parser

    typedefs = td_parser.parse_basic_typedefs(TEST_T)

    mem_map = parse_typedefs_to_mem_map(typedefs)
    saved_mem_map = copy.deepcopy(mem_map)
    saved_mem_map[0]['access'] = 99
    saved_mem_map[1]['default'] = '123'
    saved_mem_map[1]['description'] = 'test to alter description'

    print("================ MEM MAP =====================\n")
    pprint(mem_map, width=120)
    print("=============== SAVED MEM MAP ================\n")
    pprint(saved_mem_map, width=120)

    print('============ Diff ============')
    try:
        from deepdiff import DeepDiff
        pprint(DeepDiff(mem_map, saved_mem_map))
    except ImportError:
        print("Cannot make diff, try 'pip install deepdiff'")

    import_mem_map_values(mem_map, saved_mem_map)
    print("============== MODIFIED MEM MAP ===============\n")
    pprint(mem_map, width=120)

    print('============ Diff After Modified ============')
    try:
        from deepdiff import DeepDiff
        pprint(DeepDiff(mem_map, saved_mem_map))
    except ImportError:
        print("Cannot make diff, try 'pip install deepdiff'")


if __name__ == "__main__":
    main()
