#include"ThreadPool.h"
#include<iostream>
using namespace std;
 
class TubeTest1 :public ThreadPoolImpl
{
public:
	void doit() override
	{
		pt.print_string_num("   test:", this_thread::get_id());
		this_thread::sleep_for(1s);
	}
};
int main01()
{
	ThreadPool th_pool(10);
	th_pool.startPool();


	//测试线程池中线程数目的增加 减少 增加
	for (int i = 0; i < 10000; i++)
	{
		pt.print_string_num("insert:", i);
		th_pool.put(shared_ptr<TubeTest1>(new TubeTest1()));
		if (i > 200&&i<220)
		{
			this_thread::sleep_for(5s);
		}
		else if (i > 210)
		{
		}

	}

	while (true)
	{
		this_thread::sleep_for(1s);
	}
	system("pause");
	return 1;
}

//测试线程池的关闭
int main45678()
{
	
	ThreadPool th_pool(10);
	th_pool.startPool();
	for (int i = 0; i < 50; i++)
	{
		pt.print_string_num("insert:", i);
		th_pool.put(shared_ptr<TubeTest1>(new TubeTest1()));
	}
	th_pool.stopAtonce();
	system("pause");
	return 1;
}

 