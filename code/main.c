#include <stdio.h>
#include <omp.h>

#include "global.h"
#include "metrics.h"
#include "graph.h"
#include "parser.h"
#include "paths.h"

int main(int argc, char *argv[]) {
  global_initialize(); // Initialize global variables

  // We should have both input and output.
  // We need a usage function as well to call if there is an issue with arguments

  printf("File: %s\n", argv[1]);
  printf("\nParsing Input File\n");
  parse_file(argv[1]);

  // Initialize non-dominated path lists
  printf("\nInitializing Non-Dominated Path Lists\n");
  init_global_path_table();
  
  // Optimize each pair one source at a time
  // This is where parallel programming would help
  printf("\nCalculating Updated Non-Dominated Path Lists\n");
  int nodecount = global_node_count();

  // Optional omp used for speeding up the process
  #pragma omp parallel for
  for (int i=0; i<nodecount; i++) {
    char *nodename = get_global_node_name_by_index(i);
    path_optimize_source(nodename);
  }

  // Output the results
  printf("\nGenerating Results File\n");
  write_optimized_paths(stdout);

  return 0;
}
