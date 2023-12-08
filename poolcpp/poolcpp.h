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
	queue<task*>taskqueue;//�������
	int taskcout;//�������

	int tasknum;
	vector<pthread_t>worker;//�������̶߳���
	pthread_t manager;//�������߳�
	int busy;//æµ���̸߳���
	int live;//�����̸߳���
	pthread_mutex_t taskmutex;//���������
	pthread_mutex_t busymutex;//æµ�̸߳�������
	pthread_mutex_t workmutex;
	pthread_cond_t empyte;//��������������Ϊ��

	
};
void * work(void*arg);//�������߳�����
void * man(void*arg);//�������߳�����