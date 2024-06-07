#ifndef PATHS_H
#define PATHS_H

/*************************
 * Path Object Functions *
 *************************/

path_struct *create_path(char *srcname, char *dstname);
path_struct *extend_path(path_struct *path, char *nodename);
void delete_path(path_struct *path);
void display_path(FILE *outfile, char *prefix, path_struct *path, char *postfix);
int path_contains(path_struct *path, char *nodename);

/********************
 * Path Comparisons *
 ********************/

path_collection_struct *new_path_collection(char *srcname, char *dstname);
void init_global_path_table();
int dominates_path(path_struct *path, path_struct *other);
int duplicate_path(path_struct *first, path_struct *second);
int clear_dominated_paths(path_collection_struct *collection);
int add_path_attempt(path_collection_struct *collection, path_struct *path);
void path_optimize_source(char *srcname);
path_collection_struct *path_collection_lookup(char *srcname, char *dstname);
void write_optimized_paths(FILE *outfile);

#endif
