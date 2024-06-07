#ifndef METRICS_H
#define METRICS_H

/****************************
 * Global Metrics Functions *
 ****************************/
global_metric_struct *create_global_metrics_node(char *name, unsigned int opt, unsigned int combo, double arg);
void add_global_metric(char *name, unsigned int opt, unsigned int combo, double arg);
int exists_global_metric(char *name);
unsigned int global_metric_count();

/*********************
 * Metrics Functions *
 *********************/
metric_struct *create_metrics_node(char *name, double value);
metric_struct *add_metrics_node(metric_struct *current, char *name, double value);
void display_metrics(FILE *outfile, char *prefix, metric_struct *metrics, char *postfix);
metric_struct *combine_metrics(metric_struct *first, metric_struct *second);
metric_struct *copy_metrics(metric_struct *metrics);
double get_metric_value(metric_struct *metric, char *name);
void delete_metrics(metric_struct *metrics);

#endif
