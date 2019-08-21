bool EffectExportVT2(int effect,AnsiString filename,int base)
{
	const float freqs[12]={ 2093.0f,2217.4f,2349.2f,2489.0f,2637.0f,2793.8f,2960.0f,3136.0f,3322.4f,3520.0f,3729.2f,3951.0f };
	int i,j,t,n,len;
	TStringList *outtext;
	AnsiString str;
	int offset[8*12];

	if((len=EffectRealLen(curEffect))<1) return false;

	for(i=0;i<8;++i)
	{
		for(j=0;j<12;++j)
		{
			n=(int)(AY_CLOCK*8.0f/freqs[j]);

			offset[i*12+j]=n>>i;
		}
	}

	outtext=new TStringList;

	try{

		outtext->Clear();
		outtext->Add("[Sample]");
		
		if(len>0x3f) len=0x3f;

		for(i=0;i<len;++i)
		{
			str =ayBank[effect].ayEffect[i].t?"T":"t";
			str+=ayBank[effect].ayEffect[i].n?"N":"n";
			str+="e ";

			t=ayBank[effect].ayEffect[i].tone-offset[base];

			if(t<-1023) t=-1023;
			if(t>1023) t=1023;

			if(t<0) str+="-"; else str+="+";

			str+=IntToHex(abs(t),3)+"_ +";
			str+=IntToHex(ayBank[effect].ayEffect[i].noise,2)+"_ ";
			str+=IntToHex(ayBank[effect].ayEffect[i].volume,1)+"_";

			outtext->Add(str);
		}

		outtext->Add("tne +000_ +00_ 0_ L");

		outtext->SaveToFile(filename);
	}
	__finally{
		delete outtext;
	}
	
	return true;
}