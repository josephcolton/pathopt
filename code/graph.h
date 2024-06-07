#ifndef NODES_H
#define NODES_H

/*************************
 * Global Node Functions *
 *************************/

global_node_struct *create_global_node(char *name, char *description);
char *get_global_node_name_by_index(int index);
void add_global_node(char *name, char *description);
void free_global_nodes();
unsigned int global_node_count();

/************************
 * Local Node Functions *
 ************************/

node_struct *create_node(char *name);

/*************************
 * Global Edge Functions *
 *************************/

void add_global_edge(char *srcname, char *dstname, struct metric_struct *metric);
edge_struct *get_global_edge(char *srcname, char *dstname);
edge_struct *create_global_edge(char *srcname, char *dstname, struct metric_struct *metric);
unsigned int global_edge_hash_key(const char* srcname, const char* dstname);
void init_global_edge_table();
void free_global_edge_table();
unsigned int global_edge_hash_count();

#endif
