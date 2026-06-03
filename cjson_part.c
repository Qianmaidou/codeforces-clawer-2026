#include "cjson_part.h"
#include "cJSON.h"
#include <time.h>
int cjosn_info(const char *json_str, user_info *user_single) {
	cJSON *info_root = cJSON_Parse(json_str);
	if (info_root == NULL) {
		fprintf(stderr, "[ERROR] user.info JSON parse failed, API may have returned non-JSON data\n");
		return 0;
	}
	cJSON *info_status = cJSON_GetObjectItemCaseSensitive(info_root, "status");
	if (info_status == NULL) {
		fprintf(stderr, "[ERROR] Codeforces API response missing status field\n");
		cJSON_Delete(info_root);
		return 0;
	}
	if (strcmp(info_status->valuestring, "OK") != 0) {
		cJSON *info_comment = cJSON_GetObjectItemCaseSensitive(info_root, "comment");
		fprintf(stderr, "[ERROR] API returned failure: %s\n",
		        (info_comment && info_comment->valuestring) ? info_comment->valuestring : "unknown error");
		cJSON_Delete(info_root);
		return 0;
	}
//result->
	cJSON *info_result = cJSON_GetObjectItemCaseSensitive(info_root, "result");
	if (info_result == NULL || !cJSON_IsArray(info_result)) {
		cJSON_Delete(info_root);
		return 0;
	}
	cJSON *result_obj = cJSON_GetArrayItem(info_result, 0);
	if (result_obj == NULL) {
		cJSON_Delete(info_root);
		return 0;
	}
//handle
	cJSON *result_obj_handle = cJSON_GetObjectItemCaseSensitive(result_obj, "handle");
	if (cJSON_IsString(result_obj_handle) && result_obj_handle->valuestring != NULL) {
		strncpy(user_single->handle, result_obj_handle->valuestring, sizeof(user_single->handle) - 1);
		user_single->handle[sizeof(user_single->handle) - 1] = '\0';
	}
//rating
	cJSON *result_obj_rating = cJSON_GetObjectItemCaseSensitive(result_obj, "rating");
	if (cJSON_IsNumber(result_obj_rating)) {
		user_single->rating = result_obj_rating->valueint;
	} else {
		user_single->rating = 0;
	}
//rank
	cJSON *result_obj_rank = cJSON_GetObjectItemCaseSensitive(result_obj, "rank");
	if (cJSON_IsString(result_obj_rank) && result_obj_rank->valuestring != NULL) {
		strncpy(user_single->rank, result_obj_rank->valuestring, sizeof(user_single->rank) - 1);
		user_single->rank[sizeof(user_single->rank) - 1] = '\0';
	} else {
		strcpy(user_single->rank, "Unrated");
	}
//avatar_url
	cJSON *result_obj_avatar_url = cJSON_GetObjectItemCaseSensitive(result_obj, "avatar");
	if (result_obj_avatar_url && cJSON_IsString(result_obj_avatar_url)) {
		strncpy(user_single->avatar_url, result_obj_avatar_url->valuestring, sizeof(user_single->avatar_url) - 1);
		user_single->avatar_url[sizeof(user_single->avatar_url) - 1] = '\0';
	} else {
		user_single->avatar_url[0] = '\0';
	}//maxRating
	cJSON *result_obj_maxRating = cJSON_GetObjectItemCaseSensitive(result_obj, "maxRating");
	if (cJSON_IsNumber(result_obj_maxRating)) {
		user_single->maxRating = result_obj_maxRating->valueint;
	} else {
		user_single->maxRating = 0;
	}
	printf("user id: %s | rating: %d | rank: %s | maxRating: %d\n",
	       user_single->handle, user_single->rating, user_single->rank, user_single->maxRating);
	cJSON_Delete(info_root);
	return 1;
}
int cjson_rating(const char *json_str, user_info *user_single) {
//status
	cJSON *rating = cJSON_Parse(json_str);
	if (rating == NULL) {
		fprintf(stderr, "[ERROR] user.rating JSON parse failed\n");
		return 0;
	}
	cJSON *status = cJSON_GetObjectItemCaseSensitive(rating, "status");
	if (status == NULL || strcmp(status->valuestring, "OK") != 0) {
		cJSON *comment = cJSON_GetObjectItemCaseSensitive(rating, "comment");
		fprintf(stderr, "[ERROR] Rating API failed: %s\n",
		        (comment && comment->valuestring) ? comment->valuestring : "status abnormal");
		cJSON_Delete(rating);
		return 0;
	}

//result->
	cJSON *result_array = cJSON_GetObjectItemCaseSensitive(rating, "result");
	if (result_array == NULL || !cJSON_IsArray(result_array)) {
		cJSON_Delete(rating);
		return 0;
	}

//用户比赛数量
	int count = cJSON_GetArraySize(result_array);
	user_single->contest_count = count;

	if (count == 0) {
		printf("该用户未参加过任何比赛\n");
		user_single->contests = NULL;
		cJSON_Delete(rating);
		return 1;
	}//无比赛

//开内存
	user_single->contests = (contest_history *)malloc(count * sizeof(contest_history));
	if (user_single->contests == NULL) {
		fprintf(stderr, "错误：内存不足！\n");
		cJSON_Delete(rating);
		return 0;
	}
	long current_time = (long)time(NULL);
	int r180_count = 0;
	int r180_max_rating = user_single->rating; //180天时间处理
//每场比赛的数据
	for (int i = 0; i < count; i++) {
		cJSON *item = cJSON_GetArrayItem(result_array, i);
		if (item == NULL) continue;

//整型数据
		cJSON *id   = cJSON_GetObjectItemCaseSensitive(item, "contestId");
		cJSON *rank = cJSON_GetObjectItemCaseSensitive(item, "rank");
		cJSON *old  = cJSON_GetObjectItemCaseSensitive(item, "oldRating");
		cJSON *new  = cJSON_GetObjectItemCaseSensitive(item, "newRating");
		cJSON *ratingUpdateTimeSeconds = cJSON_GetObjectItemCaseSensitive(item, "ratingUpdateTimeSeconds");
//字符串>比赛名
		cJSON *c_name = cJSON_GetObjectItemCaseSensitive(item, "contestName");
//赋值
		if (cJSON_IsNumber(id)) {
			user_single->contests[i].contest_id = id->valueint;
		};
		if (cJSON_IsNumber(rank)) {
			user_single->contests[i].rank = rank->valueint;
		};
		if (cJSON_IsNumber(old)) {
			user_single->contests[i].old_rating = old->valueint;
		};
		if (cJSON_IsNumber(new)) {
			user_single->contests[i].new_rating = new->valueint;
		};
		if (cJSON_IsNumber(ratingUpdateTimeSeconds)) {
			user_single->contests[i].contest_time = ratingUpdateTimeSeconds->valueint;
		};

		if (cJSON_IsString(c_name) && c_name->valuestring != NULL) {
			int len = sizeof(user_single->contests[i].contest_name);
			strncpy(user_single->contests[i].contest_name, c_name->valuestring, len - 1);
			user_single->contests[i].contest_name[len - 1] = '\0';
		}

		long time_gap = current_time - user_single->contests[i].contest_time;
		if (time_gap <= SECONDS_180_DAYS) {
			r180_count++;//180天比赛次数
			// 最近180天的最高等级分
			if (user_single->contests[i].new_rating > r180_max_rating) {
				r180_max_rating = user_single->contests[i].new_rating;
			}
		}
	}
	user_single->contests_180day = r180_count;
	user_single->maxrating_180day = r180_max_rating;
	cJSON_Delete(rating);
	return 1;
}
int cjosn_status(const char *json_str, SubmissionInfo_lists *sub_list) {
	cJSON *status_root = cJSON_Parse(json_str);
	if (status_root == NULL) {
		fprintf(stderr, "[ERROR] user.status JSON parse failed\n");
		return 0;
	}
	cJSON *status_status = cJSON_GetObjectItemCaseSensitive(status_root, "status");
	if (status_status == NULL) {
		fprintf(stderr, "[ERROR] Status API response missing status field\n");
		cJSON_Delete(status_root);
		return 0;
	}
	if (strcmp(status_status->valuestring, "OK") != 0) {
		cJSON *comment = cJSON_GetObjectItemCaseSensitive(status_root, "comment");
		fprintf(stderr, "[ERROR] Status API failed: %s\n",
		        (comment && comment->valuestring) ? comment->valuestring : "unknown error");
		cJSON_Delete(status_root);
		return 0;
	}
//result->
	cJSON *status_result = cJSON_GetObjectItemCaseSensitive(status_root, "result");
	if (status_result == NULL || !cJSON_IsArray(status_result)) {
		cJSON_Delete(status_root);
		return 0;
	}
	int array_num = cJSON_GetArraySize(status_result);
	if (array_num == 0) {
		sub_list->submissions = NULL;
		sub_list->count = 0;
		sub_list->top_count = 0;
		cJSON_Delete(status_root);
		return 1;
	}
	sub_list->submissions = (SubmissionInfo *)malloc(sizeof(SubmissionInfo) * array_num);
	if (sub_list->submissions == NULL) {
		fprintf(stderr, "[ERROR] Memory allocation failed\n");
		cJSON_Delete(status_root);
		return 0;
	}
	sub_list->top_count = array_num;
	sub_list->count = 0;
	for (int i = 0; i < array_num; i++) {
		cJSON *item = cJSON_GetArrayItem(status_result, i);
		cJSON *verdict = cJSON_GetObjectItemCaseSensitive(item, "verdict");
		cJSON *author = cJSON_GetObjectItemCaseSensitive(item, "author");
		cJSON *participantType = author ? cJSON_GetObjectItemCaseSensitive(author, "participantType") : NULL;
		const char *verdict_str = (verdict && cJSON_IsString(verdict)) ? verdict->valuestring : "UNKNOWN";
		const char *participantType_str = (participantType && cJSON_IsString(participantType)) ? participantType->valuestring : "UNKNOWN";
		SubmissionInfo *target = &sub_list->submissions[sub_list->count];
		int len_pt_ar = sizeof(target->participantType);
		strncpy(target->participantType, participantType_str, len_pt_ar - 1);
		target->participantType[len_pt_ar - 1] = '\0';
		int len_ve_ar = sizeof(target->verdict);
		strncpy(target->verdict, verdict_str, len_ve_ar - 1);
		target->verdict[len_ve_ar - 1] = '\0';
		cJSON *creationTimeSeconds = cJSON_GetObjectItemCaseSensitive(item, "creationTimeSeconds");
		target->creationTimeSeconds = (creationTimeSeconds && cJSON_IsNumber(creationTimeSeconds)) ? creationTimeSeconds->valueint : 0;
		cJSON *contest_id = cJSON_GetObjectItemCaseSensitive(item, "contestId");
		target->contest_id = (contest_id && cJSON_IsNumber(contest_id)) ? contest_id->valueint : 0;
		cJSON *problem = cJSON_GetObjectItemCaseSensitive(item, "problem");
		cJSON *index = cJSON_GetObjectItemCaseSensitive(problem, "index");
		int len_index_ar = sizeof(target->problemIndex);
		if (index && cJSON_IsString(index)) {
			strncpy(target->problemIndex, index->valuestring, len_index_ar - 1);
			target->problemIndex[len_index_ar - 1] = '\0';
		} else {
			target->problemIndex[0] = '\0';
		}
		cJSON *rating = cJSON_GetObjectItemCaseSensitive(problem, "rating");
		if (rating && cJSON_IsNumber(rating)) {
			target->problemRating = rating->valueint;
		} else {
			target->problemRating = -1;
		}
		sub_list->count++;
	}
	cJSON_Delete(status_root);
	return 1;
}
