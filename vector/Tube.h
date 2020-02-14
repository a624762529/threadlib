#pragma once
#include"Sem_Pv.h"
#include<list>
class TubeImple
{
public:
	virtual void doit()
	{}
};

#define MAGIC 11
class Tube
{
public:
	Tube(int free_size,int nofreesize=0);
	~Tube();
	int size();
	void put(shared_ptr<TubeImple> pt);
	shared_ptr<TubeImple> get();
public:
	Tube(const Tube&) = delete;
	Tube(const Tube&&) = delete;
	void operator=(const Tube&) = delete;
	void operator=(const Tube&&) = delete;
	void changeMaxSize(int size);
private:
	int magic;
	Sem_Pv m_free;
	Sem_Pv m_nofree;
	mutex  m_mu;
	list<shared_ptr<TubeImple>> m_task_vec;
};

