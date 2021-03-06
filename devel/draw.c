#include <MagickWand/MagickWand.h>
#include "draw.h"
int run(map* args){
//	args[3].px()
//	args[3].to_double().double_str().px()
//	return 0
	write_file(img_resize(file_read(map_id(args,1),0,1),to_double(map_id(args,3)),to_int(map_id(args,4)),to_int(map_id(args,5))),map_id(args,2),0,1);
	return 0;
};
char* img_resize(char* img, double enlarge, int width,int height){
	MagickWand *mw = NULL;
	MagickWandGenesis();
	mw=NewMagickWand();
	MagickReadImageBlob(mw,img,str_len(img));

	int iwidth = MagickGetImageWidth(mw);
	int iheight = MagickGetImageHeight(mw);

	if(width && height){
		if(enlarge==-1){
			int nheight=width*1.0/iwidth*iheight;
			if(nheight>height){
				width=height*1.0/iheight*iwidth;
			}else{
				height=nheight; };
		}else if(enlarge==1){
			int nheight=width*1.0/iwidth*iheight;
			if(nheight<height){
				width=height*1.0/iheight*iwidth;
			}else{
				height=nheight; };
		}else if(!enlarge){
			int nheight=height*1.0/width*iwidth;
			int nwidth=iwidth;
			if(nheight<iheight){
				px("here",1);
				dx(int_var(iheight),"IHEIGHT",0);
				dx(int_var(iwidth),"IWIDTH",0);
				dx(int_var(nheight),"NHEIGHT",0);
				dx(int_var(nwidth),"NWIDTH",0);
				MagickCropImage(mw,nwidth,nheight,0,(iheight-nheight)/2);
			}else{
				nheight=iheight;
				nwidth=width*1.0/height*iheight;
				MagickCropImage(mw,nwidth,nheight,(iwidth-nwidth)/2,0); };
			MagickSetImageCompressionQuality(mw,95);
			MagickResetIterator(mw);
			size_t size=0;
			char* ret1=MagickGetImageBlob(mw,&size);
			char* ret=blob_dup(ret1,size);
			MagickRelinquishMemory(ret1);
			DestroyMagickWand(mw);
			MagickWandTerminus();
			return ret; };
	};
	if(enlarge==1 && width && !height && iwidth>width){ width=iwidth; };
	if(enlarge==1 && height && !width && iheight>height){ height=iheight; };
	if(enlarge==-1 && width && !height && iwidth<width){ width=iwidth; };
	if(enlarge==-1 && height && !width && iheight<height){ height=iheight; };

	if(enlarge!=1 && enlarge>0 && !width && !height){
		width=enlarge*width;
		height=enlarge*height;
	};
	if(!width){ width=height*1.0/iheight*iwidth; };
	if(!height){ height=width*1.0/iwidth*iheight; };

		

	if(width && height){
		MagickResizeImage(mw,width,height,LanczosFilter);
	};
	MagickSetImageCompressionQuality(mw,95);
	MagickResetIterator(mw);
	size_t size=0;
	char* ret1=MagickGetImageBlob(mw,&size);
	char* ret=blob_dup(ret1,size);
	MagickRelinquishMemory(ret1);
	DestroyMagickWand(mw);
	MagickWandTerminus();
	return ret;
};

char* magick;
