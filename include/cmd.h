#pragma once
#include <core.h>
#pragma fox module cmd
void* px(void* str, int newline); //cmd.fox
void xexit(int val); //cmd.fox
void* fox_error(char* msg, int dump); //cmd.fox
void* call_php(map* params, char* func); //cmd.fox
