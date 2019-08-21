bool EffectExportCSV(int effect,AnsiString filename)
{
	FILE *file;
	int aa,fxn,fx_len;
	ayfxCell *afx;

	file=fopen(filename.c_str(),"wt");

	if(!file) return false;

	fx_len=EffectRealLen(effect);
	afx=ayBank[effect].ayEffect;

	for(aa=0;aa<fx_len;aa++)
	{
		fprintf(file,"%i,%i,0x%3.3x,0x%2.2x,0x%1.1x\n",afx[aa].t,afx[aa].n,afx[aa].tone,afx[aa].noise,afx[aa].volume);
	}

	fclose(file);

	return true;
}
