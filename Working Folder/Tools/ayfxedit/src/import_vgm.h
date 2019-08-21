struct {
	unsigned char chanVol[4];
	short int     chanDiv[4];
	short int     chanCnt[4];
	
	unsigned short int noiseLFSR;
	unsigned short int noiseTBits;
	
	int tickPerSample;
	
	char latchedChan;
	char latchedType;//0 tone(noise) 1 volume
} PSG;


struct {
	unsigned char *data;
	int wait;
	int ptr;
	int ticksPerSample;
	int loopOffset;
	bool play;
} VGM;




inline void psg_out_port(unsigned char val)
{
	int chan,div;
	
	if(val&128)
	{
		chan=(val>>5)&3;
		div=(PSG.chanDiv[chan]&0xfff0)|(val&15);
		
		PSG.latchedChan=chan;
		PSG.latchedType=val&16;
	}
	else
	{
		chan=PSG.latchedChan;
		div=(PSG.chanDiv[chan]&15)|((val&63)<<4);
	}
	
	if(PSG.latchedType)
	{
		PSG.chanVol[chan]=(PSG.chanVol[chan]&16)|(val&15);
	}
	else
	{
		PSG.chanDiv[chan]=div;
		if(chan==3)
		{
			if(((div>>2)&1)) PSG.noiseTBits=9; else PSG.noiseTBits=1;
			PSG.noiseLFSR=0x8000;
		}
	}
}



int read_int32(unsigned char *mem)
{
	return (mem[0]+(mem[1]<<8)+(mem[2]<<16)+(mem[3]<<24));
}



bool ImportVGM(int fxn,const char *filename,int ichan,bool inoise)
{
	FILE *file;
	int size;
	int lenght;
	int baseFreq;
	int tvol,nvol,ovol;
	int frq,noiseDiv,toneDiv;
	int incr,wcnt,pd,pchan;
	bool done;

	file=fopen(filename,"rb");
	if(!file) return false;

	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);
	VGM.data=(unsigned char*)malloc(size);
	fread(VGM.data,size,1,file);
	fclose(file);
	
	if(memcmp(VGM.data,"Vgm ",4)!=0)
	{
		Application->MessageBox("No VGM signature found (possibly wrong file type)","Error",MB_OK);
		free(VGM.data);
		return false;
	}
	
	VGM.wait=0;
	VGM.ptr=0x40;
	baseFreq=read_int32(&VGM.data[0x0c]);

	if(baseFreq==0)
	{
		Application->MessageBox("No data for PSG found","Error",MB_OK);
		free(VGM.data);
		return false;
	}

	memset(&PSG,0,sizeof(PSG));

	EffectInit(fxn);

	noiseDiv=0;

	done=false;
	pd=0;
	pchan=ichan;

	while(!done)
	{
		switch(VGM.data[VGM.ptr])
		{
		case 0x50:
			psg_out_port(VGM.data[VGM.ptr+1]);
		case 0x4f:
			incr=2;
			break;
		case 0x61:
			VGM.wait+=VGM.data[VGM.ptr+1]+(VGM.data[VGM.ptr+2]<<8);
		case 0x51:
		case 0x52:
		case 0x53:
		case 0x54:
			incr=3;
			break;
		case 0x62:
			VGM.wait+=735;
			incr=1;
			break;
		case 0x63:
			VGM.wait+=882;
			incr=1;
			break;
		case 0x66:
			done=true;
		default:
			incr=0;
		}
		VGM.ptr+=incr;

		if(VGM.wait>=735)
		{
			if(pchan<0)
			{
				if(PSG.chanVol[0]<15) pchan=0; else
				if(PSG.chanVol[1]<15) pchan=1; else
				if(PSG.chanVol[2]<15) pchan=2;
			}

			VGM.wait-=735;

			if(pchan>=0)
			{
				tvol=15-PSG.chanVol[pchan];
				nvol=15-PSG.chanVol[3];

				if(!inoise)
				{
					nvol=0;
				}
				else
				{
					switch(PSG.chanDiv[3]&3)
					{
					case 0: noiseDiv=0x1f; break;
					case 1: noiseDiv=0x19; break;
					case 2: noiseDiv=0x10; break;
					case 3:
						noiseDiv=PSG.chanDiv[2]>>1;
						if(noiseDiv>63) noiseDiv=63;
						break;
					}
				}

				ovol=tvol;

				if(nvol>tvol) ovol=nvol;

				if(PSG.chanDiv[pchan]>0) frq=baseFreq/(PSG.chanDiv[pchan]*16); else frq=100;

				toneDiv=AY_CLOCK/8/frq;

				if(toneDiv<0) toneDiv=0;
				if(toneDiv>4095) toneDiv=4095;

				ayBank[fxn].ayEffect[pd].tone=toneDiv;
				ayBank[fxn].ayEffect[pd].noise=noiseDiv;
				ayBank[fxn].ayEffect[pd].volume=ovol;
				ayBank[fxn].ayEffect[pd].t=tvol?true:false;
				ayBank[fxn].ayEffect[pd].n=nvol?true:false;

				pd++;

				if(pd>=4096) done=true;
			}
		}
	}

	if(VGM.data) free(VGM.data);
	
	return true;
}