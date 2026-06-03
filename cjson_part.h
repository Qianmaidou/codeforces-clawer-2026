#ifndef CJSON_PART_H
#define CJSON_PART_H

#include "types.h"
//info
int cjosn_info(const char *json_str, user_info *user_single);
//rating
int cjson_rating(const char *json_str, user_info *user_single);
//status
int cjosn_status(const char *json_str, SubmissionInfo_lists *sub_list);

#endif
