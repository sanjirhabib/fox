#include <MagickWand/MagickWand.h>
#include "draw.h"
int run(map* args){
	write_file(img_resize(file_read(map_id(args,1),0,1),to_int(map_id(args,3)),to_int(map_id(args,4)),to_int(map_id(args,5))),map_id(args,2),0,1);
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
			int nheight=width*1.0/iwidth*iheight;
			if(nheight<height){
				int nwidth=height*1.0/iheight*iwidth;
				MagickCropImage(mw,width,height,(nwidth-width)/2,0);
			}else{
				MagickCropImage(mw,width,height,0,(nheight-height)/2); };
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
