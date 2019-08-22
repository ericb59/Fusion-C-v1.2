har *FT_strrev(char *str)
{
    char tmp;
    int i;
    int j;

    i = 0;
    j = strlen(str) - 1;
    while (i < j)
    {
        tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
        j--;
        i++;
    }
    return (str);
}


void FT_DecToHexa(char *DestHex, int n) 
{     
    int i = 0; 
    int temp = 0;
    while(n!=0) 
    {    
        // temporary variable to store remainder 
        temp  = 0; 
          
        // storing remainder in temp variable. 
        temp = n % 16; 
          
        // check if temp < 10 
        if(temp < 10) 
        { 
            DestHex[i] = temp + 48; 
            i++; 
        } 
        else
        { 
            DestHex[i] = temp + 55; 
            i++; 
        } 
          
        n = n/16; 
    } 
} 



void FT_Wait(int cicles)
{
  int i;
  for(i=0;i<cicles;i++) HALT;
  return;
}



void FT_ScreenCopy(char x1, char y1, char dx, char dy, char x2, char y2, char src , char dst, char mode)  
{
    int src_y=0;
    int dst_y=0;

    if (src==1)
        src_y=256;
    if (dst==1)
        dst_y=256;
    
    t.X = x1; 
    t.Y = src_y+y1; 
    t.DX = dx;  
    t.DY = dy;     
    t.X2 = x2; 
    t.Y2 = dst_y+y2;           
    t.s0 = 0; 
    t.DI = 0; 
    t.LOP = mode;
    fLMMM(&t);
}

// Round function
//
int FT_round(float n)
{
  if (n<0.0)
      return (int)(n-0.5);
  else
      return (int)(n+0.5);
}

