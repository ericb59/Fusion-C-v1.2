//---------------------------------------------------------------------------

#include <vcl.h>
#include <mmsystem.h>
#include <stdio.h>
#include <math.h>
#pragma hdrstop

#include "UnitMain.h"
#include "UnitAYChannelSelect.h"
#include "UnitPiano.h"
#include "UnitSNChannelSelect.h"
#include "UnitVT2Instrument.h"

#define TITLE_STR "AY Sound FX Editor v0.6"

#define AY_CLOCK	1773400
#define MIX_RATE 	44100

int import_chan; //-1=auto, 012=ABC
bool import_ok;
int vgm_chan;
bool vgm_noise;

unsigned char *wave=NULL;
int wave_size=0;

const int edit_xoff=8;
const int edit_yoff=35;
const int edit_head=20;
const int tone_off=151;
const int tone_wdt=326;
const int noise_off=484;
const int noise_wdt=64;
const int vol_off=555;
const int vol_wdt=64;
const int tflag_off=36;
const int tflag_wdt=9;
const int nflag_off=54;
const int nflag_wdt=9;
const int pos_wdt=27;
const int tcol_off=72;
const int tcol_wdt=27;
const int ncol_off=108;
const int ncol_wdt=18;
const int vcol_off=135;
const int vcol_wdt=9;


int line_onscr=25;
const int line_hgt=15;

int no_click;//задержка обработки кликов по форме после всяких диалогов открытия-сохранения

bool form_focus;

bool period_linear;

struct ayfxCell {
	int tone;
	int noise;
	int volume;
	bool t;
	bool n;
	bool sel;
};

#define max_fx_len	0x1000
#define max_fx_all	0x100


ayfxCell ayClipboard[max_fx_len];

struct ayfxBankCell {
	ayfxCell ayEffect[max_fx_len];
	char name[256];
};

ayfxBankCell ayBank[max_fx_all];



int curOffset;
int curPrevLine;
int curXpos;
int curYpos;
int clipLen;
POINT mousePrev;

int curEffect;   //текущий редактируемый эффект
int allEffect; //кол-во эффектов в банке всего



#include "rdwrnums.h"
#include "wav_render.h"



int PeriodToWidth(int period)
{
	if(period_linear)
	{
		return 1+tone_wdt*period/4096;
	}
	else
	{
		if(period<=0) return 0;
		return (int)float(float(tone_wdt)*log(period/8.)/log(4095./8.));
	}
}


int WidthToPeriod(int wdt)
{
	if(period_linear)
	{
		return wdt*4096/tone_wdt;
	}
	else
	{
		return (int)float(8.*exp(float(1+wdt)/(float(tone_wdt)/log(4095./8.))));
	}
}


bool SetBool(bool &var,bool val)
{
	if(var!=val)
	{
		var=val;
		return true;
	}
	return false;
}


bool SetInt(int &var,int val)
{
	if(var!=val)
	{
		var=val;
		return true;
	}
	return false;
}



AnsiString NameTrimPath(AnsiString name)
{
	name=name.Delete(1,name.LastDelimiter("\\/"));
	name=name.Delete(name.LastDelimiter("."),name.Length());
	return name;
}



void CenterView(void)
{
	if(curYpos<curOffset||curYpos>=curOffset+line_onscr)
	{
		curOffset=curYpos-line_onscr/2;
		if(curOffset<0) curOffset=0;
		if(curOffset>max_fx_len-line_onscr) curOffset=max_fx_len-line_onscr;
	}
}



int EffectRealLen(int fxn)
{
	int aa,len;
	
	len=0;
	for(aa=max_fx_len-1;aa>=0;aa--)
	{
		if(ayBank[fxn].ayEffect[aa].volume>0)
		{
			len=aa+1;
			break;
		}
	}
	return len;
}



void EffectDefaultStr(ayfxCell &cell)
{
	cell.tone=0;
	cell.noise=0;
	cell.volume=0;
	cell.t=false;
	cell.n=false;
	cell.sel=false;
}



void EffectInit(int fxn)
{
	int aa;
	for(aa=0;aa<max_fx_len;aa++) EffectDefaultStr(ayBank[fxn].ayEffect[aa]);
	curOffset=0;
	curPrevLine=-1;
	curXpos=0;
	curYpos=0;
}



int EffectDecode(int fxn,unsigned char *buf,int size)
{
	ayfxCell *afx;
	int it,pp,pd,tone,noise;
	
	afx=ayBank[fxn].ayEffect;
	
	EffectInit(fxn);
	
	pd=0;
	pp=0;
	
	while(pp<size)
	{
		it=buf[pp++];
		
		if(it&(1<<5))
		{
			tone=RdWordLH(&buf[pp])&0xfff;
			pp+=2;
		}
		if(it&(1<<6))
		{
			noise=buf[pp++];
			if(it==0xd0&&noise>=0x20) break;
			noise&=0x1f;
		}
		
		afx[pd].tone=tone;
		afx[pd].noise=noise;
		afx[pd].volume=it&0x0f;
		afx[pd].t=((it&(1<<4))?false:true);
		afx[pd].n=((it&(1<<7))?false:true);
		pd++;
	}
	
	return pp;
}



bool EffectLoad(int fxn,const char *filename)
{
	
	FILE *file;
	unsigned char *buf;
	int size;
	
	file=fopen(filename,"rb");
	if(!file) return false;
	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);
	buf=(unsigned char*)malloc(size);
	fread(buf,size,1,file);
	fclose(file);
	
	EffectDecode(fxn,buf,size);
	strcpy(ayBank[fxn].name,NameTrimPath(filename).c_str());
	
	free(buf);
	
	return true;
}



int EffectEncode(int fxn,unsigned char *buf)
{
	int aa,it,pp,fx_len,tone,noise;
	ayfxCell *afx;


	fx_len=EffectRealLen(fxn);
	tone=-1;
	noise=-1;
	pp=0;
	afx=ayBank[fxn].ayEffect;

	for(aa=0;aa<fx_len;aa++)
	{
		it =(afx[aa].volume&0x0f);
		it|=(afx[aa].t?0:(1<<4));
		it|=(afx[aa].n?0:(1<<7));
		if(afx[aa].tone!=tone)
		{
			tone=afx[aa].tone;
			it|=(1<<5);
		}
		if(afx[aa].noise!=noise)
		{
			noise=afx[aa].noise;
			it|=(1<<6);
		}
		buf[pp++]=it;
		if(it&(1<<5))
		{
			WrWordLH(&buf[pp],tone&0xfff);
			pp+=2;
		}
		if(it&(1<<6))
		{
			buf[pp++]=noise&0x1f;
		}
	}

	buf[pp++]=0xd0;//конечный маркер
	buf[pp++]=0x20;

	return pp;
}



bool EffectSave(int fxn,const char *filename)
{
	unsigned char *buf;
	FILE *file;
	int len;
	
	buf=(unsigned char*)malloc(max_fx_len*4);
	
	len=EffectEncode(fxn,buf);
	
	file=fopen(filename,"wb");
	if(!file)
	{
		free(buf);
		return false;
	}
	fwrite(buf,len,1,file);
	fclose(file);
	free(buf);
	return true;
}



