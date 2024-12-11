#include <string.h>  // strncpy, strncmp
#include <stdlib.h>  // malloc, free
#include <stdbool.h> // true, false
#include <stdio.h>   // printf, stdin

#include "global.h"
#include "metrics.h"
#include "graph.h"
#include "paths.h"

/*************************
 * Path Object Functions *
 *************************/

/*
 * create_path - Create a path object with a single edge metric
 *
 * Arguments:
 *   srcname - Name of source node
 *   dstname - Name of destination node
 *
 * Creates the path object, initializes all the variables and returns the pointer.
 */
path_struct *create_path(char *srcname, char *dstname) {
  // Create path object
  path_struct *new_path = malloc(sizeof(path_struct));
  // Copy name values
  strncpy(new_path->srcname, srcname, NAME_MAX_LENGTH);
  strncpy(new_path->dstname, dstname, NAME_MAX_LENGTH);
  // Create nodes
  new_path->nodes = create_node(srcname);
  new_path->nodes->next = create_node(dstname);
  // Copy metrics
  edge_struct *edge = get_global_edge(srcname, dstname);
  new_path->metrics = copy_metrics(edge->metrics);
  // Set next to null
  new_path->next = NULL;
  // Return the new path object
  return new_path;
}

/*
 * extend_path - Creates a new path extending the source path
 *
 * Arguments:
 *   path - The source path the copy
 *   nodename - The name of the node to add to the end
 *
 * Takes a path and a node name and adds the node to the path.
 * It creates a new longer path and returns it.
 */
path_struct *extend_path(path_struct *path, char *nodename) {
  // Create path object
  path_struct *new_path = malloc(sizeof(path_struct));
  // Copy name values
  strncpy(new_path->srcname, path->srcname, NAME_MAX_LENGTH);
  strncpy(new_path->dstname, nodename, NAME_MAX_LENGTH);
  // Copy source nodes
  new_path->nodes = create_node(path->srcname);
  node_struct *new_node = new_path->nodes;
  for (node_struct *old_node = path->nodes; old_node != NULL; old_node = old_node->next) {
    if (old_node->next != NULL) {
      new_node->next = create_node(old_node->next->name);
      new_node = new_node->next;
    }
  }
  // Create temporary new edge variable
  edge_struct *new_edge = get_global_edge(new_node->name, nodename);
  // Create and set the combined metrics
  new_path->metrics = combine_metrics(path->metrics, new_edge->metrics);

  // Add the additional node
  new_node->next = create_node(nodename);

  // Set next to null
  new_path->next = NULL;

  // Return the new path
  return new_path;
}


/*
 * delete_path - Deletes a path object and related linked lists
 *
 * Arguments:
 *   path - Path object to delete
 *
 * Deletes the path object and all related linked lists.  It does not follow the
 * next pointer to other paths.
 */
void delete_path(path_struct *path) {
  // Temp struct pointers
  node_struct *temp1;
  metric_struct *temp2;
  // Delete nodes
  while(path->nodes != NULL) {
    temp1 = path->nodes;
    path->nodes = temp1->next;
    free(temp1);
  }
  // Delete metrics
  while(path->metrics != NULL) {
    temp2 = path->metrics;
    path->metrics = temp2->next;
    free(temp2);
  }
  // disconnect next (just in case)
  path->next = NULL;
  // Delete path object
  free(path);
}

/*
 * display_path - Prints out the current path nodes
 *
 * Arguments:
 *   outfile - File pointer to print output to (includes stdout, stderr)
 *   prefix  - Text before the path nodes
 *   path    - The path object
 *   postfix - Text after the path nodes
 *
 * Prints out the prefix, then the path nodes, then the postfix.
 */
void display_path(FILE *outfile, char *prefix, path_struct *path, char *postfix) {
  fprintf(outfile, "%s", prefix);
  for (node_struct *current = path->nodes; current != NULL; current = current->next) {
    fprintf(outfile, "%s", current->name);
    if (current->next != NULL) fprintf(outfile, ",");
  }
  fprintf(outfile, "%s", postfix);
}

