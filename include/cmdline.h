#pragma once
#include <core.h>
#pragma fox module cmdline
#include <generator.h>
map* eval_params(map* sent, char* name, map* env, map* fns); //cmdline.fox
map* cmdline_params(map* args, map* funcs); //cmdline.fox
