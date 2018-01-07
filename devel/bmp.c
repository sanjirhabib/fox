//
//  main.c
//  rabfp
//
//  Created by Habibur Rahman on 11/20/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

int error(char* msg){
    fprintf(stderr,"%s",msg);
    exit(0);
}
void* myalloc(size_t size){
	void* ret=malloc(size);
	memset(ret,0,size);
	return ret;
}
int write_file(char* filename,unsigned char* data,int size){
    FILE* fp=fopen(filename,"wb");
    if(!fp) return error("can't write to output file");
    fwrite(data,1,size,fp);
    fclose(fp);
    return 1;
}
int read_file(char* filename,char** data){
    FILE* fp=fopen(filename,"rb");
    if(!fp) return error("input file not found\n");;
    fseek(fp,0,SEEK_END);
    int size=ftell(fp);
    if(!size) return error("file size is 0");
    *data=myalloc(size);
    fseek(fp,0,SEEK_SET);
    int readsize=fread(*data,1,size,fp);
    if(readsize!=size) error("file partially read");
    fclose(fp);
    return size;
}
int padding(int w,int depth){
	int pad=(w*depth/8)%4;
	if(!pad) return 0;
	return 4-pad;
}
int fix_upsidedown(unsigned char* data,int w,int h,int depth){
	if(h<0) return -h;
	w=w*depth/8+padding(w,depth);
	unsigned char* temp=malloc(w*h);
	for(int i=0;i<h;i++){
		memcpy(temp+i*w,data+(h-i-1)*w,w);
	}
	memcpy(data,temp,h*w);
	free(temp);
	return h;
}
int parse_bmp(unsigned char* filedata,int* w,int* h,int* depth,unsigned char** data,int* xppm,int* yppm){
    if(filedata[0]!='B'||filedata[1]!='M') return 0;
    int off=*(int*)(filedata+10);
    *data=filedata+off;
    *w=*(int*)(filedata+18);
    *h=*(int*)(filedata+22);
    *xppm=*(int*)(filedata+34);
    *yppm=*(int*)(filedata+38);
    *depth=*(unsigned short*)(filedata+28);
	*h=fix_upsidedown(*data,*w,*h,*depth);
    return 1;
}
int remove_padding(unsigned char* in,int w,int h,int depth){
	depth/=8;
    int padding=(w*depth)%4;
    if(padding) padding=4-padding;
    if(!padding) return w*h*depth;
    for(int i=1;i<h;i++) memmove(in+w*depth*i, in+(w*depth+padding)*i, w*depth);
    return w*h*depth;
}
int to_grayscale(unsigned char* in,unsigned char** out,int w,int h,int depth){
    int ret=w*h;
    if(depth==8){*out=in;return ret;}
    if(depth!=24) return 0;
    *out=myalloc(ret);
    for(int i=0;i<ret;i++){
        (*out)[i]=255-(in[i*3]+in[i*3+1]+in[i*3+2])/3;
    }
    return ret;
}
int to_rect(double r,double a,int cx, int cy,int* x,int* y){
    *x=cx+r*cos(a);
    *y=cy-r*sin(a);
    return 1;
}
int to_polar(int x,int y,int cx,int cy,double* r,double* a){
    int dx=x-cx;
    int dy=cy-y;
    *r=sqrt(dx*dx+dy*dy);
    *a=atan2(dy,dx);
    return 1;
}
int rotate_pixel(int x,int y,int cx,int cy,double angle,int* nx,int* ny){
    double r;
    double a;
    to_polar(x,y,cx,cy,&r,&a);
    a-=angle/180*M_PI;
    to_rect(r, a, cx, cy, nx, ny);
    return 1;
}
int rotate_image(unsigned char* data,int w,int h,double angle){
    unsigned char* ret=myalloc(w*h);
    memset(ret,0,w*h);
    int cx=w/2;
    int cy=h/2;
    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            int nx;
            int ny;
            rotate_pixel(x, y, cx, cy, angle, &nx, &ny);
            if(nx<0||nx>=w||ny<0||ny>=h) continue;
            ret[x+y*w]=data[ny*w+nx];
        }
    }
    memcpy(data,ret,w*h);
    free(ret);
    return 1;
}

