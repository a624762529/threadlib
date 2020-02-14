//#include<iostream>
//#include<mutex>
//#include<condition_variable>
//#include<thread>
//#include<vector>
//#include<list>
//#include<memory>
//#include<atomic>
//#include<functional>
//#include"log_dothing.h"
//#include"print_to_buf.h"
//#include"date_pack_.h"
//using namespace std;
//
//class TaskPack
//{
//public:
//	virtual void doit() = 0;
//};
//
//
//class TestRealize:public TaskPack
//{
//public:
//	TestRealize(int i)
//	{
//		this->i = i;
//	}
//	TestRealize() 
//	{}
//	void doit() override
//	{
//		action();
//	}
//	void action()
//	{
//		pt.print_string_num("doit", this_thread::get_id());
//		this_thread::sleep_for(5s);
//	}
//	int i;
//};
///*
//	Ϊ�˸��õ�ʹ��������� 12.31�� �Ҿ�����������е�ģ��
//	ʹ֮����Ѹ��
//*/
//
// 
//class ThreadPool
//{
//public:
//	ThreadPool();
//	~ThreadPool();
//	void consum();
//	void put(shared_ptr<TaskPack> putarg);
//private:
//	bool isEmpty();
//	bool isOverFlow();
//	 
//	shared_ptr<TaskPack> get();
//	shared_ptr<TaskPack> getVec();
//private:
//	vector<thread> vec_thread;				//û����
//	mutex mu;
//	mutex m_task;							//��� ���������Ƶ���
//	list<shared_ptr<TaskPack>> m_taskque; //�������
//	enum   {
//		max_num_vec = 7,
//		min_num_vec = 0
//	};
//	condition_variable m_convec;
//	condition_variable m_provec;
//};
// 
//ThreadPool::ThreadPool()
//{
//	vec_thread.resize(4);
//	for (vector<thread>::iterator it = vec_thread.begin();
//		it != vec_thread.end(); it++)
//	{
//		//���������� ��ʼ��ʱ�� ����������
//		thread th = std::move(thread(&ThreadPool::consum, this));
//		th.detach();
//		*(it) = std::move(th);
//	}
//}
//
//ThreadPool::~ThreadPool()
//{
//}
// 
//void ThreadPool::consum()
//{
//	while (true)
//	{
//		auto ret = this->get();
//		ret->doit();
//	}
//}
// 
// 
//bool ThreadPool::isEmpty()
//{
//	return m_taskque.size() == this->min_num_vec;
//}
// 
//bool ThreadPool::isOverFlow()
//{
//	return m_taskque.size() == this->max_num_vec;
//}
// 
//void ThreadPool::put(shared_ptr<TaskPack> putarg)
//{
//	//�����Ĺ��� ��putarg���뵽�������
//	unique_lock<mutex> lock(mu);
//	while (isOverFlow())
//	{
//		pt.print_string("���ݶ����Ѿ�����");
//		m_provec.wait(lock);
//	}
//	m_taskque.push_back(putarg);
//	m_convec.notify_one();
//}
// 
//shared_ptr<TaskPack> ThreadPool::get()
//{
//	unique_lock<mutex> lock(mu);
//	while (isEmpty())
//	{
//		pt.print_string("���ݶ����Ѿ�����");
//		m_convec.wait(lock);
//	}
//	auto ret= getVec();
//	if (ret != nullptr)
//	{
//		m_provec.notify_one();
//	}
//	return ret;
//
//}
// 
//shared_ptr<TaskPack> ThreadPool::getVec()
//{
//	//��vec�л�ȡ����  ����:�׳����ײ���Ԫ�� ����֮
//	lock_guard<mutex> lock(m_task);
//	if (m_taskque.size())
//	{
//		auto ret = (*m_taskque.begin());
//		m_taskque.pop_front();
//		return ret;
//	}
//	else
//	{
//		return nullptr;
//	}
//}
//
////���԰汾һ���̳߳� 12.31���Գɹ�
//int main123456()
//{
//	ThreadPool thpool;
//	
//	for (int i = 0; i < 100; i++)
//	{
//		thpool.put(shared_ptr<TaskPack>(new TestRealize(i)));
//	}
//	
//	while (true)
//	{
//		this_thread::sleep_for(5s);
//	}
//	return 1;
//}