bool EffectDeleteSel(int fxn)
{
	int aa,bb;
	bool done,del;
	
	done=false;
	del=false;
	
	while(!done)
	{
		done=true;
		for(aa=0;aa<max_fx_len;aa++)
		{
			if(ayBank[fxn].ayEffect[aa].sel)
			{
				for(bb=aa;bb<max_fx_len-1;bb++) ayBank[fxn].ayEffect[bb]=ayBank[fxn].ayEffect[bb+1];
				EffectDefaultStr(ayBank[fxn].ayEffect[max_fx_len-1]);
				done=false;
				del=true;
				break;
			}
		}
	}
	CenterView();
	return del;
}



void EffectInsertStr(int fxn,int pos,bool clr)
{
	int aa;
	for(aa=max_fx_len-1;aa>pos;aa--) ayBank[fxn].ayEffect[aa]=ayBank[fxn].ayEffect[aa-1];
	if(clr) EffectDefaultStr(ayBank[fxn].ayEffect[pos]);
}



void EffectCopy(int fxn,bool res)
{
	int aa;
	clipLen=0;
	
	for(aa=0;aa<max_fx_len;aa++)
	{
		if(ayBank[fxn].ayEffect[aa].sel)
		{
			if(res) ayBank[fxn].ayEffect[aa].sel=false;
			ayClipboard[clipLen]=ayBank[fxn].ayEffect[aa];
			ayClipboard[clipLen].sel=false;
			clipLen++;
		}
	}
	if(clipLen==0)
	{
		ayClipboard[0]=ayBank[fxn].ayEffect[curYpos];
		clipLen=1;
	}
}



void EffectCut(int fxn)
{
	EffectCopy(fxn,false);
	if(!EffectDeleteSel(fxn))
	{
		ayBank[fxn].ayEffect[curYpos].sel=true;
		EffectDeleteSel(fxn);
	}
}



void EffectPaste(int fxn)
{
	int aa,len;
	if(clipLen<=0) return;
	for(aa=0;aa<clipLen;aa++) EffectInsertStr(fxn,curYpos,true);
	len=clipLen;
	if(curYpos+len>=max_fx_len) len=max_fx_len-curYpos;
	for(aa=0;aa<len;aa++) ayBank[fxn].ayEffect[curYpos+aa]=ayClipboard[aa];
}



void __fastcall TFormMain::TitleUpdate(void)
{
	Caption=(AnsiString)TITLE_STR+" ["+NameTrimPath(SaveDialogBank->FileName)+".afb]";
}




void __fastcall TFormMain::DrawEdit(bool full)
{
	int aa,x,xo,y,fwdt,pp,rl;
	AnsiString str;
	TRect rect;
	TColor back,lcol,fcol;
	
	Canvas->Brush->Color=clWhite;
	Canvas->Font->Color=clBlack;
	Canvas->Pen->Color=clBlack;
	
	y=edit_yoff;
	pp=curOffset;
	rl=EffectRealLen(curEffect);
	
	if(full)
	{
		if(period_linear)
		{
			Canvas->TextOut(edit_xoff,y,"Pos|T|N|Per|Ns|V|Period (linear)                     |Noise  |Volume ");
		}
		else
		{
			Canvas->TextOut(edit_xoff,y,"Pos|T|N|Per|Ns|V|Period (log)                        |Noise  |Volume ");
		}
	}
	y+=edit_head;
	
	for(aa=0;aa<line_onscr;aa++)
	{
		if(ayBank[curEffect].ayEffect[pp].sel) back=(TColor)0xc0ffc0; else back=clWhite;
		if(pp<rl)
		{
			fcol=clBlack;
			lcol=clGray;
		}
		else
		{
			fcol=clGray;
			lcol=clSilver;
		}
		Canvas->Font->Color=fcol;
		Canvas->Pen->Color=fcol;
		Canvas->Brush->Color=back;
		Canvas->TextOut(edit_xoff,y,IntToHex(pp,3));
		str=(ayBank[curEffect].ayEffect[pp].t?"T":"-");
		Canvas->TextOut(edit_xoff+36,y,str);
		str=(ayBank[curEffect].ayEffect[pp].n?"N":"-");
		Canvas->TextOut(edit_xoff+54,y,str);
		
		if(curXpos==0&&pp==curYpos)
		{
			Canvas->Brush->Color=clBlack;
			Canvas->Font->Color=clWhite;
		}
		else
		{
			Canvas->Brush->Color=back;
			Canvas->Font->Color=fcol;
		}
		Canvas->TextOut(edit_xoff+72,y,IntToHex(ayBank[curEffect].ayEffect[pp].tone,3));
		if(curXpos==1&&pp==curYpos)
		{
			Canvas->Brush->Color=clBlack;
			Canvas->Font->Color=clWhite;
		}
		else
		{
			Canvas->Brush->Color=back;
			Canvas->Font->Color=fcol;
		}
		Canvas->TextOut(edit_xoff+108,y,IntToHex(ayBank[curEffect].ayEffect[pp].noise,2));
		if(curXpos==2&&pp==curYpos)
		{
			Canvas->Brush->Color=clBlack;
			Canvas->Font->Color=clWhite;
		}
		else
		{
			Canvas->Brush->Color=back;
			Canvas->Font->Color=fcol;
		}
		Canvas->TextOut(edit_xoff+135,y,IntToHex(ayBank[curEffect].ayEffect[pp].volume,1));
		
		x=edit_xoff+tone_off;
		fwdt=PeriodToWidth(ayBank[curEffect].ayEffect[pp].tone);//*tone_wdt/4095);
		if(fwdt<2&&ayBank[curEffect].ayEffect[pp].tone>0) fwdt=2;
		Canvas->Brush->Color=back;
		xo=x+fwdt-1;
		if(xo<x) xo=x;
		Canvas->Rectangle(xo,y+1,x+tone_wdt,y+line_hgt-1);
		Canvas->Brush->Color=lcol;
		Canvas->Rectangle(x,y+1,x+fwdt,y+line_hgt-1);
		
		x=edit_xoff+noise_off;
		fwdt=ayBank[curEffect].ayEffect[pp].noise*noise_wdt/31;
		if(fwdt<2&&ayBank[curEffect].ayEffect[pp].noise>0) fwdt=2;
		Canvas->Brush->Color=back;
		xo=x+fwdt-1;
		if(xo<x) xo=x;
		Canvas->Rectangle(xo,y+1,x+noise_wdt,y+line_hgt-1);
		Canvas->Brush->Color=lcol;
		Canvas->Rectangle(x,y+1,x+fwdt,y+line_hgt-1);
		
		x=edit_xoff+vol_off;
		fwdt=ayBank[curEffect].ayEffect[pp].volume*vol_wdt/15;
		if(fwdt<2&&ayBank[curEffect].ayEffect[pp].volume>0) fwdt=2;
		Canvas->Brush->Color=back;
		xo=x+fwdt-1;
		if(xo<x) xo=x;
		Canvas->Rectangle(xo,y+1,x+vol_wdt,y+line_hgt-1);
		Canvas->Brush->Color=lcol;
		Canvas->Rectangle(x,y+1,x+fwdt,y+line_hgt-1);
		
		y+=line_hgt;
		pp++;
	}
}



