#!/usr/bin/env python3
"""This module contains helpers for the code generator"""
import re

PRIM_TYPES = {'uint8_t': 1, 'int8_t': 1, 'uint16_t': 2, 'int16_t': 2,
              'uint32_t': 4, 'int32_t': 4, 'uint64_t': 8, 'int64_t': 8,
              'char': 1, 'float': 4, 'double': 8}

TEST_MD = {"name": "test_mem_map",
           "author": "Kevin Weiss",
           "revision": "1.00.00"}

TEST_T = [{"name": "timestamp_t",
           "size": 8,
           "description": "Time and date",
           "elements": [{"name": "second",
                         "type": "uint8_t",
                         "description": "The seconds in decimal"},
                        {"name": "min",
                         "type": "uint16_t",
                         "description": "The min, in decimal"}]},
          {"name": "sys_t",
           "description": "System settings for the bpt",
           "elements": [{"name": "sn",
                         "type": "uint8_t",
                         "array_size": 12,
                         "description": "Unique ID of the device"},
                        {"name": "time",
                         "type": "timestamp_t",
                         "description": "test time struct"},
                        {"name": "times",
                         "type": "timestamp_t",
                         "array_size": 3,
                         "description": "Testing time struct array"},
                        {"name": "mode",
                         "bit_type": "uint8_t",
                         "type": "i2c_mode_t",
                         "bitfield": [{"name": "addr_10_bit",
                                       "bits": 1,
                                       "description": "10 bit address enable"},
                                      {"name": "general_call",
                                       "bits": 3}]}]},
          {"name": "test_mem_map_t",
           "description": "This |should test many aspects of the code gener",
           "elements": [{"name": "sys_array",
                         "type": "sys_t",
                         "array_size": 2,
                         "description": "testing arrays of special types"}]}]


def to_camel_case(str_to_convert):
    """Convert a string to camel case."""
    return ''.join(x for x in str_to_convert.title().replace('_', ' ')
                   if not x.isspace())


def to_underscore_case(str_to_convert):
    """Convert a string to underscore case."""
    str_to_convert = str_to_convert.replace(' ', '_')
    str_to_convert = re.sub('(.)([A-Z][a-z]+)', r'\1_\2', str_to_convert)
    return re.sub('([a-z0-9])([A-Z])', r'\1_\2', str_to_convert).lower()


def main():
    """Tests basic helper functions."""
    print(PRIM_TYPES)
    print(TEST_MD)
    print(TEST_T)
    print(to_underscore_case("hello world_test1!"))
    print(to_camel_case("heLlo worLd_test2!"))


if __name__ == "__main__":
    main()
