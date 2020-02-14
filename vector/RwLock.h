#pragma once
#include<iostream>
#include<mutex>
#include<thread>
#include<condition_variable>
#include<atomic>
#include<string>
#include<atomic>
#include"print_to_buf.h"
using namespace std;
#define start 666

class RW_Lock
{
public:
	/*
		��д����Ч��:
			������ס��ʱ�� ֻ���ж���ǰ��
			д����ס��ʱ�� ֻ���д���
	*/
	 
	RW_Lock();
	~RW_Lock();
	RW_Lock(const RW_Lock&) = delete;
	void operator=(const RW_Lock&) = delete;
	RW_Lock(const RW_Lock&&) = delete;
	void operator=(const RW_Lock&&) = delete;

	void rd_lock();
	void rd_unlock();
	void wr_lock();
	void wr_unlock();
private:
	class Map_Node
	{
	public:
		Map_Node(char stata) 
		{
			switch (stata)
			{
			case read:
				break;
			case write:
				break;
			case free:
				break;
			default:
				cout << "д���쳣" << endl;
				throw bad_alloc();
				break;
			}
			id = this_thread::get_id(); 
			this->stata = stata;
		}
	public:
		thread::id id;
		char       stata;
		enum {
			read, write, free
			//��ȡ����  ��ȡд��  ����״̬
		};
		bool operator==(const Map_Node &nd) { return this->id == nd.id; };
		bool operator==(const thread::id id) { return this->id == id; };
	};
private:
	char getNode    (thread::id id);          //��ȡvec�нڵ��״̬
	bool changeNode(thread::id id, char val);//�ı��Ӧ�ڵ��״̬
	void addNode(const Map_Node nd);		 //��vec�м���ڵ�
	void rmNode(thread::id id);				 //�Ƴ��ڵ�
private:
	int maigic;		//��������
	mutex mu;		
	condition_variable con_read;
	atomic<int> waiting_read;	//���̵߳ĵȴ��ĸ���
	atomic<int> reading;		//�ж��ٸ��߳����ڶ�����
	condition_variable con_write;
	atomic<int> waiting_write;  //д�̵߳ȴ�����Ŀ
	atomic<int> writing;		//�ж����߳�������д״̬
	mutex	    vec_lock;
	vector<Map_Node> vec_node;

};