bool MouseCheck(int x,int y,TShiftState state,bool hold)
{
	int ln,temp;
	bool l,r,mkey;

	if(state.Contains(ssLeft))  l=true; else l=false;
	if(state.Contains(ssRight)) r=true; else r=false;
	
	if(l||r) mkey=true; else mkey=false;

	if(!form_focus) return false;
	
	if(y>=edit_yoff+edit_head&&y<edit_yoff+edit_head+line_onscr*line_hgt)
	{
		ln=(y-edit_yoff-edit_head)/line_hgt+curOffset;
		if(ln<0) ln=0;
		if(ln>=max_fx_len) ln=max_fx_len-1;
		
		if(mkey)
		{
			if(x>=edit_xoff+tone_off&&x<=edit_xoff+tone_off+tone_wdt)
			{
				temp=WidthToPeriod(x-edit_xoff-tone_off);
				if(temp>4095) temp=4095;
				if(SetInt(ayBank[curEffect].ayEffect[ln].tone,temp)) return true;
			}
			if(x>=edit_xoff+tone_off-2&&x<edit_xoff+tone_off)
			{
				if(SetInt(ayBank[curEffect].ayEffect[ln].tone,0)) return true;
			}
			if(x>=edit_xoff+noise_off&&x<=edit_xoff+noise_off+noise_wdt)
			{
				temp=1+(x-edit_xoff-noise_off)*31/noise_wdt;
				if(temp>31) temp=31;
				if(SetInt(ayBank[curEffect].ayEffect[ln].noise,temp)) return true;
			}
			if(x>=edit_xoff+noise_off-2&&x<edit_xoff+noise_off)
			{
				if(SetInt(ayBank[curEffect].ayEffect[ln].noise,0)) return true;
			}
			if(x>=edit_xoff+vol_off&&x<=edit_xoff+vol_off+vol_wdt)
			{
				temp=1+(x-edit_xoff-vol_off)*15/vol_wdt;
				if(temp>15) temp=15;
				if(SetInt(ayBank[curEffect].ayEffect[ln].volume,temp)) return true;
			}
			if(x>=edit_xoff+vol_off-2&&x<edit_xoff+vol_off)
			{
				if(SetInt(ayBank[curEffect].ayEffect[ln].volume,0)) return true;
			}
		}
		
		if(x>=edit_xoff+tflag_off&&x<=edit_xoff+tflag_off+tflag_wdt)
		{
			if(l)
			{
				if(SetBool(ayBank[curEffect].ayEffect[ln].t,true)) return true;
			}
			if(r)
			{
				if(SetBool(ayBank[curEffect].ayEffect[ln].t,false)) return true;
			}
		}
		if(x>=edit_xoff+nflag_off&&x<=edit_xoff+nflag_off+nflag_wdt)
		{
			if(l)
			{
				if(SetBool(ayBank[curEffect].ayEffect[ln].n,true)) return true;
			}
			if(r)
			{
				if(SetBool(ayBank[curEffect].ayEffect[ln].n,false)) return true;
			}
		}
		
		if(curPrevLine!=ln||!hold)
		{
			curPrevLine=ln;
			
			if(x>=edit_xoff&&x<=edit_xoff+pos_wdt)
			{
				if(l)
				{
					if(SetBool(ayBank[curEffect].ayEffect[ln].sel,true)) return true;
				}

				if(r)
				{
					if(SetBool(ayBank[curEffect].ayEffect[ln].sel,false)) return true;
				}
			}
		}
	}
	
	return false;
}



void EffectMakeWAV(int fxn,int off)
{
	int aa,pp,icnt,frames,slen,flen,ifrq;
	short int *buf;
	
	if(wave)
	{
		free(wave);
		wave=NULL;
	}
	
	ifrq=MIX_RATE/50;
	frames=EffectRealLen(fxn)+3-off;//плюс, чтобы точно отзвучало полностью
	slen=ifrq*frames;
	
	flen=slen*2+44;
	wave_size=flen;
	wave=(unsigned char*)malloc(flen);
	
	memcpy(wave,"RIFF",4);//id riff-файла
	WrDWordLH(&wave[4],flen-8);//размер файла минус 8
	memcpy(&wave[8],"WAVEfmt ",8);//id формата
	WrDWordLH(&wave[16],16);//размер заголовка
	WrWordLH(&wave[20],1);//формат PCM
	WrWordLH(&wave[22],1);//моно
	WrDWordLH(&wave[24],MIX_RATE);//частота дискретизации
	WrDWordLH(&wave[28],MIX_RATE*2);//byterate (mixrate*channels*bytespersample)
	WrDWordLH(&wave[32],2);//кол-во байт на сэмпл (channels*bytespersample)
	WrDWordLH(&wave[34],16);//кол-во бит на один сэмпл
	memcpy(&wave[36],"data",4);//id данных
	WrDWordLH(&wave[40],slen*2);//размер данных в байтах
	pp=44;
	
	buf=(short int*)(wave+pp);//оффсет после заголовка
	
	icnt=0;
	ay_init(&ayChip);
	pp=off;

	for(aa=0;aa<slen;aa++)
	{
		ay_tick(&ayChip,(AY_CLOCK/8/MIX_RATE));

		if(icnt++>=ifrq)
		{
			icnt=0;

			if(pp<max_fx_len)
			{
				ay_out(&ayChip,0,ayBank[fxn].ayEffect[pp].tone&255);
				ay_out(&ayChip,1,ayBank[fxn].ayEffect[pp].tone>>8);
				ay_out(&ayChip,6,ayBank[fxn].ayEffect[pp].noise);
				ay_out(&ayChip,7,0xf6|(ayBank[fxn].ayEffect[pp].t?0:1)|(ayBank[fxn].ayEffect[pp].n?0:8));
				ay_out(&ayChip,8,ayBank[fxn].ayEffect[pp].volume);
			}

			pp++;
		}

		buf[aa]=ayChip.out/(AY_CLOCK/8/MIX_RATE);
	}
}



void EffectPlay(int off)
{
	PlaySound(NULL,NULL,NULL);
	EffectMakeWAV(curEffect,off);
	PlaySound(wave,NULL,SND_MEMORY|SND_ASYNC);
}



void EffectStop(void)
{
	PlaySound(NULL,NULL,NULL);

	if(wave)
	{
		free(wave);
		wave=NULL;
		wave_size=0;
	}
}