int compare_color_freq(void* a,void* b){
    int* ptr1=a;
    int* ptr2=b;
    if(ptr1[0]==ptr2[0]) return 0;
    return ptr1[0]>ptr2[0] ? -1 : 1;
}
#define min(x,y) (x<y?x:y)
#define max(x,y) (x>y?x:y)
int bgcolor(unsigned char* data,int w,int h){
    int colors[256][2]={0};
    for(int i=0;i<256;i++) colors[i][1]=i;
    for(int i=0;i<w*h;i++) colors[data[i]][0]++;
    qsort(colors, 256, sizeof(int)*2, compare_color_freq);
    int i=0;
    int ret=0;
    while(w*h/colors[i][0]<270){
        ret=max(ret,colors[i][1]);
        i++;
    }
    printf("cutout at %d...",ret);
    return ret;
}
void draw_vline(unsigned char* data,int w,int h,double angle,int x){
    double dx=sin(angle/180*M_PI);
    for(int y=0;y<h;y++){
        int addx=y*dx+x;
		if(addx<0||addx>=w) continue;
        data[y*w+addx]=255;
		addx++;
		if(addx<0||addx>=w) continue;
        data[y*w+addx]=255;
    }
}
void draw_line(unsigned char* data,int w,int h,double angle,int y){
    double dy=sin(angle/180*M_PI);
    for(int x=0;x<w;x++){
        int addy=x*dy+y;
		if(addy<0||addy>=h) continue;
        data[addy*w+x]=255;
		addy++;
		if(addy<0||addy>=h) continue;
        data[addy*w+x]=255;
    }
}
/*
123  41
456  52
     63
*/

