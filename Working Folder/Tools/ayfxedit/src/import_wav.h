/* tuner code is C version of Java 5KTuner by John Montgomery, CC-BY 2008, integrated into AYFXedit in 2017 */



int *tuner_a=NULL;
int tuner_a_len;



void tuner_init(void)
{
	tuner_a=NULL;
	tuner_a_len=0;
}



void tuner_shut(void)
{
	if(tuner_a)
	{
		free(tuner_a);
		tuner_a=NULL;
	}
}




float tuner_analyze(short *wave,int rate,int len)
{
	double diff,prevDiff,dx,prevDx,maxDiff,dist,minDist,matchFreq,prevFreq,nextFreq;
	int i,j,ptr,sampleLen,value,detune;

	len>>=1;

	prevDiff=0;
	prevDx=0;
	maxDiff=0;
	sampleLen=0;

	if(!tuner_a)
	{
		tuner_a=(int*)malloc(len*sizeof(int));
		tuner_a_len=len;
	}

	if(tuner_a_len<len)
	{
		tuner_a=(int*)realloc(tuner_a,len*sizeof(int));
		tuner_a_len=len;
	}

	ptr=0;

	for(i=0;i<len;++i)
	{
		tuner_a[i]=wave[ptr+0]+wave[ptr+1];
		ptr+=2;
	}

	for(i=0;i<len;++i)
	{
		diff=0;

		for(j=0;j<len;++j) if(i+j<len) diff+=abs(tuner_a[j]-tuner_a[i+j]); else diff+=abs(tuner_a[j]);

		dx=prevDiff-diff;

		if(dx<0&&prevDx>0&&diff<(0.5*maxDiff))
		{
			sampleLen=i-1;
			break;
		}

		prevDx=dx;
		prevDiff=diff;

		if(diff>maxDiff) maxDiff=diff;
	}

	if(sampleLen<=0) return 0;

	return (double)rate/(double)sampleLen;
}



const int ayvoltab[256]={
	0,1,1,2,3,3,4,4,4,5,5,5,6,6,6,6,6,7,7,7,7,7,7,7,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,9,9,9,9,9,
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
	11,11,11,11,11,11,11,11,11,11,11,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
	12,12,12,12,12,12,12,12,12,12,12,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
	13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
	14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,
	14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,14,15,15,15,15,15,15,
	15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15
};



