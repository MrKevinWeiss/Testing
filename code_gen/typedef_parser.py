"""Fills in any missing/calculatable information of typedefs"""
import gen_helpers


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
            element["size"] = gen_helpers.PRIM_TYPES[element["bit_type"]]
        else:
            element["size"] = type_sizes[element["type"]]

        if 'array_size' in element:
            total_byte += element["size"] * element["array_size"]
        else:
            total_byte += element["size"]
    return total_byte


def _update_typedef_sizes(typedefs):
    type_sizes = {}
    type_sizes.update(gen_helpers.PRIM_TYPES)
    for typedef in typedefs:
        total_byte = _update_element_sizes(typedef["elements"], type_sizes)
        _update_typedef_size(typedef, total_byte)
        type_sizes[typedef["name"]] = typedef["size"]


def _fill_elements(elements, type_name, value):
    for element in elements:
        if type_name not in element:
            element[type_name] = value


def _fill_empty_typedefs(typedefs, type_name, value, overwrite=True):
    for typedef in typedefs:
        if type_name not in typedef:
            typedef[type_name] = value

        if overwrite:
            _fill_elements(typedef['elements'], type_name, value)
        else:
            _fill_elements(typedef['elements'], type_name, typedef[type_name])


def fill_typedef_parameters(typedefs):
    """Fills in any missing/calculatable information of typedefs"""
    _update_typedef_sizes(typedefs)
    _fill_empty_typedefs(typedefs, 'description', 'TODO: enter description')
    _fill_empty_typedefs(typedefs, 'default', 0)
    _fill_empty_typedefs(typedefs, 'access', 1, False)


def main():
    """Tests."""
    typedefs = [{"name": "timestamp_t",
                 "size": 8,
                 "description": "Time and date",
                 "elements": [{"name": "second",
                               "type": "uint8_t",
                               "description": "The seconds in decimal"}]},
                {"name": "sys_t",
                 "description": "System settings for the bpt",
                 "elements": [{"name": "sn",
                               "type": "uint8_t",
                               "array_size": 12,
                               "description": "Unique ID of the device"},
                              {"name": "time",
                               "type": "timestamp_t",
                               "description": "Test"},
                              {"name": "mode",
                               "bit_type": "uint8_t",
                               "type": "i2c_mode_t",
                               "bitfield": [{"name": "addr_10_bit",
                                             "bits": 1,
                                             "description": "10 bit address enable"},
                                            {"name": "general_call",
                                             "bits": 1,
                                             "description": "general call enable"}]}]},
                {"name": "test_t",
                 "description": "testing 1 2... testing",
                 "elements": [{"name": "xxx",
                               "type": "sys_t",
                               "array_size": 2,
                               "description": "aasdf"}]}]

    fill_typedef_parameters(typedefs)
    from pprint import pprint
    pprint(typedefs)


if __name__ == "__main__":
    main()
