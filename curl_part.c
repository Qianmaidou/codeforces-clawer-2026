#include "curl_part.h"
#include <curl/curl.h>
//回调函数
size_t WriteCallback(void *contents, size_t size, size_t data_num, void *void_box) {
//contents:指向接收到的数据块的指针
//void_box就是传递的地址
//my_box局部指针，可进行操作>realloc>memcpy
//data_num数据有多少个单位数量
//size:内存的单位大小
	size_t new_data_size = size * data_num;//实际数据量
	struct data_place*my_box = (struct data_place *)void_box;
	//申请新的内存空间(变长)
	char *ptr = realloc(my_box->memory, my_box->size + new_data_size + 1);
	//realloc重新分配内存的大小
	if (!ptr) return 0; //内存是否足够
	my_box->memory = ptr;
	//将数据放在旧数据的后面
	memcpy(&(my_box->memory[my_box->size]), contents, new_data_size);
	my_box->size += new_data_size; //记录数据长度
	my_box->memory[my_box->size] = 0; //'\0'
	return new_data_size;//返回成功接收的字节数
}

//使用curl 来抓取数据
int curl_request(const char *url, struct data_place *out_chunk) {
	if (url == NULL || out_chunk == NULL) {
		return 0;
	}
//初始化
	CURL *curl_handle = curl_easy_init();
	if (curl_handle == NULL) {
		fprintf(stderr, "[ERROR] curl_easy_init()失败\n");
		return 0;
	}
//option
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)out_chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36");
	CURLcode res = curl_easy_perform(curl_handle);
//cleanup
	curl_easy_cleanup(curl_handle);
//check
	if (res != CURLE_OK) {
		fprintf(stderr, "[ERROR]: %s\n", curl_easy_strerror(res));
		return 0;
	}
	return 1;
}
