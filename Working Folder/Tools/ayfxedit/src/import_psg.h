bool ImportPSG(int fxn,const char *filename,int ichan)
{
	FILE *file;
	unsigned char *data;
	int aa,size,ptr,pd,icnt;
    int chan,tone,noise,volume,nvolume,newchan;
    int ltone[3],lnoise;
	bool lt[3],ln[3];
	bool t,n,first;
	
	file=fopen(filename,"rb");
	if(!file) return false;
	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);
	data=(unsigned char*)malloc(size);
	fread(data,size,1,file);
	fclose(file);
	
	if(data[0]!='P'||data[1]!='S'||data[2]!='G')
	{
		Application->MessageBox("This is not a PSG file","Error",MB_OK);
		free(data);
		return false;
	}
	
	EffectInit(fxn);
	
	pd=0;
	ptr=16;
	tone=0;
	noise=0;
	volume=0;
	lnoise=0;
	for(aa=0;aa<3;aa++)
	{
		lt[aa]=false;
		ln[aa]=false;
		ltone[aa]=0;
	}
	t=false;
	n=false;
	first=true;
	chan=ichan;
	newchan=chan;
	icnt=0;
	
	while(ptr<size)
	{
		switch(data[ptr])
		{
		case 0:
			ltone[0]=(ltone[0]&0x0f00)|data[ptr+1];
			if(chan<0&&ltone[0]>0)
			{
				//chan=0;
				noise=lnoise;
				tone=ltone[0];
				t=lt[0];
				n=ln[0];
			}
			if(chan==0) tone=ltone[0];
			ptr+=2;
			break;
		case 1:
			ltone[0]=(ltone[0]&0x00ff)|(data[ptr+1]<<8);
			if(chan<0&&ltone[0]>0)
			{
				//chan=0;
				noise=lnoise;
				tone=ltone[0];
				t=lt[0];
				n=ln[0];
			}
			if(chan==0) tone=ltone[0];
			ptr+=2;
			break;
		case 2:
			ltone[1]=(ltone[1]&0x0f00)|data[ptr+1];
			if(chan<0&&ltone[1]>0)
			{
				//chan=1;
				noise=lnoise;
				tone=ltone[1];
				t=lt[1];
				n=ln[1];
			}
			if(chan==1) tone=ltone[1];
			ptr+=2;
			break;
		case 3:
			ltone[1]=(ltone[1]&0x00ff)|(data[ptr+1]<<8);
			if(chan<0&&ltone[1]>0)
			{
				//chan=1;
				noise=lnoise;
				tone=ltone[1];
				t=lt[1];
				n=ln[1];
			}
			if(chan==1) tone=ltone[1];
			ptr+=2;
			break;
		case 4:
			ltone[2]=(ltone[2]&0x0f00)|data[ptr+1];
			if(chan<0&&ltone[2]>0)
			{
				//chan=2;
				noise=lnoise;
				tone=ltone[2];
				t=lt[2];
				n=ln[2];
			}
			if(chan==2) tone=ltone[2];
			ptr+=2;
			break;
		case 5:
			ltone[2]=(ltone[2]&0x00ff)|(data[ptr+1]<<8);
			if(chan<0&&ltone[2]>0)
			{
				//chan=2;
				noise=lnoise;
				tone=ltone[2];
				t=lt[2];
				n=ln[2];
			}
			if(chan==2) tone=ltone[2];
			ptr+=2;
			break;
		case 6:
			lnoise=data[ptr+1]&0x1f;
			if(chan>=0) noise=lnoise;
			ptr+=2;
			break;
		case 7:
			if(data[ptr+1]&1)  lt[0]=false; else lt[0]=true;
			if(data[ptr+1]&8)  ln[0]=false; else ln[0]=true;
			if(data[ptr+1]&2)  lt[1]=false; else lt[1]=true;
			if(data[ptr+1]&16) ln[1]=false; else ln[1]=true;
			if(data[ptr+1]&4)  lt[2]=false; else lt[2]=true;
			if(data[ptr+1]&32) ln[2]=false; else ln[2]=true;
			if(chan>=0)
			{
				t=lt[chan];
				n=ln[chan];
			}
			ptr+=2;
			break;
		case 8:
			nvolume=data[ptr+1]&0x0f;
			if(chan<0 && nvolume>0) { newchan=0;  volume=nvolume; }
			if(chan==0) volume=nvolume;
			ptr+=2;
			break;
		case 9:
			nvolume=data[ptr+1]&0x0f;
			if(chan<0 && nvolume>0) { newchan=1; volume=nvolume; }
			if(chan==1) volume=nvolume;
			ptr+=2;
			break;
		case 10:
			nvolume=data[ptr+1]&0x0f;
			if(chan<0 && nvolume>0) { newchan=2; volume=nvolume; }
			if(chan==2) volume=nvolume;
			ptr+=2;
			break;
			
		case 255:
			if(pd==4096) ptr=size;
			icnt=1;
			ptr++;
			break;
			
		case 254:
			icnt=data[ptr+1]*4;
			ptr+=2;
			break;
			
		case 253:
			ptr=size;
			break;
			
		default:
			ptr+=2;
		}
		if(newchan>=0)
		{
			chan=newchan;
			noise=lnoise;
			tone=ltone[chan];
			t=lt[chan];
			n=ln[chan];
		}
		for(aa=0;aa<icnt;aa++)
		{
			if(pd==4096)
			{
				ptr=size;
				break;
			}
			if(first)
			{
				if(tone==0&&noise==0&&volume==0) continue;
			}
			first=false;
			ayBank[fxn].ayEffect[pd].tone=tone;
			ayBank[fxn].ayEffect[pd].noise=noise;
			ayBank[fxn].ayEffect[pd].volume=volume;
			ayBank[fxn].ayEffect[pd].t=t;
			ayBank[fxn].ayEffect[pd].n=n;
			pd++;
	       }
		icnt=0;
   }
			
   free(data);
   strcpy(ayBank[fxn].name,NameTrimPath(filename).c_str());
   
   return true;
}