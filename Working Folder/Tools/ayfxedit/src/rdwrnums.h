int RdWordHL(unsigned char *bytes)		//чтение слова в порядке байт hi/low
{ 
	return (bytes[0]<<8)+bytes[1];
}

int RdDWordHL(unsigned char *bytes)		//чтение двойного слова в порядке байт hi/low
{
	return (bytes[0]<<24)+(bytes[1]<<16)+(bytes[2]<<8)+bytes[3];
}

int RdWordLH(unsigned char *bytes)		//чтение слова в порядке байт low/hi
{
	return (bytes[1]<<8)+bytes[0];
}

int RdDWordLH(unsigned char *bytes)		//чтение двойного слова в порядке байт low/hi
{
	return (bytes[3]<<24)+(bytes[2]<<16)+(bytes[1]<<8)+bytes[0];
}

void WrWordHL(unsigned char *bytes,int num) //запись слова в порядке байт hi/low
{
	bytes[0]=(num>>8)&255;
	bytes[1]=num&255;
}

void WrDWordHL(unsigned char *bytes,int num) //запись двойного слова в порядке байт hi/low
{
	bytes[0]=(num>>24)&255;
	bytes[1]=(num>>16)&255;
	bytes[2]=(num>>8)&255;
	bytes[3]=num&255;
}

void WrWordLH(unsigned char *bytes,int num) //запись слова в порядке байт low/hi
{
	bytes[1]=(num>>8)&255;
	bytes[0]=num&255;
}

void WrDWordLH(unsigned char *bytes,int num) //запись двойного слова в порядке байт low/hi
{
	bytes[3]=(num>>24)&255;
	bytes[2]=(num>>16)&255;
	bytes[1]=(num>>8)&255;
	bytes[0]=num&255;
}