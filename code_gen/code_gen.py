#!/usr/bin/env python3
"""This module handles parsing of typedefs and creating outputs"""
import os
import json
import argparse
from td_parser import parse_basic_typedefs
from mm_parser import parse_typedefs_to_mem_map, import_mem_map_values
from td_output_parser import parse_typedefs_to_h
from mm_output_parser import parse_mem_map_to_access_c, parse_mem_map_to_csv
from mm_output_parser import parse_mem_map_to_if
from gen_helpers import to_underscore_case


def _parse_filename(f_arg, d_arg, name_contains):
    if f_arg is None:
        for file in os.listdir(os.path.join(os.path.dirname(__file__), d_arg)):
            if file.endswith(".json"):
                if name_contains in file:
                    return file
    return f_arg


def main():
    """Parses typedefs and creates outputs"""

    parser = argparse.ArgumentParser()
    parser.add_argument("--input_dir",
                        help='Directory where all input files are stored',
                        default='saved_setups/')
    parser.add_argument("--output_dir",
                        help='Output directory where results are stored',
                        default='output/')
    parser.add_argument("--import_td_f",
                        help='Typedef file to import and parse')
    parser.add_argument("--import_mm_f",
                        help='Saved memory map file to not '
                        'overwrite changes to access, '
                        'description, and default values')
    parser.add_argument("--output_mm_f",
                        help='Output file for the memory map')
    parser.add_argument("--csv_name",
                        help='Output filename of the memory map csv',
                        default='mem_map.csv')
    parser.add_argument("--td_h_name",
                        help='Output filename of the typedef c header',
                        default='app_typedef.h')
    parser.add_argument("--access_name",
                        help='Output filename of the access.c file',
                        default='app_access.c')
    parser.add_argument("--py_if_name",
                        help='Output filename of python interface')
    parser.add_argument("--if_parent",
                        help='Name of the parent class the python interface '
                             'is based on',
                        default="BaseDevice")
    args = parser.parse_args()

    import_td_f = _parse_filename(args.import_td_f, args.input_dir, "typedef")
    import_mm_f = _parse_filename(args.import_mm_f, args.input_dir, "mem_map")

    with open(args.input_dir + import_td_f) as td_f:
        json_data = json.load(td_f)
    typedefs = json_data['typedefs']
    metadata = json_data['metadata']

    typedefs = parse_basic_typedefs(typedefs)
    mem_map = parse_typedefs_to_mem_map(typedefs)

    if import_mm_f is not None:
        with open(args.input_dir + import_mm_f) as mm_f:
            imported_mem_map = json.load(mm_f)
        import_mem_map_values(mem_map, imported_mem_map)

    if not os.path.exists(args.input_dir):
        os.makedirs(args.input_dir)
    with open(args.input_dir + import_mm_f, 'w') as outfile:
        json.dump(mem_map, outfile, indent=4, sort_keys=True)

    if not os.path.exists(args.output_dir):
        os.makedirs(args.output_dir)

    with open(args.output_dir + args.csv_name, 'w') as outfile:
        outfile.write(parse_mem_map_to_csv(mem_map))

    with open(args.output_dir + args.access_name, 'w') as outfile:
        outfile.write(parse_mem_map_to_access_c(mem_map))

    if args.py_if_name is None:
        args.py_if_name = to_underscore_case(metadata['name'])
    with open(args.output_dir + args.py_if_name + '.py', 'w') as outfile:
        outfile.write(parse_mem_map_to_if(mem_map, args.py_if_name,
                                          args.if_parent))

    with open(args.output_dir + args.td_h_name, 'w') as outfile:
        outfile.write(parse_typedefs_to_h(typedefs, metadata))


if __name__ == "__main__":
    main()
