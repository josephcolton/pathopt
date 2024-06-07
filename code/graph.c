#include <string.h> // strncpy
#include <stdlib.h> // malloc
#include <stdio.h>  // printf

#include "global.h"
#include "metrics.h"
#include "graph.h"

/*************************
 * Global Node Functions *
 *************************/

/*
 * create_global_node - Create a global node for the linked list
 *
 * Arguments:
 *   name        - Node name
 *   description - Node description
 *
 * Allocates the memory for the struct, assigns values and returns
 * the completed struct.
 */
global_node_struct *create_global_node(char *name, char *description) {
  global_node_struct *new_node = malloc(sizeof(global_node_struct));
  if (new_node == NULL) {
    printf("Memory Allocation failed: create_global_node('%s', '%s')\n", name, description);
    exit(1);
  }
  strncpy(new_node->name, name, NAME_MAX_LENGTH);
  strncpy(new_node->description, description, NAME_MAX_LENGTH);
  new_node->next = NULL;
  return new_node;
}

/*
 * get_global_node_name_by_index - Lookup the name of a node based on the index number
 *
 * Arguments:
 *   index - Index number of the node name
 *
 * Returns a pointer to the name of the node at the given index.  Returns NULL if the
 * index number is not found.
 */
char *get_global_node_name_by_index(int index) {
  int curnum = 0;
  for (global_node_struct *curnode=global_nodes; curnode != NULL; curnode=curnode->next) {
    if (index == curnum) return (curnode->name);
    curnum++;
  }
  return NULL;
}

/*
 * add_global_node - Insert a global node into the linked list
 *
 * Arguments:
 *   name        - Node name
 *   description - Node description
 *
 * Creates and inserts a global node.
 */
void add_global_node(char *name, char *description) {
  // Create new node
  global_node_struct *new_node = create_global_node(name, description);
  new_node->next = global_nodes;
  global_nodes = new_node;
}

/*
 * free_global_nodes - Free all global nodes in the linked list
 *
 * Frees each node in the global_nodes linked list.
 */
void free_global_nodes() {
  // Current node
  global_node_struct *temp;
  // Remove nodes
  while (global_nodes != NULL) {
    temp = global_nodes;
    global_nodes = global_nodes->next;
    free(temp);
  }
}

/*
 * global_node_count - Counts the number of global nodes
 *
 * Returns the number of global nodes.
 */
unsigned int global_node_count() {
  unsigned int total = 0;
  global_node_struct *current;
  current = global_nodes;
  while(current != NULL) {
    total++;
    current = current->next;
  }
  return total;
}

/************************
 * Local Node Functions *
 ************************/

/*
 * create_node - Create a local node for the linked list
 *
 * Arguments:
 *   name - Name of node to create
 *
 * Allocates the memory for the struct, assigns values and returns
 * the completed struct.
 */
node_struct *create_node(char *name) {
  node_struct *new_node = malloc(sizeof(node_struct));
  if (new_node == NULL) {
    printf("Memory Allocation failed: create_node('%s')\n", name);
    exit(1);
  }
  strncpy(new_node->name, name, NAME_MAX_LENGTH);
  new_node->next = NULL;
  return new_node;
}


/*************************
 * Global Edge Functions *
 *************************/

/*
 * init_global_edge_table - initializes memory for hash table
 *
 * hash table is initialized with size HASH_SIZE
 */
void init_global_edge_table() {
  for(int i=0; i < HASH_SIZE; i++) edge_hash_table[i] = NULL;
}

/*
 * free_global_edge_table - frees all mem allocations of hash table
 *
 * frees all data in hash table of size HASH_SIZE, including item
 * connected with linked lists
 */
void free_global_edge_table() {
  edge_struct *temp;
  for(int i=0; i < HASH_SIZE; i++) {
    while(edge_hash_table[i] != NULL) {
      temp = edge_hash_table[i];
      edge_hash_table[i] = temp->next;
      free(temp);
    }
  }
}

/*
 * add_global_edge - Adds an edge node with metrics to the global list/hash
 *
 * Arguments:
 *   srcname - Source node name (should match a node name
 *   dstname - Destination name (same as above)
 *   metric  - Metrics for the edge (linked list)
 *
 * Adds an edge node and sets the srcname, dstname, and metrics.  It puts
 * the new edge node in the global linked list/hash.
 */
