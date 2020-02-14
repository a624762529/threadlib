#include"print_to_buf.h"
#include<iomanip>
using namespace std;
void Printf::print_next_line()
{
	lock_guard<mutex> lock(mu);
	cout << endl;
}
void Printf::print_string(string st)
{
	lock_guard<mutex> lock(mu);
	cout << st << endl;
}
void Printf::print_string_num(string st, int a)
{
	lock_guard<mutex> lock(mu);
	cout << st << a << endl;
}
void Printf::print_string_num(string st, thread::id a)
{
	lock_guard<mutex> lock(mu);
	cout << st << a << endl;
}

void Printf::print_string_num(string st, int val, thread::id a)
{
	lock_guard<mutex> lock(mu);
	cout<< setw(8) << st 
		<< setw(4) << "val" << setw(4) << val 
		<< setw(4) <<"pid" << setw(4) << a << endl;
}


void Printf::print_string_num(string st, int val, string info, thread::id a)
{
	lock_guard<mutex> lock(mu);
	cout << setw(8) << st
		 << setw(4) << "val" 
		 << setw(4) << val
		 << setw(4) << "pid" 
		 << setw(4)<<info
		 << setw(4) << a << endl;
}
Printf pt;