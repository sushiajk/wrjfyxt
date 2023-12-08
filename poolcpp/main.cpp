#include <cstdio>
#include"poolcpp.h"
#include<unistd.h>
void test(void*arg)
{
	int *num = (int*)arg;
	printf("test=  %d \n", *num);
	usleep(1000);
}
int main()
{
	pthreadpool*pool = new pthreadpool(500);
	for (int i = 1; i < 100; i++)
	{
		int *num = new int(i);
		task *t = new task(test,num);
		pool->puttask(t);
	}
	sleep(5);
	pool->close();
	cout << pool->getlive();
    return 0;
}