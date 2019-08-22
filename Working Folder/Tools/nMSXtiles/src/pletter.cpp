/*
  Pletter v0.5c1

  XL2S Entertainment
*/

// #include <stdlib.h>
// #include <stdio.h>
#include <QString>
// #include <QDebug>
#include <QMessageBox>

// REQUISITOS DE ENTRADA:
//------------------------
//	bool savelength = false;	// Opcional
//	unsigned offset = 0;		// Opcional
//	unsigned length = 0;		// Opcional
//	QString sourcefilename		// Obligatorio
//	QString destfilename	    // Opcional -> sourcefilename + ".plet5"

QString sourcefilename, destfilename;
unsigned length, offset;
bool savelength = false;

unsigned *last = NULL;
unsigned *prev = NULL;
unsigned char *d = NULL;

struct metadata {
    unsigned reeks;
    unsigned cpos[7], clen[7];
} *m = NULL;

struct pakdata {
    unsigned cost, mode, mlen;
} *p[7];

unsigned maxlen[7] = { 128,128+128,512+128,1024+128,2048+128,4096+128,8192+128 };
unsigned varcost[65536];

struct saves {
    unsigned char *buf = NULL;
    int ep, dp, p, e;
    void init() {
        ep=dp=p=e=0;
        buf = new unsigned char[ length * 2 ];
    }
    void add0() {
        if( p==0 ) claimevent();
        e *= 2; ++p;
        if( p==8 ) addevent();
    }
    void add1() {
        if( p==0 ) claimevent();
        e*=2; ++p; ++e;
        if( p==8 ) addevent();
    }
    void addbit(int b) {
        if( b ) add1(); else add0();
    }
    void add3(int b) {
        addbit(b&4); addbit(b&2); addbit(b&1);
    }
    void addvar( int i ) {
        int j=32768;
        while( !(i&j) ) j/=2;
        do {
            if( j==1 ) { add0(); return; }
            j/=2;
            add1();
            if( i&j ) add1();
            else add0();
        } while( 1 );
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
    bool done() {
        if (p!=0) {
            while (p!=8) { e*=2; ++p; } addevent();
        }
        FILE *file;
        if( !( file=fopen( qPrintable( destfilename ), "wb" ) ) ) {
            // qDebug() << "Error writing file!" << endl;
            QMessageBox::critical(
                NULL,
                QString( "Pletter5:" ),
                QString( "%1 %2" ).arg( "Error writing file!:" ).arg( destfilename )
            );
            return false;
        }
        fwrite( buf, 1, dp,file );
        fclose( file );
        // qDebug() << destfilename << ": " << length << " -> " << dp << endl;
        return true;
    }
} s;

/*
bool loadfile() {
    FILE *file;
    if( ( file = fopen( qPrintable( sourcefilename ), "rb" ) ) == NULL ) {
        // qDebug() << "Error opening file: " << sourcefilename << endl;
        QMessageBox::critical(
            NULL,
            QString( "Pletter5:" ),
            QString( "%1 %2" ).arg( "Error opening file:" ).arg( sourcefilename )
        );
        return false;
    }
    if( !length ) {
        fseek( file, 0, SEEK_END );
        length = (unsigned)ftell( file ) - offset;
        // qDebug() << "length = " << length << endl;
    }
    fseek( file, offset, SEEK_SET );
    d = new unsigned char[ length + 1 ];
    m = new metadata[ length + 1 ];
    if( !fread( d, length, 1, file ) ) {
        // qDebug() << "Filesize error" << endl;
        QMessageBox::critical(
            NULL,
            QString( "Pletter5:" ),
            QString( "Filesize error" )
        );
        return false;
    }
    d[ length ] = 0;
    fclose( file );
    return true;
}
*/

// OK
void initvarcost() {
    int v=1,b=1,r=1;
    while (r!=65536) {
        for (int j=0;j!=r;++j) varcost[v++]=b;
        b+=2; r*=2;
    }
}

// OK
void createmetadata() {
    unsigned i, j;
    last = new unsigned[ 65536 ];
    memset( last, -1, 65536 * sizeof(unsigned) );
    prev = new unsigned[ length + 1 ];
    for( i=0; i!=length; ++i ) {
        m[i].cpos[0]=m[i].clen[0]=0;
        prev[i]=last[d[i]+d[i+1]*256];
        last[d[i]+d[i+1]*256]=i;
    }
    unsigned r=-1,t=0;
    for( i=length-1; i != unsigned(-1); --i ) {
        if( d[i] == r ) m[i].reeks=++t;
        else { r=d[i]; m[i].reeks=t=1; }
    }
    for (int bl=0;bl!=7;++bl) {
        for (i=0;i<length;++i) {
            unsigned l,p;
            p=i;
            if (bl) {
                m[i].clen[bl]=m[i].clen[bl-1];
                m[i].cpos[bl]=m[i].cpos[bl-1];
                p=i-m[i].cpos[bl];
            }
            while( ( p=prev[p] ) != unsigned(-1) ) {
                if (i-p>maxlen[bl]) break;
                l=0;
                while (d[p+l]==d[i+l] && (i+l)<length) {
                    if (m[i+l].reeks>1) {
                        if( ( j=m[i+l].reeks ) > m[p+l].reeks ) j=m[p+l].reeks;
                        l+=j;
                    } else ++l;
                }
                if( l > m[i].clen[bl] ) { m[i].clen[bl]=l; m[i].cpos[bl]=i-p; }
            }
        }
        // qDebug() << ".";
    }
    // qDebug() << " ";
}

// OK
int getlen( pakdata *p, unsigned q ) {
    unsigned i,j,cc,ccc,kc,kmode,kl;
    p[length].cost=0;
    for( i=length-1; i!= unsigned(-1); --i ) {
        kmode=0; kl=0;
        kc=9+p[i+1].cost;
        j=m[i].clen[0];
        while( j > 1 ) {
            cc=9+varcost[j-1]+p[i+j].cost;
            if (cc<kc) { kc=cc; kmode=1; kl=j; }
            --j;
        }
        j=m[i].clen[q];
        if (q==1) ccc=9; else ccc=9+q;
        while (j>1) {
            cc=ccc+varcost[j-1]+p[i+j].cost;
            if (cc<kc) { kc=cc; kmode=2; kl=j; }
            --j;
        }
        p[i].cost=kc; p[i].mode=kmode; p[i].mlen=kl;
    }
    return p[0].cost;
}

// OK
bool save( pakdata *p, unsigned q ) {
    s.init();
    unsigned i, j;
    if( savelength ) {
        s.adddata( length & 255 );
        s.adddata( length >> 8 );
    }
    s.add3(q-1);
    s.adddata(d[0]);
    i=1;
    while( i < length ) {
        switch (p[i].mode) {
            case 0:
                s.add0();
                s.adddata(d[i]);
                ++i;
            break;
            case 1:
                s.add1();
                s.addvar(p[i].mlen-1);
                j=m[i].cpos[0]-1;
                // if( j > 127 ) qDebug() << "-j>128-";
                s.adddata(j);
                i+=p[i].mlen;
            break;
            case 2:
                s.add1();
                s.addvar(p[i].mlen-1);
                j=m[i].cpos[q]-1;
                // if( j < 128 ) qDebug() << "-j<128-";
                j-=128;
                // & tiene prioridad respecto a |
                // s.adddata( 128 | j & 127 );
                s.adddata( 128 | (j & 127) );
                switch (q) {
                    case 6: s.addbit(j&4096);
                    case 5: s.addbit(j&2048);
                    case 4: s.addbit(j&1024);
                    case 3: s.addbit(j&512);
                    case 2: s.addbit(j&256);
                            s.addbit(j&128);
                    case 1:
                    break;
                    default:
                        // qDebug() << "-2-";
                    break;
                }
                i+=p[i].mlen;
            break;
            default:
                // qDebug() << "-?-";
            break;
        }
    }

    for( i=0; i!=34; ++i ) s.add1();
    return s.done();
}

void clean() {
// unsigned *last = NULL;
// unsigned *prev = NULL;
// unsigned char *s.buf; // struct saves
// struct metadata *m;
// struct pakdata *p[7];
    if( last != NULL ) delete[] last;   //  last = new unsigned[ 65536 ];
    last = NULL;
    if( prev != NULL ) delete[] prev;   //  prev = new unsigned[ length + 1 ];
    prev = NULL;
    if( s.buf != NULL ) delete[] s.buf;     // buf = new unsigned char[ length * 2 ];
    s.buf = NULL;
    if( m != NULL ) delete[] m;               // m = new metadata[ length + 1 ];
    m = NULL;
    for( int i=1; i!=7; ++i ) {
        if( p[i] != NULL ) delete[] p[i];
        p[i] = NULL;
    }
}

bool pletter(
    unsigned char* pData,
    size_t         dataSize,
    const QString& destfile,
    bool           save_length
) {
//	REQUISITOS DE ENTRADA:
//-------------------------
//	bool savelength = false;	// Opcional
//	unsigned offset = 0;		// Opcional
//	unsigned length = 0;		// Opcional
//	QString sourcefilename		// Obligatorio
//	QString destfilename		// Opcional -> sourcefilename + ".plet5"

    if( pData == NULL ) {
        QMessageBox::critical(
            NULL,
            QString( "Pletter5:" ),
            QString( "pData is NULL" )
        );
        return false;
    }
    if( dataSize == 0 ) {
        QMessageBox::critical(
            NULL,
            QString( "Pletter5:" ),
            QString( "dataSize is 0" )
        );
        return false;
    }
    if( destfile.isNull() || destfile.isEmpty() ) {
        QMessageBox::critical(
            NULL,
            QString( "Pletter5:" ),
            QString( "destfile is Null or Empty" )
        );
        return false;
    }

    savelength = save_length;
    offset = 0;
    length = 0;

    sourcefilename = destfile;
    if( sourcefilename.contains( ".plet5", Qt::CaseInsensitive ) ) {
        sourcefilename.remove( ".plet5", Qt::CaseInsensitive );
    }
    destfilename = sourcefilename + ".plet5";

    int i=1;
    length = (unsigned)dataSize;

    d = pData; 	// new unsigned char[ length + 1 ];
    m = new metadata[ length + 1 ];
    d[ length ] = 0;

    initvarcost();
    createmetadata();

    int minlen=length*1000;
    int minbl=0;
    for( i=1; i!=7; ++i ) {
        p[i] = new pakdata[ length + 1 ];
        int l=getlen( p[i], i );
        if( l < minlen && i ) { minlen=l; minbl=i; }
        // printf( "." );
    }

    if( !save( p[minbl], minbl ) ) {
        clean();
        return false;
    }

    clean();
    return true;
}

/*
bool pletter(
    const QString& srcfile,    // archivo,
    bool           save_length // guarda_longitud
) {
//	REQUISITOS DE ENTRADA:
//-------------------------
//	bool savelength = false;	// Opcional
//	unsigned offset = 0;		// Opcional
//	unsigned length = 0;		// Opcional
//	QString sourcefilename		// Obligatorio
//	QString destfilename	    // Opcional -> sourcefilename + ".plet5"

    savelength = save_length;
    offset = 0;
    length = 0;
    sourcefilename = srcfile;
    destfilename = sourcefilename + ".plet5";
    int i=1;

    if( !loadfile() ) {
        clean();
        if( d != NULL ) delete[] d;
        d = NULL;
        return false;
    }

    initvarcost();
    createmetadata();

    int minlen=length*1000;
    int minbl=0;
    for( i=1; i!=7; ++i ) {
        p[i] = new pakdata[ length + 1 ];
        int l=getlen( p[i], i );
        if( l < minlen && i ) { minlen=l; minbl=i; }
        printf( "." );
    }

    if( !save( p[minbl], minbl ) ) {
        clean();
        if( d != NULL ) delete[] d;
        d = NULL;
        return false;
    }

    clean();
    if( d != NULL ) delete[] d;
    d = NULL;
    return true;
}
*/

/*

void LoadData( int length, unsigned char* pData ) {
    d = new unsigned char[ length + 1 ];
    m = new metadata[ length + 1 ];
    memcpy( d, pData, length );
    d[ length ] = 0;
}

int PletterMain(
    int dataSize,
    unsigned char *pData,
    QString destfile
) {

    int i;

    offset=0;
    length=0;

    destfilename = destfile;
    length = dataSize;

    LoadData( dataSize, pData );

    initvarcost();
    createmetadata();

    int minlen=length*1000;
    int minbl=0;
    for(i=1;i!=7;++i) {
        p[i]=new pakdata[length+1];
        int l=getlen(p[i],i);
        if (l<minlen && i) { minlen=l; minbl=i; }
        qDebug() << ".";
    }
    save( p[minbl], minbl );
    return 0;
}
*/

//eof

