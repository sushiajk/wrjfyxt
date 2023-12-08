#pragma once
#include"pthread.h"
#include<iostream>
#include<queue>
#include<vector>
using namespace std;

class task
{
public:
	task(void (*func)(void*),void*arg);
	~task();
	void(*func)(void*);
	void *arg;
};
class pthreadpool
{
public:
	pthreadpool(int num);
	void creat();
	void puttask(task*task);
	void dotask();
	int gettasknum();
	int getbusy();
	int getlive();
	void addexitnum();
	void jianexitnum();
	int geteixtnum();
	void delworker(const pthread_t &tid);
	pthread_mutex_t*gettaskmutex();
	pthread_mutex_t*getbusymutex();
	pthread_mutex_t*getworkmutex();
	pthread_cond_t*getempyte();
	void close();
	void jion();
	int exitnum;
	int showdown;
	~pthreadpool();
	
private:
	queue<task*>taskqueue;//任务队列
	int taskcout;//任务个数

	int tasknum;
	vector<pthread_t>worker;//工作者线程队列
	pthread_t manager;//管理者线程
	int busy;//忙碌的线程个数
	int live;//存活的线程个数
	pthread_mutex_t taskmutex;//任务队列锁
	pthread_mutex_t busymutex;//忙碌线程个数的锁
	pthread_mutex_t workmutex;
	pthread_cond_t empyte;//条件变量，队列为空

	
};
void * work(void*arg);//工作者线程任务
void * man(void*arg);//管理者线程任务