bool EffectExportWAV(int effect,const char *filename)
{
	FILE *file;
	
	EffectStop();

	EffectMakeWAV(effect,0);

	if(wave_size>0)
	{
		file=fopen(filename,"wb");
		if(file)
		{
			fwrite(wave,wave_size,1,file);
			fclose(file);
			return true;
		}
	}

	return false;
}



void __fastcall TFormMain::HoldCheck(int X,int Y, bool force)
{
	bool hold;
	int xmin,xmax;
	RECT clip;
	TPoint tpos;

	hold=false;
	
	if(Y<edit_head+edit_yoff||Y>=edit_head+edit_yoff+line_onscr*line_hgt) return;

	if(X>=edit_xoff&&X<=edit_xoff+pos_wdt)
	{
		xmin=edit_xoff;
		xmax=edit_xoff+pos_wdt;
		hold=true;
	}

	if(X>=edit_xoff+tflag_off&&X<=edit_xoff+tflag_off+tflag_wdt)
	{
		xmin=edit_xoff+tflag_off;
		xmax=edit_xoff+tflag_off+tflag_wdt;
		hold=true;
	}

	if(X>=edit_xoff+nflag_off&&X<=edit_xoff+nflag_off+nflag_wdt)
	{
		xmin=edit_xoff+nflag_off;
		xmax=edit_xoff+nflag_off+nflag_wdt;
		hold=true;
	}

	if(X>=edit_xoff+tone_off-2&&X<=edit_xoff+tone_off+tone_wdt)
	{
		xmin=edit_xoff+tone_off-2;
		xmax=edit_xoff+tone_off+tone_wdt;
		hold=true;
	}

	if(X>=edit_xoff+noise_off-2&&X<=edit_xoff+noise_off+noise_wdt)
	{
		xmin=edit_xoff+noise_off-2;
		xmax=edit_xoff+noise_off+noise_wdt;
		hold=true;
	}

	if(X>=edit_xoff+vol_off-2&&X<=edit_xoff+vol_off+vol_wdt)
	{
		xmin=edit_xoff+vol_off-2;
		xmax=edit_xoff+vol_off+vol_wdt;
		hold=true;
	}

	if(force)
	{
		xmin=mousePrev.x;
		xmax=xmin;
	}

	if(hold)
	{
		tpos.x=0;
		tpos.y=0;

		tpos=ClientToScreen(tpos);

		clip.left=tpos.x+xmin;
		clip.right=tpos.x+xmax+1;
		clip.top=tpos.y+edit_yoff+edit_head;
		clip.bottom=tpos.y+edit_yoff+edit_head+line_onscr*line_hgt;

		ClipCursor(&clip);
	}
}


void HoldRelease(void)
{
	ClipCursor(NULL);
}



void BankInit(void)
{
	int aa,bb;
	char name[16];
	for(aa=0;aa<max_fx_all;aa++)
	{
		for(bb=0;bb<max_fx_len;bb++) EffectDefaultStr(ayBank[aa].ayEffect[bb]);
		sprintf(name,"noname%3.3i",aa+1);
		strcpy(ayBank[aa].name,name);
	}
	curEffect=0;
	allEffect=1;
}


void __fastcall TFormMain::BankUpdateCount(void)
{
	char str[16];
	sprintf(str,"%3.3i/%3.3i",curEffect+1,allEffect);
	EditEffectNum->Text=str;
	EditEffectName->Text=ayBank[curEffect].name;
}



bool BankSave(const char *filename,bool names)
{
	FILE *file;
	unsigned char *buf;
	int aa,size,pp,off,len;
	
	size=max_fx_len*allEffect+260*allEffect;
	buf=(unsigned char*)malloc(size);
	
	buf[0]=allEffect&0xff;
	pp=2*allEffect+1;
	
	for(aa=0;aa<allEffect;aa++)
	{
		off=pp-aa*2-2;
		WrWordLH(&buf[1+aa*2],off);
		len=EffectEncode(aa,&buf[pp]);
		pp+=len;
		
		if(names&&ayBank[aa].name[0]!=0)
		{
			strcpy(&buf[pp],ayBank[aa].name);
			pp+=strlen(ayBank[aa].name);
			buf[pp++]=0;
		}
	}
	
	size=pp;
	
	file=fopen(filename,"wb");
	if(!file)
	{
		free(buf);
		return false;
	}
	fwrite(buf,size,1,file);
	fclose(file);
	free(buf);
	return true;
}



bool BankLoad(const char *filename)
{
	FILE *file;
	unsigned char *buf;
	char name[16];
	int aa,size,len,rlen,off;
	
	file=fopen(filename,"rb");
	if(!file) return false;
	fseek(file,0,SEEK_END);
	size=ftell(file);
	fseek(file,0,SEEK_SET);
	buf=(unsigned char*)malloc(size);
	fread(buf,size,1,file);
	fclose(file);
	
	BankInit();
	
	allEffect=buf[0];
	
	for(aa=0;aa<allEffect;aa++)
	{
		off=RdWordLH(&buf[1+aa*2])+2+aa*2;
		if(aa<allEffect-1)
		{
			len=RdWordLH(&buf[3+aa*2])+4+aa*2-off;
		}
		else
		{
			len=size-off;
		}
		rlen=EffectDecode(aa,&buf[off],len);
		
		if(rlen!=len)
		{
			strcpy(ayBank[aa].name,&buf[off+rlen]);
		}
		else
		{
			sprintf(name,"noname%3.3i",aa+1);
			strcpy(ayBank[aa].name,name);
		}
	}
	
	free(buf);
	
	return true;
}



bool BankAddEffect(void)
{
	if(allEffect>=256) return false;
	EffectInit(allEffect);
	allEffect++;
	return true;
}



bool BankDeleteEffect(int fxn)
{
	int aa;
	if(allEffect==1)
	{
		EffectInit(fxn);
		return true;
	}
	for(aa=fxn;aa<max_fx_all-1;aa++) ayBank[aa]=ayBank[aa+1];
	EffectInit(max_fx_all-1);
	allEffect--;
	return true;
}



bool BankInsertEffect(int fxn)
{
	int aa;
	char str[256];
	if(allEffect>=256) return false;
	for(aa=max_fx_all-1;aa>fxn;aa--) ayBank[aa]=ayBank[aa-1];
	EffectInit(fxn);
	sprintf(str,"inserted%3.3i",fxn);
	strcpy(ayBank[fxn].name,str);
	allEffect++;
	return true;
}



void __fastcall TFormMain::EffectSwitchEnable(void)
{
	SpeedButtonPrev->Enabled=(curEffect>0)?true:false;
	SpeedButtonNext->Enabled=(curEffect<allEffect-1)?true:false;
	SpeedButtonFirst->Enabled=(curEffect==0)?false:true;
	SpeedButtonLast->Enabled=(curEffect<allEffect-1)?true:false;
}



