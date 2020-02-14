#pragma once
#include<iostream>
#include<mutex>
#include<thread>
#include<condition_variable>
#include<atomic>
#include<string>
#include<atomic>
#include"print_to_buf.h"
#include"RwLock.h"
using namespace std;

/*
	���ļ��Ĺ����ǲ��Զ�д����һ�¹���
		1:���Զ�д���Ķ���̶߳�ȡ  :����ͬʱ��ȡ
		2:���Զ�д���Ķ���߳�д��  :����ͬʱд��
		3:���Զ�д��ʱ������ȼ�    : д�������ȼ�����
		4:���Ը����Ļ�����
		���гɹ�

	���ڶ�д�����쳣����
		1:�����߳� �������λ�ȡ����  �����ͷŶ���  
		2:�����߳� �������λ�ȡд��  �����ͷ�д��  
		3:�����߳� ��ȡд��֮�� �������
		4:�����߳� ��ȡд��֮�� �������

		����ǳ���ҵ�
*/


RW_Lock rwlock;

void testMoreThreadRead()
{
	//1:���Զ�д���Ķ���̶߳�ȡ:����ͬʱ��ȡ
	rwlock.rd_lock();
	pt.print_string_num("read ",this_thread::get_id());
	this_thread::sleep_for(3s);
	rwlock.rd_unlock();
}
 
void testMoreThreadWrite()
{
	//1:���Զ�д���Ķ���̶߳�ȡ:����ͬʱ��ȡ
	rwlock.wr_lock();
	pt.print_string_num("write ", this_thread::get_id());
	this_thread::sleep_for(3s);
	rwlock.wr_unlock();
}


int actFuncMR()//���Զ��߳�ͬʱ��ȡ����
{
	thread th[2];
	for (int i = 0; i < 2; i++)
	{
		th[i] = std::move(thread(testMoreThreadRead));
	}
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	return 1;
}
int actFuncMW()//���Զ��߳�ͬʱ��ȡд��
{
	thread th[2];
	for (int i = 0; i < 2; i++)
	{
		th[i] = std::move(thread(testMoreThreadWrite));
	}
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	return 1;
}
int actFuncMRW()//���� ������д��֮�以��
{
	thread th[2];
	//������д�� ���ȡ
	th[0] = std::move(thread(testMoreThreadWrite));
	th[1] = std::move(thread(testMoreThreadRead));
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	//�����ȶ�ȡ��д��
	th[0] = std::move(thread(testMoreThreadRead));
	th[1] = std::move(thread(testMoreThreadWrite));
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	return 1;
}

int actFuncMRW_()//���� ������д��֮�以��
{
	thread th[2];
	//������д�� ���ȡ
	th[0] = std::move(thread(testMoreThreadWrite));
	th[1] = std::move(thread(testMoreThreadRead));
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	//�����ȶ�ȡ��д��
	th[0] = std::move(thread(testMoreThreadRead));
	th[1] = std::move(thread(testMoreThreadWrite));
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	return 1;
}

int actWriLockFrist()//���� ����д��
{
	/*
		���Է���:
			����д����ס
			����ס��ʱ��  �����������߳� �Լ�һ��д�߳� �۲��һ��д������֮�� ִ�е����� �Ƕ��̻߳���д�߳�
	*/
	thread th[3];
	th[0] = std::move(thread(testMoreThreadWrite));
	th[1] = std::move(thread(testMoreThreadRead));  
	th[2] = std::move(thread(testMoreThreadWrite));
	for (int i = 0; i < 3; i++)
	{
		th[i].join();
	}
	return 1;
}


/*
	�쳣���ԣ�
		1������һ���߳� ��δ�� ������ʱ�� �������  ������ʱ��������ζ���
		2������һ���߳� ��δ�� д����ʱ�� ���д��  ������ʱ���������д��
		3: ����һ���߳� �� ��  ������ʱ�� ���д��
		   ����һ���߳� �� ��  д����ʱ�� �������
*/

void ExpTest1()
{
	//1.6���Գɹ�
	//������ʱ��������ζ���
	rwlock.rd_lock();
	rwlock.rd_lock();
	
	//����һ���߳� ��δ�� ������ʱ�� �������
	rwlock.rd_unlock();
}

void ExpTest2()
{
	//������ʱ��������ζ���
	rwlock.wr_lock();
	rwlock.wr_lock();

	//����һ���߳� ��δ�� ������ʱ�� �������
	rwlock.wr_unlock();
}

void ExpTest3()
{
	//����һ���߳� �� ��  д����ʱ�� �������
	rwlock.wr_lock();
	rwlock.rd_unlock();

	//����һ���߳� �� ��  ������ʱ�� ���д��
	rwlock.rd_lock();
	rwlock.wr_unlock();
}
int main1234()
{
	actFuncMR();//���������߳�ͬʱ��ȡ
	actFuncMW();//���������߳�ͬʱд��
	actFuncMRW_();//���Զ�д���Ļ�����Ϊ
	actWriLockFrist();//����д�������ȼ�

	/*�쳣����  һ�����к������Ծ��ﵽԤ��*/
	ExpTest1();
	ExpTest2();
	ExpTest3();
	system("pause");
	return 1;
}