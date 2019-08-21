/*

  Pletter v0.4a-crippled

  sjoerd - www.xl2s.tk


  (Crippled to use for dsk2rom by Vincent van Dam)

*/
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string>
using namespace std;

/*
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;
using std::flush;
*/

int maxcslen,maxcblen,extbits;

int nlit=0,ncop1=0,ncop2=0,pletmode=0;

int offset,length;
unsigned char *buffer;
string sourcefilename, destfilename, mode;
FILE *file;

int varcost[65536];

void initvarcost() {
  int v=1,b=1,r=1;
  while (r!=65536) {
    for (int j=0;j!=r;++j) varcost[v++]=b;
    b+=2; r*=2;
  }
}

struct combstream {
  unsigned char *buf;
  int ep, dp, p, e;
  combstream() { buf=NULL; }
  ~combstream() { if(buf) free(buf); }
  void init() {	
    ep=dp=p=e=0; 
    buf=(unsigned char*)malloc(length*2); 
  }
  void add0() { 
    if (p==0) claimevent(); 
    e*=2; ++p; 
    if (p==8) addevent(); 
  }
  void add1() { 
    if (p==0) claimevent(); 
    e*=2; ++p; ++e; 
    if (p==8) addevent(); 
  }
  void addvar(unsigned int i) {
    unsigned int j=32768;
    while (!(i&j)) j/=2;
    do {
      if (j==1) { add0(); return; }
      j/=2;
      add1(); 
      if (i&j) add1(); else add0();
    } while (1);
  }
  void adddata(unsigned char d) { 
    buf[dp++]=d; 
  }
  void addevent() { 
    buf[ep]=e; e=p=0; 
  }
  void claimevent() { 
    ep=dp; ++dp; 
  }
  void done() {
    if (p!=0) { 
      while (p!=8) { 
        e*=2; ++p; 
      } 
      addevent();
    }
  }
  unsigned char* getbuf() { 
    return buf;
  }
  int getlength() {
    return dp;
  }
} s;

struct metadata {
  int cbpos,cblen,cost,mode,mlen,cspos,cslen,prev,reeks;
};

struct pakker {
  metadata *m;
  pakker () {}
  void pak(unsigned char *data, int len) {
    int i,last[65536];
    m=new metadata[len+1];
    for (i=0;i!=65536;++i) last[i]=-1;
  
    for (i=0;i!=len;++i) { 
      m[i].cbpos=m[i].cblen=0;
      m[i].cspos=m[i].cslen=0;
      m[i].prev=last[data[i]+data[i+1]*256]; last[data[i]+data[i+1]*256]=i; }

    int r=-1,t=0;
    for (i=len-1;i!=-1;--i) if (data[i]==r) m[i].reeks=++t; else { r=data[i]; m[i].reeks=t=0; }

    for (i=len-2;i!=-1;--i) {
      int l,p;
      p=i;
      while ((p=m[p].prev)!=-1) {
        if (i-p>maxcslen) break;
        if (m[p].reeks<m[i].reeks) continue; else l=m[i].reeks+1;
        while ((i+l)<len && data[p+l]==data[i+l]) ++l;
        if (l>65535) l=65535;
        if (l>m[i].cslen) { m[i].cslen=l; m[i].cspos=i-p; }
      }
    }

    //    cout << "-";

    if (maxcblen) {
      for (i=len-2;i!=-1;--i) {
        int l,p;
        p=i;
        while ((p=m[p].prev)!=-1) {
          if (i-p>maxcblen) break;
          if (m[p].reeks<m[i].reeks) continue; else l=m[i].reeks+1;
          while ((i+l)<len && data[p+l]==data[i+l]) ++l;
          if (l>65535) l=65535;
          if (l>m[i].cblen) { m[i].cblen=l; m[i].cbpos=i-p; }
        }
      }
      //    cout << ">";
    }

    int c,mode,cc,j,l;
    m[len-1].cost=8; m[len-1].mode=0; m[len-1].mlen=0;
    m[len].cost=0;
    for (i=len-1;i!=-1;--i) {
      mode=0; l=0;
      c=9+m[i+1].cost;

      j=m[i].cslen;
      while (j>1) {
        cc=9+varcost[j-1]+m[i+j].cost;
        if (cc<c) { c=cc; mode=1; l=j; }
        --j;
      }

      if (maxcblen) {
        j=m[i].cblen;
        while (j>1) {
          cc=9+extbits+varcost[j-1]+m[i+j].cost;
          if (cc<c) { c=cc; mode=2; l=j; }
          --j;
        }
      }

      m[i].cost=c; m[i].mode=mode; m[i].mlen=l;
    }

    //    cout << "| ";

    s.init();
    i=1;
    s.adddata(data[0]);

    while(i<len) {
      switch (m[i].mode) {
      case 0: nlit++;
        s.add0();
        s.adddata(data[i]);
        i++; break;
      case 1: ncop1++;
        s.add1();
        s.addvar(m[i].mlen-1);
        s.adddata(m[i].cspos-1);
        i+=m[i].mlen; break;
      case 2: ncop2++;
        j=m[i].cbpos-1;
	//        if (j<128) cout << "Paniek!\n";
        j-=128;
        s.add1();
        s.addvar(m[i].mlen-1);
        s.adddata(128+(j&127));
        switch (extbits) {
        case 7:
          if (j&8192) s.add1(); else s.add0();
        case 6:
          if (j&4096) s.add1(); else s.add0();
        case 5:
          if (j&2048) s.add1(); else s.add0();
        case 4:
          if (j&1024) s.add1(); else s.add0();
        case 3:
          if (j&512) s.add1(); else s.add0();
        case 2:
          if (j&256) s.add1(); else s.add0();
          if (j&128) s.add1(); else s.add0();
          break;
        default:
	  //          cout << "Internal error" << endl; exit(1);
          break;
        }
        i+=m[i].mlen;
        break;
      }
    }

    s.add1();
    for (i=0;i!=32;++i) s.add1();
    s.done();
  }
  unsigned char* getbuf() { 
    return s.getbuf();
  }
  int getlength() {
    return s.getlength();
  }
} p;


