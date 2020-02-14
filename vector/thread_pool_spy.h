#pragma once
#include<iostream>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<list>
#include<functional>
#include<iomanip>
#include<map>
#include<atomic>
#include<time.h>
#include"cTaskPack.h"
#include"print_to_buf.h"
#include"thread_pool_spy.h"
using namespace std;
#define _SpyTest1_
/*
	��һ�ּ��ģʽ ����̵߳�״̬ ִ������ ��ȡ�����ʱ�� �����ϴν�ͼ���Ǹ�
*/
#define _SpyTest2_
/*
	�ڶ��ּ��ģʽ ����̳߳����̵߳�ִ�������̵߳ĸ��� ��ص����� ִ��������߼�� ����������������Ŀ
				   ���ڹ������̸߳��� ������Ϣ���̸߳���
				   ����̵߳Ĵ���
				   �Լ�����
*/
//�̳߳��ӵ������汾 :���������� ���Ը������ڵȴ����߳���Ŀ��̬����չ�̳߳����̵߳���
class ThreadPoolSpy
{
public:
	ThreadPoolSpy();
	~ThreadPoolSpy();
	void put(shared_ptr<TaskPack> arg);
	void putWithoutTaskLimit(shared_ptr<TaskPack> arg);
	void insertSpyArg(thread::id id, SpyInfoPack arg,string task);//��������뵽������
	void start();
	void setSpyTime(int time);
	void setCanUse();		   //���������Ա�ʹ��
	bool getCanUse();          //�����Ƿ�ɱ�ʹ��
	void stopPool();
	void stopUnilTaskOver();
	void reportTask();
	void destoryTask();
private:
	void consum();
private:
	//void reportTask();
	//void destoryTask();
	string getNowTime();
private:
	bool try_put(shared_ptr<TaskPack> arg);
	//void insertSpyArg(thread::id id, SpyInfoPack arg);
	bool isEmpty();            //���ݰ��Ƚ���صĺ���
	bool isOverFlow();
	bool isLargerToMaxWorking();//�߳����Ƚ���صĺ���
	bool isMinnerToMinWorking();
	bool isInBewteenWorking();
	bool isLargerToMaxWaiting();//�߳����Ƚ���صĺ���
	bool isMinnerMinWaiting();
	bool isMinnerIsZero();
	bool isInBewteenWaiting();
	void stopAllThreadRecode();
	void stopOneThreadRecode(thread::id id);
private:
	shared_ptr<TaskPack> get();
	void spyControl();
	shared_ptr<TaskPack> getDataToVec();
private:
	bool m_canuse = false;
	int spy_time;
	//vector<thread::id> m_threadvec;
	mutex m_synchr;				//ͬ����
	mutex m_task;				//������
	list<shared_ptr<TaskPack>> m_taskvec;
	enum {
		max_num_vec = 7,
		min_num_vec = 0
		/*
			�����������������Լ���С�������Ŀ
		*/
	};
	enum {
		max_num_thread = 6,
		min_num_thread = 2
		/*
			�̶߳���������̺߳���С�̵߳���Ŀ �����߳�
		*/
	};
	condition_variable m_convec;
	condition_variable m_provec;
	atomic<int> waiting = 0;
	atomic<int> working = 3;
	mutex m_end;				//�̳߳ؽ�����ʱ��ʹ��
	condition_variable m_endcon;//��������
	vector<int> vec;

	mutex m_allsolv;				//�̳߳ؽ�����ʱ��ʹ��
	condition_variable m_allsolvcon;//��������
	
	mutex m_spyinfo;
	map<thread::id, vector<SpyInfoPack>> spy_info;
};
 