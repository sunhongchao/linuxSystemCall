//一个计算线程，多个等待结果线程
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>

typedef struct
{
	int res;
	int counter;//用户给出用于判断的条件，统计获取结果线程数量
	pthread_cond_t cond;//等待的条件变量，将等待线程置于等待队列中
	pthread_mutex_t mutex;
}Result;
//计算并将结果放置Result中的线程运行函数
void *set_fn(void *arg)
{
	Result *r=(Result*)arg;
	int i=1,sum=0;
	for(;i<=100;i++){
		sum+=i;
	}
	
	//判断获得结果的线程是否准备好，没有则休眠等待.条件变量操作也是互斥
	pthread_mutex_lock(&r->mutex);
	r->res=sum;
	while(r->counter<2){//获取结果的线程是否达到两个
		pthread_mutex_unlock(&r->mutex);
		usleep(100);
		pthread_mutex_lock(&r->mutex);
	}
	pthread_mutex_unlock(&r->mutex);
	//唤醒等待结果的那个线程
	pthread_cond_broadcast(&r->cond);
	
	return (void*)0;
}
void *get_fn(void *arg)
{
	Result *r=(Result*)arg;
	pthread_mutex_lock(&r->mutex);
	r->counter++;//获取结果的线程已经准备好
	pthread_cond_wait(&r->cond,&r->mutex);//线程阻塞等待被唤醒
	//获取结果
	int res=r->res;
	pthread_mutex_unlock(&r->mutex);
	
	printf("0x%lx get sum is %d\n",pthread_self(),res);
	return (void*)0;
}
int main(void)
{
	int err;
	pthread_t cal,get0,get1;
	Result r;
	r.counter=0;
	pthread_cond_init(&r.cond,NULL);
	pthread_mutex_init(&r.mutex,NULL);
	if((err=pthread_create(&cal,NULL,set_fn,(void*)&r))!=0){
		perror("pthread_creat error");	
	}
	if((err=pthread_create(&get0,NULL,get_fn,(void*)&r))!=0){
		perror("pthread_creat error");	
	}
	if((err=pthread_create(&get1,NULL,get_fn,(void*)&r))!=0){
		perror("pthread_creat error");	
	}
	pthread_join(cal,NULL);
	pthread_join(get0,NULL);
	pthread_join(get1,NULL);
	pthread_cond_destroy(&r.cond);
	pthread_mutex_destroy(&r.mutex);
	return 0;

}