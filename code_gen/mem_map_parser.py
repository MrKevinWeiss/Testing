import copy
import gen_helpers
import collections

def _copy_elements(typename, typedefs):
    for typedef in typedefs:
        if typename == typedef["name"]:
            return {'elements': copy.deepcopy(typedef["elements"])}
    raise ValueError("Cannot find {}".format(typename))


def _copy_typedef_elements(element, expanded_typedefs):
    e_type = element["type"]
    if "array_size" in element:
        a_size = element["array_size"]
        element['array'] = [_copy_elements(e_type, expanded_typedefs) for i in range(a_size)]
    else:
        element.update(_copy_elements(e_type, expanded_typedefs))


def _expand_typedefs(typedefs):
    expanded_typedefs = []
    for typedef in typedefs:
        for element in typedef["elements"]:
            if element["type"] not in gen_helpers.PRIM_TYPES:
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


def _parse_mem_map(elements, mem_map=[], name=[]):

    for element in elements:
        if "elements" in element:
            name.append(element["name"])
            _parse_mem_map(element["elements"], mem_map, name)
            name.pop()
        elif "array" in element:
            name.append(element["name"])
            for i, array_val in enumerate(element["array"]):
                name.append("[%d]" % i)
                _parse_mem_map(array_val["elements"], mem_map, name)
                name.pop()
            name.pop()
        else:
            mem_map.append({})

            mem_map[-1]["size"] = element["size"]
            mem_map[-1]["offset"] = element["offset"]
            mem_map[-1]["description"] = element["description"]
            name.append(element["name"])
            if "array_size" in element:
                name.append("[%d]" % element["array_size"])
            mem_map[-1]["name"] = copy.deepcopy(name)
            if "array_size" in element:
                name.pop()
            name.pop()

    return mem_map

def asdf(typedefs, mem_map=None):
    mem_map = _expand_mem_map(typedefs, mem_map=None)
    mem_map_expanded = _parse_mem_map(mem_map['elements'])
    return mem_map_expanded


def main():
    """Tests."""
    typedefs = [{"name": "timestamp_t",
                 "size": 8,
                 "description": "Time and date",
                 "elements": [{"name": "second",
                               "type": "uint8_t",
                               "description": "The seconds in decimal"},
                              {"name": "min",
                               "type": "uint8_t",
                               "description": "The min in decimal"}]},
                {"name": "sys_t",
                 "description": "System settings for the bpt",
                 "elements": [{"name": "sn",
                               "type": "uint8_t",
                               "array_size": 12,
                               "description": "Unique ID of the device"},
                              {"name": "time",
                               "type": "timestamp_t",
                               "description": "Test"},
                              {"name": "times",
                               "type": "timestamp_t",
                               "array_size": 3,
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
                  "description": "testing 1 2 testingXXXXXXXXXXXXXXXXXX",
                  "elements": [{"name": "xxx",
                                "type": "sys_t",
                                "array_size": 2,
                                "description": "aasdf"}]}]

    from pprint import pprint
    import typedef_parser
    typedef_parser.fill_typedef_parameters(typedefs)
    exp_type = _expand_typedefs(typedefs)
    exp_type[1]['elements'][2]['array'][0]['elements'][0]['description'] = "test1"
    exp_type[1]['elements'][2]['array'][1]['elements'][0]['description'] = "test2"

    #pprint(exp_type)
    mem_map = _expand_mem_map(typedefs)
    mem_map2 = copy.deepcopy(mem_map)
    mem_map2['elements'][0]['array'][1]['elements'][3]['access'] = 55

    #print("=========================================\n")
    #pprint(mem_map, width=120)
    #print("=========================================\n")
    #pprint(mem_map2, width=120)
    #print("=========================================\n")
    mem_map_expanded = asdf(typedefs)
    pprint(mem_map_expanded, width=120)



if __name__ == "__main__":
    main()
