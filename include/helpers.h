#ifndef HELPERS_H
#define HELPERS_H

struct CmdLnArgs {
    int max_concurrent_slaves;
    char* filename;
    int max_runtime;
};

char** split_string(char* str, char* delimeter);
char* get_timestamp();
void print_usage();
struct CmdLnArgs parse_cmd_line_args(int argc, char* argv[]);
void set_timer(int duration);

#endif
