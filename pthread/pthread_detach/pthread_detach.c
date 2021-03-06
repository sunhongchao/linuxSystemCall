#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
void out_state(pthread_attr_t *attr)
{
	int state;
	if(pthread_attr_getdetachstate(attr,&state)!=0){
		perror("getdetachstate error");
	}else{
		if(state==PTHREAD_CREATE_JOINABLE){
			printf("joinable state\n");
		}else if(state==PTHREAD_CREATE_DETACHED){
			printf("detached state\n");
		}else{
			printf("error state\n");
		}
		
;	}
}
void *th_fn(void*arg)
{
	int i;
	int sum=0;
	for(i=1;i<=100;i++)
		sum+=i;
	return (void*)sum;
}
int main(void)
{
	int err;
	pthread_t default_th,detach_th;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	
	out_state(&attr);
	if((err=pthread_create(&default_th,&attr,th_fn,(void*)0))!=0){
		perror("pthread_creat error");	
	}
	int res;
	if((err=pthread_join(default_th,(void*)&res))!=0)
		perror("pthread join error");
	else 
		printf("default return is %d\n",(int)res);	
	
	printf("----------------------------\n");
	
	//设置分离属性为分离状态
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	out_state(&attr);
	if((err=pthread_create(&detach_th,&attr,th_fn,(void*)0))!=0){
		perror("pthread_creat error");	
	}
    //分离状态启动调用pthread_join会出错,因为资源已经被自动释放
	if((err=pthread_join(detach_th,(void*)&res))!=0)
		perror("pthread join error");
	else 
		printf("default return is %d\n",(int)res);
    pthread_attr_destroy(&attr);
	printf("0x%lx finished\n",pthread_self());
	sleep(1);
}