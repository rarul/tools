#ifndef _SSID3_H_
#define _SSID3_H_

#define LOG(fmt, ...)                                                   \
    fprintf(stdout, "[ssid3:%s] [%s:%d] : " fmt "\n", __FUNCTION__, __FILE__, __LINE__, \
            ##__VA_ARGS__)
#define WARN(fmt, ...)                                                  \
    fprintf(stdout, "[ssid3:%s] [%s:%d] : " fmt "\n", __FUNCTION__, __FILE__, __LINE__, \
            ##__VA_ARGS__)

typedef struct {
    size_t offset;
    size_t size;
    const char* frame_name;
    const char* frame_body;
    const char* filename;
} print_context_t;

#endif /* _SSID3_H_ */