/*
 * path_contains - Indicates if a node exists in a path
 *
 * Arguments:
 *   path - Path to search
 *   nodename - Node to search for
 *
 * Searchs through a path to determine if the nodename node exists.
 * return true if found, otherwise it returns false.
 */
int path_contains(path_struct *path, char *nodename) {
  for(node_struct *curnode=path->nodes;curnode != NULL; curnode=curnode->next) {
    if (strncmp(curnode->name, nodename, NAME_MAX_LENGTH) == 0) return true;
  }
  return false;
}

/********************
 * Path Comparisons *
 ********************/

/*
 * new_path_collection - Create a path collection object
 *
 * Arguments:
 *   srcname - Source node name
 *   dstname - Destination node name
 *
 * Creates a single path_collection_struct with a single path entry for
 * the direct path.  Returns the created path_collection_struct object.
 */
path_collection_struct *new_path_collection(char *srcname, char *dstname) {
  // Create path collection object
  path_collection_struct *newcollection = malloc(sizeof(path_collection_struct));
  // Copy name values
  strncpy(newcollection->srcname, srcname, NAME_MAX_LENGTH);
  strncpy(newcollection->dstname, dstname, NAME_MAX_LENGTH);
  // Create initial direct path
  newcollection->optlist = create_path(srcname, dstname);
  // Set next to null
  newcollection->next = NULL;
  // Return the new path collection
  return newcollection;
}

/*
 * init_global_path_table - Initalizes the global path collection table
 *
 * Creates the initial path collection tables with a single direct connection
 * entry for each one.  At this point it should be ready for path expansion
 * exploration.
 */
void init_global_path_table() {
  // Set path hash table pointers to NULL
  for(int i=0; i<HASH_SIZE; i++) path_hash_table[i] = NULL;

  // path_collection_struct *path_hash_table[HASH_SIZE];
  for(global_node_struct *srcnode=global_nodes; srcnode != NULL; srcnode=srcnode->next) {
    for(global_node_struct *dstnode=global_nodes; dstnode != NULL; dstnode=dstnode->next) {
      if (srcnode == dstnode) continue;
      // Create the path collection
      path_collection_struct *newcollection = new_path_collection(srcnode->name, dstnode->name);
      // Put the path collection in the correct location in the hash table
      unsigned int key = global_edge_hash_key(srcnode->name, dstnode->name);
      newcollection->next = path_hash_table[key];
      path_hash_table[key] = newcollection;
    }
  }
}

/*
 * dominates_path - Determine if a path dominates another path
 *
 * Arguments:
 *   path - Path to see if it dominates the other one
 *   other - The path to compare against
 *
 * Returns true if path dominates the other path.  Otherwise returns false.
 */
int dominates_path(path_struct *path, path_struct *other) {
  int better = 0;
  int worse = 0;

  // A path cannot dominate itself
  if (path == other) return false;

  // for loop through global metrics list
  for (global_metric_struct *gms = global_metrics; gms != NULL; gms = gms->next) {
    // Get values
    double value1, value2;
    value1 = get_metric_value(path->metrics, gms->name);
    value2 = get_metric_value(other->metrics, gms->name);

    // Optimizer comparison
    if (gms->opt == OPT_MAX) {
      if (value1 > value2) better++;
      if (value1 < value2) worse++;
    }
    if (gms->opt == OPT_MIN) {
      if (value1 < value2) better++;
      if (value1 > value2) worse++;
    }
  }

  if (better > 0 && worse == 0) {
    if (DEBUG) {
      display_metrics(stdout, "Path1: ", path->metrics, " dominates ");
      display_metrics(stdout, "Path2: ", other->metrics, "\n");
    }
    return true;
  }
  return false;
}

/*
 * duplicate_path - Sees if two paths have the same nodes
 *
 * Arguments:
 *   first  - First path to compare
 *   second - Second path to compare
 *
 * Checks the nodes and returns true if they match in the same order.
 */
int duplicate_path(path_struct *first, path_struct *second) {
  // Set nodes to the start of the list
  node_struct *node1 = first->nodes;
  node_struct *node2 = second->nodes;
  // Walk through the nodes comparing them
  while(node1 != NULL && node2 != NULL) {
    if (strncmp(node1->name, node2->name, NAME_MAX_LENGTH) == 0) {
      node1 = node1->next;
      node2 = node2->next;
    } else return false;
  }
  // See if we broke because both matched the whole way
  if (node1 == NULL && node2 == NULL) return true;
  // I guess only one was NULL, so no match
  return false;
}

