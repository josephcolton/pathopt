#ifndef PARSER_H
#define PARSER_H

/********************
 * Parser Functions *
 ********************/

void parse_file(char* filename);
void parse_metric(char *line);
void parse_node(char *line);
void parse_edge(char *line);

#endif
