#include "poolcpp.h"
#include<unistd.h>

task::task(void(*func)(void *), void * arg)
{
	this->func = func;
	this->arg = arg;
}

pthreadpool::pthreadpool(int num)
{
	this->showdown = 1;
	this->busy = 0;
	this->exitnum = 0;
	this->manager = 0;
	int x=pthread_mutex_init(&this->taskmutex,NULL);
	int y=pthread_mutex_init(&this->busymutex, NULL);
	int o = pthread_mutex_init(&this->workmutex, NULL);
	int z=pthread_cond_init(&this->empyte, NULL);
	if (!x && !y && !z&&!o)
	{
		printf("config init creatsuccessful\n");
	}
	for (int i = 0; i < num; i++)
	{
		pthread_t tid = 0;
		int a=pthread_create(&tid, NULL, work, this);
		if (a == 0)
		{
			printf("workthreadID= %ld creatsuccessful\n", tid);
			this->worker.push_back(tid);
		}
	}
	int c=pthread_create(&this->manager, NULL, man, this);
	if (c == 0)
	{
		printf("managerthreadID= %ld creatsuccessful\n", this->manager);
	}
}

void pthreadpool::creat()
{
	pthread_mutex_lock(this->getbusymutex());
	for (int i = 0; i < 2; i++)
	{
		pthread_t tid = 0;
		int s=pthread_create(&tid, NULL, work,this);
		if (!s)
		{
			this->worker.push_back(tid);
			printf("task is too many preparecreat!!!\n");
		}
		
	}
	pthread_mutex_unlock(this->getbusymutex());
}

void pthreadpool::puttask(task * task)
{
	pthread_mutex_lock(this->gettaskmutex());
	this->taskqueue.push(task);
	this->tasknum = this->taskqueue.size();
	pthread_cond_signal(this->getempyte());
	pthread_mutex_unlock(this->gettaskmutex());
}

void pthreadpool::dotask()
{
	pthread_mutex_lock(this->gettaskmutex());
	while (this->taskqueue.size() == 0)
	{
		pthread_cond_wait(this->getempyte(), this->gettaskmutex());
		if (this->geteixtnum() != 0)
		{
			this->jianexitnum();
			printf("threadID= %ld is KILLING!!!\n", pthread_self());
			this->delworker(pthread_self());
			pthread_mutex_unlock(this->gettaskmutex());
			pthread_exit(NULL);
		}
	}
	if (this->showdown == 0)
	{
		pthread_mutex_unlock(this->gettaskmutex());
		printf("threadID= %ld pool closed so exit!!!\n", pthread_self());
		pthread_exit(NULL);
	}
	 task*t=this->taskqueue.front();
	 pthread_mutex_lock(this->getbusymutex());
	 this->busy++;
	 printf("threadID= %ld is working\n", pthread_self());
	 pthread_mutex_unlock(this->getbusymutex());
	 t->func(t->arg);
	 pthread_mutex_lock(this->getbusymutex());
	 this->busy--;
	 printf("threadID= %ld is endworking\n", pthread_self());
	 pthread_mutex_unlock(this->getbusymutex());
	 this->taskqueue.pop();
	pthread_mutex_unlock(this->gettaskmutex());
	sleep(5);
	
}

int pthreadpool::gettasknum()
{
	int tasknum;
	pthread_mutex_lock(this->gettaskmutex());
	this->tasknum = this->taskqueue.size();
	tasknum = this->tasknum;
	pthread_mutex_unlock(this->gettaskmutex());
	return tasknum;
}

int pthreadpool::getbusy()
{
	int busy;
	pthread_mutex_lock(this->getbusymutex());
	busy = this->busy;
	pthread_mutex_unlock(this->getbusymutex());
	return busy;
}

int pthreadpool::getlive()
{
	int live;
	pthread_mutex_lock(this->getbusymutex());
	live = this->worker.size();
	pthread_mutex_unlock(this->getbusymutex());
	return live;
}

void pthreadpool::addexitnum()
{
	pthread_mutex_lock(this->getbusymutex());
	this->exitnum++;
	pthread_mutex_unlock(this->getbusymutex());
}

void pthreadpool::jianexitnum()
{
	pthread_mutex_lock(this->getbusymutex());
	this->exitnum--;
	pthread_mutex_unlock(this->getbusymutex());
}

int pthreadpool::geteixtnum()
{
	int exitnu;
	pthread_mutex_lock(this->getbusymutex());
	exitnu = this->exitnum;
	pthread_mutex_unlock(this->getbusymutex());

	return exitnu;
}

void pthreadpool::delworker(const pthread_t & tid)
{
	pthread_mutex_lock(this->getbusymutex());
	vector<pthread_t>::iterator it = this->worker.begin();
	for (it;it != this->worker.end(); it++)
	{
		if ((*it) == tid)
		{
			this->worker.erase(it);
			break;
		}
	}
	pthread_mutex_unlock(this->getbusymutex());
}

pthread_mutex_t * pthreadpool::gettaskmutex()
{
	return &this->taskmutex;
}

pthread_mutex_t * pthreadpool::getbusymutex()
{
	return &this->busymutex;
}

pthread_mutex_t * pthreadpool::getworkmutex()
{
	return &this->workmutex;
}

pthread_cond_t * pthreadpool::getempyte()
{
	return &this->empyte;
}

void pthreadpool::close()
{
	this->showdown = 0;
	pthread_join(this->manager, NULL);
}

void pthreadpool::jion()
{
	/*for (int i = 0; i < this->getlive();i++)
	{
		pthread_join(this->worker[i], NULL);

	}*/
	pthread_join(this->manager, NULL);
}

void * work(void * arg)
{
	pthreadpool*pool = (pthreadpool*)arg;
	while (1)
	{
		pool->dotask();
	}
	return nullptr;
}

void * man(void * arg)
{
	pthreadpool*pool = (pthreadpool *) arg;
	while (1)
	{
		sleep(1);
		if (pool->gettasknum() > 2 * pool->getlive())
		{
			pool->creat();
		}
		if (pool->getbusy() < pool->getlive() * 2)
		{
			printf("too many\n");
			if(pool->geteixtnum()==0)
			{
				printf("too many123\n");
				pool->addexitnum();
				printf("eixt is %d\n",pool->geteixtnum());
				for (int i = 0; i < 50; i++)
				{
					pthread_cond_signal(pool->getempyte());
				}
			}	
		}
		if (pool->showdown == 0)
		{
			for (int i = 0; i < pool->getlive(); i++)
			{
				pool->addexitnum();
				pthread_cond_signal(pool->getempyte());
			}
			sleep(3);
			pthread_exit(NULL);
		}
	}
	return nullptr;
}
