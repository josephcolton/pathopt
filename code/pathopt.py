#!/usr/bin/python3

import ctypes
import sys

class PathOpt:
    def __init__(self, filename):
        self.pathopt = ctypes.CDLL("./pathopt.so")

        # Initialize pathopt library
        self.pathopt.global_initialize()

        # Read in file
        self.pathopt.parse_file.argtypes = [ctypes.c_char_p]
        self.filename = ctypes.c_char_p(filename.encode('utf-8'))
        self.pathopt.parse_file(self.filename)

        # Initialize non-dominated path lists
        self.pathopt.init_global_path_table();

        # Optimize each pair one source at a time
        nodecount = self.pathopt.global_node_count()
        for i in range(nodecount):
            self.pathopt.path_optimize_source_id(i)

    def get_results(self, outfile=None):
        # Output the results
        print("Generating Results")
        self.pathopt.write_optimized_paths(outfile)

###################
# Execute to Test #
###################
if __name__ == "__main__":
    po = PathOpt(sys.argv[1])
    po.get_results()
