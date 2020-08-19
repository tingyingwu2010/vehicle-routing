#ifndef SOLUTION
#define SOLUTION

#include"Customer.h"
#include"Data.h"
#include"Node.h"
#include"Route.h"

class Solution {
private:
	double allTime;   //����������ܳ���ʱ��,һ��·��������ʱ�������������·���ľ�����ֵ���
	double allLength;  //�������������·���ϵľ���֮��
	vector<Route*> routeSet; //��������е�·����

public:
	Solution() {}
	//�Զ��忽������
	void printSolution();

	//ʹ��s�¹���һ���������
	void doCopy(Solution* s);

	void computeLength(Data* data);  //��������·�����ܳ���
	void computeTime();   //��������·�����ܳ���ʱ��

	bool isRegular(Data* d);

	/*Ѱ�ұ��Ϊu�Ŀͻ�����һ��·����*/
	void search(double u, int* route_u, Node** node_u);

	void do2Exchange(Route* r, Node* i, Node* i_succ, Node* j, Node* j_succ);
	void restore2Exchange(Route* r, Node* i, Node* i_succ, Node* j, Node* j_succ);

	void doOrExchange(Node* i_pred, Node* i, Node* i_succ, Node* j, Node* j_succ);
	void restoreOrExchange(Node* i_pred, Node* i, Node* i_succ, Node* j, Node* j_succ);

	void doRelocation(Node* i_pred, Node* i, Node* i_succ, Node* j, Node* j_succ);
	void restoreRelocation(Node* i_pred, Node* i, Node* i_succ, Node* j, Node* j_succ);

	void doExchange(Node* i_pred, Node* i, Node* i_succ, Node* j_pred, Node* j, Node* j_succ);
	void restoreExchange(Node* i_pred, Node* i, Node* i_succ, Node* j_pred, Node* j, Node* j_succ);

	void doCrossover(Node* i, Node* i_succ, Node* j, Node* j_succ);
	void restoreCrossover(Node* i, Node* i_succ, Node* j, Node* j_succ);

	friend class Solve;
};
#endif
