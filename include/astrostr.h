#pragma once
#include <core.h>
#include <astro.h>
#include <dir.h>
#include <tsv.h>
#pragma fox module astrostr
#pragma fox cc -lastro
double hijri_month_length(int lun, char* cal); //astrostr.fox
double hijri_month_start(int lun, char* cal); //astrostr.fox
void hijri_month_cache_bd(); //astrostr.fox
void hijri_month_cache_en(); //astrostr.fox
double hijri_month_start_en(int lun); //astrostr.fox
double hijri_month_start_bd(int lun); //astrostr.fox
double hijri_month_start75(int lun); //astrostr.fox
double hijri_lunation(double jd, char* cal); //astrostr.fox
char* hijri_date(double jd, double tz, char* cal); //astrostr.fox
double current_hijri_month(char* cal); //astrostr.fox
double current_hijri_year(char* cal); //astrostr.fox
double dhaka_lng(); //astrostr.fox
double dhaka_lat(); //astrostr.fox
double kaba_lng(); //astrostr.fox
double kaba_lat(); //astrostr.fox
double qibla_direction(double lng, double lat); //astrostr.fox
double kaba_sun_set(double jd); //astrostr.fox
double kaba_moon_set(double jd); //astrostr.fox
double kaba_tz(); //astrostr.fox
double jd_now(); //astrostr.fox
char* hijri_now(double tz, char* cal); //astrostr.fox
double hijri_day(double jd, char* cal); //astrostr.fox
double hijri2jd(int y, int m, int d, char* cal); //astrostr.fox
double jd(char* date, double tz); //astrostr.fox
char* jd_str(double jd, double tz); //astrostr.fox
char* jdstring_ar(double jd, double tz); //astrostr.fox
char* jdstring_bg(double jd, double tz); //astrostr.fox
char* date_only(char* in); //astrostr.fox
char* jd_human(double jd, double tz); //astrostr.fox
char* wday_en(int val); //astrostr.fox
char* wday_en3(int val); //astrostr.fox
char* month_bg3(int val); //astrostr.fox
char* month_bg(int val); //astrostr.fox
char* ar_wday(int wday); //astrostr.fox
char* wday_bg(int wday); //astrostr.fox
char* month_ar(int month); //astrostr.fox
char* month_en(int val); //astrostr.fox
char* astro_nth(int n); //astrostr.fox
int cemonth(int ym); //astrostr.fox
int ceyear(int ym); //astrostr.fox
char* hijri_month_name_bg(int v); //astrostr.fox
char* hijri_month_name_ar(int v); //astrostr.fox
char* hijri_month_name(int v); //astrostr.fox
int jd_wday(double jd, double tz); //astrostr.fox
double tz(); //astrostr.fox
void rgb2hsl(int r, int g, int b, double* out_h, double* out_s, double* out_l); //astrostr.fox
double jday(double jd); //astrostr.fox
char* bearing_name3(double lng1, double lat1, double lng2, double lat2); //astrostr.fox
char* bearing_name(double lng1, double lat1, double lng2, double lat2); //astrostr.fox
double hue_2_rgb(double v1, double v2, double v_h); //astrostr.fox
void hsl2rgb(double h, double s, double l, int* ret_r, int* ret_g, int* ret_b); //astrostr.fox
map* salat_time(double jd, double lng, double lat); //astrostr.fox
double esha(double jd, double lng, double lat); //astrostr.fox
double fajar18(double jd, double lng, double lat); //astrostr.fox
double noon(double jd, double lng, double lat); //astrostr.fox
double asar2(double jd, double lng, double lat); //astrostr.fox
double asar1(double jd, double lng, double lat); //astrostr.fox
