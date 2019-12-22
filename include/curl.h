#pragma once
#pragma fox module curl
#include <core.h>
#pragma fox module curl
#pragma fox cc -lcurl
size_t fox_curl_cat(void* ptr, size_t size, size_t num, void* old); //curl.fox
char* url_fetch(char* url); //curl.fox