void __fastcall TFormMain::EnterFromPiano(int div,bool st,int vol,int step,int fill)
{
	int aa;
	
	for(aa=0;aa<fill;aa++)
	{
		if(curYpos+aa>=max_fx_len) break;
		ayBank[curEffect].ayEffect[curYpos+aa].tone=div;
		if(st) ayBank[curEffect].ayEffect[curYpos+aa].t=true;
		if(vol>=0) ayBank[curEffect].ayEffect[curYpos+aa].volume=vol;
	}
	
	for(aa=0;aa<step;aa++)
	{
		if(curYpos<max_fx_len-1)
		{
			curYpos++;
			if(curYpos>=curOffset+line_onscr)
			{
				curOffset++;
				if(curOffset>max_fx_len-line_onscr) curOffset=max_fx_len-line_onscr;
			}
		}
	}
	DrawEdit(false);
}



AnsiString __fastcall TFormMain::GetEffectFileName(int id)
{
	char temp[256];
	AnsiString name;
	int i;

	strcpy(temp,ayBank[id].name);

	for(i=0;i<(int)strlen(temp);++i)
	{
		if(temp[i]=='\\'||temp[i]=='/'||temp[i]==':'||temp[i]=='*'||temp[i]=='?'||temp[i]=='<'||temp[i]=='>'||temp[i]=='|') temp[i]='_';
	}

	name=temp;

	return name;
}



void __fastcall TFormMain::ColumnAdjust(int delta)
{
	int row;

	for(row=0;row<max_fx_len;++row)
	{
		if(!ayBank[curEffect].ayEffect[row].sel) continue;

		switch(curXpos)
		{
		case 0:
			ayBank[curEffect].ayEffect[row].tone+=delta;
			if(ayBank[curEffect].ayEffect[row].tone<0) ayBank[curEffect].ayEffect[row].tone=0;
			if(ayBank[curEffect].ayEffect[row].tone>4095) ayBank[curEffect].ayEffect[row].tone=4095;
			break;

		case 1:
			ayBank[curEffect].ayEffect[row].noise+=delta;
			if(ayBank[curEffect].ayEffect[row].noise<0) ayBank[curEffect].ayEffect[row].noise=0;
			if(ayBank[curEffect].ayEffect[row].noise>31) ayBank[curEffect].ayEffect[row].noise=31;
			break;

		case 2:
			ayBank[curEffect].ayEffect[row].volume+=delta;
			if(ayBank[curEffect].ayEffect[row].volume<0) ayBank[curEffect].ayEffect[row].volume=0;
			if(ayBank[curEffect].ayEffect[row].volume>15) ayBank[curEffect].ayEffect[row].volume=15;
			break;
		}
	}

	DrawEdit(true);
}



#include "import_wav.h"
#include "import_psg.h"
#include "import_vtx.h"
#include "import_vgm.h"

