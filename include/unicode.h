#pragma once
#include <core.h>
#pragma fox module unicode
char* utf_letter(char* in, char* out); //unicode.fox
int utf_strlen(char* in); //unicode.fox
int unicode_len(int ucs2); //unicode.fox
char* unicode_utf(int ucs2, char* ret); //unicode.fox
int utf_len(char* in); //unicode.fox
int utf_unicode(char* ptr); //unicode.fox
