#include "Solve.h"

#include<math.h>

#define DEMAND 1000

//��������ʽ�㷨��Ҫ�Ĳ���
/*1 1 1 0
  1 2 1 0
  1 1 0 1
  1 2 0 1*/
#define U1 1
#define U2 1      
#define M1 1
#define M2 0

  //Russell(1995)�������
#define V1 0.5
#define V2 0.5

//Russell(2004)�������
#define T 3000 //��ʼ�¶�
#define EPS 1e-8 //��ֹ�¶�
#define R 0.98 //�¶Ƚ�������

void Solve::record(Solution* temp_s)
{
	ofstream out;
	out.open("C:\\Users\\MiaoAYao\\Desktop\\insertion.txt", std::ios::app);

	/*
	out << "·����Ŀ��";
	out << temp_s->routeSet.size() << endl;
	out << "·�̳���ʱ�䣺";
	out << temp_s->allTime << endl;
	out << "·���ܳ��ȣ�";
	out << temp_s->allLength << endl;
	if (temp_s->isRegular(data))
		out << "�ý�������ϸ�..." << endl;
	else
		out << "�ý���������ϸ�..." << endl;
	out << "-------------------------------------" << endl;*/

	out << temp_s->routeSet.size() << "  " << temp_s->allLength * 0.7 + temp_s->allTime * 0.3;
	out << endl;

}

Solve::Solve(Data* d)
{
	cus = &(d->data);
	data = d;
}

/*------------------------------------------------------------------------------*/
/*-------------------------------solomon(1987)----------------------------------*/
/*--------------------------��ʡ����ʽ·�������㷨-------------------------------*/
/*------------------------------------------------------------------------------*/
void Solve::buildSavings()
{
	vector<double> *saving;
	int size = cus->size();

	//�����ʡ��ֵ
	for (int i = 1; i < size - 1; ++i)
	{
		for (int j = i + 1; j < size; ++j)
		{
			//ֻ����ͬһ������ʱ�ż������
			if ((*cus)[i]->box_x == (*cus)[j]->box_x && (*cus)[i]->box_y == (*cus)[j]->box_y)
			{
				saving = new vector<double>();
				saving->push_back(data->distance(i + 1, 1) + data->distance(j + 1, 1) + data->distance(i + 1, j + 1));
				saving->push_back(i + 1);
				saving->push_back(j + 1);

				savings.push_back(saving);
				// ��savings���е���
				int m = savings.size();
				if (m > 1)
				{
					int n = m - 1;
					while (n > 0)
					{
						int nn = (n - 1) / 2;
						if ((*savings[n])[0] > (*savings[nn])[0])
						{
							vector<double> *temp;
							temp = savings[n];
							savings[n] = savings[nn];
							savings[nn] = temp;
						}
						n = nn;
					}
				}
			}
		}
	}
	cout << "Finish building the savings..." << endl;
}

void Solve::printSaving()
{
	for (int i = 0; i < savings.size(); ++i)
	{
		for (int j = 0; j < savings[i]->size(); ++j)
			cout << (*savings[i])[j] << "   ";

		cout << endl;
	}
}

void Solve::deleteSavings(int cus_i, int cus_j)
{
	//i��j�����ܵ���0����Ϊ0����ֿ⡣�������������˵��ֻɾ��������ġ���������������ɾ��������ģ���ɾ�������ġ�
	vector<double> * tmp = savings.back();
	savings[0] = tmp;
	savings.pop_back();
	downAdjust(0);

	int size = savings.size();
	for (int i = size - 1; i >= 0; --i)
	{
		vector<double>* temp = savings[i];
		if ((*temp)[1] == cus_i || (*temp)[1] == cus_j || (*temp)[2] == cus_i || (*temp)[2] == cus_j)
		{
			//cout << "***************************************" << endl;
			tmp = savings.back();
			savings[i] = tmp;
			savings.pop_back();
			downAdjust(i);
		}
	}
}

void Solve::downAdjust(int i)
{
	int size = savings.size();
	int j = i;
	while(true)
	{
		int maxSav = j * 2 + 1;
		if (maxSav >= size)
			break;

		vector<double>* temp = savings[j];
		if (maxSav + 1 < size)
		{
			if ((*savings[maxSav])[0] < (*savings[maxSav + 1])[0])
				maxSav = maxSav + 1;
		}

		if ((*temp)[0] < (*savings[maxSav])[0])
		{
			savings[j] = savings[maxSav];
			savings[maxSav] = temp;

			j = maxSav;
		}
		else
			break;
	}
}

void Solve::useSavings()
{
	Route *route;

	//Build original solution
	for (int i = 1; i < (*cus).size(); ++i)
	{
		route = new Route(i + 1, data);
		s.routeSet.push_back(route);
	}

	//��ʼ�������
	cout << "-------------------------��ʼ�������--------------------------" << endl;
	s.computeTime();
	s.computeLength(data);
	s.printSolution();
	cout << "---------------------------------------------------------------" << endl;

	//Build better solution
	while (savings.size() != 0)
	{
		cout << "--------------------------------------------" << endl;
		cout << "Savings����Ŀ��Ϊ�� " << savings.size() << endl;
		//printSaving();

		/* �����cus_i -> cus_j���ܹ���ʡ������ıߵ���Ϣ*/
		int cus_i = (*savings[0])[1];
		int cus_j = (*savings[0])[2];
		double sav = (*savings[0])[0];

		cout << "��̽�����ӽڵ㣺" << cus_i << "��" << cus_j << endl;

		/* �ҵ��������ϵ������ڵ�ֱ����ĸ�·���� */
		/* route_i:�����ڵ�i��·��   route_j:�����ڵ�j��·��
		   flag_i = 0:�ڵ�i��·��ͷ  flag_i = 1:�ڵ�i��·��β*/
		   /* del_i = 0:��ʾ�ϲ�����·���󣬽ڵ�i����·��ͷ��β����֮del_i = 1*/
		int route_i = -1, route_j = -1;
		Node* node_i = NULL;
		Node* node_j = NULL;
		int flag_i = -1, flag_j = -1;
		int del_i = 0, del_j = 0;

		//�ҵ�·�����а����ͻ�cus_i��cus_j������·��
		s.search(cus_i, &route_i, &node_i);
		s.search(cus_j, &route_j, &node_j);

		if (node_i == (s.routeSet[route_i]->head)->succ)
			flag_i = 0;
		else if (node_i == (s.routeSet[route_i]->tail)->pred)
			flag_i = 1;

		if (node_j == (s.routeSet[route_j]->head)->succ)
			flag_j = 0;
		else if (node_j == (s.routeSet[route_j]->tail)->pred)
			flag_j = 1;

		if (s.routeSet[route_i]->size > 1)
		{
			del_i = 1;
		}
		if (s.routeSet[route_j]->size > 1)
		{
			del_j = 1;
		}

		//�������������saving���ı��ϵ������ͻ����Ѿ���һ��·�����ˡ�
		if (route_i == route_j)
		{
			deleteSavings();
			continue;
		}


		/* �ϲ�·�� */
		/* �Ƿ����Ҫ�ϲ�·����Ҫ�����ж�һ�¿����� */
		/* 1. ·���ϵĽڵ�������Ƿ񳬹�������������*/
		/* 2. ·���ϵĽڵ��Ƿ�Υ��ʱ�䴰Լ��*/
		/* ������У���ô����·���ĺϲ�������ɾ����ǰ��ʡ*/
		if (s.routeSet[route_i]->demand + s.routeSet[route_j]->demand > DEMAND)
		{
			//·���ϵĽڵ�����󳬹�������������
			deleteSavings();
			continue;
		}

		if (flag_i == 0 && flag_j == 1)
		{
			/* Ҫ����һ��ĵ�Ϊi��j��i��·��route_i��ͷ��j��·��route_j��β
			   �������ӷ�ʽ��
			   1.��route_i�ϵĽڵ�ӵ�route_j��
			   2.��route_i��route_j��ת������route_j�ӵ�route_i�� */

			//1.��route_i�ϵĽڵ�ӵ�route_j���Ƿ����
			if (s.routeSet[route_i]->isFeasibleGC(node_j->num, node_j->d_t, node_i, data))
			{
				cout << "ȷ�����ӽڵ㣺" << cus_i << "��" << cus_j << endl;

				//������У���route_i�ϲ���route_j��������·��
				s.routeSet[route_j]->mergeRoute(s.routeSet[route_i], data);
				s.routeSet[route_j]->updateRoute(node_i, data);

				//ɾ��·��route_i
				s.routeSet.erase(s.routeSet.begin() + route_i);
			}
			/*��route_i��route_j��ת������route_j�ӵ�route_i��*/
			else
			{
				//��ת·��i��j
				s.routeSet[route_i]->reverseRoute();
				s.routeSet[route_j]->reverseRoute();

				//��route_j�ϵĽڵ�ӵ�route_i���Ƿ����
				if (s.routeSet[route_i]->isFeasibleAddRoute(s.routeSet[route_j], data))
				{
					cout << "ȷ�����ӽڵ㣺" << cus_i << "��" << cus_j << endl;

					//�������
					s.routeSet[route_i]->mergeRoute(s.routeSet[route_j], data);
					s.routeSet[route_i]->updateRoute(s.routeSet[route_i]->head->succ, data);

					//ɾ��·��route_j
					s.routeSet.erase(s.routeSet.begin() + route_j);
				}
				//���������
				else
				{
					//��������У���ԭ·��i��j�ķ�ת
					s.routeSet[route_i]->reverseRoute();
					s.routeSet[route_j]->reverseRoute();

					deleteSavings();
					continue;
				}
			}
		}
		else if (flag_i == 1 && flag_j == 0)
		{
			/* Ҫ����һ��ĵ�Ϊi��j��i��·��route_i��β��j��·��route_j��ͷ
			   �������ӷ�ʽ��
			   1.��route_i��route_j��ת������route_i�ϵĽڵ�ӵ�route_j��
			   2.��route_j�ӵ�route_i�� */

			//��ת·��i��j
			s.routeSet[route_i]->reverseRoute();
			s.routeSet[route_j]->reverseRoute();

			//��route_i�ϵĽڵ�ӵ�route_j���Ƿ����
			if (s.routeSet[route_j]->isFeasibleAddRoute(s.routeSet[route_i], data))
			{
				cout << "ȷ�����ӽڵ㣺" << cus_i << "��" << cus_j << endl;

				//������У���route_i�ϲ���route_j��������·��
				s.routeSet[route_j]->mergeRoute(s.routeSet[route_i], data);
				s.routeSet[route_j]->updateRoute(s.routeSet[route_j]->head->succ, data);

				//ɾ��·��route_i
				s.routeSet.erase(s.routeSet.begin() + route_i);
			}
			else
			{
				//��ԭ��ת·��i��j
				s.routeSet[route_i]->reverseRoute();
				s.routeSet[route_j]->reverseRoute();

				//��route_j�ϵĽڵ�ӵ�route_i���Ƿ����
				if (s.routeSet[route_j]->isFeasibleGC(node_i->num, node_i->d_t, node_j, data))
				{
					cout << "ȷ�����ӽڵ㣺" << cus_i << "��" << cus_j << endl;

					//�������
					s.routeSet[route_i]->mergeRoute(s.routeSet[route_j], data);
					s.routeSet[route_i]->updateRoute(node_j, data);

					//ɾ��·��route_j
					s.routeSet.erase(s.routeSet.begin() + route_j);
				}
				else
				{
					deleteSavings();
					continue;
				}
			}
		}
		else if(flag_i == 0 && flag_j == 0)
		{
		/* Ҫ����һ��ĵ�Ϊi��j��i��j����·��route_i��ͷ
		   �������ӷ�ʽ��
		   1.route_j��ת������route_i�ϵĽڵ�ӵ�route_j��
		   2.��route_i��ת������route_j�ӵ�route_i�� */

			//��ת·��j
			s.routeSet[route_j]->reverseRoute();
			//��route_i�ϵĽڵ�ӵ�route_j���Ƿ����
			if (s.routeSet[route_j]->isFeasibleAddRoute(s.routeSet[route_i], data))
			{
				cout << "ȷ�����ӽڵ㣺" << cus_i << "��" << cus_j << endl;

				//������У���route_i�ϲ���route_j��������·��
				s.routeSet[route_j]->mergeRoute(s.routeSet[route_i], data);
				s.routeSet[route_j]->updateRoute(s.routeSet[route_j]->head->succ, data);

				//ɾ��·��route_i
				s.routeSet.erase(s.routeSet.begin() + route_i);
			}
			else
			{
				//��ԭ��תj
				s.routeSet[route_j]->reverseRoute();

				//��תi
				s.routeSet[route_i]->reverseRoute();

				//��route_j�ϵĽڵ�ӵ�route_i���Ƿ����
				if (s.routeSet[route_i]->isFeasibleAddRoute(s.routeSet[route_j], data))
				{
					cout << "ȷ�����ӽڵ㣺" << cus_i << "��" << cus_j << endl;

					//�������
					s.routeSet[route_i]->mergeRoute(s.routeSet[route_j], data);
					s.routeSet[route_i]->updateRoute(s.routeSet[route_i]->head->succ, data);

					//ɾ��·��route_j
					s.routeSet.erase(s.routeSet.begin() + route_j);
				}
				else
				{
					//��ԭi��ת
					s.routeSet[route_i]->reverseRoute();

					deleteSavings();
					continue;
				}
			}
		}
		else if (flag_i == 1 && flag_j == 1)
		{

			//��תi
			s.routeSet[route_i]->reverseRoute();

			//��route_i�ϵĽڵ�ӵ�route_j���Ƿ����
			if (s.routeSet[route_i]->isFeasibleGC(node_j->num, node_j->d_t, node_i, data))
			{
				cout << "ȷ�����ӽڵ㣺" << cus_i << "��" << cus_j << endl;

				//������У���route_i�ϲ���route_j��������·��
				s.routeSet[route_j]->mergeRoute(s.routeSet[route_i], data);
				s.routeSet[route_j]->updateRoute(node_i, data);

				//ɾ��·��route_i
				s.routeSet.erase(s.routeSet.begin() + route_i);
			}
			else
			{
				//��ԭi��ת
				s.routeSet[route_i]->reverseRoute();

				//��ת·��j
				s.routeSet[route_j]->reverseRoute();

				//��route_j�ϵĽڵ�ӵ�route_i���Ƿ����
				if (s.routeSet[route_j]->isFeasibleGC(node_i->num, node_i->d_t, node_j, data))
				{
					cout << "ȷ�����ӽڵ㣺" << cus_i << "��" << cus_j << endl;

					//�������
					s.routeSet[route_i]->mergeRoute(s.routeSet[route_j], data);
					s.routeSet[route_i]->updateRoute(node_j, data);

					//ɾ��·��route_j
					s.routeSet.erase(s.routeSet.begin() + route_j);
				}
				else
				{
					//��ԭ��תj
					s.routeSet[route_j]->reverseRoute();

					deleteSavings();
					continue;
				}

			}

		}


		/* ɾ��savings���Ѳ���·��ͷ��β�Ľڵ����ڵıߣ�������һ�μӱ�*/
		if (del_i == 1 && del_j == 1)
		{
			cout << "ɾ������Ϊ" << cus_i << "����" << cus_j  << "��saving" << endl;
			deleteSavings(cus_i, cus_j);
		}
		else if (del_i == 1 && del_j == 0)
		{
			cout << "ɾ������Ϊ" << cus_i << "��saving" << endl;
			deleteSavings(cus_i);
		}
		else if (del_i == 0 && del_j == 1)
		{
			cout << "ɾ������Ϊ" << cus_j << "��saving" << endl;
			deleteSavings(cus_j);
		}
		else
		{
			deleteSavings();
		}

		s.computeTime();
		s.computeLength(data);
		s.printSolution();
		s.isRegular(data);
	}
	cout << "------------------------------------------------------" << endl;
	cout << "---------�ɽ�ʡ����ʽ�㷨�õ���һ�����н�-------------" << endl;
	cout << "------------------------------------------------------" << endl;
	s.computeTime();
	s.computeLength(data);
	s.printSolution();
	s.isRegular(data);
}

