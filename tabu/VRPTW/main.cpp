#include <iostream>
using namespace std;

#include "Data.h"
#include "Solve.h"
#include<string>
#include<time.h>

int main()
{
	clock_t start, finish;

	ofstream out;
	out.open("C:\\Users\\MiaoAYao\\Desktop\\tabu.csv", std::ios::app);

	string dir = "C:\\Users\\MiaoAYao\\Desktop\\tabu\\VRPTW\\Data\\";
	string file;
	cout << "�����ļ�����" << endl;
	getline(cin, file);

	/*
	out << "**************************************************" << endl;
	out << "*************���������㷨**************************" << endl;
	out << "***************************************************" << endl; */

	string file_name;
	ifstream in(file);

	while (!in.eof())
	{
		getline(in, file_name);
		cout << file_name << endl;

		//�����ļ��е����ݹ���ͻ��ڵ�
		Data data(dir + file_name);
		//���������Ϣ
		data.computeDis();

		//data.print();

		Solve solve(&data);
		//out << "=====================================" << endl;
		//out << file_name << ",";

		start = clock();

		/*��������ʽ�㷨*/
		solve.useInsertion();
		solve.tabuSearch();

		finish = clock();
		double duration = (double)(finish - start) / CLOCKS_PER_SEC;
		out  << duration << endl;
	}
	return 0;
}