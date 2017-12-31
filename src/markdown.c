#include <mkdio.h>
#include <fox.h>
//char* md_url(char* in,int len,void* junk){
//	url=in.sub_str(0,len)	
//	if !url.url_host() || url.url_host()===:localhost || url.url_host().str_has(:habibur) => return url
//	ext3=url.sub_str(-4)
//	if !ext3.is_word(".jpg jpeg .png .gif") => return url
//	id=url.md5()
//	file="imgs/$id.jpg"
//	if file.is_file() => return file.home_url()
//	data=url.fox_curl()
//	if data => data.write_file(file,0,0); return file.home_url()
//	else return "BAD--"..url
//}
char* file_markdown(char* infile,char* outfile){ return write_file(fox_markdown(file_read(infile,1,1)),outfile,0,1); };
char* fox_markdown(char* in){
	FILE* out;
	char* outbuff;
	size_t outsize=0;
	out=open_memstream(&outbuff,&outsize);
	void* md=mkd_string(in,str_len(in),0);
//	mkd_e_url(md,(mkd_callback_t)md_url)
	markdown(md,out,0);
	fclose(out);
	char* ret=str_dup(outbuff);
	free(outbuff);
	return ret;
};