/*------------------------------------------------------------------------------*/
/*-------------------------------solomon(1987)----------------------------------*/
/*--------------------------��������ʽ·�������㷨-------------------------------*/
/*------------------------------------------------------------------------------*/

void Solve::printInfor()
{
	for (int i = 0; i < infor.size(); ++i)
	{

		double u = infor[i].u_num;  //Ҫ����Ľڵ���Ϊu
		cout << u << "   ";
	}
	cout << endl;
}

void Solve::initInfor()
{
	Insertion* tmp;
	for (int i = 1; i < cus->size(); ++i)
	{
		tmp = new Insertion();
		tmp->u_num = (*cus)[i]->num;
		tmp->u_pred = NULL;
		tmp->u_succ = NULL;
		tmp->cost = -1;

		infor.push_back(*tmp);
	}
}

void Solve::updateInfor(Route* route)
{
	for (int i = 0; i < infor.size(); ++i)
	{

		double u = infor[i].u_num;  //Ҫ����Ľڵ���Ϊu
		
		Node* node_m = route->head;
		Node* node_n;
		double m, n;  //���ڵ�u���뵽�ڵ���Ϊm��n�Ľڵ�֮��

		Node* insert_m = NULL;    //��õĲ���λ��
		Node* insert_n = NULL;

		double c_1 = -1;		//��Ҫ�����ֵ
		double c_11;
		double c_12;

		//����Υ������Լ��ʱ���Ž��п��ǲ���λ�õ�ѡ��
		if ((*cus)[u - 1]->d + route->demand < DEMAND)
		{
			//m�뵱ǰ�ڵ�u�ľ��룻m��n�ľ��룻u��n�ľ���
			double dis_mu, dis_mn, dis_un;

			/*at_u������ڵ�u��ʱ��*/
			double at_u;
			double wt_u;
			double dt_u;
			double at_n;

			while (true)
			{
				node_n = node_m->succ;

				m = node_m->num;
				n = node_n->num;

				dis_mu = data->distance(m, u);
				dis_un = data->distance(u, n);
				dis_mn = data->distance(m, n);

				at_u = node_m->d_t + dis_mu;
				/*�ж��Ƿ�Υ���˽ڵ�u��ʱ�䴰Լ��
				���Υ���ˣ���ô����Ȼ�ڵ�ǰλ�ò��붼�������ˣ���ô֮���λ��Ҳ���ó�����*/
				if (at_u > (*cus)[u - 1]->l_t)
				{
					break;
				}

				//�����ǰ�ڵ�û��Υ��ʱ�䴰Լ��
				/*�������λ�ò���·��ĩβ*/
				c_11 = dis_mu + dis_un - U1 * dis_mn;

				//at_n:����ڵ�u�󵽴�n��ʱ�䣻wt_u���ڽڵ�u�ĵȴ�ʱ�䣻dt_u���ӽڵ�u�ĳ���ʱ��
				wt_u = at_u > (*cus)[u - 1]->e_t ? 0 : (*cus)[u - 1]->e_t - at_u;
				dt_u = at_u + wt_u + (*cus)[u - 1]->s_t;
				at_n = dt_u + dis_un;
				double wt_n = at_n > (*cus)[n - 1]->e_t ? 0 : (*cus)[n - 1]->e_t - at_n;
				c_12 = at_n + wt_n - (node_n->a_t + node_n->w_t);


				//ֻ�в���ڵ�u�󣬿���ʹ·������u֮��Ľڵ㶼���У����ܲ��������λ��
				if (route->isFeasible(u, dt_u, node_n, data))
				{
					double tmp_c_1 = M1 * c_11 + M2 * c_12;
					if (c_1 == -1 || tmp_c_1 < c_1)
					{
						c_1 = tmp_c_1;
						insert_m = node_m;
						insert_n = node_n;
					}
				}

				node_m = node_n;
				if (node_m == route->tail)
				{
					break;
				}
			}
		}

		infor[i].u_pred = insert_m;
		infor[i].u_succ = insert_n;
		infor[i].cost = c_1;
	}
}

void Solve::initRoute(int flag)
{
	//ѡ���������ʼ��·���Ľڵ���
	double c_num;
	//infor��Ҫɾ������Ŀ
	int del;

	/*����ѡ��1����ֿ���Զ�Ľڵ㣻2������ʱ�俪ʼ����Ľڵ�*/
	if (flag == 1)
	{
		c_num = infor[0].u_num;
		double maxDis = data->distance(c_num, 1);
		del = 0;

		//��û������·������ֿ���Զ�Ľڵ�
		for (int i = 1; i < infor.size(); ++i)
		{
			if (data->distance(infor[i].u_num, 1) > maxDis)
			{
				c_num = infor[i].u_num;
				maxDis = data->distance(c_num, 1);
				del = i;
			}
		}
	}
	else if (flag == 2)
	{
		c_num = infor[0].u_num;
		double eTime = (*cus)[c_num - 1]->e_t;
		del = 0;

		//��û������·���з���ʱ�俪ʼ����Ľڵ�
		for (int j = 1; j < infor.size(); ++j)
		{
			if ((*cus)[infor[j].u_num - 1]->e_t < eTime)
			{
				c_num = infor[j].u_num;
				eTime = (*cus)[c_num - 1]->e_t;
				del = j;
			}
		}
	}

	//ɾ��infor�е�del����Ŀ
	infor.erase(infor.begin() + del);

	Route* route = new Route(c_num, data);
	s.routeSet.push_back(route);

}

