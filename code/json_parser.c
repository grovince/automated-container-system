#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../cJSON library/cJSON.h"
#include "json_parser.h"

int parse_json_config(const char* filename, ContainerConfig** configs) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* buffer = malloc(file_size + 1);
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    fclose(file);

    // json 데이터 파싱 후 변수에 저장
    cJSON* json = cJSON_Parse(buffer);
    free(buffer);

    if (!json) {
        printf("Failed to parse JSON\n");
        return -1;
    }

    cJSON* containers = cJSON_GetObjectItem(json, "containers");
    if (containers) {
        int count = cJSON_GetArraySize(containers);
        *configs = malloc(sizeof(ContainerConfig) * count);

        for (int i = 0; i < count; i++) {
            cJSON* container = cJSON_GetArrayItem(containers, i);
            const char* name = cJSON_GetObjectItem(container, "name")->valuestring;
            const char* image = cJSON_GetObjectItem(container, "image")->valuestring;
            cJSON* ports = cJSON_GetObjectItem(container, "ports");
            cJSON* volumes = cJSON_GetObjectItem(container, "volumes");
            cJSON* environment = cJSON_GetObjectItem(container, "environment");

            strncpy((*configs)[i].name, name, sizeof((*configs)[i].name));
            strncpy((*configs)[i].image, image, sizeof((*configs)[i].image));

            // port 처리
            char port_str[256] = "";
            for (int j = 0; j < cJSON_GetArraySize(ports); j++) {
                cJSON* port = cJSON_GetArrayItem(ports, j);
                char port_str_temp[32];
                snprintf(port_str_temp, sizeof(port_str_temp), "%s ", port->valuestring);
                strcat(port_str, port_str_temp);
            }
            port_str[strlen(port_str) - 1] = '\0';
            strncpy((*configs)[i].ports, port_str, sizeof((*configs)[i].ports));

            // volume 처리
            char volume_str[512] = "";
            for (int j = 0; j < cJSON_GetArraySize(volumes); j++) {
                cJSON* volume = cJSON_GetArrayItem(volumes, j);
                char volume_str_temp[256];
                snprintf(volume_str_temp, sizeof(volume_str_temp), "%s ", volume->valuestring);
                strcat(volume_str, volume_str_temp);
            }
            volume_str[strlen(volume_str) - 1] = '\0';
            strncpy((*configs)[i].volumes, volume_str, sizeof((*configs)[i].volumes));

            // environment 처리
            char env_str[512] = "";
            cJSON* env_item = environment->child;
            while (env_item) {
                char env_str_temp[256];
                snprintf(env_str_temp, sizeof(env_str_temp), "%s=%s ", env_item->string, env_item->valuestring);
                strcat(env_str, env_str_temp);
                env_item = env_item->next;
            }
            env_str[strlen(env_str) - 1] = '\0'; // 마지막 공백 제거
            strncpy((*configs)[i].environment, env_str, sizeof((*configs)[i].environment));
        }
    }

    cJSON_Delete(json);

    return cJSON_GetArraySize(containers);
}
