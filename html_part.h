#ifndef HTML_PART_H
#define HTML_PART_H
#include "types.h"

//生成报告
int generate_html_report(const char *filename,
    const user_info *user,
    const SubmissionInfo_lists *subs);

//rating对应
const char *rating_to_color(int rating);
const char *rating_to_rank(int rating);

//json写入辅助
void json_write_escaped(FILE *file, const char *str);
void json_write_key_str(FILE *file, const char *key, const char *value);
void json_write_key_int(FILE *file, const char *key, int value);
void json_write_key_bool(FILE *file, const char *key, int value);

#endif
