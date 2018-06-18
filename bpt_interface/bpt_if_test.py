import bpt_if
import logging
import argparse


parser = argparse.ArgumentParser()
parser.add_argument("--log", help='Set the logging level (DEBUG, INFO, WARN)')
args = parser.parse_args()

if args.log is not None:
    loglevel = args.log
    numeric_level = getattr(logging, loglevel.upper(), None)
    if not isinstance(numeric_level, int):
        raise ValueError('Invalid log level: %s' % loglevel)
    logging.basicConfig(level=loglevel)

bpt = bpt_if.BptIf()
bpt.execute_changes()
bpt.reset_mcu()
cmds = bpt.get_command_list()
logging.debug("================================================================\
======")
for cmd in cmds:
    cmd()
    logging.debug("------------------------------------------------------------\
----------")
logging.debug("================================================================\
======")