bool ImportWAV(int effect,const char *filename)
{
	FILE *file;
	unsigned char *src_data;
	int src_channels,src_sample_rate,src_bit_depth,src_length;
	int i,j,pd,smp,ptr,size,min,max,freq;
	int wave_length;
	short int *wave_data;
	float ffrq,fptr,fstep,mul;
	int frame_samples=44100/50;
	int frames_look_ahead=2;
	bool pre_normalize=true;
	bool post_normalize=true;



	file=fopen(filename,"rb");

	if(!file) return false;

	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);

	src_data=(unsigned char*)malloc(size);

	fread(src_data,size,1,file);
	fclose(file);
	
	//parse wave format
	
	ptr=0;
	
	if(memcmp(&src_data[ptr],"RIFF",4))
	{
		Application->MessageBox("This is not a WAVE file","Error",MB_OK);
		free(src_data);
		return false;
	}
	
	ptr+=8;

	if(memcmp(&src_data[ptr],"WAVEfmt ",8))
	{
		Application->MessageBox("This is not a WAVE file","Error",MB_OK);
		free(src_data);
		return false;
	}
	
	if(RdWordLH(&src_data[ptr+12])!=1)
	{
		Application->MessageBox("Only unpacked PCM files supported","Error",MB_OK);
		free(src_data);
		return false;
	}

	src_channels   =RdWordLH (&src_data[ptr+14]);
	src_sample_rate=RdDWordLH(&src_data[ptr+16]);
	src_bit_depth  =RdWordLH (&src_data[ptr+26]);
	
	if(src_bit_depth!=8&&src_bit_depth!=16)
	{
		Application->MessageBox("Only 8/16bit samples supported","Error",MB_OK);
		free(src_data);
		return false;
	}

	ptr+=28;

	while(ptr<size)
	{
		if(!memcmp(&src_data[ptr],"data",4)) break;

		++ptr;
	}

	if(ptr>=size)
	{
		Application->MessageBox("No DATA chunk in file","Error",MB_OK);
		free(src_data);
		return false;
	}

	src_length=RdDWordLH(&src_data[ptr+4])/(src_bit_depth>>3);

	ptr+=8;



	//convert to 44100 Hz 16-bit mono, with crude resampling and prenormalization

	wave_length=(int)(float(src_length)/float(src_sample_rate)*44100.0f);

	if((wave_length%frame_samples)>0) wave_length=wave_length/frame_samples*frame_samples+frame_samples;

	wave_length+=frame_samples*(frames_look_ahead-1);

	wave_data=(short int*)malloc(wave_length*sizeof(short int));

	fptr=ptr;
	fstep=float(src_length)/float(wave_length);

	min= 0xffff;
	max=-0xffff;

	ptr=-1;

	if(src_bit_depth==8)
	{
		for(i=0;i<wave_length;++i)
		{
			pd=int(fptr);

			if(pd<src_length)
			{
				if(pd!=ptr)
				{
					ptr=pd;
					smp=0;

					for(j=0;j<src_channels;++j)
					{
						smp+=((int(src_data[ptr+j])-128)<<8);
					}

					smp/=src_channels;

					if(smp>max) max=smp;
					if(smp<min) min=smp;
				}
			}
			else
			{
				smp=0;
			}

			wave_data[i]=smp;

			fptr+=fstep;
		}
	}
	else
	{
		for(i=0;i<wave_length;++i)
		{
			pd=int(fptr);

			if(pd<src_length)
			{
				if(pd!=ptr)
				{
					ptr=pd<<1;
					smp=0;

					for(j=0;j<(src_channels<<1);j+=2)
					{
						smp+=((short int)RdWordLH(&src_data[ptr+j]));
					}

					ptr=pd;

					smp/=src_channels;

					if(smp>max) max=smp;
					if(smp<min) min=smp;
				}
			}
			else
			{
				smp=0;
			}

			wave_data[i]=smp;

			fptr+=fstep;
		}
	}

	free(src_data);

	if(pre_normalize)
	{
		if(max-min>0) mul=65535.0f/float(max-min); else mul=1.0f;

		for(i=0;i<wave_length;++i)
		{
			wave_data[i]=(short int)(float(wave_data[i])*mul);
		}
	}



	//conversion

	EffectInit(effect);

	if(post_normalize)
	{
		max=1;

		for(ptr=0;ptr<wave_length-(frame_samples*(frames_look_ahead-1));ptr+=frame_samples)
		{
			smp=0;
			for(i=0;i<frame_samples*frames_look_ahead;++i) smp+=abs(wave_data[ptr+i]);
			smp=smp/(frame_samples*frames_look_ahead);
			if(smp>max) max=smp;
		}

		mul=32768.0f/float(max);
	}
	else
	{
		mul=1.0f;
	}

	tuner_init();

	pd=0;

	for(ptr=0;ptr<wave_length-(frame_samples*(frames_look_ahead-1));ptr+=frame_samples)
	{
		//analyze frame frequency

		ffrq=tuner_analyze(&wave_data[ptr],44100,frame_samples*frames_look_ahead);

		if(ffrq<=0) ffrq=.0001f;	//prevent division by zero

		freq=(int)(AY_CLOCK/8.0/ffrq);

		if(freq<0) freq=0;
		if(freq>4095) freq=4095;

		ayBank[effect].ayEffect[pd].noise=freq>>7;
		ayBank[effect].ayEffect[pd].tone=freq;

		//calculate frame volume

		smp=0;

		for(i=0;i<frame_samples;++i) smp+=abs(wave_data[ptr+i]);

		smp=smp/frame_samples;
		smp=(int)(float(smp)*mul);
		smp>>=7;

		if(smp>255) smp=255;

		ayBank[effect].ayEffect[pd].t=true;
		ayBank[effect].ayEffect[pd].n=!freq?true:false;
		ayBank[effect].ayEffect[pd].volume=ayvoltab[smp];

		++pd;

		if(pd>=max_fx_len) break;
	}

	tuner_shut();

	free(wave_data);

	strcpy(ayBank[effect].name,NameTrimPath(filename).c_str());

	return true;
}