void Solve::useInsertion()
{
	/*
	int flag;
	cout << "------- --ѡ���ʼ��һ��·���ķ���----------------------" << endl;
	cout << "----------���ѡ��ʹ����ֿ���Զ�Ľڵ��ʼ��·�������룺1---------------" << endl;
	cout << "----------���ѡ��ʹ�÷���ʱ�俪ʼ����Ľڵ��ʼ��·�������룺2---------" << endl;
	cin >> flag;*/

	int flag = 1;

	Route* nRoute;
	initInfor();
	while (infor.size() != 0)   //һֱ�����µ�·����ֱ�����еĽڵ㶼���뵽һ��·����
	{
		cout << "����һ���µ�·��..." << endl;
		//����һ���µ�·����ӵ�·������ĩβ
		initRoute(flag);
		//nRouteΪ��ǰ��·��
		nRoute = s.routeSet.back();


		/*����ܹ��ڵ�ǰ·���п����ҵ����еĲ���*/
		while (true)
		{

			cout << "-----------------------------------------" << endl;
			cout << "��ǰ·��Ϊ��";
			nRoute->printRoute();

			//����infor
			updateInfor(nRoute);

			//�ҵ���õ��Ǹ��ڵ���뵽·����
			double c_2;
			double num = -1; //Ҫ����Ľڵ����ĿΪinfor[num]
			int i;
			for (i = 0; i < infor.size(); ++i)
			{
				if (infor[i].cost != -1) //infor[i][1]Ϊ-1ʱ��ʾ�ڵ�ǰ·����û�п��еĲ���ѡ��
				{
					c_2 = U2 * data->distance(infor[i].u_num, 1) - infor[i].cost;
					num = i;
					break;
				}
			}
			for (i = i + 1; i < infor.size(); ++i)
			{
				if (infor[i].cost != -1)
				{
					double tmp_c_2 = U2 * data->distance(infor[i].u_num, 1) - infor[i].cost;
					if (tmp_c_2 > c_2)
					{
						c_2 = tmp_c_2;
						num = i;
					}
				}
			}

			if (num == -1)  //�ڵ�ǰ·�����޷��ҵ����еĲ���
			{
				break;
			}
			else
			{
				double u;
				Node* insert_m;
				Node* insert_n;  //�ڱ�insert_m��insert_n�ڵ��м����ýڵ�

				//������Ϣ��infor[num]�д洢
				u = infor[num].u_num;
				insert_m = infor[num].u_pred;
				insert_n = infor[num].u_succ;

				//����Ŀinfor[num]ɾ������ʾ�ڵ�u�Ѿ���ӵ�·����
				//cout << "ɾ����" << u << "��ص���Ŀǰ" << endl;
				//printInfor();
				infor.erase(infor.begin() + num);
				//cout << "ɾ����" << u << "��ص���Ŀ��" << endl;
				//printInfor();

				/*���ڵ�u���뵽insert_m��insert_n֮��*/
				cout << "��" << insert_m->num << "��" << insert_n->num << "֮�����ڵ㣺" << u << endl;
				Node* node_u = new Node();
				node_u->num = u;

				insert_m->succ = node_u;
				insert_n->pred = node_u;
				node_u->succ = insert_n;
				node_u->pred = insert_m;

				nRoute->size++;
				nRoute->demand += (*cus)[u - 1]->d;

				/*���½ڵ�u,�Լ���֮��ڵ�ĵ���ʱ�����Ϣ*/
				nRoute->updateRoute(node_u, data);
			}
		}
	}


	cout << "------------------------------------------------------" << endl;
	cout << "---------�ɲ�������ʽ�㷨�õ���һ�����н�-------------" << endl;
	cout << "------------------------------------------------------" << endl;
	s.computeLength(data);
	s.computeTime();
	s.printSolution();
	s.isRegular(data);

	record(&s);

	/*����һ�·�ת·���Ƿ�ת��ȷ
	for (int i = 0; i < s.routeSet.size(); ++i)
	{
		cout << "��" << i + 1 << "��·��.............." << endl;
		s.routeSet[i]->reverseRoute();
		s.routeSet[i]->printRoute();
		s.routeSet[i]->reverseRoute();
		s.routeSet[i]->printRoute();
	}*/
}


/*------------------------------------------------------------------------------*/
/*-------------------------------Local Search----------------------------------*/
/*-------------------------------------------------------------------------------*/

