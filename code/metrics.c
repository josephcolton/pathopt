#include <string.h>  // strncpy, strncmp
#include <stdlib.h>  // malloc, free
#include <stdbool.h> // true, false
#include <stdio.h>   // printf

#include "global.h"
#include "metrics.h"

/****************************
 * Global Metrics Functions *
 ****************************/

/*
 * create_global_metrics_node - Create a global metrics node for the linked list
 *
 * Arguments:
 *   name  - Metrics name string
 *   opt   - Optimization method
 *   combo - Combination method
 *   arg   - Optional value
 *
 * Allocates the menory for the struct, assigns values and returns
 * the completed struct.
 */
global_metric_struct *create_global_metrics_node(char *name, unsigned int opt, unsigned int combo, double arg) {
  global_metric_struct *new_global_metrics_node = malloc(sizeof(global_metric_struct));
  if (new_global_metrics_node == NULL) {
    printf("Memory Allocation failed: global_metric_struct('%s', %u, %u, %f)\n", name, opt, combo, arg);
    exit(1);
  }
  strncpy(new_global_metrics_node->name, name, NAME_MAX_LENGTH);
  new_global_metrics_node->opt = opt;
  new_global_metrics_node->combo = combo;
  new_global_metrics_node->arg = arg;
  return new_global_metrics_node;
}

/*
 * add_global_metrics - Add metrics to global list of metrics
 *
 * Arguments:
 *   name  - Name of the metric
 *   opt   - Optimization strategy
 *   combo - Combination method
 *   arg   - Optional argument (see combo)
 *
 * Creates a new global metric node and adds it to the global_metrics linked list.
 */
void add_global_metric(char *name, unsigned int opt, unsigned int combo, double arg) {
  if (DEBUG) printf("add_global_metric(%s, %d, %d, %f)\n", name, opt, combo, arg);

  // Verify it does not already exist
  if (exists_global_metric(name)) {
    printf("Global Metric \"%s\" already exists!\n", name);
    return;
  }
  // Create new node
  global_metric_struct *new_node = create_global_metrics_node(name, opt, combo, arg);
  // Add new node to global metrics list
  new_node->next = global_metrics;
  global_metrics = new_node;
}

/*
 * exists_global_metric - Verifies if a name already exists in global_metrics
 *
 * Returns true if found, otherwise returns false.
 */
int exists_global_metric(char *name) {
  // External variables
  extern global_metric_struct *global_metrics;
  // Loop and look
  for(global_metric_struct *current = global_metrics; current != NULL; current = current->next) {
    if (strncmp(current->name, name, NAME_MAX_LENGTH) == 0) return true;
  }
  return false;
}

/*
 * global_metric_count - Counts the number of global metrics
 *
 * Returns the number of global metrics.
 */
unsigned int global_metric_count() {
  unsigned int total = 0;
  global_metric_struct *current;
  current = global_metrics;
  while(current != NULL) {
    total++;
    current = current->next;
  }
  return total;
}



/*********************
 * Metrics Functions *
 *********************/

/*
 * create_metrics_node - Create a metrics node for the linked list
 *
 * Arguments:
 *   name  - Metrics name string
 *   value - Metrics value string
 *
 * Allocates the memory for the struct, assigns values and returns
 * the completed struct.
 */
metric_struct *create_metrics_node(char *name, double value) {
  metric_struct *new_metrics_node = malloc(sizeof(metric_struct));
  if (new_metrics_node == NULL) {
    printf("Memory Allocation failed: create_metric_node('%s', %f)\n", name, value);
    exit(1);
  }
  strncpy(new_metrics_node->name, name, NAME_MAX_LENGTH);
  new_metrics_node->value = value;
  new_metrics_node->next = NULL;
  return new_metrics_node;
}

/*
 * add_metrics_node - Adds an additional metrics node
 *
 * Arguments:
 *   name  - Metrics name string
 *   value - Metrics value string
 *
 * Creates a new metric_struct and adds it to the existing linked list.
 * It then returns the new pointer to the linked list.
 */
metric_struct *add_metrics_node(metric_struct *current, char *name, double value) {
  metric_struct *new_node = create_metrics_node(name, value);
  new_node->next = current;
  return new_node;
}