#include "export_vt2.h"
#include "export_csv.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormMain *FormMain;
//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MFileExitClick(TObject *Sender)
{
	//надо-бы спрашивать, хочет-ли юзер закрыть
	Close();
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::FormPaint(TObject *Sender)
{
	DrawEdit(true);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::FormMouseDown(TObject *Sender, TMouseButton Button,
TShiftState Shift, int X, int Y)
{
	bool upd;
	
	if(no_click) return;
	
	mousePrev.y=Y;
	mousePrev.x=X;
	
	if(Shift.Contains(ssLeft)||Shift.Contains(ssRight)) HoldCheck(X,Y,false);

	if(MouseCheck(X,Y,Shift,false)) DrawEdit(false);
	
	if(Y>=edit_yoff+edit_head&&Y<edit_yoff+edit_head+line_onscr*line_hgt)
	{
		upd=false;

		if(X>=edit_xoff+tcol_off&&X<=edit_xoff+tcol_off+tcol_wdt)
		{
			curXpos=0;
			upd=true;
		}

		if(X>=edit_xoff+ncol_off&&X<=edit_xoff+ncol_off+ncol_wdt)
		{
			curXpos=1;
			upd=true;
		}

		if(X>=edit_xoff+vcol_off&&X<=edit_xoff+vcol_off+vcol_wdt)
		{
			curXpos=2;
			upd=true;
		}

		if(upd)
		{
			curYpos=curOffset+(Y-edit_yoff-edit_head)/line_hgt;
			DrawEdit(false);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormMouseMove(TObject *Sender, TShiftState Shift,
int X, int Y)
{
	bool redraw,force;
	int delta,yy,ys;
	float xx,xs;
	
	if(no_click) return;

	if(Shift.Contains(ssCtrl))
	{
		X=mousePrev.x;
		force=true;
	}
	else
	{
		force=false;
	}

	if(Shift.Contains(ssLeft)||Shift.Contains(ssRight)) HoldCheck(X,Y,force);

	delta=labs(mousePrev.y-Y);
	
	if(mousePrev.y>=0&&delta>=line_hgt)
	{
		redraw=false;
		yy=mousePrev.y;
		xx=float(mousePrev.x);
		xs=(float(X)-xx)/float(delta);

		if(mousePrev.y<Y) ys=1; else ys=0-1;

		while(delta>=0)
		{
			if(MouseCheck(int(xx),yy,Shift,true)) redraw=true;
			yy+=ys;
			xx+=xs;
			delta-=1;
		}
		
		if(redraw) DrawEdit(false);
	}
	else
	{
		if(MouseCheck(X,Y,Shift,true)) DrawEdit(false);
	}
	
	mousePrev.y=Y;
	mousePrev.x=X;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormKeyDown(TObject *Sender, WORD &Key,
TShiftState Shift)
{
	int aa,len,num,nval;
	bool draw;

	draw=false;
	
	if(Shift.Contains(ssCtrl))
	{
		switch(Key)
		{
		case 'A':
			len=EffectRealLen(curEffect);
			for(aa=0;aa<len;aa++) ayBank[curEffect].ayEffect[aa].sel=true;
			draw=true;
			break;
		case 'I':
			len=EffectRealLen(curEffect);
			for(aa=0;aa<len;aa++) ayBank[curEffect].ayEffect[aa].sel=!ayBank[curEffect].ayEffect[aa].sel;
			draw=true;
			break;
		case 'Z':
			//undo
			return;
		case 'X':
			EffectCut(curEffect);
			draw=true;
			break;
		case 'C':
			EffectCopy(curEffect,true);
			draw=true;
			break;
		case 'V':
			EffectPaste(curEffect);
			draw=true;
			break;
		}

		if(draw)
		{
			DrawEdit(false);
			return;
		}
	}
	
	num=-1;

	if(Key>='0'&&Key<='9') num=Key-'0';
	if(Key>='A'&&Key<='F') num=Key-'A'+10;
	
	if(num>=0)
	{
		switch(curXpos)
		{
		case 0:
			ayBank[curEffect].ayEffect[curYpos].tone=((ayBank[curEffect].ayEffect[curYpos].tone<<4)|num)&0xfff;
			break;
		case 1:
			nval=((ayBank[curEffect].ayEffect[curYpos].noise<<4)|num)&0xff;
			if(nval<=0x1f) ayBank[curEffect].ayEffect[curYpos].noise=nval; else ayBank[curEffect].ayEffect[curYpos].noise=num;
			break;
		case 2:
			ayBank[curEffect].ayEffect[curYpos].volume=num;
			break;
		}

		DrawEdit(false);
		return;
	}
	
	switch(Key)
	{
	case VK_ADD:
	case VK_OEM_PLUS:
		if(!Shift.Contains(ssShift))
		{
			SpeedButtonNextClick(Sender);
		}
		else
		{
			ColumnAdjust(1);
		}
		return;

	case VK_SUBTRACT:
	case VK_OEM_MINUS:
		if(!Shift.Contains(ssShift))
		{
			SpeedButtonPrevClick(Sender);
		}
		else
		{
			ColumnAdjust(-1);
		}
		return;
		
	case VK_UP:
		if(curYpos>0)
		{
			curYpos--;
			if(curYpos<curOffset)
			{
				curOffset--;
				if(curOffset<0) curOffset=0;
			}
			draw=true;
		}
		break;

	case VK_DOWN:
		if(curYpos<max_fx_len-1)
		{
			curYpos++;
			if(curYpos>=curOffset+line_onscr)
			{
				curOffset++;
				if(curOffset>max_fx_len-line_onscr) curOffset=max_fx_len-line_onscr;
			}
			draw=true;
		}
		break;

	case VK_LEFT:
		curXpos--;
		if(curXpos<0) curXpos=2;
		draw=true;
		break;

	case VK_RIGHT:
		curXpos++;
		if(curXpos>2) curXpos=0;
		draw=true;
		break;

	case VK_PRIOR:
		curYpos-=line_onscr;
		if(curYpos<0) curYpos=0;
		curOffset-=line_onscr;
		if(curOffset<0) curOffset=0;
		draw=true;
		break;

	case VK_NEXT:
		curYpos+=line_onscr;
		if(curYpos>=max_fx_len) curYpos=max_fx_len-1;
		curOffset+=line_onscr;
		if(curOffset>max_fx_len-line_onscr) curOffset=max_fx_len-line_onscr;
		draw=true;
		break;

	case VK_HOME:
		if(curYpos>0)
		{
			curYpos=0;
			CenterView();
			draw=true;
		}
		break;

	case VK_END:
		curYpos=EffectRealLen(curEffect)-1;
		if(curYpos<0) curYpos=0;
		CenterView();
		draw=true;
		break;

	case VK_RETURN:
		if(Shift.Contains(ssCtrl))
		{
			EffectPlay(curYpos);
		}
		else
		{
			EffectPlay(0);
		}
		break;

	case VK_SPACE:
		EffectStop();
		break;

	case VK_OEM_3:	//tilde
		MViewPianoInputClick(Sender);
		break;

	case VK_DELETE:
		if(!EffectDeleteSel(curEffect))
		{
			ayBank[curEffect].ayEffect[curYpos].sel=true;
			EffectDeleteSel(curEffect);
		}
		draw=true;
		break;

	case VK_INSERT:
		if(Shift.Contains(ssCtrl))
		{
			EffectInsertStr(curEffect,curYpos,false);
		}
		else
		{
			EffectInsertStr(curEffect,curYpos,true);
		}
		draw=true;
		break;

	case 'T':
		ayBank[curEffect].ayEffect[curYpos].t=!ayBank[curEffect].ayEffect[curYpos].t;
		draw=true;
		break;

	case 'N':
		ayBank[curEffect].ayEffect[curYpos].n=!ayBank[curEffect].ayEffect[curYpos].n;
		draw=true;
		break;
	}

	if(Shift.Contains(ssShift))
	{
		if(SetBool(ayBank[curEffect].ayEffect[curYpos].sel,true)) draw=true;
	}

	if(draw) DrawEdit(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MFileEffectClearClick(TObject *Sender)
{
	EffectInit(curEffect);
	DrawEdit(false);
	BankUpdateCount();
	no_click=50;
}
//---------------------------------------------------------------------------

int window_border;

void __fastcall TFormMain::FormCreate(TObject *Sender)
{
	int aa;
	
	mousePrev.y=-1;
	mousePrev.x=-1;
	BankInit();
	EffectInit(curEffect);
	TitleUpdate();
	no_click=0;
	window_border=Height-ClientHeight;
	MViewLinearPeriodClick(Sender);
	for(aa=0;aa<max_fx_len;aa++) EffectDefaultStr(ayClipboard[aa]);
	clipLen=0;
	BankUpdateCount();
	EffectSwitchEnable();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonPlayClick(TObject *Sender)
{
	EffectPlay(0);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonStopClick(TObject *Sender)
{
	EffectStop();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MFileEffectLoadClick(TObject *Sender)
{
	if(OpenDialogEffect->Execute())
	{
		if(EffectLoad(curEffect,OpenDialogEffect->FileName.c_str()))
		{
			BankUpdateCount();
			DrawEdit(false);
			no_click=50;
		}
		else
		{
			Application->MessageBox("Can't load effect!","Error",MB_OK);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MFileEffectSaveClick(TObject *Sender)
{
	if(EffectRealLen(curEffect)==0)
	{
		Application->MessageBox("Effect is empty, nothing to save.","Save effect",MB_OK);
	}
	else
	{
		SaveDialogEffect->FileName=AnsiString(ayBank[curEffect].name)+".afx";
		if(SaveDialogEffect->Execute())
		{
			EffectSave(curEffect,SaveDialogEffect->FileName.c_str());
			no_click=50;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormMouseWheelDown(TObject *Sender,
TShiftState Shift, TPoint &MousePos, bool &Handled)
{
	if(curYpos<max_fx_len-1)
	{
		curYpos+=5;
		if(curYpos>=max_fx_len) curYpos=max_fx_len-1;
		if(curYpos>=curOffset+line_onscr)
		{
			curOffset=curYpos-line_onscr+1;
			if(curOffset>max_fx_len-line_onscr) curOffset=max_fx_len-line_onscr;
		}
		DrawEdit(false);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormMouseWheelUp(TObject *Sender,
TShiftState Shift, TPoint &MousePos, bool &Handled)
{
	if(curYpos>0)
	{
		curYpos-=5;
		if(curYpos<0) curYpos=0;
		if(curYpos<curOffset)
		{
			curOffset=curYpos;
		}
		DrawEdit(false);
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormResize(TObject *Sender)
{
	DrawEdit(true);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormCanResize(TObject *Sender, int &NewWidth,
int &NewHeight, bool &Resize)
{
	NewWidth=Width;
	if(NewHeight<200) NewHeight=200;
	NewHeight=(NewHeight-edit_yoff-edit_head-(window_border))/line_hgt*line_hgt+edit_yoff+edit_head+(window_border);
	line_onscr=(NewHeight-edit_yoff-edit_head-window_border)/line_hgt;
}
//---------------------------------------------------------------------------





void __fastcall TFormMain::MImportWAVClick(TObject *Sender)
{
	OpenDialogImport->Title="Load wave file";
	OpenDialogImport->DefaultExt="wav";
	OpenDialogImport->Filter="Windows sound file (*.wav)|*.wav";
	if(OpenDialogImport->Execute())
	{
		if(ImportWAV(curEffect,OpenDialogImport->FileName.c_str()))
		{
			SaveDialogEffect->FileName="imported.afx";
			TitleUpdate();
			BankUpdateCount();
			DrawEdit(false);
			no_click=50;
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormMouseUp(TObject *Sender, TMouseButton Button,
TShiftState Shift, int X, int Y)
{
	HoldRelease();
	mousePrev.y=-1;
	mousePrev.x=-1;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::Timer1Timer(TObject *Sender)
{
	if(no_click>0) no_click--;
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::MImportPSGClick(TObject *Sender)
{
	OpenDialogImport->Title="Load PSG file";
	OpenDialogImport->DefaultExt="psg";
	OpenDialogImport->Filter="PSG AY register dump (*.psg)|*.psg";
	if(OpenDialogImport->Execute())
	{
		FormAyChn->ShowModal();
		if(import_ok)
		{
			if(ImportPSG(curEffect,OpenDialogImport->FileName.c_str(),import_chan))
			{
				SaveDialogEffect->FileName="imported.afx";
				TitleUpdate();
				BankUpdateCount();
				DrawEdit(false);
				no_click=50;
			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MEditUnselectAllClick(TObject *Sender)
{
	int aa;
	for(aa=0;aa<max_fx_len;aa++) ayBank[curEffect].ayEffect[aa].sel=false;
	DrawEdit(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MEditSelectAllClick(TObject *Sender)
{
	int aa,len;
	len=EffectRealLen(curEffect);
	for(aa=0;aa<len;aa++) ayBank[curEffect].ayEffect[aa].sel=true;
	DrawEdit(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MEditInverseSelectionClick(TObject *Sender)
{
	int aa,len;
	len=EffectRealLen(curEffect);
	for(aa=0;aa<len;aa++) ayBank[curEffect].ayEffect[aa].sel=!ayBank[curEffect].ayEffect[aa].sel;
	DrawEdit(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MEditDeleteClick(TObject *Sender)
{
	if(!EffectDeleteSel(curEffect))
	{
		ayBank[curEffect].ayEffect[curYpos].sel=true;
		EffectDeleteSel(curEffect);
	}
	DrawEdit(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MViewLinearPeriodClick(TObject *Sender)
{
	MViewLinearPeriod->Checked=true;
	period_linear=true;
	DrawEdit(true);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MViewLogPeriodClick(TObject *Sender)
{
	MViewLogPeriod->Checked=true;
	period_linear=false;
	DrawEdit(true);
}
//---------------------------------------------------------------------------



void __fastcall TFormMain::FormActivate(TObject *Sender)
{
	form_focus=true;	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormDeactivate(TObject *Sender)
{
	form_focus=false;	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MEditCopyClick(TObject *Sender)
{
	EffectCopy(curEffect,true);
	DrawEdit(false);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MEditCutClick(TObject *Sender)
{
	EffectCut(curEffect);
	DrawEdit(false);	
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MEditPasteClick(TObject *Sender)
{
	EffectPaste(curEffect);
	DrawEdit(false);	
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::MImportVTXClick(TObject *Sender)
{
	OpenDialogImport->Title="Load VTX file";
	OpenDialogImport->DefaultExt="vtx";
	OpenDialogImport->Filter="VTX AY register dump (*.vtx)|*.vtx";
	if(OpenDialogImport->Execute())
	{
		FormAyChn->ShowModal();
		if(import_ok)
		{
			if(ImportVTX(curEffect,OpenDialogImport->FileName.c_str(),import_chan))
			{
				SaveDialogEffect->FileName="imported.afx";
				TitleUpdate();
				BankUpdateCount();
				DrawEdit(false);
				no_click=50;
			}
		}
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::MFileNewBankClick(TObject *Sender)
{
	BankInit();
	DrawEdit(true);
	BankUpdateCount();
	SaveDialogBank->FileName="noname.afb";
	TitleUpdate();
	no_click=50;
	EffectSwitchEnable();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonPrevClick(TObject *Sender)
{
	if(curEffect>0)
	{
		curEffect--;
		BankUpdateCount();
		DrawEdit(true);
		EffectSwitchEnable();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonNextClick(TObject *Sender)
{
	if(curEffect<allEffect-1)
	{
		curEffect++;
		BankUpdateCount();
		DrawEdit(true);
		EffectSwitchEnable();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MFileMultiLoadClick(TObject *Sender)
{
	int aa,pp;
	if(OpenDialogMulti->Execute())
	{
		pp=allEffect-1;
		if(EffectRealLen(pp)>0) pp+=1;
		
		for(aa=0;aa<OpenDialogMulti->Files->Count;aa++)
		{
			if(pp>=256) break;
			if(!EffectLoad(pp,OpenDialogMulti->Files->Strings[aa].c_str()))
			{
				Application->MessageBox(("Can't load effect '"+OpenDialogMulti->Files->Strings[aa]+"'!").c_str(),"Error",MB_OK);
			}
			else
			{
				SpeedButtonDel->Enabled=true;
				MBankDelete->Enabled=true;
			}
			pp++;
		}
		allEffect=pp;
		
		TitleUpdate();
		BankUpdateCount();
		DrawEdit(false);
		EffectSwitchEnable();
		no_click=50;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MFileSaveBankClick(TObject *Sender)
{
	if(SaveDialogBank->Execute())
	{
		BankSave(SaveDialogBank->FileName.c_str(),true);
		TitleUpdate();
		no_click=50;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MFileLoadBankClick(TObject *Sender)
{
	if(OpenDialogBank->Execute())
	{
		if(BankLoad(OpenDialogBank->FileName.c_str()))
		{
			SaveDialogBank->FileName=OpenDialogBank->FileName;
			TitleUpdate();
			DrawEdit(false);
			BankUpdateCount();
			EffectSwitchEnable();
			no_click=50;
		}
		else
		{
			Application->MessageBox("Can't load bank!","Error",MB_OK);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MFileSaveBankNnmClick(TObject *Sender)
{
	if(SaveDialogBank->Execute())
	{
		BankSave(SaveDialogBank->FileName.c_str(),false);
		TitleUpdate();
		no_click=50;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::EditEffectNameKeyPress(TObject *Sender, char &Key)
{
	if(Key==VK_RETURN)
	{
		EditEffectName->Enabled=false;
		EditEffectName->Enabled=true;
		EditEffectName->ReadOnly=true;
		EditEffectName->Font->Color=clGrayText;
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::EditEffectNameClick(TObject *Sender)
{
	EditEffectName->ReadOnly=false;
	EditEffectName->Font->Color=clWindowText;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::EditEffectNameChange(TObject *Sender)
{
	strcpy(ayBank[curEffect].name,EditEffectName->Text.c_str());
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MBankAddClick(TObject *Sender)
{
	SpeedButtonAddClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonAddClick(TObject *Sender)
{
	if(BankAddEffect())
	{
		curEffect=allEffect-1;
		BankUpdateCount();
		DrawEdit(true);
		SpeedButtonDel->Enabled=true;
		MBankDelete->Enabled=true;
		EffectSwitchEnable();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonDelClick(TObject *Sender)
{
	if(Application->MessageBox("Delete effect?","Sure?",MB_YESNO)==IDYES)
	{
		if(BankDeleteEffect(curEffect))
		{
			if(curEffect>=allEffect&&curEffect>0) curEffect=allEffect-1;
			BankUpdateCount();
			DrawEdit(true);
			if(allEffect==1)
			{
				SpeedButtonDel->Enabled=false;
				MBankDelete->Enabled=false;
				EffectSwitchEnable();
			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MBankDeleteClick(TObject *Sender)
{
	SpeedButtonDelClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MBankInsertClick(TObject *Sender)
{
	if(BankInsertEffect(curEffect))
	{
		BankUpdateCount();
		DrawEdit(true);
		SpeedButtonDel->Enabled=true;
		MBankDelete->Enabled=true;
	}
}
//---------------------------------------------------------------------------

enum {
	TYPE_AFX=0,
	TYPE_VT2,
	TYPE_WAV,
	TYPE_CSV
};

bool __fastcall TFormMain::MultiSaveExport(int type)
{
	int id,cnt;
	char str[256];
	AnsiString name;
	const char *ext[]={"afx","txt","wav","csv"};

	cnt=0;

	for(id=0;id<allEffect;++id) if(EffectRealLen(id)>0) ++cnt;

	if(!cnt)
	{
		Application->MessageBox("Bank is empty, nothing to save or export.","Multi save/export",MB_OK);

		return false;
	}

	SaveDialogMulti->FileName=NameTrimPath(SaveDialogBank->FileName);

	if(!SaveDialogMulti->Execute()) return false;

	for(id=0;id<allEffect;++id)
	{
		if(EffectRealLen(id)<=0) continue;

		if(SaveDialogMulti->FilterIndex==1)
		{
			sprintf(str,"%s_%3.3i_%s.%s",NameTrimPath(SaveDialogBank->FileName).c_str(),id,GetEffectFileName(id).c_str(),ext[type]);
		}
		else
		{
			sprintf(str,"%s.%s",GetEffectFileName(id).c_str(),ext[type]);
		}

		name=str;

		if(FileExists(name))
		{
			if(Application->MessageBox(("File '"+name+"' already exists, overwrite?").c_str(),"Confirm",MB_YESNO)==IDNO) continue;
		}

		switch(type)
		{
		case TYPE_AFX: EffectSave(id,name.c_str()); break;
		case TYPE_VT2: EffectExportVT2(id,name.c_str(),FormVortexExp->BaseNote); break;
		case TYPE_WAV: EffectExportWAV(id,name.c_str()); break;
		case TYPE_CSV: EffectExportCSV(id,name.c_str()); break;
		}
	}

	return true;
}

void __fastcall TFormMain::MFileMultiSaveClick(TObject *Sender)
{
	SaveDialogMulti->Title="Multi-save effects from bank";

	MultiSaveExport(TYPE_AFX);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::SpeedButtonFirstClick(TObject *Sender)
{
	if(curEffect>0)
	{
		curEffect=0;
		BankUpdateCount();
		DrawEdit(true);
		EffectSwitchEnable();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonLastClick(TObject *Sender)
{
	if(curEffect<allEffect-1)
	{
		curEffect=allEffect-1;
		BankUpdateCount();
		DrawEdit(true);
		EffectSwitchEnable();
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MViewPianoInputClick(TObject *Sender)
{
	SpeedButtonPiano->Down^=true;
	MViewPianoInput->Checked=SpeedButtonPiano->Down;
	FormPiano->Visible=SpeedButtonPiano->Down;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::SpeedButtonPianoClick(TObject *Sender)
{
	MViewPianoInput->Checked=SpeedButtonPiano->Down;
	FormPiano->Visible=SpeedButtonPiano->Down;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MImportVGMClick(TObject *Sender)
{
	OpenDialogImport->Title="Load VGM file";
	OpenDialogImport->DefaultExt="vgm";
	OpenDialogImport->Filter="Sega hardwave sound log (*.vgm)|*.vgm";
	if(OpenDialogImport->Execute())
	{
		FormSnChn->ShowModal();
		if(import_ok)
		{
			if(ImportVGM(curEffect,OpenDialogImport->FileName.c_str(),vgm_chan,vgm_noise))
			{
				SaveDialogEffect->FileName="imported.afx";
				TitleUpdate();
				BankUpdateCount();
				DrawEdit(false);
				no_click=50;
			}
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MExportVT2Click(TObject *Sender)
{
	FormVortexExp->ShowModal();

	if(!FormVortexExp->Confirm) return;

	if(!MExportAll->Checked)
	{
		SaveDialogExport->Title="Export Vortex Tracker II instrument";
		SaveDialogExport->FileName=MExportAll->Checked?"bank":ayBank[curEffect].name;
		SaveDialogExport->DefaultExt="txt";
		SaveDialogExport->Filter="Text file (*.txt)|*.txt";

		if(!SaveDialogExport->Execute()) return;

		EffectExportVT2(curEffect,SaveDialogExport->FileName.c_str(),FormVortexExp->BaseNote);
	}
	else
	{
		SaveDialogMulti->Title="Multi-export effects from bank as VT2 instruments";

		MultiSaveExport(TYPE_VT2);
	}

	no_click=50;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MExportWavClick(TObject *Sender)
{
	if(!MExportAll->Checked)
	{
		SaveDialogExport->Title="Export wave file";
		SaveDialogExport->FileName=MExportAll->Checked?"bank":ayBank[curEffect].name;
		SaveDialogExport->DefaultExt="wav";
		SaveDialogExport->Filter="Windows sound file (*.wav)|*.wav";

		if(!SaveDialogExport->Execute()) return;

		EffectExportWAV(curEffect,SaveDialogExport->FileName.c_str());
	}
	else
	{
		SaveDialogMulti->Title="Multi-export effects from bank as wave files";

		MultiSaveExport(TYPE_WAV);
	}

	no_click=50;
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::MExportCSVClick(TObject *Sender)
{
	if(!MExportAll->Checked)
	{
		SaveDialogExport->Title="Export CSV file";
		SaveDialogExport->FileName=MExportAll->Checked?"bank":ayBank[curEffect].name;
		SaveDialogExport->DefaultExt="csv";
		SaveDialogExport->Filter="CSV file (*.csv)|*.csv";

		if(!SaveDialogExport->Execute()) return;

		EffectExportCSV(curEffect,SaveDialogExport->FileName.c_str());
	}
	else
	{
		SaveDialogMulti->Title="Multi-export effects from bank as CSV files";

		MultiSaveExport(TYPE_CSV);
	}

	no_click=50;
}
//---------------------------------------------------------------------------