int flipped_index(int x,int y,int w,int h,int angle){
	if(angle<0) angle+=4;
	if(angle>=4) angle%=4;
	int nx=x;
	int ny=y;
	int nw=w;
	int nh=h;
	if(angle==1){
		nx=h-y-1;
		ny=x;
		nw=h;
		nh=w;
	}
	if(angle==2){
		nx=w-x-1;	
		ny=h-x-1;
	}
	if(angle==3){
		nx=y;	
		ny=w-x-1;
		nw=h;
		nh=w;
	}
	return nx+nw*ny;
}
unsigned char* flip_bmp(unsigned char* data,int w,int h,int angle){
	unsigned char* ret=myalloc(w*h);
	for(int x=0;x<w;x++) for(int y=0;y<h;y++) ret[flipped_index(x,y,w,h,angle)]=data[x+w*y];
	return ret;
}
int* find_lines(unsigned char* data,int w,int h,double angle){
    double dy=sin(angle/180*M_PI);
    int maxdy=w*dy;
    //int fromy=h*.05;
    //int uptoy=h-1-h*.05;
	int fromy=0;
	int uptoy=h-1;
    if(maxdy<0) fromy-=maxdy;
    if(maxdy>0) uptoy-=maxdy;
    int* diffs=myalloc(h*sizeof(int)*2);
    for(int y=fromy;y<uptoy;y++){
        int total=0;
        for(int x=0;x<w;x++){
            int addy=x*dy;
            int bottom=data[(y+addy+1)*w+x];
            int top=data[(addy+y)*w+x];
            if(top<bottom) continue;
            total+=(top-bottom);
        }
        diffs[(y-fromy)*2]=total;
        diffs[(y-fromy)*2+1]=y;
    }
    qsort(diffs, uptoy-fromy, sizeof(int)*2, compare_color_freq);
	return diffs;
}
double best_edge(unsigned char* data,int w,int h,double angle,int *edgey){
	int* diffs=find_lines(data,w,h,angle);
    double ret=diffs[0];
    *edgey=diffs[1];
    free(diffs);
    return ret;
}
double find_best_angle(unsigned char* data,int w,int h,double angle,double range,double step,int* aty){
    double ret=0;
    double best=0;
    int besty=0;
    for(double a=angle-range;a<=angle+range;a+=step){
        int y;
        double weight=best_edge(data, w, h,a,&y);
        if(weight>=best){
            ret=a;
            best=weight;
            besty=y;
        }
    }
    *aty=besty;
    return ret;
}
double turn_angle(unsigned char* data,int w,int h){
    int y;
    double ret=find_best_angle(data, w, h,0,20,5,&y);
    ret=find_best_angle(data, w, h,ret,5,1,&y);
    ret=find_best_angle(data, w, h,ret,1,.2,&y);
//    printf("best angle=%f,best weight=\n",ret);
    //draw_line(data, w, h, ret, y);
    return ret;
}
int turn_bg_white(unsigned char* data,int w,int h){
    int bg=bgcolor(data,w,h);
    for(int i=0;i<w*h;i++){ if(data[i]<=bg) data[i]=0; else data[i]-=bg;};
    return bg;
}
int to_grayscale_bmp(unsigned char* data,int w,int h,unsigned char** out,int xppm,int yppm){
    int header_size=13+256;
    int padding=w%4;
    if(padding) padding=4-padding;
    int wi=(w+padding)/4;
    int size=wi*h+header_size;
    unsigned int* ints=myalloc(size*4);
    memset(ints,0,size*4);
    int header[13]={size*4+2,0,header_size*4+2,40,w,-h,(8<<16)+1,0,0,xppm,yppm,256,0};
    memcpy(ints,header,sizeof(header));
    for(int i=0;i<256;i++) ints[13+i]=((255-i)<<16)+((255-i)<<8)+(255-i);
    for(int i=0;i<h;i++) memcpy(ints+header_size+wi*i,data+w*i,w);
    *out=ints;
    return size*4;
}
int save_bmp(char* filename,unsigned char* data,int size){
    FILE* fp=fopen(filename,"wb");
    if(!fp) return error("failed to write to file");
    fprintf(fp,"BM");
    fwrite(data,size,1,fp);
    fclose(fp);
    return 1;
}
int usage(){
    printf("Usage:\n");
	printf("\tbmp gray <IN_BMP_FILE> <OUT_BMP_FILE>\n");
	printf("\tbmp topup <IN_BMP_FILE> <OUT_BMP_FILE>\n");
	printf("\tbmp trim <IN_BMP_FILE> <OUT_BMP_FILE>\n");
	printf("\tbmp white <IN_BMP_FILE> <OUT_BMP_FILE>\n");
	printf("\tbmp flip <IN_BMP_FILE> <OUT_BMP_FILE>\n");
    return 0;
}
unsigned char* grab_part(unsigned char* data,int w,int h,int cx,int cy,int w2,int h2){
    unsigned char* ret=myalloc(w2*h2);
    for(int ny=0;ny<h2;ny++){
        for(int nx=0;nx<w2;nx++){
            ret[nx+ny*w2]=data[cx-w2/2+nx+(cy-h2/2+ny)*w];
        }
    }
    return ret;
}
int save_gray_bmp(char* outfile,unsigned char* bits,int w,int h,int xppm,int yppm){
    unsigned char* graybmp;
    int size=to_grayscale_bmp(bits, w, h, &graybmp,xppm,yppm);
    save_bmp(outfile,graybmp,size);
	free(graybmp);
	//free(bits);
	return 0;
}
unsigned char* read_gray_bmp(char* infile,int* w,int* h,int* xppm,int* yppm){
    unsigned char* data=NULL;
    int size=read_file(infile,&data);
    unsigned char* rgb;
	int depth;
    parse_bmp(data,w,h,&depth,&rgb,xppm,yppm);
    unsigned char* bits;
    remove_padding(rgb, *w, *h,depth);
    to_grayscale(rgb, &bits, *w, *h, depth);
    if(bits!=rgb) free(data);
	return bits;
}
int line_between(int* lines,int total,int from,int upto){
	for(int i=0;i<total;i++) if(lines[i*2+1]>=from && lines[i*2+1]<=upto) return lines[i*2+1];
	return 0;
}
int bmp_trim(char* infile,char* outfile){
	int w,h,xppm,yppm;
	unsigned char* bits=read_gray_bmp(infile,&w,&h,&xppm,&yppm);
    double rotate=turn_angle(bits, w, h);
    rotate_image(bits,w,h,rotate);
	int* lines=find_lines(bits,w,h,0);
	int top=line_between(lines,h,h*.05,h*.2);
	int bottom=line_between(lines,h,h*.8,h*.95);
//	for(int i=0;i<25;i++){
//		printf("%d,%d\n",lines[i*2],lines[i*2+1]);
//		draw_line(bits,w,h,0,lines[i*2+1]);
//	}
	unsigned char* flipped=flip_bmp(bits,w,h,1);
	int* lines2=find_lines(flipped,h,w,0);
	int left=line_between(lines2,w,w*.02,w*.1);
	int right=line_between(lines2,w,w*.9,w*.98);
//	for(int i=0;i<200;i++){
//		printf("%d,%d\n",lines2[i*2],lines2[i*2+1]);
//		draw_vline(bits,w,h,0,lines2[i*2+1]-1);
//	}
	free(lines);
	free(lines2);
	int cx=(left+right)/2;
	int cy=(top+bottom)/2;
	int nw=right-left;
	int nh=bottom-top;
	unsigned char* mid=grab_part(bits,w,h,cx,cy,nw,nh);
	printf("rotate:%g,width=%d,height=%d,left=%d,right=%d,top=%d,bottom=%d,ratio=%g\n",rotate,w,h,left,right,top,bottom,nw/nh*100);
	return save_gray_bmp(outfile,mid,nw,nh,xppm,yppm);
	draw_line(bits,w,h,0,top);
	draw_line(bits,w,h,0,bottom);
	draw_vline(bits,w,h,0,left);
	draw_vline(bits,w,h,0,right);
	return save_gray_bmp(outfile,bits,w,h,xppm,yppm);
    //turn_bg_white(bits, w, h);
}
int bmp_flip(char* infile,char* outfile){
	int w,h,xppm,yppm;
	unsigned char* bits=read_gray_bmp(infile,&w,&h,&xppm,&yppm);
    unsigned char* flipped=flip_bmp(bits,w,h,1);
	return save_gray_bmp(outfile,flipped,h,w,xppm,yppm);
}
int bmp_topup(char* infile,char* outfile){
	int w,h,xppm,yppm;
	unsigned char* bits=read_gray_bmp(infile,&w,&h,&xppm,&yppm);
    double rotate=turn_angle(bits, w, h);
	//double rotate=0;
    rotate_image(bits,w,h,rotate);
	return save_gray_bmp(outfile,bits,w,h,xppm,yppm);
}
int bmp_gray(char* infile,char* outfile){
	int w,h,xppm,yppm;
	unsigned char* bits=read_gray_bmp(infile,&w,&h,&xppm,&yppm);
	return save_gray_bmp(outfile,bits,w,h,xppm,yppm);
    //turn_bg_white(bits, w, h);
}
int main(int argc, const char * argv[]){
    if(argc<4) return usage();
    char* op=argv[1];
	char* infile=argv[2];
	char* outfile=argv[3];
	if(strcmp(op,"gray")==0) return bmp_gray(infile,outfile);
	if(strcmp(op,"topup")==0) return bmp_topup(infile,outfile);
	if(strcmp(op,"trim")==0) return bmp_trim(infile,outfile);
	if(strcmp(op,"flip")==0) return bmp_flip(infile,outfile);
	return 0;
/*
    double parts[1][4]={{.1,.2,.2,.1}};
    for(int part=0;part<1;part++){
        int nx=w*parts[part][0];
        int ny=h*parts[part][1];
        int w2=w*parts[part][2];
        int h2=h*parts[part][3];
        unsigned char* partbits=grab_part(bits, w, h, nx, ny, w2, h2);
        sprintf(filename,"part%d.%s",part,argv[1]);
        unsigned char* partdata;
        int size2=to_grayscale_bmp(partbits, w2, h2, &partdata, xppm, yppm);
        save_bmp(filename,partdata,size2);
        free(partdata);
        free(partbits);
    }
    printf("ok.\n");
    return 0;
*/
}

