#ifndef DATA
#define DATA

#include<iostream>
#include<fstream>
using namespace std;

#include"Customer.h"

class Data {
private:
	vector<Customer*> data;					//��¼�ڵ����Ϣ
	vector<vector<double>> dis;         //��¼ÿ�����ڵ�֮��ľ���

public:
	Data() {}
	Data(string s);
	void buildGrid(double DIV);
	void computeDis();
	double distance(double cus_i, double cus_j);   //���ؿͻ�cus_i��cus_j֮��ľ���

	void printDistance();

	friend class Node;
	friend class Route;
	friend class Solution;
	friend class Solve;
};
#endif