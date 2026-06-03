#include "html_part.h"
#include <time.h>
//颜色
const char *rating_to_color(int rating) {
	if (rating >= 4000) return "#FF0000";
	if (rating >= 3000) return "#FF0000";
	if (rating >= 2600) return "#FF0000";
	if (rating >= 2400) return "#FF0000";
	if (rating >= 2300) return "#FF8C00";
	if (rating >= 2100) return "#FF8C00";
	if (rating >= 1900) return "#AA00AA";
	if (rating >= 1600) return "#0000FF";
	if (rating >= 1400) return "#03A89E";
	if (rating >= 1200) return "#008000";
	if (rating >  0)    return "#808080";
	return "#000000";
}
//头衔
const char *rating_to_rank(int rating) {
	if (rating >= 4000) return "Tourist";
	if (rating >= 3000) return "Legendary Grandmaster";
	if (rating >= 2600) return "International Grandmaster";
	if (rating >= 2400) return "Grandmaster";
	if (rating >= 2300) return "International Master";
	if (rating >= 2100) return "Master";
	if (rating >= 1900) return "Candidate Master";
	if (rating >= 1600) return "Expert";
	if (rating >= 1400) return "Specialist";
	if (rating >= 1200) return "Pupil";
	if (rating >  0)    return "Newbie";
	return "Unrated";
}
//json写入辅助
void json_write_escaped(FILE *file, const char *str) {
	fputc('"', file);
	for (const char *p = str; *p; p++) {
		switch (*p) {
			case '"':
				fputs("\\\"", file);
				break;
			case '\\':
				fputs("\\\\", file);
				break;
			case '\n':
				fputs("\\n", file);
				break;
			case '\r':
				fputs("\\r", file);
				break;
			case '\t':
				fputs("\\t", file);
				break;
			default:
				fputc(*p, file);
		}
	}
	fputc('"', file);
}

