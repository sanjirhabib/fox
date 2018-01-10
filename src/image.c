#include <wand/MagickWand.h>
#include <fox.h>

char* wand_img(void* mw){
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
void* img_wand(char* img){
	MagickWand* mw=NULL;
	MagickWandGenesis();
	mw=NewMagickWand();
	MagickReadImageBlob(mw,img,str_len(img));
	return mw;
};
char* img_resize(char* img, double enlarge, int width,int height){
	MagickWand* mw=img_wand(img);

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
				MagickCropImage(mw,nwidth,nheight,0,(iheight-nheight)/2);
			}else{
				nheight=iheight;
				nwidth=width*1.0/height*iheight;
				MagickCropImage(mw,nwidth,nheight,(iwidth-nwidth)/2,0); }; };
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
		//MagickResizeImage(mw,width,height,LanczosFilter,1)
		MagickResizeImage(mw,width,height,LanczosFilter);
	};
	return wand_img(mw);
};