/*
 * clear_dominated_paths - Removes any now dominated paths
 *
 * Arguments:
 *   collection - Path collection to clean up
 *
 * Looks through all the paths in the optimized list and sees
 * if any are now dominated.  If they are dominated, they are removed.
 * The number of paths removed is returned.
 */
int clear_dominated_paths(path_collection_struct *collection) {
  int removed = 0;
  int restart = false;

  // Do this loop as long as we keep removing paths
  do {
    restart = false;
    // Iterate through the list of paths
    for(path_struct *current=collection->optlist; current!=NULL; current=current->next) {
      // See if any of the other paths dominates this one.
      for(path_struct *test=collection->optlist; test!=NULL; test=test->next) {
	if (current == test) continue; // Cannot dominate itself

	if (dominates_path(test, current)) {
	  // Remove current from the list.
	  path_struct *listend = current->next; // Everything after the path to delete

	  // Remove if collection is the front path
	  if (collection->optlist == current) {
	    collection->optlist = listend;
	    delete_path(current); // delete path
	    removed++;
	  } else {
	    // Remove in the middle
	    path_struct *check = collection->optlist;
	    while(check != NULL) {
	      if (check->next == current) {
		check->next = listend;
		delete_path(current); // delete path
		removed++;
		break;
	      } else {
		check = check->next;
	      }
	    }
	  }
	  // Indicate we need to start the search over
	  restart = true;
	  break;
	} // End of dominatated path discovery
      }
      if (restart) break;
    }
  } while(restart);

  return removed;
}

/*
 * add_path_attempt - Trys to add a path to the collection
 *
 * Arguments:
 *   collection - Collection to attempt to add the path
 *   path - The path to attempt to add to the collection
 *
 * Tries to add the path to the collection.  If the path is not dominated
 * by any other paths in the collection, it is added and the value 1
 * is returned.  If it is not added, the number 0 is returned.
 */
int add_path_attempt(path_collection_struct *collection, path_struct *path) {
  if (collection == NULL) fprintf(stderr, "Error: Collection is NULL.  Cannot add path!\n");
  if (path == NULL) fprintf(stderr, "Error: Path is NULL.  Cannot add to collection!\n");

  // See if any of the paths dominate the path, or it is in the collection
  for(path_struct *current=collection->optlist; current!=NULL; current=current->next) {
    if (dominates_path(current, path)) return 0; // Domination check
    if (duplicate_path(current, path)) return 0; // Duplicate check
  }

  // Add the path if not dominated
  path->next = collection->optlist;
  collection->optlist = path;

  // Return 1 because the path was added
  return 1;
}

/*
 * path_optimize_source_id - Tries to optimize all routes starting with the source by id
 *
 * Arguments:
 *   id - Global node name index id
 *
 * Looks up the node name and calls the path_optimize_source function using the srcnode
 * name instead of the id.
 */
void path_optimize_source_id(int id) {
  char *nodename = get_global_node_name_by_index(id);
  path_optimize_source(nodename);
}


/*
 * path_optimize_source - Tries to optimize all routes starting with the source
 *
 * Arguments:
 *   srcname - Source node to start optimizing from
 *
 * Loops through all destinations and tries to optimize the path between the
 * pair.  It stops when it is able to run through a complete set of destinations
 * without finding any new optimized paths.
 */
