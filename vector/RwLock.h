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
		读写锁的效果:
			读锁锁住的时候 只能有读锁前来
			写锁锁住的时候 只能有串行
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
				cout << "写入异常" << endl;
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
			//获取读锁  获取写锁  空闲状态
		};
		bool operator==(const Map_Node &nd) { return this->id == nd.id; };
		bool operator==(const thread::id id) { return this->id == id; };
	};
private:
	char getNode    (thread::id id);          //获取vec中节点的状态
	bool changeNode(thread::id id, char val);//改变对应节点的状态
	void addNode(const Map_Node nd);		 //在vec中加入节点
	void rmNode(thread::id id);				 //移除节点
private:
	int maigic;		//启动参数
	mutex mu;		
	condition_variable con_read;
	atomic<int> waiting_read;	//读线程的等待的个数
	atomic<int> reading;		//有多少个线程正在读数据
	condition_variable con_write;
	atomic<int> waiting_write;  //写线程等待的数目
	atomic<int> writing;		//有多少线程正处于写状态
	mutex	    vec_lock;
	vector<Map_Node> vec_node;

};
