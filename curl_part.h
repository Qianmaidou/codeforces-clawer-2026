#ifndef CURL_PART_H
#define CURL_PART_H
#include "types.h"
//WriteCallback
size_t WriteCallback(void *contents, size_t size, size_t data_num, void *void_box);
//curl_request
int curl_request(const char *url, struct data_place *out_chunk);
#endif
