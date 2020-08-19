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

	/*��ʡ����ʽ�㷨*/
	void useSavings();  //ʹ�ý�ʡ����ʽ�㷨������ʼ�������
	void buildSavings();	//�����ʡ
	void deleteSavings(int i = 0, int j = 0);	//ɾ���ߵĶ���Ϊi����j��savingֵ��û�в�����Ĭ��ֻɾ��savings�ĵ�һ��Ԫ��
	void downAdjust(int i);	//�������������Ҫɾ���Ľڵ���������ֵ���ǣ������µ���
	void printSaving(); 

	/*��������ʽ�㷨*/
	void useInsertion();
	void initRoute(int flag);
	void initInfor();
	void updateInfor(Route* route);
	void printInfor();

	/*or-opt�㷨*/
	/*�Ȱ�������·���ϵ�Or����
	�ְ�������·��֮���Or����*/
	bool useOrOpt();
	void OrExchangeNeighbour(Node* node_u, int r_num);
	void localSearch();

	/*Russell(1995)*/
	/*ʹ�þֲ������㷨�����еĽ��������������*/
	void localSearch1();
	void computeU();
	void printU();

	/*Russell(2004)*/
	/*ģ���˻��㷨*/
	void minimizeRoute();
	void simulatedAnneal();
	//�ж�e1<e2,����true
	bool compareE(vector<double>* e1, vector<double>* e2);

	/*�����ھӽ�������ļ���*/
	void generateNeighbour(int operate, double num, Solution* n_s);
	void generate2Exchange(double num, Solution* n_s);
	void generateOrExchange(double num, Solution* n_s);
	void generateRelocation(double num, Solution* n_s);  //ֻ����������п��ܻ����·����
	void generateExchange(double num, Solution* n_s);
	void generateCrossover(double num, Solution* n_s);

	void changeToNeighbour(vector<double>* n, Solution* temp_s);

	/*����Ŀ�꺯��ֵ*/
	void computeEvaluation(vector<double>** e,vector<double>* n, Solution* n_s);
	
	/*������С��·����Ŀ��Ŀ�꺯��ֵ�����а���������ֵ*/
	/*�����һ��Ŀ�꺯��ֵ*/
	double computeFirstEvaluation(vector<double>* n, Solution* n_s);
	/*����ڶ���Ŀ�꺯��ֵ*/
	double computeSecondEvaluation(vector<double>* n, Solution* n_s);
	/*���������Ŀ�꺯��ֵ*/
	double computeThirdEvaluation(vector<double>* n, Solution* n_s);
	//����������n_s����Сʱ���ӳ�
	double computeMinimalDelay(Solution* n);
	double computeLatestArriveTime(Node* node_j_succ); //����·����������ڵ�node_j_succ��ʱ��

	/*����Ŀ�꺯��ֵ���ھӽ�����������*/
	void computeOrder();

	void minimizeTravelCost();
	void selectCustomers(vector<double>* cus_set, int n);
	void orderRelatedness(vector<vector<double>>* relatedness);

	/*���������㷨*/
	void tabuSearch();
	void generateCross(Solution* n_s);
	/*��������*/
    //�ж��Ƿ��ڽ��ɱ��У����򷵻�true�����򷵻�false
	bool isTabu(vector<vector<double>*>* tabu_list, vector<double>* n);
	void updateTabuList(vector<vector<double>*>* tabu_list, int iteration, int tenure);
	void addToTabuList(vector<vector<double>*>* tabu_list, int iteration, vector<double>* n);
	void reverseOperate(vector<double>** r_n, vector<double>* n, Solution* n_s);

	void record(Solution* temp_s);

private:
	//���ļ��ж�ȡ������
	vector<Customer*>* cus;
	Data* data;

	//��õĽ������
	Solution s;

	/*��ʡ����ʽ�㷨��Ҫ�洢����Ϣ*/
	vector<vector<double> *> savings;	//��¼��ʡ����Ϣ

	/*��������ʽ�㷨��Ҫ�洢����Ϣ*/
	vector<Insertion> infor; /*δ����·���Ľڵ��ţ���õĲ���λ�ã�����ýڵ�����ӵĿ���*/

	/*Russell(1995)������㷨����Ҫ�洢����Ϣ*/
	vector<vector<double>> u;

	/*Russell(2004)������㷨����Ҫ�洢����Ϣ*/
	/*����С��·����ĿΪĿ������Ҫ�洢����Ϣ*/
	vector<vector<double>> evaluation;
	vector<vector<double>*> neighbour;
	vector<int> order;   //����evaluation��С���ھӽ�������

	//Ŀ�꺯��ֵΪ�ڶ�������
	vector<vector<double>> g;   //�洢Ŀ�꺯��ֵ

	friend class Solution;
};

#endif
