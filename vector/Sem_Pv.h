#pragma once
#ifndef SEM_PV_H
#define SEM_PV_H
#include<thread>
#include<condition_variable>
#include<iostream>
#include<atomic>

using namespace std;

/*
	��Ȼ���Ѿ������ �ѽ�������ϵͳд�ɶ��߳�
	��˶����˵ʹ��������������Ǧ�ʵ���������
	��Ȼ��� �������Լ�дһ��pv��
	����ʡ���ľ��� ��Ȼ������Linux�Դ���api ���� ������  ϵͳ��ʵ�ֻ����Ͼ���C++
	�����ž������ܵ���C++ ���½����ٶȸ���������ʱ�� ʹ�ú�ȥ��������linux api
	�Ҿ���C++11�����Ӱ�
*/

/*
	ʵ��ԭ��
		ʹ�������� ��������ʵ��
	�ź����ĳ�ֵһ���趨���ݸı�

	�������pv�� ��֧��+1 -1
*/
#define MagicNum 11
//��magicΪ11��ʱ�򷽿������ź���
class Sem_Pv
{
public:
	Sem_Pv();
	Sem_Pv(int val);
	~Sem_Pv();
	void set_CanUse();
	void sem_P();      //put ���ź����д��Ԫ��
	void sem_Try_P();  //���������ź����д���Ԫ��
	void sem_V();      //get ���ź�����ȡ��Ԫ��
	bool sem_Try_V();  //���������ź�����ȡ��Ԫ��
	void set_Val(int val);
private:
	int magic;                    //�����ź����Ƿ����
	atomic<int> waiting_num;      //�ȴ����߳���Ŀ

	atomic<int> free_qua; //��ǰ���ź����� �м���������Դ
	mutex mu;
	condition_variable con;

	mutex mx;
};

#endif // SEM_PV_H
