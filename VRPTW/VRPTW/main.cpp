#include <iostream>
using namespace std;

#include "Data.h"
#include "Solve.h"
#include<string>
#include<direct.h>

#define DIV 6

int main()
{
	/*��ȡ��ǰĿ¼
	char* direct = new char(100);
	cout << _getcwd(direct, 100) << endl;*/
	ofstream out;
	out.open("C:\\Users\\MiaoAYao\\Desktop\\insertion.txt", std::ios::app);

	string dir = "C:\\Users\\MiaoAYao\\Desktop\\VRPTW\\VRPTW\\Data\\";
	string file;
	cout << "�����ļ�����" << endl;
	getline(cin, file);

	string file_name;
	ifstream in(file);

	/*
	out << "*******************************************" << endl;
	out << "-------------��ʡ����ʽ�㷨-----------------" << endl;
	out << "*******************************************" << endl;
	out << endl;
	out << endl;
	*/

	while (!in.eof())
	{
		getline(in, file_name);
		cout << file_name << endl;

		//�����ļ��е����ݹ���ͻ��ڵ�
		Data data(dir + file_name);
		//�Կͻ��ڵ���зָ�
		data.buildGrid(DIV);
		//���������Ϣ
		data.computeDis();
		//���������Ϣ
		//data.printDistance();

		Solve solve(&data);

		/*ѡ��ʹ������·�������㷨
		int flag;
		cout << "-----------ѡ��·�������㷨-------------" << endl;
		cout << "-----------���ѡ���ʡ����ʽ�㷨���룺1------------" << endl;
		cout << "-----------���ѡ���������ʽ�㷨���룺2------------" << endl;
		cin >> flag;

		if (flag == 1)
		{
			solve.buildSavings();     //���ݾ�����Ϣ�����ʡ
			solve.useSavings();
		}
		else if (flag == 2)
		{
			solve.useInsertion();
			//problem.localSearch1();
		}
		*/

		/*ʹ�ý�������Ľ��㷨�Խ���������иĽ�
		cout << "-----------ѡ���Ƿ����·����Ŀ��С��---------------" << endl;
		cout << "-----------�������·����Ŀ��С�����룺1------------" << endl;
		cout << "-----------�������룺2-----------------------------" << endl;
		cin >> flag;
		if(flag == 1)
			solve.minimizeRoute(); */

		//solve.localSearch();
		
		/*��ʡ����ʽ�㷨*/
		/*solve.buildSavings();     //���ݾ�����Ϣ�����ʡ
		solve.useSavings();*/


		out << "=====================================" << endl;
		out << file_name << endl;

		/*��������ʽ�㷨*/
		solve.useInsertion();

		//��ͳ�ľֲ������㷨
		//solve.localSearch();

		//solve.simulatedAnneal();
		//solve.tabuSearch();

	}

	/*ʹ��R101.txt���в���
	string file_name = "R101.txt";*/

	/* �Լ�ѡ���ļ�
	cout << "Enter the name of the file" << endl;
	getline(cin, file_name);
	ifstream in(file_name); */

	
	return 0;
}