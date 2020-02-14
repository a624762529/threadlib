#include "cTaskPack.h"


solveTaskEd::solveTaskEd()
{
	task = "solveTaskEd";
}
void solveTaskEd::doit() {
	//cout << "----->end" << endl;
}

SpyInfoPack::SpyInfoPack() {}

SpyInfoPack::SpyInfoPack(int tm, string info, int work)
{
	m_num = tm;
	m_info = info;
	working = work;
}

SpyInfoPack::SpyInfoPack(const SpyInfoPack& arg)
{
	m_num = arg.m_num;
	m_info = arg.m_info;
	working = arg.working;
}
bool SpyInfoPack::operator==(const SpyInfoPack& pr)
{
	return this->m_info == pr.m_info;
}

void SpyInfoPack::overThread()
{
	working = false;
}
void SpyInfoPack::upperThreadTm()
{
	m_num++;
}






