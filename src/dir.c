#include <fox.h>

char* file_dir(char* file){
	int len=str_len(file);
	int i=0;
	for(i=len-1;i>=0;i--){
		if(file[i]=='/'){ break; }; };
	if(i==-1){ return NULL; };
	return sub_str(file,0,i+1);
};
char* file_rename(char* file,char* dir,char* delext,char* addext,char* prefix,char* postfix){
	char* olddir=file_dir(file);
	char* ret=file;
	if(olddir){ ret=sub_str(ret,str_len(olddir),-2147483648); };
	if(delext && str_end(ret,delext)){ ret=sub_str(ret,0,-str_len(delext)); };
	if(prefix){ ret=xstr(prefix,ret, End); };
	if(postfix){ ret=xcat(ret,postfix, End); };
	if(addext){ ret=xcat(ret,addext, End); };
	if(dir){ olddir=dir; };
	return xstr(sane_dir(olddir),ret, End);
};
char* sane_dir(char* dir){ return dir ? xstr(str_rtrim(dir,"/"),"/", End) : NULL; };
char* write_file(char* data,char* filename,int readonly,int verbose){
	if(!filename){ return data; };
	if(str_eq(filename,"-")){ return px(data,1); };
	if(verbose){ px(xstr("-> ", filename, End),1); };
	if(readonly && is_file(filename)){ chmod(filename,0666); };
	FILE* fp=fopen(filename,"w");
	if(!fp){ fox_error(xstr("writting to file ", filename, " failed", End),0); };
	print(data,fp);
	fclose(fp);
	if(readonly){ chmod(filename,0444); };
	return data;
};
char* file_read(char* filename, int istext, int error_on_fail){
	assert(filename);
	FILE* fp=fopen(filename,"r");
	size_t size=0;
	size_t read=0;
	if(!fp){ return error_on_fail ? fox_error(xstr("Can't read file ", filename, End),1) : NULL; };
	fseek(fp,0,SEEK_END);
	size=ftell(fp);
	char* ret=istext ? new_str(size) : new_blob(size);
	fseek(fp,0,SEEK_SET);
	read=fread(ret,1,size,fp);
	fclose(fp);
	if(read!=size){ return NULL; };
	assert(!ret[str_len(ret)]);
	assert(str_len(ret)==size);
	return ret;
};
char* read_stdin(int size,char* terminator){
	char* ret=NULL;
	char buff[1024+1]={0};
	int block=1024;
	if(size && size<block){
		block=size; };
	int done=0;
	while((done=fread(buff,1,block,stdin))){
		buff[done]='\0';
		ret=xcat(ret,buff, End);
		if(size){
			size-=done;
			if(size<=0){ break; };
			block=min(size,1024); };
	};
	return ret;
};
map* dir_files(char* path){
	if(!path){ return NULL; };
	DIR *d=opendir(path);
	if(!d){ return NULL; };
	map* ret=new_vec();
	struct dirent *dir;
	char* padd=NULL;
	if(!str_eq(path,".")){
		padd=path;
		if(fox_at(padd,-1)!='/'){ padd=xcat(padd,"/", End); };
	};
	while((dir=readdir(d))){
		char* name=dir->d_name;
		if(is_word(name,". ..")){ continue; };
		vec_add(ret,xstr(padd, name, End));
	};
	closedir(d);
	return ret;
};
size_t file_time(char* file){
	struct stat statbuf;
	if(stat(file,&statbuf)==-1){
		perror(file);
		return 0;
	};
	return statbuf.st_mtime;
};
