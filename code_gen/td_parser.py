#!/usr/bin/env python3
"""Fills in any missing/calculatable information of typedefs"""
from copy import deepcopy
from gen_helpers import PRIM_TYPES


def _fill_res_element(typedef, total_byte):
    res = {'name': 'res',
           'type': 'uint8_t',
           'array_size': typedef['size'] - total_byte,
           'size': 1,
           'description': 'Reserved bytes',
           'access': 0x00,
           'default': 0x00}
    typedef["elements"].append(res)


def _update_typedef_size(typedef, total_byte):
    if 'size' in typedef:
        if typedef['size'] < total_byte:
            raise ValueError("{} to large".format(typedef["name"]))

        if typedef['size'] is not total_byte:
            _fill_res_element(typedef, total_byte)
    else:
        typedef['size'] = total_byte


def _update_element_sizes(elements, type_sizes):
    total_byte = 0
    for element in elements:
        if 'bitfield' in element:
            element["size"] = PRIM_TYPES[element["bit_type"]]
        else:
            element["size"] = type_sizes[element["type"]]

        if 'array_size' in element:
            total_byte += element["size"] * element["array_size"]
        else:
            total_byte += element["size"]
    return total_byte


def _update_typedef_sizes(typedefs):
    type_sizes = {}
    type_sizes.update(PRIM_TYPES)
    for typedef in typedefs:
        total_byte = _update_element_sizes(typedef["elements"], type_sizes)
        _update_typedef_size(typedef, total_byte)
        type_sizes[typedef["name"]] = typedef["size"]


def _fill_elements(elements, type_name, value, bitfield):
    for element in elements:
        if type_name not in element:
            element[type_name] = value
        if bitfield and 'bitfield' in element:
            for bf_val in element['bitfield']:
                if type_name not in bf_val:
                    bf_val[type_name] = value


def _fill_empty_typedefs(typedefs, type_name, value,
                         overwrite=True, bitfield=False):
    for typedef in typedefs:
        if type_name not in typedef:
            typedef[type_name] = value

        if overwrite:
            _fill_elements(typedef['elements'], type_name, value, bitfield)
        else:
            _fill_elements(typedef['elements'], type_name,
                           typedef[type_name], bitfield)


def parse_basic_typedefs(typedefs):
    """Fills in any missing/calculatable information of typedefs."""
    # Calculates sizes and res bytes
    cp_typedefs = deepcopy(typedefs)
    _update_typedef_sizes(cp_typedefs)
    _fill_empty_typedefs(cp_typedefs, 'description', '', bitfield=True)
    _fill_empty_typedefs(cp_typedefs, 'default', 0)
    _fill_empty_typedefs(cp_typedefs, 'access', 1, False)
    return cp_typedefs


def main():
    """Prints example of the parsing."""
    from pprint import pprint
    from gen_helpers import TEST_T

    print('============ Before Parsing ============')
    pprint(TEST_T)
    typedefs = parse_basic_typedefs(TEST_T)
    print('============ After Parsing ============')
    pprint(typedefs)
    print('============ Diff ============')
    try:
        from deepdiff import DeepDiff

        pprint(DeepDiff(TEST_T, typedefs))
    except ImportError:
        print("Cannot make diff, try 'pip install deepdiff'")


if __name__ == "__main__":
    main()
