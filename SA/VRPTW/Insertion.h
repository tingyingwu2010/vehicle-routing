#ifndef INSERTION
#define INSERTION

#include<iostream>
using namespace std;

#include<vector>
#include"Node.h"

class Insertion {
private:
	/*������ĿҪ�洢����Ϣ*/
	double u_num;			//�ڵ���
	
	Node* u_pred;
	Node* u_succ;

	double cost;

public:
	Insertion() {}

	friend class Solve;
};
#endif
