#include"Tube.h"
#include"print_to_buf.h"
#include<iostream>
using namespace std;
Tube tu(8, 0);
/*
	基于C++11实现的管程
*/
class Test :public TubeImple
{
public:
	Test(int i)
	{
		this->i = i;
	}
	void doit() override
	{
		pt.print_string_num("doit", i);
		this_thread::sleep_for(1s);
	}
private:
	int i;
};

atomic<int> r = 0;
mutex mu;
void put()
{
	for (size_t i = 0; i < 100; i++)
	{
		//pt.print_string_num("put", i);
		{
			lock_guard<mutex> lock(mu);
			tu.put(shared_ptr<Test>(new Test(r)));
			r++;
		}
	}
	return;
}
void get()
{
	while (true)
	{
		auto i = tu.get();
		if (i == nullptr)
		{
			break;
		}
		i->doit();
	}
	cout << "get over" << endl;
}
int main12345()
{
	thread th[4];
	th[0] = std::move(thread(put));
	th[1] = std::move(thread(get));

	th[2] = std::move(thread(put));
	th[3] = std::move(thread(get));
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}
	return 1;
}

 
mutex mu_think[5];
//使用tuple测试哲学家就餐模型
class Test1 :public TubeImple
{
public:
	Test1(int i)
	{
		this->i = i;
	}
	void doit() override
	{
		//lock_guard<mutex> lock1(mu_think[i]);
		mu_think[i].lock();
		if (i + 1 == 5)
		{
			//lock_guard<mutex> lock1(mu_think[0]);
			mu_think[0].lock();
		}
		else
		{
			//lock_guard<mutex> lock1(mu_think[i + 1]);
			mu_think[i + 1].lock();
		}
		this_thread::sleep_for(1s);
		pt.print_string_num("哲学家:", i);
		if (i + 1 == 5)
		{
			//lock_guard<mutex> lock1(mu_think[0]);
			mu_think[0].unlock();
		}
		else
		{
			//lock_guard<mutex> lock1(mu_think[i + 1]);
			mu_think[i + 1].unlock();
		}
		//lock_guard<mutex> lock1(mu_think[i]);
		mu_think[i].unlock();

		//pt.print_string_num("think", i);


	}
private:
	int i;
};
class ThinkEat
{
public:
	ThinkEat() :tb(5)
	{}
	void fillTube()
	{
		tb.put(shared_ptr<Test1>(new Test1(0)));
		tb.put(shared_ptr<Test1>(new Test1(2)));
		tb.put(shared_ptr<Test1>(new Test1(1)));
		tb.put(shared_ptr<Test1>(new Test1(3)));
		tb.put(shared_ptr<Test1>(new Test1(4)));

		//2:3006 3:3006  4:3006  5:3009
	/*
		tb.put(shared_ptr<Test1>(new Test1(0)));
		tb.put(shared_ptr<Test1>(new Test1(1)));
		tb.put(shared_ptr<Test1>(new Test1(2)));
		tb.put(shared_ptr<Test1>(new Test1(3)));
		tb.put(shared_ptr<Test1>(new Test1(4))); */
		//2:4006 3:3007 4:3005  5:3005
	}
	void eat()
	{
		while (true)
		{
			auto i = tb.get();
			if (!i)
			{
				break;
			}
			i->doit();
		}
	}

	void start()
	{
		fillTube();
		thread th[5];
		int index = 3;
		for (int i = 0; i < index; i++)
		{
			th[i] = std::move(thread(&ThinkEat::eat, this));
		}

		for (int i = 0; i < index; i++)
		{
			tb.put(shared_ptr<Test1>(nullptr));
		}
		for (int i = 0; i < index; i++)
		{
			th[i].join();
		}
	}
private:
	Tube tb;
};
int main()
{
	ThinkEat thinker;
	int time = 0;
	for (int i = 0; i < 5; i++)
	{
		int pre = clock();
		thinker.start();
		int _time = clock() - pre;
		cout << "time access" <<_time  << endl;
		time += _time;
	}
	cout << "aveg time" << time / 5 << endl;
	system("pause");
	return 1;
}


void thinker(int i)
{
	if (i % 2 == 0)
	{
		//lock_guard<mutex> lock1(mu_think[i]);
		mu_think[i].lock();
		if (i + 1 == 5)
		{
			//lock_guard<mutex> lock1(mu_think[0]);
			mu_think[0].lock();
		}
		else
		{
			//lock_guard<mutex> lock1(mu_think[i + 1]);
			mu_think[i + 1].lock();
		}
	}
	else
	{
		if (i + 1 == 5)
		{
			//lock_guard<mutex> lock1(mu_think[0]);
			mu_think[0].lock();
		}
		else
		{
			//lock_guard<mutex> lock1(mu_think[i + 1]);
			mu_think[i + 1].lock();
		}
		//lock_guard<mutex> lock1(mu_think[i]);
		mu_think[i].lock();
	}
	
	//this_thread::sleep_for(1s);
	if (i + 1 == 5)
	{
		//lock_guard<mutex> lock1(mu_think[0]);
		mu_think[0].unlock();
	}
	else
	{
		//lock_guard<mutex> lock1(mu_think[i + 1]);
		mu_think[i + 1].unlock();
	}
	//lock_guard<mutex> lock1(mu_think[i]);
	mu_think[i].unlock();
	
	//pt.print_string_num("think", i);
}
int main34567()
{
	int tv = 0;
	for (int i = 0; i < 10000; i++)
	{
		int time_now = clock();
		thread th[5];
		for (int i = 0; i < 5; i++)
		{
			th[i] = std::move(thread(thinker, i));
		}
		for (int i = 0; i < 5; i++)
		{
			th[i].join();
		}
		tv += clock() - time_now;
		//cout << "access" << clock() - time_now << endl;
	}
	cout << "ave time" << (double)tv / 10000 << endl;
	system("pause");
	return 1;
}