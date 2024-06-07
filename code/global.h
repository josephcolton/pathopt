#ifndef GLOBAL_H
#define GLOBAL_H

#define DEBUG 0

// Optimization options for metrics
#define OPT_MIN 0
#define OPT_MAX 1

// Combination options for combining metrics
#define COMBO_ADD 0
#define COMBO_MIN 1
#define COMBO_MAX 2

// Section values for parser
#define SECTION_METRICS 0
#define SECTION_NODES 1
#define SECTION_EDGES 2

// Status values
#define ERROR -1

// Maximum string length
#define BUFFER_LEN 512
#define NAME_MAX_LENGTH 50

// Hash Map
#define HASH_SIZE 37

/***********
 * Structs *
 ***********/

// Global Metrics
typedef struct global_metric_struct {
  char name[NAME_MAX_LENGTH]; // Metric name
  unsigned int opt;           // Optimization method
  unsigned int combo;         // Combination method
  double arg;                 // Optional additional argument
  struct global_metric_struct *next;
} global_metric_struct;

// Edge Metrics
typedef struct metric_struct {
  char name[NAME_MAX_LENGTH]; // Metric name
  double value;
  struct metric_struct *next;
} metric_struct;

// Global Edges
typedef struct edge_struct {
  char srcname[NAME_MAX_LENGTH];
  char dstname[NAME_MAX_LENGTH];
  metric_struct *metrics;
  struct edge_struct *next;
} edge_struct;

// Global Nodes
typedef struct global_node_struct {
  char name[NAME_MAX_LENGTH];
  char description[NAME_MAX_LENGTH];
  struct global_node_struct *next;
} global_node_struct;

// Local Nodes
typedef struct node_struct {
  char name[NAME_MAX_LENGTH];
  struct node_struct *next;
} node_struct;

// Path Object
typedef struct path_struct {
  char srcname[NAME_MAX_LENGTH];
  char dstname[NAME_MAX_LENGTH];
  node_struct *nodes;
  metric_struct *metrics;
  struct path_struct *next;
} path_struct;

// Path Hash Table
typedef struct path_collection_struct {
  char srcname[NAME_MAX_LENGTH];
  char dstname[NAME_MAX_LENGTH];
  path_struct *optlist;
  struct path_collection_struct *next;
} path_collection_struct;

/********************
 * Global Variables *
 ********************/
void global_initialize();

// Command line flags
extern int verbose;                     // verbose debugging level
extern char infile[NAME_MAX_LENGTH];    // Used for input when not stdin
extern char outfile[NAME_MAX_LENGTH];   // Used for output when not stdout
extern char debugfile[NAME_MAX_LENGTH]; // Used for output when not stdout

// Internal data structures
extern global_metric_struct *global_metrics;               // Keeps track of list of metrics
extern global_node_struct *global_nodes;                   // Keeps track of the list of nodes
extern edge_struct *edge_hash_table[HASH_SIZE];            // Create the edge hash table
extern path_collection_struct *path_hash_table[HASH_SIZE]; // Create the path hash table

#endif
