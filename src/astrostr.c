#include <fox.h>

void hijri_month_cache_bd(){
	for(int i=0;i<2000*12;i++) {printf("%09.1f\n",hijri_month_start_bd2(i));};
};
void hijri_month_cache(){
	for(int i=0;i<2000*12;i++) {printf("%09.1f\n",hijri_month_start2(i));};
};

double dhaka_lng(){return 90+25.0/60.0;}; //90d 25m
double dhaka_lat(){return 23+48.0/60.0;}; //23d 48m
double kaba_lng(){return 39.8262013;};
double kaba_lat(){return 21.4224945;};
double qibla_direction(double lng, double lat){
	return bearing(lng, lat, kaba_lng(), kaba_lat());
};
double kaba_sun_set(double jd){ return sun_set(jd,kaba_lng(),kaba_lat());};
double kaba_moon_set(double jd){ return moon_set(jd,kaba_lng(),kaba_lat());};
double hijri_month_length(int lun){
	return hijri_month_start(lun+1)-hijri_month_start(lun);
};
double hijri_month_length_bd(int lun){ return hijri_month_start_bd(lun+1)-hijri_month_start_bd(lun); };
double hijri_month_start3(int lun){
	char* name="/data/hijri.cache";
	if(!is_file(name)) {return 0;};
	FILE* fp=fopen(name,"r");
	if(!fp) {return 0;};
	fseek(fp,lun*10,SEEK_SET);
	char* ret=read_line(fp);
	fclose(fp);
	return to_double(ret);
};
double hijri_month_start_bd3(int lun){
	char* name="/data/bd.hijri.cache";
	if(!is_file(name)) {return 0;};
	FILE* fp=fopen(name,"r");
	if(!fp) {return 0;};
	fseek(fp,lun*10,SEEK_SET);
	char* ret=read_line(fp);
	fclose(fp);
	return to_double(ret);
};
double hijri_month_start_bd(int lun){
	double cache=hijri_month_start_bd3(lun);
	if(cache) {return cache;};
	return hijri_month_start_bd2(lun);
};
double hijri_month_start(int lun){
	double cache=hijri_month_start3(lun);
	if(cache) {return cache;};
	return hijri_month_start2(lun);
};
double hijri_month_start_bd2(int lun){
	double jd=lunation_date(lun); // lng and lat is for patuakhali, the most visible place in bd.
	double lng=90.381;
	double lat=22.007;
	jd=yallop_best_time(jd,lng,lat);
	while(yallop_q(jd,lng,lat)<.2) {jd=yallop_best_time(jd+1,lng,lat);};
	double ret=jday(jd)+1;
	return ret;
};
double kaba_tz(){ return 3; };
double hijri_month_start75(int lun){
	double jd=lunation_date(lun);
	if(lun>1370*12){
		double jd2=kaba_sun_set(jd);
		double diff=(jd2-jd)*24;
		if(diff<7.5) {return jday(jd2)+2;};
		return jday(jd2)+1;
	};
	return 0;
};
double hijri_month_start2(int lun){
	double jd=lunation_date(lun);
	if(lun>1370*12){
		double jd2=kaba_sun_set(jd);
		double diff=(jd2-jd)*24;
		if(diff<9) {return jday(jd2)+2;};
		return jday(jd2)+1;
	};
	int required_q=1;
//	if(lun<=1370*12) required_q=3
	jd=yallop_best_time(jd,kaba_lng(),kaba_lat());
	while(translate_yallop_q(yallop_q(jd,kaba_lng(),kaba_lat()))<required_q) {jd=yallop_best_time(jd+1,kaba_lng(),kaba_lat());};
	double ret=jday(jd)+1;
	return ret;
};
double hijri_lunation_bd(double jd){
	jd=jday(jd);
	double lun=lunation(jd);
	if(jd<hijri_month_start_bd(lun)) {return lun-1;};
	if(jd>hijri_month_start_bd(lunation(lun+1))) {return lun+1;};
	return lun;
};
char* hijri_date_bd(double jd){
	if(!jd) {jd=jd_now();};
	jd+=6/24;
	return mstr("%04d-%02d-%02d",hijri_year(hijri_lunation_bd(jd)),hijri_month(hijri_lunation_bd(jd)),hijri_day_bd(jd), End);
};
double hijri_lunation(double jd){
	jd=jday(jd);
	double lun=lunation(jd);
	if(jd<hijri_month_start(lun)) {return lun-1;};
	if(jd>hijri_month_start(lunation(lun+1))) {return lun+1;};
	return lun;
};
double current_hijri_month_bd(){
	return hijri_month(hijri_lunation_bd(jd_now()));
};
double current_hijri_month(){
	double jd=jd_now();
	double lun=hijri_lunation(jd);
	return hijri_month(lun);
};
double current_hijri_year(){
	double jd=jd_now();
	double lun=hijri_lunation(jd);
	return hijri_year(lun);
};
double jd_now(){ return jd(NULL,0); };
char* hijri_now(double tz){return hijri_date(jd_now(),tz);};
char* hijri_now_bd(double tz){return hijri_date_bd(jd_now());};
double hijri_day_bd(double jd){
	double lun=hijri_lunation_bd(jd);
	double jd0=hijri_month_start_bd(lun);
	double day=jday(jd)-jd0+1;
	return day;
};
double hijri_day(double jd){
	double lun=hijri_lunation(jd);
	double jd0=hijri_month_start(lun);
	double day=jday(jd)-jd0+1;
	return day;
};
double hijri2jd(int y, int m, int d){
	return hijri_month_start(y*12+m)+d-1;
};
char* hijri_date(double jd,double tz){
	jd+=tz/24;
	return mstr("%04d-%02d-%02d",hijri_year(hijri_lunation(jd)),hijri_month(hijri_lunation(jd)),hijri_day(jd), End);
};
double jd(char* date,double tz){
	if(!date){ return time(0)/86400.-10957.5; };
	map* parts=str_split(date," ",3);
	int secs=0;
	if(map_id(parts,1)){
		map* part2=str_split(map_id(parts,1),":",0);
		secs=to_int(map_id(part2,0))*3600+to_int(map_id(part2,1))*60+to_int(map_id(part2,2)); };
	map* part3=str_split(map_id(parts,0),"-",0);
	return jd3(to_int(map_id(part3,0)),to_int(map_id(part3,1)),to_int(map_id(part3,2))+secs/86400.-tz/24);
};
char* jd_str(double jd, double tz){
	if(jd <= -1000000){ return "0000-00-00 00:00:00"; };
	double z = (jd + 2451545 + tz / 24) + 0.5 + 0.5 / 86400;
	double f = z - (int)(z);
	z = (int)(z);
	double a = z;
	if(z>=2299161){
		int alpha = (int)((z - 1867216.25) / 36524.25);
		a = z + 1 + alpha - (int)(alpha / 4); };
	double b = a + 1524;
	int c = (int)((b - 122.1) / 365.25);
	int d = (int)(365.25 * c);
	int e = (int)((b - d) / 30.6001);
	int dm = b - d - (int)(30.6001 * e);

	int m = e - 1;
	if(e>=14){ m = e - 13; };

	int y = c - 4715;
	if(m > 2){ y = c - 4716; };
	int h=(int)(f*24);
	int min=(int)(frac(f*24)*60);
	double s=(frac(f*24*60)*60);
	char* tzstr=NULL;
	if(tz){
		tzstr=mstr(" %c%02d:%02d", tz<0 ? '-' : '+', (int)(tz), (int)(frac(tz)*60), End); };
	char ret[32];
	return mstr("%04d-%02d-%02d %02d:%02d:%02d%s",y,m,dm,h,min,(int)s,tzstr, End);
};
double frac(double val){ return val>0 ? val-(int)val : -(val-(int)(val)); };
char* jdstring_ar(double jd, double tz){
	map* ret=str_split(map_id(str_split(jd_str(jd,tz)," ",0),0),"-",0);
	return xstr(to_str(int_var(to_int(map_id(ret,0))),"",0),"-",month_ar(to_int(map_id(ret,1))),map_id(ret,2), End);
};
char* jdstring_bg(double jd, double tz){
	map* ret=str_split(map_id(str_split(jd_str(jd,tz)," ",0),0),"-",0);
	return xstr(to_str(int_var(to_int(map_id(ret,0))),"",0),"-",month_bg(to_int(map_id(ret,1))),map_id(ret,2), End);
};
char* date_only(char* in){ return sub_str(in,0,10); };
char* jd_human(double jd, double tz){
	map* ret=str_split(map_id(str_split(jd_str(jd,tz)," ",0),0),"-",0);
	return xstr(to_str(int_var(to_int(map_id(ret,0))),"",0),"-",month_en(to_int(map_id(ret,1))),map_id(ret,2), End);
};
char* wday_en(int val){
	map* ret=xvec("Saturday","Sunday","Monday","Tuesday","Wednesday","Thursday","Friday", End);
	return map_id(ret,val%7);
};
char* wday_en3(int val){
	map* ret=xvec("Sat","Sun","Mon","Tue","Wed","Thu","Fri", End);
	return map_id(ret,val%7);
};
char* month_bg3(int val){
	map* ret=xvec("জানু","ফেব্রু:","মার্চ","এপ্রিল","মে","জুন","জুলাই","অগাষ্ট","সেপ্টে","অক্টো","নভে","ডিসে", End);
	if(!val) {return NULL;};
	return map_id(ret,val-1);
};
char* month_bg(int val){
	map* ret=xvec("জানুয়ারি","ফেব্রুয়ারি","মার্চ","এপ্রিল","মে","জুন","জুলাই","অগাষ্ট","সেপ্টেম্বর","অক্টোবর","নভেম্বর","ডিসেম্বর", End);
	if(!val) {return NULL;};
	return map_id(ret,val-1);
};
char* ar_wday(int wday){
	map* ret=xvec("الأحد","الإثنين","الثلاثاء","الأربعاء","wday_ar","الجمعة","السبت", End);
	if(!wday) {wday=7;};
	return map_id(ret,wday-1);
};
char* wday_bg(int wday){
	map* ret=xvec("রবি","সোম","মঙ্গল","বুধ","বৃহস্পতি","শুক্র","শনি", End);
	if(!wday) {wday=7;};
	return map_id(ret,wday-1);
};
char* month_ar(int month){
	map* ret=xvec("جانفي","فيفري","مارس","أفريل","ماي","جوان","جويلية","أوت","سبتمبر","أكتوبر","نوفمبر","ديسمبر", End);
	return map_id(ret,month-1);
};
char* month_en(int val){
	map* ret=xvec("January","February","March","April","May","June","July","August","September","October","November","December", End);
	if(!val) {return NULL;};
	return map_id(ret,val-1);
};
char* month_en3(int val){
	map* ret=xvec("Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec", End);
	if(!val) {return NULL;};
	return map_id(ret,val-1);
};
char* astro_nth(int n){
	if(n==11||n==12||n==13) {return "th";};
	if(n%10==1) {return "st";};
	if(n%10==2) {return "nd";};
	if(n%10==3) {return "rd";};
	return "th";
};
int cemonth(int ym){ return ym-ceyear(ym)*12; };
int ceyear(int ym){ return (ym-2)/12; };
char* hijri_month_name_bg(int v){
	map* months=xvec(
		"মুহাররম",
		"সফর",
		"রবিউল আউয়াল",
		"রবিউস সানি",
		"জামাদিউল আউয়াল",
		"জামাদিউস সানি",
		"রজব",
		"শাবান",
		"রমজান",
		"শাওয়াল",
		"জিলকদ",
		"জিলহজ্জ"
	, End);
	if(!v) {return NULL;};
	return map_id(months,v-1);
};
char* hijri_month_name_ar(int v){
	map* months=xvec(
		"محرم",
		"صفر",
		"ربيع الأول",
		"ربيع الثاني",
		"جمادى الأول",
		"جمادى الثاني",
		"رجب",
		"شعبان",
		"رمضان",
		"شوال",
		"ذو القعدة ",
		"ذو الحجة"
	, End);
	if(!v) {return NULL;};
	return map_id(months,v-1);
};
char* hijri_month_name(int v){
	map* months=xvec(
		"muharram",
		"safar",
		"rabi al-awwal",
		"rabi al-thani",
		"jumada al-awwal",
		"jumada al-thani",
		"rajab",
		"shaban",
		"ramadan",
		"shawwal",
		"dhu al-qidah",
		"dhu al-hijjah"
	, End);
	if(!v) {return NULL;};
	return map_id(months,v-1);
};
int jd_wday(double jd, double tz){
	return (int)(jd + 2451545.5 + tz / 24 + 0.5 / 86400 + 2) % 7;
};
double tz(){
	char buffer[20];
	time_t timer=time(0);
	strftime(buffer,20, "%z",localtime(&timer));
	int h;
	int m;
	char sign;
	sscanf(buffer,"%c%02d%02d",&sign,&h,&m);
	return h+m/60.*(sign=='+' ? -1 : 1);
};
void rgb2hsl(int r, int g, int b, double* out_h, double* out_s, double*out_l){
	int var_r = r / 255.;
	int var_g = g / 255.;
	int var_b = b / 255.;
	double var_min = min(min(var_r, var_g), var_b);
	double var_max = max(max(var_r, var_g), var_b);
	double del_max = var_max - var_min;
	double l = (var_max + var_min) / 2;
	double h = 0;
	double s = 0;
	if(del_max){
		s = l < 0.5 ? del_max / (var_max + var_min) : del_max / (2 - var_max - var_min);
		double del_r = (((var_max - var_r) / 6) + (del_max / 2)) / del_max;
		double del_g = (((var_max - var_g) / 6) + (del_max / 2)) / del_max;
		double del_b = (((var_max - var_b) / 6) + (del_max / 2)) / del_max;
		if(var_r == var_max){ h = del_b - del_g; }
		else if(var_g == var_max){ h = (1 / 3) + del_r - del_b; }
		else if(var_b == var_max){ h = (2 / 3) + del_g - del_r; };
		if(h < 0){ h = h + 1; };
		if(h > 1){ h = h - 1; }; };
	*out_h=h;
	*out_s=s;
	*out_l=l;
};
double jday(double jd){ return floor(jd - 0.5) + 0.5; };
char* bearing_name3(double lng1, double lat1, double lng2, double lat2){
	char* id=bearing_name(lng1, lat1, lng2, lat2);
	map* names=xmap( "N", "North", "NE", "North East", "E", "East", "SE", "South East", "S", "South", "SW", "South West", "W", "West", "NW", "North West", End);
	return map_val(names,id);
};
char* bearing_name(double lng1, double lat1, double lng2, double lat2){
	double ret=bearing(lng1, lat1, lng2, lat2);
	int direct=(int)((ret+22.5)/45);
	char* names[]={"N","NE","E","SE","S","SW","W","NW","N"};
	return names[direct];
};
static double hue_2_rgb(double v1, double v2, double v_h){
	if(v_h){ v_h = v_h < 0 ? v_h+1 : v_h-1; };
	if(6 * v_h < 1){ return v1 + (v2 - v1) * 6 * v_h; };
	if(2 * v_h < 1){ return v2; };
	if(3 * v_h < 2){ return v1 + (v2 - v1) * ((2 / 3) - v_h) * 6; };
	return v1;
};
void hsl2rgb(double h, double s, double l,int* ret_r,int* ret_g, int* ret_b){
	double r = l * 255;
	double g = l * 255;
	double b = l * 255;
	if(s){
		double var_2 =  l < 0.5 ? var_2 = l * (1 + s) : (l + s) - (s * l);
		double var_1 = 2 * l - var_2;
		r = 255 * hue_2_rgb(var_1, var_2, h + (1 / 3));
		g = 255 * hue_2_rgb(var_1, var_2, h);
		b = 255 * hue_2_rgb(var_1, var_2, h - (1 / 3)); };
	*ret_r = (int)(r);
	*ret_g = (int)(g);
	*ret_b = (int)(b);
};
//main(){
//	in=jd()
//	in.px()
//	in.jd_str(6).px()
//	in.jd_str(6).jd(6).px()
//	2017.jd3(12,7).dx(:jd)
//	"2017-12-07".jd().dx(:strjd)
//	"2017-12-07 00:00:00".jd().dx(:strjdtime)
//	jd_now().salat_time(dhaka_lng(),dhaka_lat()).px()
//	tz().dx(:tz)
//}
map* salat_time(double jd, double lng, double lat){
	return xmap(
		"fajar18",double_var( fajar18(jd,lng,lat)),
		"rise",double_var( sun_rise(jd,lng,lat)),
		"noon",double_var( noon(jd,lng,lat)),
		"asar1",double_var( asar1(jd,lng,lat)),
		"asar2",double_var( asar2(jd,lng,lat)),
		"set",double_var( sun_set(jd,lng,lat)),
		"esha",double_var( esha(jd,lng,lat)
	), End);
};
double esha(double jd, double lng, double lat){
	return sun_rise_set(-18, 1, floor(jd + lng / 360 + 0.5) - lng / 360, lng, lat);
};
double fajar18(double jd, double lng, double lat){
	return sun_rise_set(-18, -1, floor(jd + lng / 360 + 0.5) - lng / 360, lng, lat);
};
double noon(double jd, double lng, double lat){
	return sun_transit(jd, lng, lat);
};
double asar2(double jd, double lng, double lat){
	double noon = sun_transit(jd, lng, lat);
	double salt = sun_alt(noon, lng, lat);
	double asar = 90 - atand(tand(90 - salt) + 2);
	return sun_rise_set(asar, 1, floor(jd + lng / 360 + 0.5) - lng / 360, lng, lat);
};
double asar1(double jd, double lng, double lat){
	double noon = sun_transit(jd, lng, lat);
	double salt = sun_alt(noon, lng, lat);
	double asar = 90 - atand(tand(90 - salt) + 1);
	double ret=sun_rise_set(asar, 1, floor(jd + lng / 360 + 0.5) - lng / 360, lng, lat);
	return ret;
};
/*
double StringDMS($v){
	if(!$v) return ''
	$d=$m=$s=0
	TimeString(Abs($v), $d, $m, $s)
	$s=(int)$s
	$ret=''
	if($d){
		$ret.=$d.'d '.sprintf("%02d",$m) . "' " . sprintf("%02d",$s) . '"'
	}
	else if($m){
		$ret.=$m."' " . sprintf("%02d",$s) . '"'
	}
	else if($s){
		$ret.=$s . '"'
	}
	if($v < 0){
		$ret = "-" . $ret
	}
	return $ret
}
double StringHMS($v){
	$d=$m=$s=0
	$v/=15
	TimeString(Abs($v), $d, $m, $s)
    $StringDMS = sprintf("%d",$d) . ":" . sprintf("%02d",$m) . ":" . sprintf("%02d",(int)($s * 100) / 100)
	if($v < 0){
		$StringDMS = "-" . $StringDMS
	}
	return $StringDMS
}
double lha2($jd,$t,$elng,$elat){
	$obl = Obl4($t)
	$bra = RA($elng, $elat, $obl)
	$bdec = decli($elng, $elat, $obl)
	$sid = Sidereal4($jd)
	$ha = lha($bra, $sid, $lng)
}
double SunHA(double jd, double lng, double lat){
	$t=JulianCenturies($jd)
	$elng = SunLng4($t)
	$elat = 0
	return lha2($jd,$t,$elng,$elat)
}
*/