void add_global_edge(char *srcname, char *dstname, struct metric_struct *metric) {
  /* this function's input arguments metric_struct *metric
   * then it will create and insert the edge into the hashmap by calling helper functions:
   * edge_struct *create_global_edge(char *srcname, char *dstname, struct metric_struct *metric) {
   * unsigned int global_edge_hash(const char* srcname, const char* dstname, int size);
   */
  if (DEBUG) {
    printf("add_global_edge('%s', '%s', metrics)\n", srcname, dstname);
    display_metrics(stdout, " - metrics: ", metric, "\n"); // See metrics.c .h
  }

  edge_struct *new_edge = create_global_edge(srcname, dstname, metric);
  unsigned int key = global_edge_hash_key(srcname, dstname);

  // Insert the edge into the edge_hash_table
  if (edge_hash_table[key] == NULL) {
    // Insert into empty list
    edge_hash_table[key] = new_edge;
  } else {
    // Add to hash table stack
    new_edge->next = edge_hash_table[key];
    edge_hash_table[key] = new_edge;
  }
}

/*
 * get_global_edge - Returns a pointer to the requested edge
 *
 * Arguments:
 *   srcname - Source node name (should match a node name)
 *   dstname - Destination name (same as above)
 *
 * Looks up the edge in the edge edge_hash_table and returns a pointer to the active edge node.
 */
edge_struct *get_global_edge(char *srcname, char *dstname) {
  unsigned int key = global_edge_hash_key(srcname, dstname);

  for(edge_struct *current = edge_hash_table[key]; current != NULL; current = current->next) {
    if (strncmp(current->srcname, srcname, NAME_MAX_LENGTH) == 0 &&
	strncmp(current->dstname, dstname, NAME_MAX_LENGTH) == 0) return current;
  }

  return NULL;
}


/*
 * create_global_edge - Creates an edge node with metrics to the global list/hash
 *
 * Arguments:
 *   srcname - Source node name (should match a node name)
 *   dstname - Destination name (same as above)
 *   metric  - Metrics for the edge (linked list)
 *
 * Creates an edge node and sets the srcname, dstname, and metrics.  It puts
 * the new edge node in the global linked list/hash.
 */
edge_struct *create_global_edge(char *srcname, char *dstname, struct metric_struct *metric) {
  // allocate mem for edge
  edge_struct *new_edge = malloc(sizeof(edge_struct));
  if (new_edge == NULL) {
    printf("Memory Allocation failed: create_global_edge('%s', '%s', %s)\n", srcname, dstname, "metric");
    exit(1);
  }
  // Store source and destination for edge_struct
  strncpy(new_edge->srcname, srcname, NAME_MAX_LENGTH);
  strncpy(new_edge->dstname, dstname, NAME_MAX_LENGTH);
  new_edge->next = NULL;

  // store metric ptr with edge
  new_edge->metrics = metric;
  return new_edge;
}

/*
 * global_edge_hash_key - Creates a edge hash using src and dst as key
 *
 * Arguments:
 *   srcname - Source node name (should match a node name)
 *   dstname - Destination name (same as above)
 *
 * Creates a key for edge edge_hash_table to search and add edge_struct nodes
 */
unsigned int global_edge_hash_key(const char *srcname, const char *dstname) {
    unsigned int hash = 0;

    // Add source node name to hash
    for(int i=0; i < NAME_MAX_LENGTH; i++) {
      if (srcname[i] == 0) break;
      hash += ((int)srcname[i] * (i + 1));
    }

    // Add destination node name to hash
    for(int i=0; i < NAME_MAX_LENGTH; i++) {
      if (dstname[i] == 0) break;
      hash += ((int)dstname[i] * (i + 1));
    }

    return (hash % HASH_SIZE);
}

/*
 * global_edge_hash_count - Counts the number of edges in the hash
 *
 * Returns the number of edges in the hash table.
 */
unsigned int global_edge_hash_count() {
  unsigned int total = 0;
  edge_struct *current;
  for(int i=0; i < HASH_SIZE; i++) {
    current = edge_hash_table[i];
    while(current != NULL) {
      total++;
      current = current->next;
    }
  }
  return total;
}
