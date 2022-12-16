#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#define oops(msg) {perror(msg); exit(1);}
/*
아래 코드 저작권 : 경북대학교 컴퓨터학부 21학번 송주현(2021115593)
내용 : C언어용 OOP 기반 String 라이브러리
권한 : 이 코드는 누구나 자유롭게 수정, 사용 가능 합니다. 
*/


unsigned int get_char_count(const char* source);
void copy_str_deep(const char* source, char* destination, unsigned int length);
char* build_str(const char* initial_string);
char* append(char* in_string, const char* s);

char* read_str_from_path(const char* file_path);
void read_limited_str_from_path(const char* file_path, char out_str[], unsigned int limit);

enum ContentType
{
    CONTENT_HTML, CONTENT_JSON, CONTENT_JS, CONTENT_CSS
};
void build_meminfo_json(char out_json[]);
void build_cpuusage_json(char out_json[]);
void build_netusage_json(char out_json[]);
char* build_http_response(const char* content, unsigned int return_type);
void append_static_http_response(char out_response[], char content[], unsigned int return_type);