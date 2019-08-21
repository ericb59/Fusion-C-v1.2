//обрезок AY - всего один тональник и шум


struct toneStruct {
	int count;
	int state;
};

struct noiseStruct {
	int count;
	int reg;
	int qcc;
	int state;
};

struct chipStruct {
	toneStruct tone[1];
	noiseStruct noise;
	
	int reg[16];
	int dac[1];
	int out;
	
	int freqDiv;
};


chipStruct ayChip;


//табличка громкости (реальная, (С)HackerKAY)

int volTab[16]={ 0,836/3,1212/3,1773/3,2619/3,3875/3,5397/3,8823/3,10392/3,16706/3,23339/3,29292/3,36969/3,46421/3,55195/3,65535/3 };



void ay_init(chipStruct *ay)
{
	memset(ay,0,sizeof(chipStruct));
	
	ay->noise.reg=0x0ffff;
	ay->noise.qcc=0;
	ay->noise.state=0;
}



inline void ay_out(chipStruct *ay,int reg,int value)
{
	switch(reg)
	{
	case 1:
		value&=15;
		break;
	case 0:
	case 7:
		break;
	case 8:
	case 6:
		value&=31;
		break;
	default:
		return;
	}
	
	ay->reg[reg]=value;
}



inline void ay_tick(chipStruct *ay,int ticks)
{
	int noise_di;
	int aa,ta,na;
	
	ay->out=0;
	
	for(aa=0;aa<ticks;++aa)
	{
		//делитель тактовой частоты
		
		ay->freqDiv^=1;
		
		//тональник
		if(ay->tone[0].count>=(ay->reg[0]|(ay->reg[1]<<8)))
		{
			ay->tone[0].count=0;
			ay->tone[0].state^=1;
		}
		
		++ay->tone[0].count;
		
		if(ay->freqDiv)
		{	
			
			//шум (реальный алгоритм, (C)HackerKAY)
			
			if(ay->noise.count==0)
			{
				noise_di=(ay->noise.qcc^((ay->noise.reg>>13)&1))^1;
				ay->noise.qcc=(ay->noise.reg>>15)&1;
				ay->noise.state=ay->noise.qcc;
				ay->noise.reg=(ay->noise.reg<<1)|noise_di;
			}
			
			ay->noise.count=(ay->noise.count+1)&31;
			
			if(ay->noise.count>=ay->reg[6]) ay->noise.count=0;
		}
		
		//микшер
		
		ta=ay->tone[0].state|((ay->reg[7]>>0)&1);
		na=ay->noise.state|((ay->reg[7]>>3)&1);
		
		if(ta&na) ay->dac[0]=ay->reg[8]; else ay->dac[0]=0;
		
		ay->out+=volTab[ayChip.dac[0]];
	}
}