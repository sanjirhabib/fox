#line 2 "src/curl.fox"

#include <curl/curl.h>
#include <core.h>
#include <curl.h>

#pragma fox cc -lcurl

size_t fox_curl_cat(void* ptr, size_t size, size_t num, void* old){
	char** old1=(char**)old;
	*old1=cat(*old1,ptr,size*num);
	return size*num;
};
char* url_fetch(char* url){
	char* ret=new_blob(0);
	CURL* curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle,CURLOPT_URL, url);
	curl_easy_setopt(curl_handle,CURLOPT_WRITEFUNCTION,fox_curl_cat);
	curl_easy_setopt(curl_handle,CURLOPT_WRITEDATA, (void*)&ret);
	curl_easy_setopt(curl_handle,CURLOPT_USERAGENT, "Mozilla/5.0 (compatible; habibur/1; +http://news.habibur.com/)");
	curl_easy_setopt(curl_handle,CURLOPT_FOLLOWLOCATION,1);
	curl_easy_setopt(curl_handle,CURLOPT_ENCODING,0);
	curl_easy_setopt(curl_handle,CURLOPT_ACCEPT_ENCODING,"");
	int res = curl_easy_perform(curl_handle);
	curl_easy_cleanup(curl_handle);
	return res ? NULL : ret;
};
