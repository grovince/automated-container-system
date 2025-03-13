#ifndef JSON_PARSER_H
#define JSON_PARSER_H

typedef struct {
    char name[256];
    char image[256];
    char ports[256];
    char volumes[512];
    char environment[512];
} ContainerConfig;

int parse_json_config(const char* filename, ContainerConfig** configs);

#endif
