#ifndef TYPES_H
#define TYPES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SECONDS_PER_DAY   (24L * 60 * 60)
#define SECONDS_30_DAYS   (30L * SECONDS_PER_DAY)
#define SECONDS_180_DAYS  (180L * SECONDS_PER_DAY)
#define SECONDS_1_YEAR    (365L * SECONDS_PER_DAY)
struct data_place {
	char *memory;//指向实际数据的指针
	size_t size;//已经得到的数据大小
};
typedef struct {
	int contest_id;//比赛id
	long contest_time;//比赛时间
	char contest_name[128];//比赛名
	int rank;//排名
	int old_rating;//原等级分
	int new_rating;//新等级分
} contest_history;
typedef struct {
	char handle[25];//id
	int rating;//等级分
	char rank[30];//头衔
	int maxRating;//最高等级分
	char avatar_url[256];//头像url
	contest_history *contests;//历史比赛
	int contest_count;//比赛数量
	int contests_180day;//180天比赛数量
	int maxrating_180day;//180天比赛最高分
} user_info;
typedef struct {
	int contest_id;//比赛名
	long creationTimeSeconds;//提交时间
	char verdict[40];//题目结果
	char participantType[30];//类型："PRACTICE"：赛后补题"CONTESTANT":赛中提交
	char problemIndex[10];//题目序号
	int problemRating;//题目难度等级分
} SubmissionInfo;
typedef struct {
	SubmissionInfo *submissions;//结构体数组指针
	int count;
	int top_count;
} SubmissionInfo_lists;

#endif
