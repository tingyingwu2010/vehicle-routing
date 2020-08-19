#ifndef SOLVE
#define SOLVE

#include<fstream>
#include<iostream>
using namespace std;

#include"Node.h"
#include"Route.h"
#include"Solution.h"
#include"Insertion.h"

#include<vector>
#include<ctime>

class Solve {
public:
	Solve() {};
	Solve(Data* d);

	/*��������ʽ�㷨*/
	void useInsertion();
	void initRoute(int flag);
	void initInfor();
	void updateInfor(Route* route);
	void printInfor();

	/*�����ھӽ�������ļ���*/
	void generateNeighbour(int operate, double num, Solution* n_s);
	void generate2Exchange(double num, Solution* n_s);
	void generateOrExchange(double num, Solution* n_s);
	void generateRelocation(double num, Solution* n_s);  //ֻ����������п��ܻ����·����
	void generateExchange(double num, Solution* n_s);
	void generateCrossover(double num, Solution* n_s);

	void changeToNeighbour(vector<double>* n, Solution* temp_s);


	/*���������㷨*/
	void tabuSearch();
	/*��������*/
    //�ж��Ƿ��ڽ��ɱ��У����򷵻�true�����򷵻�false
	bool isTabu(vector<double>* n);
	void updateTabuList(int iteration);
	void addToTabuList(int iteration, vector<double>* n);
	void reverseOperate(vector<double>** r_n, vector<double>* n, Solution* n_s);

	void record(Solution* temp_s);

private:
	//���ļ��ж�ȡ������
	vector<Customer*>* cus;
	Data* data;

	//��õĽ������
	Solution s;

	/*��������ʽ�㷨��Ҫ�洢����Ϣ*/
	vector<Insertion> infor; /*δ����·���Ľڵ��ţ���õĲ���λ�ã�����ýڵ�����ӵĿ���*/

	/*���������㷨*/
	vector<vector<double>*> tabu_list;
	int tenure;  //��������

	//��õ��ھӽ�������Ŀ���
	vector<double> best_neighbour;
	double g_best_neighbour;

	friend class Solution;
};

#endif