/*
int main(int argc, char *argv[]) {
  int i=1;
  unsigned char* output;

  if (argc==1) cout << endl;
  cout << "Pletter v0.4a - www.xl2s.tk" << endl;
  if (argc==1) {
    cout << "\nUsage:\npletter lookbackbuffersize sourcefile [offset [length [destinationfile]]]\n";
    cout << "\nLookback buffer sizes:\n";
    cout << "  8 -   256 bytes\n";
    cout << "  9 -   640 bytes\n";
    cout << "  0 -  1152 bytes\n";
    cout << "  1 -  2176 bytes\n";
    cout << "  2 -  4224 bytes\n";
    cout << "  3 -  8320 bytes\n";
    cout << "  4 - 16512 bytes\n";
    exit(1);
  }

  offset=0;
  length=0;

  if (argv[i]) mode=argv[i++];
  if (argv[i]) sourcefilename=argv[i++];
  if (argv[i]) offset=atoi(argv[i++]);
  if (argv[i]) length=atoi(argv[i++]);
  if (argv[i]) destfilename=argv[i++];

  if (!mode[0]) { cout << "No pletter mode" << endl; exit(1); }
  if (!sourcefilename[0]) { cout << "No inputfile" << endl; exit(1); }
  if (!destfilename[0]) destfilename=sourcefilename;
  destfilename+=".plet"+mode;

  pletmode=mode[0]-48;
  switch (pletmode) {
  case 8: maxcslen=256; maxcblen=0;         extbits=0; break;
  case 9: maxcslen=128; maxcblen=  512+128; extbits=2; break;
  case 0: maxcslen=128; maxcblen= 1024+128; extbits=3; break;
  case 1: maxcslen=128; maxcblen= 2048+128; extbits=4; break;
  case 2: maxcslen=128; maxcblen= 4096+128; extbits=5; break;
  case 3: maxcslen=128; maxcblen= 8192+128; extbits=6; break;
  case 4: maxcslen=128; maxcblen=16384+128; extbits=7; break;
  default:
    cout << "Illegal pletter mode" << endl; exit(1);
  }

  if ((file=fopen(sourcefilename.c_str(),"rb"))==NULL) { cout << "Error opening file:" << sourcefilename << endl; exit(1); }
  if (!length) {
    fseek(file,0,SEEK_END); 
    length=ftell(file)-offset;
  }
  fseek(file,offset,SEEK_SET);
  buffer=(unsigned char*)malloc(length);
  if (!fread(buffer,length,1,file)) { cout << "Filesize error" << endl; exit(1); }
  fclose(file);

  initvarcost();

  p.pak(buffer,length);

  FILE *file;
  if (!(file=fopen(destfilename.c_str(),"wb"))) { cout << "whaaa!\n"; exit(1); }
  fwrite(p.getbuf(),1,p.getlength(),file); fclose(file);
  cout << destfilename << ": " << length << " -> " << p.getlength() << endl;

  free(buffer);

  return 0;
}
*/




//-----------------------------------------------------------------------------

void pletter( unsigned char*  input,  int  input_length,
	      unsigned char** output, int* output_length,
	      int pletmode  )
{
  int i=1;

  offset=0;
  length=input_length;

  switch (pletmode) {
  case 8: maxcslen=256; maxcblen=0;         extbits=0; break;
  case 9: maxcslen=128; maxcblen=  512+128; extbits=2; break;
  case 0: maxcslen=128; maxcblen= 1024+128; extbits=3; break;
  case 1: maxcslen=128; maxcblen= 2048+128; extbits=4; break;
  case 2: maxcslen=128; maxcblen= 4096+128; extbits=5; break;
  case 3: maxcslen=128; maxcblen= 8192+128; extbits=6; break;
  case 4: maxcslen=128; maxcblen=16384+128; extbits=7; break;
  default: maxcslen=128; maxcblen=16384+128; extbits=7; break;//just a default
  }
  
  buffer=(unsigned char*)malloc(input_length);
  memcpy(buffer,input,input_length);

  initvarcost();
  p.pak(buffer,length);
  
  if (p.getlength()>=length) {
    *output_length=length;
    *output = (unsigned char*)malloc(length);
    memcpy(*output,input,length);
  } else {
    *output_length = p.getlength();
    *output = (unsigned char*)malloc(*output_length);
    memcpy(*output,p.getbuf(),*output_length);
  }

  free(buffer);

  return;
}



// eof pletter.cpp
