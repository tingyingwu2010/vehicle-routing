#ifndef ROUTE
#define ROUTE

#include"Node.h"
#include"Customer.h"
#include"Data.h"

class Route {
private:
	double demand;   //·���ϵ�������
	int size;        //·���Ͻڵ����Ŀ
	Node* head;      //·���ϵ�һ���ڵ�
	Node* tail;      //·�������һ���ڵ�

public:
	Route(){}
	Route(double u, Data* d);      //�ñ��Ϊnum�Ľڵ��ʼ��һ��·��

	Node* searchNode(double num);

	double computeNewAT(double u, double dt_u, Node* p_j, Data* d);

	//�ж�·���Ͻڵ�p_j�Լ������Ľڵ��Ƿ����
	//uΪǰһ���ڵ�ı�ţ�dt_uΪ�ڵ�u���뿪ʱ��
	//������·���ķ���û�з����ı�����
	bool isFeasible(double u, double dt_u, Node* p_j, Data* d);
	//����һ��������Ƿ���У�����Ӧ�����κ����
	bool isFeasibleGC(double u, double dt_u, Node* p_j, Data* d);

	//��·��r��ӵ���ǰ·�������Ƿ����
	//����ǰ·�����з�ת�õ���ʱ��ʹ����һ�����ж�������
	bool isFeasibleAddRoute(Route* r, Data* d);

	//����·���ϴ�p_j��ʼ�Ľڵ���Ϣ
	void updateRoute(Node* p_j, Data* d);

	void reverseRoute();	//��תȫ��·��
	//�ڵ�pi�ڽڵ�pjǰ��
	void reversePartialRoute(Node* pi, Node* pj); //��ת·���е�һ����,����ת�ڵ�pi���ڵ�pj��·��

	//��·��j�ϵĽڵ�ϲ���·��i��
	void mergeRoute(Route* p_j, Data* d);

	void printRoute();

	friend class Solution;
	friend class Solve;
};
#endif
