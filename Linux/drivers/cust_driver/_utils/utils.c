

#include<linux/delay.h> 


void Delay(int sec)
{
	int n = sec * 1000000;	// 1000000 == 1 sec

	udelay(n);

	return;
}




















