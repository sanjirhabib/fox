#pragma once
#include <core.h>
#pragma fox module foxphp
#include <generator.h>
#include <dir.h>
char* fox_phpc(char* infile, char* outfile); //foxphp.fox
static char* write_phpconfig(); //foxphp.fox
static char* write_configm4(char* name, char* outfile); //foxphp.fox
