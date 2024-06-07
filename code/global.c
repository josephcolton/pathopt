#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"

#include "graph.h"

/*******************************
 * Initialize Global Variables *
 *******************************/
global_metric_struct *global_metrics;               // Keeps track of list of metrics
global_node_struct *global_nodes;                   // Keeps track of the list of nodes
edge_struct *edge_hash_table[HASH_SIZE];            // Keeps track of edges
path_collection_struct *path_hash_table[HASH_SIZE]; // Keeps track of non-dominated paths

// CLI variables
int verbose;                     // verbose debugging level
char infile[NAME_MAX_LENGTH];    // Used for input when not stdin
char outfile[NAME_MAX_LENGTH];   // Used for output when not stdout
char debugfile[NAME_MAX_LENGTH]; // Used for output when not stdout

// Global Initialization function
void global_initialize() {
  global_metrics = NULL;
  global_nodes = NULL;
  init_global_edge_table();
}