void Solve::OrExchangeNeighbour(Node* node_u, int r_num)
{
	/*�����һ�β������ھӵ�����ֵ����Ϣ*/
	vector<vector<double>*>().swap(neighbour);
	vector<vector<double>>().swap(evaluation);
	vector<int>().swap(order);

	double i = node_u->num;
	Node* node_i = node_u;
	int r = r_num; //�ͻ�c���ڵ�·��
	Node* node_i_succ = node_i->succ;
	Node* node_i_pred = node_i->pred;

	//��i_pred->i,i->i_succ,j->j_succ����i_pred->i_succ,j->i,i->j_succ
	//i�̶���i_pred��i_succ���ǹ̶��ģ��ı����j
	//j�ı仯,ֻҪj������i_pred,i������
	//j�����ǴӲֿ⵽�ص��ֿ�ǰ�����һ���ڵ�
	vector<double>* n;
	vector<double>* e;
	Node* node_j;
	Node* node_j_succ;

	/*j��i��ͬһ��·����*/
	//j��iǰ��
	Node* tmp = s.routeSet[r]->head;
	while (tmp != node_i_pred)
	{

		node_j = tmp;
		node_j_succ = tmp->succ;

		cout << "------------------------------------------------------------" << endl;
		cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����OrExchange..." << endl;
		cout << "·��r��";
		s.routeSet[r]->printRoute();
		cout << "=====" << endl;

		s.doOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

		cout << "·��r��";
		s.routeSet[r]->printRoute();

		//�ж�OrExchange�ھ��Ƿ����
		if (s.routeSet[r]->isFeasibleGC(node_j->num, node_j->d_t, node_i, data))
		{
			//�������
			//��¼���ھ���Ϣ
			n = new vector<double>();
			n->push_back(2);
			n->push_back(node_i->num);
			n->push_back(node_j->num);
			n->push_back(r);
			n->push_back(1);

			//������������µĿ���
			e = new vector<double>();
			e->push_back(s.routeSet.size());
			double tmp_length = s.allLength + data->distance(node_i_pred->num, node_i_succ->num)
				+ data->distance(node_j->num, node_i->num)
				+ data->distance(node_i->num, node_j_succ->num)
				- data->distance(node_i_pred->num, node_i->num)
				- data->distance(node_i->num, node_i_succ->num)
				- data->distance(node_j->num, node_j_succ->num);
			e->push_back(tmp_length);
			double tmp_time = s.routeSet[r]->computeNewAT(node_j->num, node_j->d_t, node_i, data);
			tmp_time -= (*cus)[0]->e_t;
			e->push_back(s.allTime + tmp_time - (s.routeSet[r]->tail->a_t - s.routeSet[r]->head->d_t));

			neighbour.push_back(n);
			evaluation.push_back(*e);

			s.restoreOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
		}
		else
		{
			cout << "����OrExchange������..." << endl;
			//���������
			s.restoreOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
		}

		tmp = tmp->succ;
	}

	//j��i����
	tmp = node_i_succ;
	while (tmp != s.routeSet[r]->tail)
	{
		node_j = tmp;
		node_j_succ = node_j->succ;

		cout << "------------------------------------------------------------" << endl;
		cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����OrExchange..." << endl;
		cout << "·��r��";
		s.routeSet[r]->printRoute();
		cout << "=====" << endl;

		s.doOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

		cout << "·��r��";
		s.routeSet[r]->printRoute();

		//�ж�OrExchange�ھ��Ƿ����
		if (s.routeSet[r]->isFeasible(node_j->num, node_j->d_t, node_i, data))
		{
			//�������
			//��¼���ھ���Ϣ
			n = new vector<double>();
			n->push_back(2);
			n->push_back(node_i->num);
			n->push_back(node_j->num);
			n->push_back(r);
			n->push_back(0);

			
			//������������µĿ���
			e = new vector<double>();
			e->push_back(s.routeSet.size());
			double tmp_length = s.allLength + data->distance(node_i_pred->num, node_i_succ->num)
				+ data->distance(node_j->num, node_i->num)
				+ data->distance(node_i->num, node_j_succ->num)
				- data->distance(node_i_pred->num, node_i->num)
				- data->distance(node_i->num, node_i_succ->num)
				- data->distance(node_j->num, node_j_succ->num);
			e->push_back(tmp_length);
			double tmp_time = s.routeSet[r]->computeNewAT(node_j->num, node_j->d_t, node_i, data);
			tmp_time -= (*cus)[0]->e_t;
			e->push_back(s.allTime + tmp_time - (s.routeSet[r]->tail->a_t - s.routeSet[r]->head->d_t));

			neighbour.push_back(n);
			evaluation.push_back(*e);

			s.restoreOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

		}
		else
		{
			//���������
			cout << "����OrExchange������..." << endl;
			s.restoreOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
		}

		tmp = tmp->succ;
	}

	/*j��i����ͬһ��·����*/
	for (int m = 0; m < s.routeSet.size(); ++m)
	{
		//����i�ڵ�·����
		//���Ҽ��Ͻڵ�i֮��·��m���ᳬ������
		if (m != r && s.routeSet[m]->demand + (*cus)[node_i->num - 1]->d <= DEMAND)
		{
			Node* tmp = s.routeSet[m]->head;
			while (tmp != s.routeSet[m]->tail)
			{
				node_j = tmp;
				node_j_succ = node_j->succ;

				cout << "------------------------------------------------------------" << endl;
				cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����Relocation..." << endl;
				cout << "·��r��";
				s.routeSet[r]->printRoute();
				cout << "·��m��";
				s.routeSet[m]->printRoute();
				cout << "=====" << endl;

				s.doRelocation(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

				cout << "·��r��";
				s.routeSet[r]->printRoute();
				cout << "·��m��";
				s.routeSet[m]->printRoute();

				//�ж�Relocation�ھ��Ƿ����
				if (s.routeSet[m]->isFeasibleGC(node_j->num, node_j->d_t, node_i, data))
				{
					cout << "����..." << endl;
					//�������
					//��¼���ھ���Ϣ
					n = new vector<double>();
					n->push_back(3);
					n->push_back(node_i->num);
					n->push_back(node_j->num);
					n->push_back(r);
					n->push_back(m);


					//������������µĿ���
					e = new vector<double>();
					double s_size = s.routeSet.size();
					if (s.routeSet[r]->size == 1)
						--s_size;
					e->push_back(s_size);
					double tmp_length;
					double tmp_time_r, tmp_time_m;
					if (s.routeSet[r]->size == 1)
					{
						tmp_length = s.allLength
							+ data->distance(node_j->num, node_i->num)
							+ data->distance(node_i->num, node_j_succ->num)
							- data->distance(node_i_pred->num, node_i->num)
							- data->distance(node_i->num, node_i_succ->num)
							- data->distance(node_j->num, node_j_succ->num);
						tmp_time_r = 0;
					}
					else
					{
						tmp_length = s.allLength + data->distance(node_i_pred->num, node_i_succ->num)
							+ data->distance(node_j->num, node_i->num)
							+ data->distance(node_i->num, node_j_succ->num)
							- data->distance(node_i_pred->num, node_i->num)
							- data->distance(node_i->num, node_i_succ->num)
							- data->distance(node_j->num, node_j_succ->num);
						tmp_time_r = s.routeSet[r]->computeNewAT(node_i_pred->num, node_i_pred->d_t, node_i_succ, data);
						tmp_time_r -= (*cus)[0]->e_t;
					}
						
					tmp_time_m = s.routeSet[m]->computeNewAT(node_j->num, node_j->d_t, node_i, data);
					tmp_time_m -= (*cus)[0]->e_t;
					e->push_back(s.allTime + tmp_time_m + tmp_time_r - (s.routeSet[r]->tail->a_t - s.routeSet[r]->head->d_t) - (s.routeSet[m]->tail->a_t - s.routeSet[m]->head->d_t));

					neighbour.push_back(n);
					evaluation.push_back(*e);

					s.restoreRelocation(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

				}
				else
				{
					cout << "����Relocation������..." << endl;
					s.restoreRelocation(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
				}

				tmp = tmp->succ;
			}
		}
	}
}

bool Solve::useOrOpt()
{
	
	for (int i = 0; i < s.routeSet.size(); )
	{
		Node* node_u = s.routeSet[i]->head->succ;
		while (node_u != s.routeSet[i]->tail)
		{
			OrExchangeNeighbour(node_u, i);
			computeOrder();

			double v_1 = s.routeSet.size();
			double v_2 = s.allLength;
			double v_3 = s.allTime;

			if (neighbour.size() == 0)
			{
				node_u = node_u->succ;
				continue;
			}

			if (evaluation[order[0]][0] < v_1 ||
				(evaluation[order[0]][0] == v_1 && evaluation[order[0]][1] < v_2) ||
				(evaluation[order[0]][0] == v_1 && evaluation[order[0]][1] == v_2 && evaluation[order[0]][2] < v_3))
			{
				changeToNeighbour(neighbour[order[0]], &s);

				cout << "---------------------------------------------------------" << endl;
				s.computeTime();
				s.computeLength(data);
				s.printSolution();
				s.isRegular(data);
				cout << "----------------------------------------------------------" << endl;

				return true;
			}
			else
			{
				node_u = node_u->succ;
				continue;
			}
			node_u = node_u->succ;
		}
		++i;
	}
	return false;
}

void Solve::localSearch()
{
	
	/*
	Route *route;

	//Build original solution
	cout << "------------------------��ʼ�������------------------------" << endl;
	for (int i = 1; i < (*cus).size(); ++i)
	{
		route = new Route(i + 1, data);
		s.routeSet.push_back(route);
	}

	s.computeTime();
	s.computeLength(data);
	s.printSolution();
	s.isRegular(data);
	cout << "------------------------------------------------------------" << endl; 
	*/

	int i = 0;
	while (true)
	{
		if (i > 10)
			break;
		if (useOrOpt() == false)
			break;
		++i;
	}
}

/*------------------------------------------------------------------------------*/
/*-------------------------------Russell(1995)----------------------------------*/
/*-------------------------------------------------------------------------------*/
void Solve::printU()
{
	for (int i = 0; i < u.size(); ++i)
	{
		for (int j = 0; j < u[i].size(); ++j)
		{
			cout << u[i][j] << " ";
		}
		cout << endl;
	}
}

void Solve::computeU()
{
	//Ϊ���������У���ʼ��u��u[0]�Ǳ��Ϊ2�Ľڵ��U_i
	int size = (*cus).size();
	vector<double>* tmp;
	for (int i = 0; i < size - 1; ++i)
	{
		tmp = new vector<double>();
		//M1
		tmp->push_back(-1);
		tmp->push_back(-1);
		//M2
		tmp->push_back(-1);
		tmp->push_back(-1);
		u.push_back(*tmp);
	}

	//��·������u
	for (int i = 0; i < s.routeSet.size(); ++i)
	{
		Route* tmp = s.routeSet[i];
		Node* temp = tmp->head;
		double m = 1, n; //��ǰ�ڵ��ǰһ���ڵ�ͺ�һ���ڵ�ı��
		double u_1, u_2;  //M1�еĽڵ�1�ͽڵ�2
		double v_1 = -1, v_2 = -1;  //M2�еĽڵ�1�ͽڵ�2
		while (temp)
		{
			//ȷ��u_1��u_2
			u_1 = temp->num;
			if (temp->succ != NULL)
			{
				temp = temp->succ;
				u_2 = temp->num;
				if (temp->succ != NULL)
				{
					n = temp->succ->num;
				}
				else
				{
					n = 1;
				}
			}
			else
			{
				temp = temp->succ;
				//·����ֻ��һ���ڵ�
				u_2 = -1;
				n = 1;
			}

			//ȷ��v_1��v_2
			double d_1 = -1, d_2 = -1;
			double dis_mv, dis_vn;
			for (int j = 0; j < s.routeSet.size(); ++j)
			{
				if (j != i)
				{
					Node* v = (s.routeSet[j])->head;
					for (; v; v = v->succ)
					{
						dis_mv = data->distance(v->num, m);
						dis_vn = data->distance(v->num, n);

						double temp_d = dis_mv + dis_vn;
						if (d_1 == -1 || temp_d <= d_1)
						{
							d_2 = d_1;
							v_2 = v_1;

							d_1 = temp_d;
							v_1 = v->num;
						}
						else if (d_2 == -1 || temp_d <= d_2)
						{
							d_2 = temp_d;
							v_2 = v->num;
						}
					}
				}
			}
			double num = u_1 - 2;
			u[num][0] = u_1;
			u[num][1] = u_2;
			u[num][2] = v_1;
			u[num][3] = v_2;
		}
	}
}

void Solve::localSearch1()
{
	//ʹ��z�������Ƿ��µĽ��ԭʼ�����
	double z = V1 * s.allLength + V2 * s.allTime;
	//·��������
	int v = s.routeSet.size();

	//�����Ӽ�U_i
	computeU();
	/*��ӡ�Ӽ�U_i
	printU();*/

	int i = 0;

}


/*------------------------------------------------------------------------------*/
/*-------------------------------Russell(2004)----------------------------------*/
/*--------------------------------ģ���˻��㷨-----------------------------------*/
/*-------------------------------------------------------------------------------*/

double Solve::computeLatestArriveTime(Node* node_j_succ)
{
	double t;
	double j = node_j_succ->num;

	if(node_j_succ->succ == NULL)
	{
		t = (*cus)[0]->l_t;
		return t;
	}
	else
	{
		double j_succ = node_j_succ->succ->num;
		double d = data->distance(j, j_succ);

		t = computeLatestArriveTime(node_j_succ) - d - (*cus)[j - 1]->s_t;
	}
	return (*cus)[j - 1]->l_t < t ? (*cus)[j - 1]->l_t : t;
}

void Solve::computeOrder()
{
	for (int i = 0; i < evaluation.size(); ++i)
	{
		order.push_back(i);
		for (int j = i - 1; j >= 0; --j)
		{
			if ((evaluation[order[j + 1]][0] < evaluation[order[j]][0]) ||
				(evaluation[order[j + 1]][0] == evaluation[order[j]][0] && evaluation[order[j + 1]][1] < evaluation[order[j]][1]) ||
				(evaluation[order[j + 1]][0] == evaluation[order[j]][0] && evaluation[order[j + 1]][1] < evaluation[order[j]][1] && evaluation[order[j + 1]][2] >= evaluation[order[j]][2]))
			{
				int temp = order[j + 1];
				order[j + 1] = order[j];
				order[j] = temp;
			}
			else
			{
				break;
			}
		}
	}
}

void Solve::generateNeighbour(int operate, double num, Solution* n_s)
{
	int o = operate;

	if (o == 1)  //����2-exchange  ��һ��·����
	{
		generate2Exchange(num, n_s);
	}
	else if (o == 2)  //����Or-exchange  ��һ��·����
	{
		generateOrExchange(num, n_s);
	}
	else if (o == 3) //����Relocation  ������·����
	{
		generateRelocation(num, n_s);
	}
	else if (o == 4) //����Exchange  ������·����
	{
		generateExchange(num, n_s);
	}
	else if (o == 5) //����Crossover  ������·����
	{
		generateCrossover(num, n_s);
	}
}

void Solve::generate2Exchange(double num, Solution* n_s)
{
	double i = num;
	Node* node_i;
	int r; //�ͻ�c���ڵ�·��
	n_s->search(i, &r, &node_i);
	Node* node_i_succ = node_i->succ;
	double i_succ = node_i_succ->num;  //��ǰ�ͻ��ڵ�i�ĺ�һ���ڵ�ı��

	//����i->i_succ,j->j_succ����i->j,i_succ->j_succ
	vector<double>* n;
	vector<double>* e;

	//��j��i����ʱ
	Node* node_j;
	Node* node_j_succ;

	Node* tmp = node_i_succ;  //i����Ľڵ�i+1
	if (tmp != n_s->routeSet[r]->tail) //����ڵ㲻Ϊ�ֿ�ڵ�ʱ
	{
		tmp = tmp->succ; //�ڵ�i+2
		while (tmp != n_s->routeSet[r]->tail)
		{
			node_j = tmp;
			node_j_succ = node_j->succ;
			
			cout << "------------------------------------------------------------" << endl;
			cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����2Exchange..." << endl;
			cout << "·��r��";
			n_s->routeSet[r]->printRoute();
			cout << "=====" << endl;

			n_s->do2Exchange(n_s->routeSet[r], node_i,  node_i_succ, node_j, node_j_succ);

			cout << "·��r��";
			n_s->routeSet[r]->printRoute();

			//�жϽ���2Exchange֮��õ����ھӿɲ�����
			if (n_s->routeSet[r]->isFeasibleGC(node_i->num, node_i->d_t, node_j, data))
			{
				//�������
				//��¼���ھ���Ϣ
				n = new vector<double>();
				n->push_back(1);
				n->push_back(node_i->num);
				n->push_back(node_j->num);
				n->push_back(r);

				n_s->restore2Exchange(n_s->routeSet[r], node_i, node_i_succ, node_j, node_j_succ);

				//������������µĿ���
				e = new vector<double>();
				computeEvaluation(&e, n, n_s);

				/*
				e->push_back(computeFirstEvaluation(n, n_s));
				e->push_back(computeSecondEvaluation(n, n_s));
				e->push_back(computeThirdEvaluation(n, n_s));*/

				neighbour.push_back(n);
				evaluation.push_back(*e);
			}
			else
			{
				//���������
				cout << "���е�2Exchange������..." << endl;
				n_s->restore2Exchange(n_s->routeSet[r], node_i, node_i_succ, node_j, node_j_succ);

			}

			tmp = tmp->succ;
		}
	}

	//��j��iǰ��ʱ
	tmp = n_s->routeSet[r]->head;
	if (tmp != node_i->pred)
	{
		while (tmp->succ != node_i->pred)
		{
			node_j = tmp;
			node_j_succ = node_j->succ;

			cout << "------------------------------------------------------------" << endl;
			cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����2Exchange..." << endl;
			cout << "·��r��";
			n_s->routeSet[r]->printRoute();
			cout << "=====" << endl;

			n_s->do2Exchange(n_s->routeSet[r], node_j, node_j_succ, node_i, node_i_succ);

			cout << "·��r��";
			n_s->routeSet[r]->printRoute();

			//�жϽ���2Exchange֮��õ����ھӿɲ�����
			if (n_s->routeSet[r]->isFeasibleGC(node_j->num, node_j->d_t, node_i, data))
			{
				//�������
				//��¼���ھ���Ϣ
				n = new vector<double>();
				n->push_back(1);
				n->push_back(node_j->num);
				n->push_back(node_i->num);
				n->push_back(r);

				n_s->restore2Exchange(n_s->routeSet[r], node_j, node_j_succ, node_i, node_i_succ);

				//������������µĿ���
				e = new vector<double>();
				computeEvaluation(&e, n, n_s);

				/*
				e->push_back(computeFirstEvaluation(n, n_s));
				e->push_back(computeSecondEvaluation(n, n_s));
				e->push_back(computeThirdEvaluation(n, n_s));*/

				neighbour.push_back(n);
				evaluation.push_back(*e);
			}
			else
			{
				cout << "���е�2Exchange������..." << endl;
				//���������
				n_s->restore2Exchange(n_s->routeSet[r], node_j, node_j_succ, node_i, node_i_succ);
			}

			tmp = tmp->succ;
		}
	}
}

void Solve::generateOrExchange(double num, Solution* n_s)
{
	double i = num;
	Node* node_i;
	int r; //�ͻ�c���ڵ�·��
	n_s->search(i, &r, &node_i);
	Node* node_i_succ = node_i->succ;
	Node* node_i_pred = node_i->pred;

	//��i_pred->i,i->i_succ,j->j_succ����i_pred->i_succ,j->i,i->j_succ
	//i�̶���i_pred��i_succ���ǹ̶��ģ��ı����j
	//j�ı仯,ֻҪj������i_pred,i������
	//j�����ǴӲֿ⵽�ص��ֿ�ǰ�����һ���ڵ�
	vector<double>* n;
	vector<double>* e;
	Node* node_j;
	Node* node_j_succ;

	//j��iǰ��
	Node* tmp = n_s->routeSet[r]->head;
	while (tmp != node_i_pred)
	{
		
		node_j = tmp;
		node_j_succ = tmp->succ;

		cout << "------------------------------------------------------------" << endl;
		cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����OrExchange..." << endl;
		cout << "·��r��";
		n_s->routeSet[r]->printRoute();
		cout << "=====" << endl;
		
		n_s->doOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

		cout << "·��r��";
		n_s->routeSet[r]->printRoute();

		double at_i, wt_i, dt_i;
		at_i = node_j->d_t + data->distance(node_j->num, node_i->num);
		wt_i = at_i > (*cus)[node_i->num - 1]->e_t ? 0 : (*cus)[node_i->num - 1]->e_t - at_i;
		dt_i = at_i + wt_i + (*cus)[node_i->num - 1]->s_t;

		//�ж�OrExchange�ھ��Ƿ����
		if (n_s->routeSet[r]->isFeasible(node_i->num, dt_i, node_j_succ, data))
		{
			//�������
			//��¼���ھ���Ϣ
			n = new vector<double>();
			n->push_back(2);
			n->push_back(node_i->num);
			n->push_back(node_j->num);
			n->push_back(r);
			n->push_back(1);  //Ϊ1��ʾj��iǰ��

			n_s->restoreOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

			//������������µĿ���
			e = new vector<double>();
			computeEvaluation(&e, n, n_s);

			/*
			e->push_back(computeFirstEvaluation(n, n_s));
			e->push_back(computeSecondEvaluation(n, n_s));
			e->push_back(computeThirdEvaluation(n, n_s));*/

			neighbour.push_back(n);
			evaluation.push_back(*e);

		}
		else
		{
			cout << "����OrExchange������..." << endl;
			//���������
			n_s->restoreOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
		}

		tmp = tmp->succ;
	}

	//j��i����
	tmp = node_i_succ;
	while (tmp != n_s->routeSet[r]->tail)
	{
		node_j = tmp;
		node_j_succ = node_j->succ;

		cout << "------------------------------------------------------------" << endl;
		cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����OrExchange..." << endl;
		cout << "·��r��";
		n_s->routeSet[r]->printRoute();
		cout << "=====" << endl;

		n_s->doOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

		cout << "·��r��";
		n_s->routeSet[r]->printRoute();

		double at_i, wt_i, dt_i;
		at_i = node_j->d_t + data->distance(node_j->num, node_i->num);
		//���i��j������i�������У���ôj����Ķ����ó�����
		if (at_i > (*cus)[node_i->num - 1]->l_t)
		{
			//��ԭ����
			n_s->restoreOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
			break;
		}
		else
		{
			wt_i = at_i > (*cus)[node_i->num - 1]->e_t ? 0 : (*cus)[node_i->num - 1]->e_t - at_i;
			dt_i = at_i + wt_i + (*cus)[node_i->num - 1]->s_t;

			//�ж�OrExchange�ھ��Ƿ����
			if (n_s->routeSet[r]->isFeasible(node_i->num, dt_i, node_j_succ, data))
			{
				//�������
				//��¼���ھ���Ϣ
				n = new vector<double>();
				n->push_back(2);
				n->push_back(node_i->num);
				n->push_back(node_j->num);
				n->push_back(r);
				n->push_back(0);     //Ϊ0��ʾj��i����

				n_s->restoreOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

				//������������µĿ���
				e = new vector<double>();
				computeEvaluation(&e, n, n_s);

				/*
				e->push_back(computeFirstEvaluation(n, n_s));
				e->push_back(computeSecondEvaluation(n, n_s));
				e->push_back(computeThirdEvaluation(n, n_s));*/

				neighbour.push_back(n);
				evaluation.push_back(*e);

			}
			else
			{
				//���������
				cout << "����OrExchange������..." << endl;
				n_s->restoreOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
			}
		}
		tmp = tmp->succ;
	}
}

void Solve::generateRelocation(double num, Solution* n_s)
{
	double i = num;
	Node* node_i;
	int r; //�ͻ�c���ڵ�·��
	n_s->search(i, &r, &node_i);
	Node* node_i_succ = node_i->succ;
	Node* node_i_pred = node_i->pred;

	//��i_pred->i,i->i_succ,j->j_succ����i_pred->i_succ,j->i,i->j_succ
	//i�̶���i_pred��i_succ���ǹ̶��ģ��ı����j
	//j����������·���ϵĽڵ㣬�Ӳֿ⵽�ֿ�ǰ�����һ���ڵ�
	vector<double>* n;
	vector<double>* e;
	Node* node_j;
	Node* node_j_succ;
	for (int m = 0; m < n_s->routeSet.size(); ++m)
	{
		//����i�ڵ�·����
		//���Ҽ��Ͻڵ�i֮��·��m���ᳬ������
		if (m != r && n_s->routeSet[m]->demand + (*cus)[node_i->num - 1]->d <= DEMAND)
		{
			Node* tmp = n_s->routeSet[m]->head;
			while (tmp != n_s->routeSet[m]->tail)
			{
				node_j = tmp;
				node_j_succ = node_j->succ;

				cout << "------------------------------------------------------------" << endl;
				cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����Relocation..." << endl;
				cout << "·��r��";
				n_s->routeSet[r]->printRoute();
				cout << "·��m��";
				n_s->routeSet[m]->printRoute();
				cout << "=====" << endl;

				n_s->doRelocation(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

				cout << "·��r��";
				n_s->routeSet[r]->printRoute();
				cout << "·��m��";
				n_s->routeSet[m]->printRoute();


				double at_i, wt_i, dt_i;
				at_i = node_j->d_t + data->distance(node_j->num, node_i->num);
				//���i��j������i�������У���ôj����Ķ����ó�����
				if (at_i > (*cus)[node_i->num - 1]->l_t)
				{
					//��ԭ����
					n_s->restoreRelocation(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
					break;
				}
				else
				{
					wt_i = at_i > (*cus)[node_i->num - 1]->e_t ? 0 : (*cus)[node_i->num - 1]->e_t - at_i;
					dt_i = at_i + wt_i + (*cus)[node_i->num - 1]->s_t;

					//�ж�Relocation�ھ��Ƿ����
					if (n_s->routeSet[m]->isFeasible(node_i->num, dt_i, node_j_succ, data))
					{
						cout << "����..." << endl;
						//�������
						//��¼���ھ���Ϣ
						n = new vector<double>();
						n->push_back(3);
						n->push_back(node_i->num);
						n->push_back(node_j->num);
						n->push_back(r);
						n->push_back(m);

						n_s->restoreRelocation(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

						//������������µĿ���
						e = new vector<double>();
						computeEvaluation(&e, n, n_s);

						/*
						e->push_back(computeFirstEvaluation(n, n_s));
						e->push_back(computeSecondEvaluation(n, n_s));
						e->push_back(computeThirdEvaluation(n, n_s));*/

						neighbour.push_back(n);
						evaluation.push_back(*e);

					}
					else
					{
						cout << "����Relocation������..." << endl;
						n_s->restoreRelocation(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
					}
				}
				tmp = tmp->succ;
			}
		}
	}
	cout << "here" << endl;
}

void Solve::generateExchange(double num, Solution* n_s)
{
	double i = num;
	Node* node_i;
	int r; //�ͻ�c���ڵ�·��
	n_s->search(i, &r, &node_i);
	Node* node_i_succ = node_i->succ;
	Node* node_i_pred = node_i->pred;

	//��i_pred->i,i->i_succ,j->pred->j,j->j_succ����i_pred->j,j->i_succ,j_pred->i,i->j_succ
	//i�̶���i_pred��i_succ���ǹ̶��ģ��ı����j
	//j����������·���ϵĽڵ㣬�Ӳֿ���һ���ڵ㵽�ֿ�ǰ�����һ���ڵ�
	vector<double>* n;
	vector<double>* e;
	Node* node_j_pred;
	Node* node_j;
	Node* node_j_succ;
	for (int m = 0; m < n_s->routeSet.size(); ++m)
	{
		//����i�ڵ�·����
		if (m != r) 
		{
			Node* tmp = n_s->routeSet[m]->head->succ;
			while (tmp != n_s->routeSet[m]->tail)
			{
				node_j_pred = tmp->pred;
				node_j = tmp;
				node_j_succ = node_j->succ;

				//����·���ϵ������󲻻ᳬ������������
				if (n_s->routeSet[m]->demand + (*cus)[node_i->num - 1]->d - (*cus)[node_j->num - 1]->d <= DEMAND &&
					n_s->routeSet[r]->demand + (*cus)[node_j->num - 1]->d - (*cus)[node_i->num - 1]->d <= DEMAND)
				{

					
					cout << "------------------------------------------------------------" << endl;
					cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����Exchange..." << endl;
					cout << "·��r��";
					n_s->routeSet[r]->printRoute();
					cout << "·��m��";
					n_s->routeSet[m]->printRoute();
					cout << "=====" << endl;

					n_s->doExchange(node_i_pred, node_i, node_i_succ, node_j_pred, node_j, node_j_succ);

					cout << "·��r��";
					n_s->routeSet[r]->printRoute();
					cout << "·��m��";
					n_s->routeSet[m]->printRoute();

					double at_i, wt_i, dt_i;
					double at_j, wt_j, dt_j;
					at_i = node_j_pred->d_t + data->distance(node_j_pred->num, node_i->num);
					//���i��j������i�������У���ôj����Ķ����ó�����
					if (at_i > (*cus)[node_i->num - 1]->l_t)
					{
						//��ԭ����
						n_s->restoreExchange(node_i_pred, node_i, node_i_succ, node_j_pred, node_j, node_j_succ);
						break;
					}
					else
					{
						at_j = node_i_pred->d_t + data->distance(node_i_pred->num, node_j->num);
						//���i���У�j������
						if (at_j > (*cus)[node_j->num - 1]->l_t)
						{
							//��ԭ����
							n_s->restoreExchange(node_i_pred, node_i, node_i_succ, node_j_pred, node_j, node_j_succ);
							tmp = tmp->succ;
							continue;
						}
						//���i��j������
						else
						{
							wt_i = at_i > (*cus)[node_i->num - 1]->e_t ? 0 : (*cus)[node_i->num - 1]->e_t - at_i;
							dt_i = at_i + wt_i + (*cus)[node_i->num - 1]->s_t;

							wt_j = at_j > (*cus)[node_j->num - 1]->e_t ? 0 : (*cus)[node_j->num - 1]->e_t - at_j;
							dt_j = at_j + wt_j + (*cus)[node_j->num - 1]->s_t;

							//�ж�Exchange�ھ��Ƿ����
							if (n_s->routeSet[m]->isFeasible(node_i->num, dt_i, node_j_succ, data) &&
								n_s->routeSet[r]->isFeasible(node_j->num, dt_j, node_i_succ, data))
							{

								//�������
								//��¼���ھ���Ϣ
								n = new vector<double>();
								n->push_back(4);
								n->push_back(node_i->num);
								n->push_back(node_j->num);
								n->push_back(r);
								n->push_back(m);

								n_s->restoreExchange(node_i_pred, node_i, node_i_succ, node_j_pred, node_j, node_j_succ);

								//������������µĿ���
								e = new vector<double>();
								computeEvaluation(&e, n, n_s);

								/*
								e->push_back(computeFirstEvaluation(n, n_s));
								e->push_back(computeSecondEvaluation(n, n_s));
								e->push_back(computeThirdEvaluation(n, n_s));*/

								neighbour.push_back(n);
								evaluation.push_back(*e);

							}
							else
							{
								cout << "����Exchange������..." << endl;
								n_s->restoreExchange(node_i_pred, node_i, node_i_succ, node_j_pred, node_j, node_j_succ);
							}
						}
					}	
				}
				tmp = tmp->succ;
			}
		}
	}
}

void Solve::generateCrossover(double num, Solution* n_s)
{
	double i = num;
	Node* node_i;
	int r; //�ͻ�c���ڵ�·��
	n_s->search(i, &r, &node_i);
	Node* node_i_succ = node_i->succ;

	double r_1_demand, r_2_demand = 0;
	for (Node* t = node_i_succ; t != n_s->routeSet[r]->tail; t = t->succ)
		r_2_demand += (*cus)[t->num - 1]->d;
	r_1_demand = n_s->routeSet[r]->demand - r_2_demand;

	double m_1_demand, m_2_demand = 0;

	//��i->i_succ,j->j_succ����j->i_succ,i->j_succ
	//i�̶���i_succ���ǹ̶��ģ��ı����j
	vector<double>* n;
	vector<double>* e;
	Node* node_j;
	Node* node_j_succ;
	for (int m = 0; m < n_s->routeSet.size(); ++m)
	{
		//����i�ڵ�·����
		if (m != r)
		{
			Node* tmp = n_s->routeSet[m]->head;
			m_2_demand = n_s->routeSet[m]->demand;
			while (tmp != n_s->routeSet[m]->tail)
			{
				node_j = tmp;
				node_j_succ = node_j->succ;

				//��������ڵ㶼�ֱ��Ǳ˴�·�������һ���ͻ��ڵ㣬��ô���س���crossover����Ϊ��û���κθı�
				if (node_i->succ == n_s->routeSet[r]->tail && node_j->succ == n_s->routeSet[m]->tail)
				{
					tmp = tmp->succ;
					continue;
				}

				m_2_demand -= (*cus)[node_j->num - 1]->d;
				m_1_demand = n_s->routeSet[m]->demand - m_2_demand;

				//�������������ƣ��ſ�������
				if (r_1_demand + m_2_demand < DEMAND && m_1_demand + r_2_demand < DEMAND)
				{

					cout << "------------------------------------------------------------" << endl;
					cout << "��" << node_i->num << "��" << node_j->num << "��crossover..." << endl;
					cout << "·��r��";
					n_s->routeSet[r]->printRoute();
					cout << "·��m��";
					n_s->routeSet[m]->printRoute();
					cout << "=====" << endl;

					n_s->doCrossover(node_i, node_i_succ, node_j, node_j_succ);
					Node* ptail = n_s->routeSet[r]->tail;
					n_s->routeSet[r]->tail = n_s->routeSet[m]->tail;
					n_s->routeSet[m]->tail = ptail;

					cout << "·��r��";
					n_s->routeSet[r]->printRoute();
					cout << "·��m��";
					n_s->routeSet[m]->printRoute();

					//�ж�Crossover�ھ��Ƿ����
					if (n_s->routeSet[m]->isFeasible(node_j->num, node_j->d_t, node_i_succ, data) &&
						n_s->routeSet[r]->isFeasible(node_i->num, node_i->d_t, node_j_succ, data))
					{	
						//��¼���ھ���Ϣ
						n = new vector<double>();
						n->push_back(5);
						n->push_back(node_i->num);
						n->push_back(node_j->num);
						n->push_back(r);
						n->push_back(m);

						cout << "����,��¼�ھ���Ϣ..." << endl;
						n_s->restoreCrossover(node_i, node_i_succ, node_j, node_j_succ);
						ptail = n_s->routeSet[r]->tail;
						n_s->routeSet[r]->tail = n_s->routeSet[m]->tail;
						n_s->routeSet[m]->tail = ptail;

						

						//������������µĿ���
						e = new vector<double>();
						computeEvaluation(&e, n, n_s);

						/*
						e->push_back(computeFirstEvaluation(n, n_s));
						e->push_back(computeSecondEvaluation(n, n_s));
						e->push_back(computeThirdEvaluation(n, n_s));*/

						neighbour.push_back(n);
						evaluation.push_back(*e);

					}
					else
					{
						cout << "���ֲ���������..." << endl;
						n_s->restoreCrossover(node_i, node_i_succ, node_j, node_j_succ);
						ptail = n_s->routeSet[r]->tail;
						n_s->routeSet[r]->tail = n_s->routeSet[m]->tail;
						n_s->routeSet[m]->tail = ptail;
					}

				}

				tmp = tmp->succ;
			}
		}
	}
}

double Solve::computeFirstEvaluation(vector<double>* n, Solution* n_s)
{
	double x;
	x = n_s->routeSet.size();

	//���ֿ��ܼ���·����Ŀ�Ĳ���
	if ((*n)[0] == 3) //relocation�ھ�
	{
		double r1;
		r1 = (*n)[3];
		if (n_s->routeSet[r1]->size == 1)
			x--;
	}
	else if ((*n)[0] == 5) //crossover�ھ�
	{
		double i = (*n)[1];
		double j = (*n)[2];
		double r1 = (*n)[3];
		double r2 = (*n)[4];
		Node* node_i = n_s->routeSet[r1]->searchNode(i);
		Node* node_j = n_s->routeSet[r2]->searchNode(j);
		if (node_i->succ == n_s->routeSet[r1]->tail && node_j == n_s->routeSet[r2]->head)
			x--;
	}

	return x;
}

double Solve::computeSecondEvaluation(vector<double>* n, Solution* n_s)
{
	double r1, r2;
	double sum = 0;
	if ((*n)[0] == 1 || (*n)[0] == 2 || (*n)[0] == 4)
	{
		//ÿ��·���ϵĽڵ���Ŀ��û�з����ı�	
		for (int m = 0; m < n_s->routeSet.size(); ++m)
		{
			sum += pow(n_s->routeSet[m]->size, 2);
		}
	}
	else if ((*n)[0] == 3) //relocation�ھ�
	{
		r1 = (*n)[3];
		r2 = (*n)[4];

		for (int n = 0; n < n_s->routeSet.size(); ++n)
		{
			if (n == r1)
				sum += pow(n_s->routeSet[n]->size - 1, 2);
			else if (n == r2)
				sum += pow(n_s->routeSet[n]->size + 1, 2);
			else
				sum += pow(n_s->routeSet[n]->size, 2);
		}
	}
	else if ((*n)[0] == 5) //crossover�ھ�
	{
		double i = (*n)[1];
		double j = (*n)[2];

		r1 = (*n)[3];
		r2 = (*n)[4];

		double r1_1, r1_2 = 0;
		double r2_1, r2_2 = 0;

		Node* node_i = n_s->routeSet[r1]->searchNode(i);
		Node* node_j;
		if (j == 1)
			node_j = n_s->routeSet[r2]->head;
		else
			node_j = n_s->routeSet[r2]->searchNode(j);

		for (Node* tmp = node_i->succ; tmp != n_s->routeSet[r1]->tail; tmp = tmp->succ)
		{
			r1_2++;
		}
		r1_1 = n_s->routeSet[r1]->size - r1_2;

		for (Node* tmp = node_j->succ; tmp != n_s->routeSet[r2]->tail; tmp = tmp->succ)
		{
			r2_2++;
		}
		r2_1 = n_s->routeSet[r2]->size - r2_2;

		for (int n = 0; n < n_s->routeSet.size(); ++n)
		{
			if (n == r1)
				sum += pow(r1_1 + r2_2, 2);
			else if (n == r2)
				sum += pow(r2_1 + r1_2, 2);
			else
				sum += pow(n_s->routeSet[n]->size, 2);
		}
	}

	return -sum;
}

double Solve::computeMinimalDelay(Solution* n)
{
	double num = 0;

	//r��ʾ�ڵ������ٵ�·���������ҵ�·��r
	int r = 0;
	for (int i = 1; i < n->routeSet.size(); ++i)
	{
		if (n->routeSet[i]->size < n->routeSet[r]->size)
		{
			r = i;
		}
	}

	double t_num;
	Node* node_u = n->routeSet[r]->head->succ;
	while (node_u != n->routeSet[r]->tail)
	{
		t_num = -1;

		Node* node_u_pred = node_u->pred;
		Node* node_u_succ = node_u->succ;

		double u_num = node_u->num;
		for (int i = 0; i < n->routeSet.size(); ++i)
		{

			//���ڲ��ǵ�ǰ·����·��
			if (i != r)
			{
				//�޷����뵱ǰ·��
				if ((*cus)[u_num - 1]->d + n->routeSet[i]->demand > DEMAND)
				{
					continue;
				}
				else
				{
					//���Խ��ڵ�node_u��·��routeSet[i]���ض�λ
					Node* node_j = n->routeSet[i]->head;
					Node* node_j_succ;
					while (node_j != n->routeSet[i]->tail)
					{

						node_j_succ = node_j->succ;

						//�����ж���node_j�������᲻�����node_u�Լ�node_u����Ľڵ㲻����
						n->doRelocation(node_u_pred, node_u, node_u_succ, node_j, node_j_succ);
						if (n->routeSet[i]->isFeasible(node_j->num, node_j->d_t, node_u, data))
						{
							//�������λ�ÿ���
							t_num = 0;
							n->restoreRelocation(node_u_pred, node_u, node_u_succ, node_j, node_j_succ);
							break;
						}
						else
						{
							//�������λ�ò�����
							double x = node_j->d_t + data->distance(node_j->num, node_u->num) - (*cus)[node_u->num - 1]->l_t;
							double n1 = x > 0 ? x : 0;

							x = node_u->d_t + data->distance(node_j_succ->num, node_u->num) - computeLatestArriveTime(node_j_succ);
							double n2 = x > 0 ? x : 0;

							if (t_num == -1 || n1 + n2 < t_num)
								t_num = n1 + n2;

							n->restoreRelocation(node_u_pred, node_u, node_u_succ, node_j, node_j_succ);
						}

						node_j = node_j->succ;
					}
				}
			}
			if (t_num == 0)
				break;
		}

		if (t_num = -1)
			num = -1;
		else
			num += t_num;

		node_u = node_u->succ;
	}

	return num;
}

double Solve::computeThirdEvaluation(vector<double>* n, Solution* n_s)
{
	Solution* temp_s = new Solution();
	temp_s->doCopy(n_s);

	changeToNeighbour(n, temp_s);
	double x = computeMinimalDelay(temp_s);

	delete temp_s;
	
	return x;
}

void Solve::computeEvaluation(vector<double>** e, vector<double>* n, Solution* n_s)
{
	Solution* temp_s = new Solution();
	temp_s->doCopy(n_s);

	changeToNeighbour(n, temp_s);
	(*e)->push_back(temp_s->routeSet.size());
	(*e)->push_back(temp_s->allLength);
	(*e)->push_back(temp_s->allTime);

	delete temp_s;
}

void Solve::changeToNeighbour(vector<double>* n, Solution* temp_s)
{
	if ((*n)[0] == 0)
		return;

	double r, m;
	double i, j;
	double x;
	Node* node_i, *node_j;
	Node* node_i_pred, *node_i_succ;
	Node* node_j_pred, *node_j_succ;

	i = (*n)[1];
	j = (*n)[2];
	r = (*n)[3];

	if ((*n)[0] == 1) //2-exchange�ھ�
	{
		if(i == 1)
			node_i = temp_s->routeSet[r]->head;
		else
			node_i = temp_s->routeSet[r]->searchNode(i);
		node_i_succ = node_i->succ;

		node_j = temp_s->routeSet[r]->searchNode(j);
		node_j_succ = node_j->succ;

		temp_s->do2Exchange(temp_s->routeSet[r], node_i, node_i_succ, node_j, node_j_succ);

		temp_s->routeSet[r]->updateRoute(node_j, data);
	}
	else if ((*n)[0] == 2) //or-exchange�ھ�
	{
		double flag = (*n)[4];

		node_i = temp_s->routeSet[r]->searchNode(i);
		node_i_succ = node_i->succ;

		if (j == 1)
			node_j = temp_s->routeSet[r]->head;
		else
			node_j = temp_s->routeSet[r]->searchNode(j);
		node_j_succ = node_j->succ;

		node_i_pred = node_i->pred;

		temp_s->doOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

		if(flag == 1)   //j��iǰ��
			temp_s->routeSet[r]->updateRoute(node_i, data);
		else if(flag == 0)  //j��i����
			temp_s->routeSet[r]->updateRoute(node_i_succ, data);

	}
	else if ((*n)[0] == 3) //relocation�ھ�
	{
		m = (*n)[4];

		/*
		cout << "·����";
		temp_s->routeSet[r]->printRoute();
		cout << "Ѱ�ң�" << i << endl;
		cout << "��һ��·����";
		temp_s->routeSet[m]->printRoute();
		cout << "Ѱ�ң�" << j << endl;
		*/

		node_i = temp_s->routeSet[r]->searchNode(i);
		node_i_succ = node_i->succ;

 		
		if (j == 1)
			node_j = temp_s->routeSet[m]->head;
		else
			node_j = temp_s->routeSet[m]->searchNode(j);
		node_j_succ = node_j->succ;

		node_i_pred = node_i->pred;

		temp_s->doRelocation(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
	
		temp_s->routeSet[r]->size--;
		temp_s->routeSet[m]->size++;

		temp_s->routeSet[r]->demand -= (*cus)[i - 1]->d;
		temp_s->routeSet[m]->demand += (*cus)[i - 1]->d;

		temp_s->routeSet[m]->updateRoute(node_i, data);
		//r1���ܻ���
		if (temp_s->routeSet[r]->size == 0)
		{
			temp_s->routeSet.erase(temp_s->routeSet.begin() + r);
		}
		else
			temp_s->routeSet[r]->updateRoute(node_i_succ, data);
	}
	else if ((*n)[0] == 4)  //exchange�ھ�
	{
		m = (*n)[4];

		node_i = temp_s->routeSet[r]->searchNode(i);
		node_i_succ = node_i->succ;

		node_j = temp_s->routeSet[m]->searchNode(j);
		node_j_succ = node_j->succ;

		node_i_pred = node_i->pred;
		node_j_pred = node_j->pred;

		temp_s->doExchange(node_i_pred, node_i, node_i_succ, node_j_pred, node_j, node_j_succ);

		temp_s->routeSet[r]->updateRoute(node_j, data);
		temp_s->routeSet[m]->updateRoute(node_i, data);

		temp_s->routeSet[r]->demand += (*cus)[j - 1]->d - (*cus)[i - 1]->d;
		temp_s->routeSet[m]->demand += (*cus)[i - 1]->d - (*cus)[j - 1]->d;

	}
	else if ((*n)[0] == 5) //crossover�ھ�
	{
		m = (*n)[4];	

		node_i = temp_s->routeSet[r]->searchNode(i);
		node_i_succ = node_i->succ;

		if (j == 1)
			node_j = temp_s->routeSet[m]->head;
		else
			node_j = temp_s->routeSet[m]->searchNode(j);
		node_j_succ = node_j->succ;

		double r_1_demand, r_2_demand = 0;
		double r_1, r_2 = 0;
		for (Node* t = node_i_succ; t != temp_s->routeSet[r]->tail; t = t->succ)
		{
			++r_2;
			r_2_demand += (*cus)[t->num - 1]->d;
		}
		r_1 = temp_s->routeSet[r]->size - r_2;
		r_1_demand = temp_s->routeSet[r]->demand - r_2_demand;

		double m_1_demand, m_2_demand = 0;
		double m_1, m_2 = 0;

		for (Node* t = node_j_succ; t != temp_s->routeSet[m]->tail; t = t->succ)
		{
			++m_2;
			m_2_demand += (*cus)[t->num - 1]->d;
		}
		m_1 = temp_s->routeSet[m]->size - m_2;
		m_1_demand = temp_s->routeSet[m]->demand - m_2_demand;

		temp_s->doCrossover(node_i, node_i_succ, node_j, node_j_succ);
		Node* ptail = temp_s->routeSet[r]->tail;
		temp_s->routeSet[r]->tail = temp_s->routeSet[m]->tail;
		temp_s->routeSet[m]->tail = ptail;

		//����·��������
		temp_s->routeSet[r]->demand = r_1_demand + m_2_demand;
		temp_s->routeSet[m]->demand = m_1_demand + r_2_demand;
		//����·���Ľڵ���Ŀ
		temp_s->routeSet[r]->size = r_1 + m_2;
		temp_s->routeSet[m]->size = m_1 + r_2;

		//·��m���ܱ�գ�·��r������ֱ�յ����
		temp_s->routeSet[r]->updateRoute(node_j_succ, data);
		if (temp_s->routeSet[m]->size != 0)
			temp_s->routeSet[m]->updateRoute(node_i_succ, data);
		else
		{
			//��m����·��ɾ��
			delete temp_s->routeSet[m];
			(temp_s->routeSet).erase(temp_s->routeSet.begin() + m);
		}
	}
	temp_s->computeLength(data);
	temp_s->computeTime();
}

void Solve::minimizeRoute()
{
	int maxIteration = 20;
	/*
	Route *route;

	Build original solution
	cout << "------------------------��ʼ�������------------------------" << endl;
	for (int i = 1; i < (*cus).size(); ++i)
	{
		route = new Route(i + 1, data);
		s.routeSet.push_back(route);
	}
	s.printSolution();
	cout << "------------------------------------------------------------" << endl; */

	
	double t;
	int o;  //������1.Two-exchange 2.Or-exchange 3.Relocation 4.Exchange 5.Crossover
	double c; //���������һ���ͻ����

	//��һ����Ϊ0ʱ��ʾ�������tmp_s���ڵ�ǰ��������������κα仯
	//1����һ��2-exchange 2����һ��or-exchange 3����һ��relocation 4����һ��exchange 5����һ��crossover
	Solution* n_s = new Solution();
	n_s->doCopy(&s);
	vector<double>* flag = new vector<double>();
	flag->push_back(0);
	
	/*
	n_s->routeSet[1]->mergeRoute(n_s->routeSet[2], data);
	cout << "-----------------n_s--------------" << endl;
	n_s->printSolution();

	cout << "-------------------s--------------" << endl;
	s.printSolution();
	*/

	//��õ�ǰʱ��
	time_t iTime = time(0);
	time_t nTime = time(0);
	while (nTime - iTime < 10)
	{
		t = T;
		while (nTime - iTime < 10 && t > EPS)
		{
			for (int i = 1; i < maxIteration; i++)
			{
				o = (rand() % 5) + 1;
				c = (double)(rand() % 99) + 2;

				cout << "*******************************��" << i << "�β���**********************" << endl;
				cout << "�Կͻ�" << c << "����" << o << "����" << endl;
				//����n_s���ھӲ������ھӵ�����ֵ
				generateNeighbour(o, c, n_s);
				if (neighbour.size() == 0)
				{
					if(o == 1)
						cout << "�Կͻ�" << c << "û�п��е�" << "2-exchange����" << endl;
					else if(o == 2)
						cout << "�Կͻ�" << c << "û�п��е�" << "or-exchange����" << endl;
					else if(o == 3)
						cout << "�Կͻ�" << c << "û�п��е�" << "relocation����" << endl;
					else if(o == 4)
						cout << "�Կͻ�" << c << "û�п��е�" << "exchange����" << endl;
					else if(o == 5)
						cout << "�Կͻ�" << c << "û�п��е�" << "crossover����" << endl;
					continue;
				}

				//�����ھӵ�����
				computeOrder();

				double v1 = computeFirstEvaluation(flag, &s);
				double v2 = computeSecondEvaluation(flag, &s);
				double v3 = computeThirdEvaluation(flag, &s);
				double v_1 = computeFirstEvaluation(flag, n_s);
				double v_2 = computeSecondEvaluation(flag, n_s);
				double v_3 = computeThirdEvaluation(flag, n_s);
				if (evaluation[order[0]][0] > v1 && evaluation[order[0]][1] > v2 && evaluation[order[0]][2] > v3)
				{
					int r = ceil((rand() / double(RAND_MAX)) * order.size());
					double diff = 0.5 * (evaluation[order[r]][0] - v_1) + 0.3 * (evaluation[order[r]][1] - v_2) + 0.2 * (evaluation[order[r]][2] - v_3);
					if (evaluation[order[r]][0] > v_1 && evaluation[order[r]][1] > v_2 && evaluation[order[r]][2] > v_3)
					{
						if (rand() % 100 / (double)100 <= exp(diff / t))
						{
							changeToNeighbour(neighbour[order[r]], n_s);
						}
					}
					else
					{
						changeToNeighbour(neighbour[order[r]], n_s);
					}
				}
				else
				{
					changeToNeighbour(neighbour[order[0]], &s);
					changeToNeighbour(neighbour[order[0]], n_s);
				}

				s.computeLength(data);
				s.computeTime();
				s.printSolution();
				s.isRegular(data);
			}
			t = R * t;
			nTime = time(0);
		}
	}

	cout << "------------------------------------------------------" << endl;
	cout << "---------��С��·����Ŀ��õ���һ�����н�-------------" << endl;
	cout << "------------------------------------------------------" << endl;
	s.computeLength(data);
	s.computeTime();
	s.printSolution();
	s.isRegular(data);
}

bool Solve::compareE(vector<double>* e1, vector<double>* e2)
{
	//���e1<e2,�򷵻�true
	if ((*e1)[0] < (*e2)[0] ||
		((*e1)[0] == (*e2)[0] && (*e1)[1] < (*e2)[1]) ||
		((*e1)[0] == (*e2)[0] && (*e1)[1] == (*e2)[1] && (*e1)[2] < (*e2)[2]))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Solve::simulatedAnneal()
{
	int maxIteration = 100;

	/*Route *route;

	Build original solution
	cout << "------------------------��ʼ�������------------------------" << endl;
	for (int i = 1; i < (*cus).size(); ++i)
	{
		route = new Route(i + 1, data);
		s.routeSet.push_back(route);
	}
	s.printSolution();
	cout << "------------------------------------------------------------" << endl; */

	double t;
	int o;  //������1.Two-exchange 2.Or-exchange 3.Relocation 4.Exchange 5.Crossover
	double c; //���������һ���ͻ����

	//��һ����Ϊ0ʱ��ʾ�������tmp_s���ڵ�ǰ��������������κα仯
	//1����һ��2-exchange 2����һ��or-exchange 3����һ��relocation 4����һ��exchange 5����һ��crossover
	Solution* n_s = new Solution();
	n_s->doCopy(&s);
	vector<double>* flag = new vector<double>();
	flag->push_back(0);

	Solution* best_s = new Solution();
	best_s->doCopy(&s);

	/*
	n_s->routeSet[1]->mergeRoute(n_s->routeSet[2], data);
	cout << "-----------------n_s--------------" << endl;
	n_s->printSolution();

	cout << "-------------------s--------------" << endl;
	s.printSolution();
	*/

	//��õ�ǰʱ��
	time_t iTime = time(0);
	time_t nTime = time(0);
	while (nTime - iTime < 10)
	{
		t = T;
		while (nTime - iTime < 10 && t > EPS)
		{
			for (int i = 1; i < maxIteration; i++)
			{
				//����n_s���ھӲ������ھӵ�����ֵ
				/*�����һ�β������ھӵ�����ֵ����Ϣ*/
				vector<vector<double>*>().swap(neighbour);
				vector<vector<double>>().swap(evaluation);
				vector<int>().swap(order);
				for (int m = 0; m < 5; ++m)
				{
					o = (rand() % 5) + 1;
					c = (double)(rand() % 99) + 2;

					cout << "*******************************��" << i << "�β���**********************" << endl;
					cout << "�Կͻ�" << c << "����" << o << "����" << endl;

					generateNeighbour(o, c, n_s);
				}
					

				if (neighbour.size() == 0)
				{
					if (o == 1)
						cout << "�Կͻ�" << c << "û�п��е�" << "2-exchange����" << endl;
					else if (o == 2)
						cout << "�Կͻ�" << c << "û�п��е�" << "or-exchange����" << endl;
					else if (o == 3)
						cout << "�Կͻ�" << c << "û�п��е�" << "relocation����" << endl;
					else if (o == 4)
						cout << "�Կͻ�" << c << "û�п��е�" << "exchange����" << endl;
					else if (o == 5)
						cout << "�Կͻ�" << c << "û�п��е�" << "crossover����" << endl;
					continue;
				}

				//�����ھӵ�����
				computeOrder();

				vector<double>* e;
				vector<double>* n_e;
				e = new vector<double>();
				n_e = new vector<double>();
				computeEvaluation(&e, flag, best_s);
				computeEvaluation(&n_e, flag, n_s);
				if (compareE(e, &evaluation[order[0]]))
				{
					int r = rand() % order.size();
					cout << order.size() << endl;
					double diff = 0.5 * (evaluation[order[r]][0] - (*n_e)[0]) + 0.3 * (evaluation[order[r]][1] - (*n_e)[1]) + 0.2 * (evaluation[order[r]][2] - (*n_e)[2]);
					if (compareE(n_e, &evaluation[order[0]]))
					{
						if (rand() % 100 / (double)100 <= exp(diff / t))
						{
							changeToNeighbour(neighbour[order[r]], n_s);
						}
					}
					else
					{
						changeToNeighbour(neighbour[order[r]], n_s);
					}
				}
				else
				{
					changeToNeighbour(neighbour[order[0]], n_s);
					delete best_s;
					best_s = new Solution();
					best_s->doCopy(n_s);

					best_s->printSolution();
					best_s->isRegular(data);
				}

				/*s.computeLength(data);
				s.computeTime();*/

			}
			t = R * t;
			nTime = time(0);
		}
	}

	/*cout << "------------------------------------------------------" << endl;
	cout << "---------��С��·����Ŀ��õ���һ�����н�-------------" << endl;
	cout << "------------------------------------------------------" << endl;
	s.computeLength(data);
	s.computeTime();
	s.printSolution();
	s.isRegular(data);

	best_s->computeLength(data);
	best_s->computeTime();*/

	best_s->printSolution();
	best_s->isRegular(data);

	record(best_s);
}

/*
void Solve::orderRelatedness(vector<vector<double>>* relatedness)
{

}

void Solve::selectCustomers(vector<double>* cus_set, int n)
{
	vector<double>().swap(*cus_set);

	vector<vector<double>>* relatedness;

	//c[num - 2] == 1��ʾû�м���cus_set��
	vector<double> c;
	for (int i = 1; i < cus->size(); ++i)
	{
		c.push_back(1);
	}

	//���ѡ��һ���ͻ�
	cus_set->push_back((double)(rand() % 99) + 2);
	c[(*cus_set)[0]] = 0;

	for (int i = 2; i <= n; ++i)
	{
		double c = (double)(rand() % cus_set->size());
		vector<vector<double>>().swap(*relatedness);
		for (int num = 2; num < 102; ++num)
		{

		}
	}
}

void Solve::minimizeTravelCost()
{
	vector<double>* cus_set;
	int maxSearches = 5;
	int maxIterations = 5;
	int p = 10;
	for (int l = 1; l <= maxSearches; ++l)
	{
		for (int n = 1; n <= p; ++n)
		{
			for (int i = 1; i < maxIterations; ++i)
			{
				selectCustomers(cus_set, n);
				//ѡ����С�������ھ�
				//�ж��ھ��Ƿ�ȵ�ǰ�����ã�����ã��򽫵�ǰ��������Ϊ�ھӷ���
			}
		}
	}
}

*/

/*------------------------------------------------------------------------------*/
/*--------------------------------���������㷨-----------------------------------*/
/*-------------------------------------------------------------------------------*/
void Solve::generateCross(Solution* n_s)
{
	/*�����һ�β������ھӵ�����ֵ����Ϣ*/
	vector<vector<double>*>().swap(neighbour);
	vector<vector<double>>().swap(evaluation);
	vector<int>().swap(order);


}

bool Solve::isTabu(vector<vector<double>*>* tabu_list, vector<double>* n)
{

	for (int i = 0; i < tabu_list->size(); ++i)
	{
		int j;
		for (j = 0; j < n->size();)
		{
			if ((*n)[j] == (*((*tabu_list)[i]))[j + 1])
				++j;
		}
		if (j == n->size())
			return true;
	}

	return false;
}

void Solve::updateTabuList(vector<vector<double>*>* tabu_list, int iteration, int tenure)
{
	for (int i = 0; i < tabu_list->size(); ++i)
	{
		if ((*((*tabu_list)[i]))[0] + tenure >= iteration)
		{
			tabu_list->erase(tabu_list->begin() + i);
		}
	}
}

void Solve::addToTabuList(vector<vector<double>*>* tabu_list, int iteration, vector<double>* n)
{
	vector<double>* tabu_entry = new vector<double>();
	tabu_entry->push_back(iteration);
	for (int j = 0; j < n->size(); ++j)
	{
		tabu_entry->push_back((*n)[j]);
	}
	tabu_list->push_back(tabu_entry);
}

void Solve::reverseOperate(vector<double>** r_n, vector<double>* n, Solution* n_s)
{
	double o = (*n)[0];
	double i = (*n)[1];
	double j = (*n)[2];
	double r = (*n)[3];
	double m;

	if (o == 1)
	{
		Node* node_i;
		if (i == 1)
			node_i = n_s->routeSet[r]->head;
		else
			node_i = n_s->routeSet[r]->searchNode(i);
		(*r_n)->push_back(1);
		(*r_n)->push_back(i);
		(*r_n)->push_back(node_i->succ->num);
		(*r_n)->push_back(r);
	}
	else if (o == 2)
	{
		Node* node_i;
		node_i = n_s->routeSet[r]->searchNode(i);
		(*r_n)->push_back(2);
		(*r_n)->push_back(i);
		(*r_n)->push_back(node_i->pred->num);
		(*r_n)->push_back(r);
	}
	else if (o == 3)
	{
		m = (*n)[4];

		Node* node_i;
		node_i = n_s->routeSet[r]->searchNode(i);
		(*r_n)->push_back(3);
		(*r_n)->push_back(i);
		(*r_n)->push_back(node_i->pred->num);
		(*r_n)->push_back(m);
		(*r_n)->push_back(r);
	}
	else if (o == 4)
	{
		m = (*n)[4];

		Node* node_i;
		(*r_n)->push_back(4);
		(*r_n)->push_back(i);
		(*r_n)->push_back(j);
		(*r_n)->push_back(m);
		(*r_n)->push_back(r);
	}
	else if (o == 5)
	{
		m = (*n)[4];

		Node* node_i;
		(*r_n)->push_back(5);
		(*r_n)->push_back(i);
		(*r_n)->push_back(j);
		(*r_n)->push_back(r);
		(*r_n)->push_back(m);
	}
}

void Solve::tabuSearch()
{
	Solution* best_s = new Solution();
	best_s->doCopy(&s);

	Solution* n_s = new Solution();
	n_s->doCopy(&s);
	vector<double>* flag = new vector<double>();
	flag->push_back(0);


	double t;
	int o;  //������1.Two-exchange 2.Or-exchange 3.Relocation 4.Exchange 5.Crossover
	double c; //���������һ���ͻ����

	vector<vector<double>*> tabu_list;
	/*����ʱ�ĵ�������+�ھӲ���*/
	vector<double>* tabu_entry;
	int tenure;
	int maxIteration = 500;
	tenure = maxIteration / 2;
	for (int i = 0; i < maxIteration; ++i)
	{
		updateTabuList(&tabu_list, i, tenure);

		o = (rand() % 5) + 1;
		c = (double)(rand() % 99) + 2;

		cout << "*******************************��" << i << "�β���**********************" << endl;
		cout << "�Կͻ�" << c << "����" << o << "����" << endl;

		//����n_s���ھӲ������ھӵ�����ֵ
		/*�����һ�β������ھӵ�����ֵ����Ϣ*/
		vector<vector<double>*>().swap(neighbour);
		vector<vector<double>>().swap(evaluation);
		vector<int>().swap(order);
		for(int m = 0; m < 10;++m)
			generateNeighbour(o, c, n_s);

		if (neighbour.size() == 0)
		{
			if (o == 1)
				cout << "�Կͻ�" << c << "û�п��е�" << "2-exchange����" << endl;
			else if (o == 2)
				cout << "�Կͻ�" << c << "û�п��е�" << "or-exchange����" << endl;
			else if (o == 3)
				cout << "�Կͻ�" << c << "û�п��е�" << "relocation����" << endl;
			else if (o == 4)
				cout << "�Կͻ�" << c << "û�п��е�" << "exchange����" << endl;
			else if (o == 5)
				cout << "�Կͻ�" << c << "û�п��е�" << "crossover����" << endl;
			continue;
		}

		//�����ھӵ�����
		computeOrder();

		int m;
		for (m = 0; m < neighbour.size(); ++m)
		{
			if (!isTabu(&tabu_list, neighbour[order[m]]))
				break;
		}

		//��neighbour[order[m]]�ķ���������뵽���ɱ���
		vector<double>* r_n = new vector<double>();
		reverseOperate(&r_n, neighbour[order[m]], n_s);
		addToTabuList(&tabu_list, i, r_n);

		changeToNeighbour(neighbour[order[m]], n_s);

		vector<double>* e;
		vector<double>* n_e;
		e = new vector<double>();
		n_e = new vector<double>();
		computeEvaluation(&e, flag, best_s);
		computeEvaluation(&n_e, flag, n_s);
		//��������������������õĽ����������,�ı���õĽ������
		if (compareE(n_e, e))
		{
			delete best_s;
			best_s = new Solution();
			best_s->doCopy(n_s);

			best_s->printSolution();
			best_s->isRegular(data);
		}	
	}

	best_s->printSolution();
	best_s->isRegular(data);

	record(best_s);
}