#pragma once
#pragma fox module dir
#include <dirent.h>
#include <sys/stat.h>
#include <glob.h>
#include <core.h>
int is_file(char* filename); //dir.fox
int file_size(char* filename); //dir.fox
char* file_dir(char* file); //dir.fox
char* file_rename(char* file, char* dir, char* delext, char* addext, char* prefix, char* postfix); //dir.fox
char* sane_dir(char* dir); //dir.fox
char* write_file(char* data, char* filename, int readonly, int verbose); //dir.fox
char* file_read(char* filename, int istext, int error_on_fail); //dir.fox
char* read_stdin(int size, char* terminator); //dir.fox
map* dir_files(char* path); //dir.fox
size_t file_time(char* file); //dir.fox
char* file_path(char* file); //dir.fox
char* file_nodir(char* filename); //dir.fox
char* file_noext(char* filename); //dir.fox
char* file_ext(char* filename, char* def); //dir.fox
map* dir_glob(char* name); //dir.fox
char* read_line(FILE* fp); //dir.fox
