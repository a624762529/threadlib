#pragma once
#include<iostream>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<vector>
#include<string>
using namespace std;
 
class Printf
{
public:
	void print_next_line();
	void print_string(string st);
	void print_string_num(string st, int a);
	void print_string_num(string st, thread::id a);
	void print_string_num(string st, int val,thread::id a);
	void print_string_num(string st, int val, string info,thread::id a);
private:
	mutex mu;
};
extern Printf pt;
 