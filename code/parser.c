#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "metrics.h"
#include "graph.h"
#include "parser.h"

/********************
 * Parser Functions *
 ********************/

/*
 * parse_file - Parses the metrics, nodes, and edges of a file into the global data structures.
 *
 * Parses a file in the format of the files in the "examples" subfolder, with an [edges], [nodes], and
 * [metrics] section, into the global data structures specified in metrics.h.
 * It then calls each of the individual line processing functions to parse the lines in each of their
 * respective sections.
 */
void parse_file(char* filename) {
  if (DEBUG) printf("parse_file(\"%s\")\n", filename);

  FILE* file = fopen(filename, "r");

  if (file == NULL) {
    printf("Error opening file!");
    return;
  }

  char line[BUFFER_LEN]; // defined in global.h
  int section = ERROR;   // defined in global.h

  while (fgets(line, BUFFER_LEN, file)) {
    // Remove newline character if present.
    line[strcspn(line, "\n")] = 0;
    
    // Ignore blank lines and comments
    if (strlen(line) == 0) continue;
    if (line[0] == '#') continue;
    
    // Detect section headings
    if (line[0] == '[') {
      // Determine the section
      if (strncmp(line, "[metrics]", 9) == 0) section = SECTION_METRICS;
      else if (strncmp(line, "[nodes]", 7) == 0) section = SECTION_NODES;
      else if (strncmp(line, "[edges]", 7) == 0) section = SECTION_EDGES;
      else {
	printf("Error: Invalid section \"%s\"\n", line);
	exit(1);
      }
      
    } else {
      if (section == ERROR) {
	printf("Error: Not in a section \"%s\"", line);
	exit(1);
      }
      
      // Parse section lines
      if (section == SECTION_METRICS) parse_metric(line);
      else if (section == SECTION_NODES) parse_node(line);
      else if (section == SECTION_EDGES) parse_edge(line);
      else {
	printf("Error: Invalid Section ID %d\n", section);
	exit(1);
      }
    } // End of section lines
  } // End of input

  // Display loaded statistics
  if (DEBUG) {
    printf("\nParsed Statistics:\n");
    printf("Total Metrics: %d\n", global_metric_count());
    printf("Total Nodes: %d\n", global_node_count());
    printf("Total Edges: %d\n", global_edge_hash_count());
  }
  
} // End of parse_file

/*
 * parse_metric - Parses a single metric line
 *
 * Arguments:
 *   line - A line of text from the metrics section of an input file
 *
 * Parses a line from the metrics file into the parts required by 
 * add_global_metric, then passes the parts to that function.
 */
void parse_metric(char *line) {
  char *token;
  char metric[NAME_MAX_LENGTH];
  int optimization = ERROR, combination = ERROR;
  double arg = 0;

  if (DEBUG) printf("parse_metric(\"%s\")\n", line);

  // Get metric name
  token = strtok(line, "|");
  strncpy(metric, token, NAME_MAX_LENGTH);
	  
  // Get metric optimization
  token = strtok(NULL, "|");
  if (strncmp(token, "MIN", 3) == 0) optimization = OPT_MIN;
  else if (strncmp(token, "MAX", 3) == 0) optimization = OPT_MAX;
  else {
    printf("Error: Invalid optimization strategy \"%s\"\n", token);
    exit(1);
  }

  // Get metric combination method
  token = strtok(NULL, "|");
  if (strncmp(token, "MIN", 3) == 0) combination = COMBO_MIN;
  else if (strncmp(token, "MAX", 3) == 0) combination = COMBO_MAX;
  else if (strncmp(token, "ADD", 3) == 0) {
    combination = COMBO_ADD;
    // Parse optional arg
    if (token[3] == '+') arg = atof(token+4);
  }
  else {
    printf("Error: Invalid combination method \"%s\"\n", token);
    exit(1);
  }

  // Add metric information to global list
  add_global_metric(metric, optimization, combination, arg);
}

/*
 * parse_node - Parses a single node line
 *
 * Arguments:
 *   line - A line of text from the nodes section of an input file
 *
 * Parses a line from the nodes section into the parts required by 
 * add_global_node, then passes the parts to that function.
 */
void parse_node(char *line) {
  char *token;
  char name[NAME_MAX_LENGTH];
  char descr[NAME_MAX_LENGTH];

  if (DEBUG) printf("parse_node(\"%s\")\n", line);

  // Get the name
  token = strtok(line, "|");
  strncpy(name, token, NAME_MAX_LENGTH);

  // Get the description
  token = strtok(NULL, "|");
  strncpy(descr, token, NAME_MAX_LENGTH);

  // Add node to global list
  add_global_node(name, descr);
}

/*
 * parse_edge - Parses a single edge line
 *
 * Arguments:
 *   line - A line of text from the edges section of an input file
 *
 * Parses a line from the edges section into the parts required by 
 * add_global_edge, then passes the parts to that function.
 */
void parse_edge(char *line) {
  char *token;
  char *metric_token;
  char metric_name[NAME_MAX_LENGTH];
  double metric_value;
  char srcnode[NAME_MAX_LENGTH];
  char dstnode[NAME_MAX_LENGTH];
  metric_struct *metrics = NULL;
  int i;
  
  if (DEBUG) printf("parse_edge(\"%s\")\n", line);

  // Get the source node
  token = strtok(line, "|");
  strncpy(srcnode, token, NAME_MAX_LENGTH);

  // Get the destination node
  token = strtok(NULL, "|");
  strncpy(dstnode, token, NAME_MAX_LENGTH);

  // Get the metrics
  token = strtok(NULL, "|");
  metric_token = strtok(token, ",");
  while(metric_token != NULL) {
    // Divide name=value
    strncpy(metric_name, metric_token, NAME_MAX_LENGTH);
    for(i=0; i < NAME_MAX_LENGTH; i++) {
      if (metric_name[i] == '=') {
	metric_name[i] = 0; // Metric Name
	metric_value = atof(metric_name + i + 1); // Metric value
	// Create or add to metrics list
	if (metrics == NULL) {
	  metrics = create_metrics_node(metric_name, metric_value);
	} else {
	  metrics = add_metrics_node(metrics, metric_name, metric_value);
	}
	break;
      }
    }
    metric_token = strtok(NULL, ",");
  }

  add_global_edge(srcnode, dstnode, metrics);
}