void path_optimize_source(char *srcname) {
  if (DEBUG) printf("Optimizing Source %s\n", srcname);
  int changes = 0, status;
  int round = 0;

  // Process all destinations from source until no new paths are found
  do {
    round++;
    changes = 0;

    for(global_node_struct *dstnode=global_nodes; dstnode != NULL; dstnode=dstnode->next) {
      // Source and destination cannot be the same
      if (strncmp(srcname, dstnode->name, NAME_MAX_LENGTH) == 0) continue;
      // Load the path collection
      path_collection_struct *collection = path_collection_lookup(srcname, dstnode->name);

      // Try adding this path's dstname to all other non-dominated lists
      // for other destinations and see if they create non-dominated paths.
      for(global_node_struct *relay=global_nodes; relay != NULL; relay=relay->next) {
	// Skip invalid relay nodes
	if (strncmp(srcname, relay->name, NAME_MAX_LENGTH) == 0) continue;       // Cannot use source as relay
	if (strncmp(dstnode->name, relay->name, NAME_MAX_LENGTH) == 0) continue; // Cannot use destation as relay

	// Load relay collection to attempt to add the destination node
	path_collection_struct *relay_collection = path_collection_lookup(srcname, relay->name);
	for (path_struct *relaypath=relay_collection->optlist; relaypath != NULL; relaypath=relaypath->next) {
	  // Do not attempt to add destination node if in path
	  if (path_contains(relaypath, dstnode->name)) continue;
	  // Create trial path
	  path_struct *trial_path = extend_path(relaypath, dstnode->name);
	  if (DEBUG) display_path(stdout, "Attempting to add ", trial_path, "\n");
	  status = add_path_attempt(collection, trial_path);
	  changes += status; // Update the changes
	  if (status == 0) delete_path(trial_path); // Delete if not used
	  else changes += clear_dominated_paths(collection); // Delete any newly dominated paths
	}
      }

      // Clear any now dominated paths
      changes += clear_dominated_paths(collection);
    }

    if (DEBUG) printf("Round %d - Changes: %d\n", round, changes);
  } while(changes > 0);
}

/*
 * path_collection_lookup - Lookup a path collection object
 *
 * Arguments:
 *   srcname - Name of the source node
 *   dstname - Name of the destination node
 *
 * Look through the path collection hash table and find the path
 * collection object that contains the list of non-dominated
 * paths for that source/destination pair.
 */
path_collection_struct *path_collection_lookup(char *srcname, char *dstname) {
  // Find the correct hash bucket
  unsigned int key = global_edge_hash_key(srcname, dstname);
  // Look down the list for the collection object
  path_collection_struct *collection = path_hash_table[key];
  while(collection != NULL) {
    if (strncmp(collection->srcname, srcname, NAME_MAX_LENGTH) == 0 &&
        strncmp(collection->dstname, dstname, NAME_MAX_LENGTH) == 0) return collection;
    collection = collection->next;
  }
  fprintf(stderr, "Error: Could not find path collection!\n");
  return NULL;
}

/*
 * write_optimized_paths - Write the optimized paths to a file
 *
 * Arguments:
 *   outfile - File descriptor (open file, stdout, stderr)
 *
 * Writes all the current optimized paths out to the FILE pointer
 * passed in.
 */
void write_optimized_paths(FILE *outfile) {
  // Create variables
  path_collection_struct *collection = NULL;

  // Loop through all source/destination pairs
  for(global_node_struct *srcnode=global_nodes; srcnode != NULL; srcnode=srcnode->next) {
    for(global_node_struct *dstnode=global_nodes; dstnode != NULL; dstnode=dstnode->next) {
      if (srcnode == dstnode) continue;
      // Display the node pair
      if (outfile == NULL) printf("%s|%s\n", srcnode->name, dstnode->name);
      else fprintf(outfile, "%s|%s\n", srcnode->name, dstnode->name);
      // Get the collection of pareto optimal routes
      collection = path_collection_lookup(srcnode->name, dstnode->name);
      for (path_struct *path=collection->optlist;path!=NULL; path=path->next) {
	// Print out the metrics for the path
	display_metrics(outfile, "", path->metrics, "|");
	// print out the node list for the path
	for(node_struct *node=path->nodes;node!=NULL;node=node->next) {
	  if (outfile == NULL) printf("%s", node->name);
	  else fprintf(outfile, "%s", node->name);
	  if (node->next != NULL) {
	    if (outfile == NULL) printf(",");
	    else fprintf(outfile, ",");
	  }
	}
	if (outfile == NULL) printf("\n");
	else fprintf(outfile, "\n"); // End of the non-dominated path entry
      }
      if (outfile == NULL) printf("\n");
      else fprintf(outfile, "\n"); // Divide the pairs
    }
  }
}