/*
 * display_metrics - Prints out the metrics
 *
 * Arguments:
 *   outfile - File pointer to print output to (includes stdout, stderr)
 *   prefix  - Text before the metrics
 *   metrics - The metrics linked list
 *   postfix - Text after the metrics
 *
 * Prints out the prefix, then the metrics, then the postfix.
 */
void display_metrics(FILE *outfile, char *prefix, metric_struct *metrics, char *postfix) {
  // Prefix
  if (outfile == NULL) printf("%s", prefix);
  else fprintf(outfile, "%s", prefix);
  // Metrics
  for (metric_struct *current = metrics; current != NULL; current = current->next) {
    if (outfile == NULL) {
      printf("%s=%0.2f", current->name, current->value);
      if (current->next != NULL) printf(",");
    } else {
      fprintf(outfile, "%s=%0.2f", current->name, current->value);
      if (current->next != NULL) fprintf(outfile, ",");
    }
  }
  // Postfix
  if (outfile == NULL) printf("%s", postfix);
  else fprintf(outfile, "%s", postfix);
}

/*
 * combine_metrics - Takes two metrics and returns a combined metric linked list
 *
 * Arguments:
 *   first  - The first metric linked list
 *   second - The second metric linked list
 *
 * Walks through the global list of metrics and conbines each metric.  Returns a
 * new linked list with the combined metrics and leaves the original metrics
 * linked lists alone.  Returns NULL if any of the linked lists contain value
 * errors.
 */
metric_struct *combine_metrics(metric_struct *first, metric_struct *second) {
  metric_struct *new_metrics = NULL;
  // for loop through global metrics list
  for (global_metric_struct *gms = global_metrics; gms != NULL; gms = gms->next) {
    double value1, value2, new_value;
    value1 = get_metric_value(first, gms->name);
    value2 = get_metric_value(second, gms->name);

    // Combine the values
    if (gms->combo == COMBO_ADD) {
      new_value = value1 + value2;
      if (gms->arg) new_value += gms->arg; // Optional additional value
    }
    if (gms->combo == COMBO_MIN) {
      new_value = (value1 < value2) ? (value1) : (value2); // Set to the minimum of the values
    }
    if (gms->combo == COMBO_MAX) {
      new_value = (value1 > value2) ? (value1) : (value2); // Set to the maximum of the values
    }

    metric_struct *temp = create_metrics_node(gms->name, new_value);
    if (new_metrics == NULL) {
      new_metrics = temp;
    } else {
      temp->next = new_metrics;
      new_metrics = temp;
    }
  }

  return new_metrics;
}

/*
 * copy_metrics - Make a copy of a metrics linked list
 *
 * Arguments:
 *   metrics - Metrics linked list to copy
 *
 * Copys the linked list and returns the new copy of the list.
 */
metric_struct *copy_metrics(metric_struct *metrics) {
  // Create a new metrics pointer
  metric_struct *new_metrics = NULL;
  // Iterate through the metrics linked list and copy each node
  for (metric_struct *current = metrics; current != NULL; current = current->next) {
    if (new_metrics == NULL) new_metrics = create_metrics_node(current->name, current->value);
    else new_metrics = add_metrics_node(new_metrics, current->name, current->value);
  }
  // Return the metrics copy
  return new_metrics;
}

/*
 * delete_metrics - Delete a metrics linked list
 *
 * Arguments:
 *   metrics - Metrics linked list
 *
 * Deletes metrics nodes in a metrics linked list
 */
void delete_metrics(metric_struct *metrics) {
  metric_struct *temp;
  while(metrics != NULL) {
    temp = metrics;
    metrics = metrics->next;
    free(temp);
  }
}


/* 
 * get_metric_value - Returns a requested metric value
 *
 * Arguments:
 *   metric - Linked list to the metrics
 *   name   - String name of the metric requested
 *
 * Walks through the list of metrics and returns the metric value for the
 * metric with the matching name.  If the metric is not found, it returns
 * the value ERROR (-1).
 */
double get_metric_value(metric_struct *metric, char *name) {
  for(metric_struct *current = metric; current != NULL; current = current->next) {
    if (strncmp(current->name, name, NAME_MAX_LENGTH) == 0) {
      return current->value;
    }
  }
  return ERROR;
}

