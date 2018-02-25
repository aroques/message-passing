#ifndef HELPERS_H
#define HELPERS_H

char** split_string(char* str, char* delimeter);
char* get_timestamp();
void print_usage();
int parse_cmd_line_args(int argc, char *argv[]);
void set_timer(int duration);

#endif
