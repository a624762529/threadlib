#pragma once
#include<iostream>
#include<atomic>
#include"thread_pool_spy.h"



using namespace std;
/*
	2020.1.5
	�̳߳ص�����˵��:
		�Ѿ����������� ��������Ķ�� �Զ������� ���߼����̵߳���Ŀ
		���Ҽ���ڶ��̵߳�����Ķ�Ӧ�����ĸı� ���繤���߳� �ȴ������������̣߳�
												   �̳߳ص��������������Ķ��
												   ����߳�ִ��ѭ���Ĵ��� ��
												   ����Ӧ���߳��Ƿ��ڹ�����״̬
		��������ʱ�� ������������ Ҳ���Բ���������
		�ر��̳߳ص�ʱ����Եȴ��̳߳��е��������ִ�н���
		Ҳ���Բ��ȴ�������е�ִ�н���

		��ض�Ӧ�߳�������������<��������������������>

	��һ�汾���µ�����:
		�Լ���ȡ�����ʱ��<Ҫ�ǳ���100s>		��֮Ϊ�����߳�<C++11���ܽ���һ���߳�>ͷ��
		�����û���ʹ������̳߳������ʱ����Ų�Ҫ�����м�����������

	ʹ��˵����
		�κ�����뵽�̵߳����� ����̳�TaskPack������� ʵ�����е�doit(��Ӧ��������doit��ִ��)�ӿ�
		���������ʱ��ʹ��share_ptr���м���/��
*/
class TestRealize :public TaskPack
{
public:
	TestRealize()
	{
		task = "TestRealize";
	}

	TestRealize(int i)
	{
		this->i = i;
		task = "TestRealize";
	}

	void doit() override
	{
		action();
	}

	void action()
	{
		pt.print_string_num("doit  ", i, "  threadid:  ", this_thread::get_id());
		this_thread::sleep_for(2s);
	}
private:
	int i;
};
int main123213()
{
	int pre = clock();
	ThreadPoolSpy pool;
	pool.setCanUse();
	pool.start();

	/*
		���Է���
			�ڼ��� 1-40�������ʱ�� ֱ�Ӳ���
			�ڲ��� 40-70�������ʱ�� 20s����һ��

	*/


	while (true)
	{
		static int i = 0;
		//pool.putWithoutTaskLimit(shared_ptr<TaskPack>(new TestRealize(i)));
		pool.put(shared_ptr<TaskPack>(new TestRealize(i)));
		if (i > 40)
		{
			this_thread::sleep_for(2s);
		} 
	    if (i > 70)
		{
			break;
		}
		i++;
	}

	//ֹͣpool����ص�����
	pt.print_string("start: task_over");
	//����ִ�н���֮������߳�
	//pool.stopUnilTaskOver();
	pool.stopPool();
	cout << "end: task over" << endl;
	int now = clock();
	cout << "���ĵ�ʱ��Ϊ" << now - pre << endl;

	//cout << "start: report" << endl;
	//pool.reportTask();
	while (true)
	{
		this_thread::sleep_for(10s);
	}
	system("pause");
	return 0;
}