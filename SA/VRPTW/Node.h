#ifndef NODE
#define NODE

#include<iostream>
using namespace std;

#include<vector>
#include"Data.h"

class Node {
private:
	/*·���еĽڵ���Ҫ�洢����Ϣ*/
	double num;			//�ڵ���

	double a_t;       //ARRIVE TIME
	double w_t;       //WAIT TIME
	double b_t;       //��ʼ����ʱ�� b_t = a_t + d_t
	double d_t;       //DEPART TIME

	double z_t;       //������ʱ��

	Node* pred;
	Node* succ;

public:
	Node(){}

	void doCopy(Node* n)
	{
		num = n->num;

		a_t = n->a_t;
		w_t = n->w_t;
		b_t = n->b_t;
		d_t = n->d_t;

		z_t = n->z_t;
	}

	friend class Route;
	friend class Solution;
	friend class Solve;
};
#endif