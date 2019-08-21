//код работы с lh5 взят из vtx-плеера by SMT

#include "lh5/ar.h"

int unpackable;            /* global, set in io.c */
ulong compsize, origsize;  /* global */
uchar *src_0;

uchar bf[DICSIZ+1000];

uchar getc_arcfile()
{
	return *src_0++;
}

#include "lh5/io.c"
#include "lh5/huf.c"
#include "lh5/maketbl.c"
#include "lh5/maketree.c"
#include "lh5/decode.c"


void decode_buffer(unsigned char *dst, unsigned dstsize, unsigned char *src, unsigned srcsize)
{
	unsigned blksize;
	
	src_0=src;
	compsize=srcsize;
	decode_start();
	while(dstsize)
	{
		if(DICSIZ<dstsize) blksize=DICSIZ; else blksize=dstsize;
		decode(blksize, bf);
		memcpy(dst, bf, blksize);
		dst += blksize;
		dstsize -= blksize;
	}
}



bool ImportVTX(int fxn,const char *filename,int ichan)
{
	const int regord[11]={5,6,7,8,9,10,0,1,2,3,4};
	
	FILE *file;
	unsigned char *data,*pdata;
	int aa,size,ptr,pd,pp;
    int usize,block;
    int chan,tone,noise,volume,nvolume,newchan;
    int ltone[3],lnoise;
	bool t,n,lt[3],ln[3];
	
	file=fopen(filename,"rb");
	if(!file) return false;
	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);
	pdata=(unsigned char*)malloc(size);
	fread(pdata,size,1,file);
	fclose(file);
	
	if((pdata[0]!='a'||pdata[1]!='y')&&(pdata[0]!='y'||pdata[1]!='m'))
	{
		Application->MessageBox("This is not a VTX file","Error",MB_OK);
		free(pdata);
		return false;
	}

	usize=RdDWordLH(&pdata[12]);
	
	ptr=16;
	for(aa=0;aa<5;aa++)
	{
		while(ptr<size)
		{
			if(pdata[ptr]==0) break;
			ptr++;
		}
		ptr++;
	}
	
	data=(unsigned char*)malloc(usize);
	decode_buffer(data,usize,&pdata[ptr],usize);
	free(pdata);
	size=usize;
	
	EffectInit(fxn);
	
	pd=0;
	pp=0;
	tone=0;
	noise=0;
	lnoise=0;
	volume=0;
	for(aa=0;aa<3;aa++)
	{
		lt[aa]=false;
		ln[aa]=false;
		ltone[aa]=0;
	}
	t=false;
	n=false;
	chan=ichan;
	newchan=chan;
	block=size/14;
	
	while(pp<block)
	{
		for(aa=0;aa<11;aa++)
		{
			ptr=pp+regord[aa]*block;
			switch(regord[aa])
			{
			case 0:
				ltone[0]=(ltone[0]&0x0f00)|data[ptr];
				if(chan==0) tone=ltone[0];
				break;
			case 1:
				ltone[0]=(ltone[0]&0x00ff)|(data[ptr]<<8);
				if(chan==0) tone=ltone[0];
				break;
			case 2:
				ltone[1]=(ltone[1]&0x0f00)|data[ptr];
				if(chan==1) tone=ltone[1];
				break;
			case 3:
				ltone[1]=(ltone[1]&0x00ff)|(data[ptr]<<8);
				if(chan==1) tone=ltone[1];
				break;
			case 4:
				ltone[2]=(ltone[2]&0x0f00)|data[ptr];
				if(chan==2) tone=ltone[2];
				break;
			case 5:
				ltone[2]=(ltone[2]&0x00ff)|(data[ptr]<<8);
				if(chan==2) tone=ltone[2];
				break;
			case 6:
				lnoise=data[ptr]&0x1f;
				if(chan>=0) noise=lnoise;
				break;
			case 7:
				if(data[ptr]&1)  lt[0]=false; else lt[0]=true;
				if(data[ptr]&8)  ln[0]=false; else ln[0]=true;
				if(data[ptr]&2)  lt[1]=false; else lt[1]=true;
				if(data[ptr]&16) ln[1]=false; else ln[1]=true;
				if(data[ptr]&4)  lt[2]=false; else lt[2]=true;
				if(data[ptr]&32) ln[2]=false; else ln[2]=true;
				if(chan>=0)
				{
					t=lt[chan];
					n=ln[chan];
				}
				break;
			case 8:
				nvolume=data[ptr]&0x0f;
				if(chan<0 && nvolume>0) { newchan=0; volume=nvolume; }
				if(chan==0) volume=nvolume;
				break;
			case 9:
				nvolume=data[ptr]&0x0f;
				if(chan<0 && nvolume>0) { newchan=1; volume=nvolume; }
				if(chan==1) volume=nvolume;
				break;
			case 10:
				nvolume=data[ptr]&0x0f;
				if(chan<0 && nvolume>0) {newchan=2; volume=nvolume; }
				if(chan==2) volume=nvolume;
				break;
			}
			if(newchan>=0)
			{
					chan=newchan;
					noise=lnoise;
					tone=ltone[chan];
					t=lt[chan];
					n=ln[chan];
			}
		}	
		if(pd>=4096) break;
		
		if(chan>=0)
		{
			ayBank[fxn].ayEffect[pd].tone=tone;
			ayBank[fxn].ayEffect[pd].noise=noise;
			ayBank[fxn].ayEffect[pd].volume=volume;
			ayBank[fxn].ayEffect[pd].t=t;
			ayBank[fxn].ayEffect[pd].n=n;
			pd++;
		}
		pp++;
	}
	
	free(data);
	strcpy(ayBank[fxn].name,NameTrimPath(filename).c_str());
	
	return true;
}