void json_write_key_str(FILE *file, const char *key, const char *value) {
	json_write_escaped(file, key);
	fputc(':', file);
	json_write_escaped(file, value);
}
void json_write_key_int(FILE *file, const char *key, int value)         {
	json_write_escaped(file, key);
	fprintf(file, ":%d", value);
}
void json_write_key_bool(FILE *file, const char *key, int value)         {
	json_write_escaped(file, key);
	fputs(value ? ":true" : ":false", file);
}
//报告生成
int generate_html_report(const char *filename,
                         const user_info *user,
                         const SubmissionInfo_lists *sub_list) {
	if (!filename || !user || !sub_list) return 0;
//多路径查找template模板
	const char *paths[] = {"../reference/template.html", "template/template.html", "reference/template.html"};
	FILE *template_file = NULL;
	for (int i = 0; i < 3; i++) {
		template_file = fopen(paths[i], "rb");
		if (template_file) break;
	}
	if (!template_file) {
		fprintf(stderr, "Error: template not found\n");
		return 0;
	}
	//读取模板文件
	fseek(template_file, 0, SEEK_END);
	long template_size = ftell(template_file);
	fseek(template_file, 0, SEEK_SET);
	char *template_buf = (char *)malloc((size_t)template_size + 1);
	if (!template_buf) {
		fclose(template_file);
		return 0;
	}
	fread(template_buf, 1, (size_t)template_size, template_file);
	template_buf[template_size] = '\0';
	fclose(template_file);

	char *placeholder = strstr(template_buf, "{{DATA}}");
	if (!placeholder) {
		fprintf(stderr, "Error: {{DATA}} not found\n");
		free(template_buf);
		return 0;
	}
//打开输出文件，写入模板前半段
	FILE *output_file = fopen(filename, "w");
	if (!output_file) {
		fprintf(stderr, "Error: Cannot open %s\n", filename);
		free(template_buf);
		return 0;
	}
	fwrite(template_buf, 1, (size_t)(placeholder - template_buf), output_file);
//用户基本信息
	fputs("const USER_DATA={", output_file);
	json_write_key_str(output_file, "handle", user->handle);
	fputc(',', output_file);
	json_write_key_int(output_file, "rating", user->rating);
	fputc(',', output_file);
	json_write_key_int(output_file, "maxRating", user->maxRating);
	fputc(',', output_file);
	json_write_key_str(output_file, "rank", rating_to_rank(user->rating));
	fputc(',', output_file);
	json_write_key_str(output_file, "avatar", user->avatar_url);
	fputc(',', output_file);
	json_write_key_str(output_file, "handleColor", rating_to_color(user->rating));
	fputc(',', output_file);
	json_write_key_bool(output_file, "specialHandle", user->rating >= 3000);
	fputc(',', output_file);
	if (user->rating >= 4000)
		json_write_key_str(output_file, "firstLetterColor", "#FF0000");
	else if (user->rating >= 3000)
		json_write_key_str(output_file, "firstLetterColor", "#000000");
	else
		json_write_key_str(output_file, "firstLetterColor", "");
	fputc(',', output_file);
//参赛总数、最高 Rating、近半年参赛数、近半年最高分
	fputs("\"stats\":{", output_file);
	json_write_key_int(output_file, "contestCount", user->contest_count);
	fputc(',', output_file);
	json_write_key_int(output_file, "maxRating", user->maxRating);
	fputc(',', output_file);
	json_write_key_int(output_file, "recent180Contests", user->contests_180day);
	fputc(',', output_file);
	json_write_key_int(output_file, "recent180MaxRating", user->maxrating_180day);
	fputs("},", output_file);
//将用户的比赛记录序列化为 JSON 数组
	fputs("\"ratingHistory\":[", output_file);
	for (int i = 0; i < user->contest_count; i++) {
		contest_history *c = &user->contests[i];
		if (i > 0) fputc(',', output_file);
		fprintf(output_file, "{");
		json_write_key_str(output_file, "name", c->contest_name);
		fputc(',', output_file);
		json_write_key_int(output_file, "time", (int)c->contest_time);
		fputc(',', output_file);
		json_write_key_int(output_file, "rank", c->rank);
		fputc(',', output_file);
		json_write_key_int(output_file, "oldRating", c->old_rating);
		fputc(',', output_file);
		json_write_key_int(output_file, "newRating", c->new_rating);
		fprintf(output_file, "}");
	}
	fputs("],", output_file);

//O(S)提交记录扫描
	struct contest_prob {
		char idx;
		int rtg;
		int ok_c;
		int ok_a;
	};
	struct contest_prob_data {
		struct contest_prob problems[26];
		int prob_count;
	} *all_problems = NULL;
	if (user->contest_count > 0) {
		all_problems = (struct contest_prob_data *)calloc(user->contest_count, sizeof(struct contest_prob_data));
	}

	for (int i = 0; i < sub_list->count; i++) {
//提交记录与比赛的关联匹配
		SubmissionInfo *sub = &sub_list->submissions[i];
		int cid = sub->contest_id;
		int contest_idx = -1;
		for (int c = 0; c < user->contest_count; c++) {
			if (user->contests[c].contest_id == cid) {
				contest_idx = c;
				break;
			}
		}
		if (contest_idx < 0) continue;
//提取单条提交的关键信息
		struct contest_prob_data *cp = &all_problems[contest_idx];
		char idx = sub->problemIndex[0];
		int is_ok = (strcmp(sub->verdict, "OK") == 0);
		int in_contest = (strcmp(sub->participantType, "PRACTICE") != 0);
//去重合并
		int found = 0;
		for (int j = 0; j < cp->prob_count; j++) {
			if (cp->problems[j].idx == idx) {
				found = 1;
				if (sub->problemRating > 0) {
					cp->problems[j].rtg = sub->problemRating;
				}
				if (is_ok) {
					if (in_contest) cp->problems[j].ok_c = 1;
					else cp->problems[j].ok_a = 1;
				}
				break;
			}
		}
//新增题目
		if (!found && cp->prob_count < 26) {
			struct contest_prob *p = &cp->problems[cp->prob_count];
			p->idx = idx;
			p->rtg = sub->problemRating > 0 ? sub->problemRating : 0;
			p->ok_c = 0;
			p->ok_a = 0;
			if (is_ok) {
				if (in_contest) p->ok_c = 1;
				else p->ok_a = 1;
			}
			cp->prob_count++;
		}
	}

	//字母冒泡排序
	for (int c = 0; c < user->contest_count; c++) {
		struct contest_prob_data *cp = &all_problems[c];
		for (int a = 0; a < cp->prob_count - 1; a++)
			for (int b = a + 1; b < cp->prob_count; b++)
				if (cp->problems[a].idx > cp->problems[b].idx) {
					struct contest_prob tmp = cp->problems[a];
					cp->problems[a] = cp->problems[b];
					cp->problems[b] = tmp;
				}
	}
//比赛详情 + 题目解题状态排序
	fputs("\"contests\":[", output_file);
	for (int i = user->contest_count - 1; i >= 0; i--) {
		contest_history *c = &user->contests[i];
		if (i < user->contest_count - 1) fputc(',', output_file);
		fprintf(output_file, "{");
		json_write_key_str(output_file, "name", c->contest_name);
		fputc(',', output_file);
		json_write_key_int(output_file, "time", (int)c->contest_time);
		fputc(',', output_file);
		json_write_key_int(output_file, "rank", c->rank);
		fputc(',', output_file);
		json_write_key_int(output_file, "oldRating", c->old_rating);
		fputc(',', output_file);
		json_write_key_int(output_file, "newRating", c->new_rating);
		fputc(',', output_file);
//每场比赛的题目列表输出
		fputs("\"problems\":", output_file);
		struct contest_prob_data *cp = &all_problems[i];
		fputc('[', output_file);
		int first = 1;
		for (int j = 0; j < cp->prob_count; j++) {
			struct contest_prob *p = &cp->problems[j];
			if (!p->ok_c && !p->ok_a) continue;
			if (!first) fputc(',', output_file);
			first = 0;
			int ic = p->ok_c;
			int af = (!ic && p->ok_a);
			fprintf(output_file, "{\"index\":\"%c\",\"rating\":%d,\"inContest\":%s,\"afterContest\":%s}",
			        p->idx, p->rtg, ic ? "true" : "false", af ? "true" : "false");
		}
		fputc(']', output_file);

		fprintf(output_file, "}");
	}
	fputs("],", output_file);
	free(all_problems);
//题目难度分布统计
	{
		long current_time = (long)time(NULL);
		long cutoff_year = current_time - 365L * 24 * 3600;
		long cutoff_180days = current_time - 180L * 24 * 3600;
		long cutoff_month = current_time - 30L * 24 * 3600;
		int dist_all[40] = {0}, dist_year[40] = {0}, dist_halfyear[40] = {0}, dist_month[40] = {0};
		int unrated_all = 0, unrated_yr = 0, unrated_hf = 0, unrated_mo = 0;
//按难度分桶+按时间分层
		for (int i = 0; i < sub_list->count; i++) {
			SubmissionInfo *sub = &sub_list->submissions[i];
			if (strcmp(sub->verdict, "OK") != 0) continue;
			int rating = sub->problemRating;
			if (rating == -1) {
				unrated_all++;
				if (sub->creationTimeSeconds >= cutoff_year)  unrated_yr++;
				if (sub->creationTimeSeconds >= cutoff_180days) unrated_hf++;
				if (sub->creationTimeSeconds >= cutoff_month)  unrated_mo++;
				continue;
			}
			if (rating <= 0) continue;
			int bucket = rating / 100;
			if (bucket < 0) bucket = 0;
			if (bucket >= 40) bucket = 39;
			dist_all[bucket]++;
			if (sub->creationTimeSeconds >= cutoff_year)  dist_year[bucket]++;
			if (sub->creationTimeSeconds >= cutoff_180days) dist_halfyear[bucket]++;
			if (sub->creationTimeSeconds >= cutoff_month)  dist_month[bucket]++;
		}
		//难度分布数据序列化为json
		fputs("\"problemRatingDist\":{", output_file);
		fputs("\"unrated\":[", output_file);
		fprintf(output_file, "%d,%d,%d,%d", unrated_all, unrated_yr, unrated_hf, unrated_mo);
		fputs("],", output_file);
		const char *dist_keys[] = {"all", "year", "halfYear", "month"};
		int *dist_arrays[] = {dist_all, dist_year, dist_halfyear, dist_month};
		for (int t = 0; t < 4; t++) {
			if (t > 0) fputc(',', output_file);
			json_write_escaped(output_file, dist_keys[t]);
			fputs(":[", output_file);
			for (int b = 0; b < 40; b++) {
				if (b > 0) fputc(',', output_file);
				fprintf(output_file, "%d", dist_arrays[t][b]);
			}
			fputc(']', output_file);
		}
		fputs("}", output_file);
	}

	fputs("};\n", output_file);
	fwrite(placeholder + 8, 1, (size_t)template_size - (size_t)(placeholder + 8 - template_buf), output_file);
	fclose(output_file);
	free(template_buf);
	printf("报告成功生成：%s[3/3]\n", filename);
	return 1;
}
