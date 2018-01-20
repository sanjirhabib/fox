#include <fox.h>
#pragma once
char* img_resize(char* img, double enlarge, int width, int height); //draw.fox
char* draw_version(); 
void* draw_invoke(map* v, char* name); 
map* draw_reflect(); 
int cmdline(); 
int exec_cmdline(map* args); 
void* user_invoke(map* params, char* name); 
map* user_funcs(); 
