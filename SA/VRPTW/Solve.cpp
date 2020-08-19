#include "Solve.h"

#include<math.h>

#define DEMAND 200

//��������ʽ�㷨��Ҫ�Ĳ���
/*1 1 1 0
  1 2 1 0
  1 1 0 1
  1 2 0 1*/
#define U1 1
#define U2 1      
#define M1 1
#define M2 0

//Ŀ�꺯������
#define G1 0.7
#define G2 0.3

//ģ���˻��㷨��Ҫ�Ĳ���
#define T 3000 //��ʼ�¶�
#define EPS 1e-8 //��ֹ�¶�
#define R 0.98 //�¶Ƚ�������


void Solve::record(Solution* temp_s)
{
	ofstream out;
	out.open("C:\\Users\\MiaoAYao\\Desktop\\SA.csv", std::ios::app);

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

	out << temp_s->routeSet.size() << " ,";
	out << temp_s->allLength << ",";
	out << temp_s->allTime << ",";
	out << 0.7 * temp_s->allLength + 0.3 * temp_s->allTime << ",";

}

Solve::Solve(Data* d)
{
	cus = &(d->data);
	data = d;
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
		//cout << "����һ���µ�·��..." << endl;
		//����һ���µ�·����ӵ�·������ĩβ
		initRoute(flag);
		//nRouteΪ��ǰ��·��
		nRoute = s.routeSet.back();


		/*����ܹ��ڵ�ǰ·���п����ҵ����еĲ���*/
		while (true)
		{

			//cout << "-----------------------------------------" << endl;
			//cout << "��ǰ·��Ϊ��";
			//nRoute->printRoute();

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
				//cout << "��" << insert_m->num << "��" << insert_n->num << "֮�����ڵ㣺" << u << endl;
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


	//cout << "------------------------------------------------------" << endl;
	//cout << "---------�ɲ�������ʽ�㷨�õ���һ�����н�-------------" << endl;
	//cout << "------------------------------------------------------" << endl;
	s.computeLength(data);
	s.computeTime();
	//s.printSolution();
	//s.isRegular(data);

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
/*------------------------- ------ģ���˻��㷨----------------------------------*/
/*-----------------------------------------------------------------------------*/
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
			
			//cout << "------------------------------------------------------------" << endl;
			//cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����2Exchange..." << endl;
			//cout << "·��r��";
			//n_s->routeSet[r]->printRoute();
			//cout << "=====" << endl;

			n_s->do2Exchange(n_s->routeSet[r], node_i,  node_i_succ, node_j, node_j_succ);

			//cout << "·��r��";
			//
			//n_s->routeSet[r]->printRoute();

			//�жϽ���2Exchange֮��õ����ھӿɲ�����
			if (n_s->routeSet[r]->isFeasibleGC(node_i->num, node_i->d_t, node_j, data))
			{
				//�������
				//������������µĿ���
				double tmp_g;
				double tmp_allLength, tmp_allTime;
				double r_time;
				tmp_allLength = n_s->allLength -
					data->distance(node_j->num, node_j_succ->num) -
					data->distance(node_i->num, node_i_succ->num) +
					data->distance(node_j->num, node_i->num) +
					data->distance(node_j_succ->num, node_i_succ->num);
				r_time = n_s->routeSet[r]->computeNewAT(node_i->num, node_i->d_t, node_j, data);
				tmp_allTime = n_s->allTime + r_time - n_s->routeSet[r]->tail->a_t;
				tmp_g = G1 * tmp_allLength + G2 * tmp_allTime;

				if (g_best_neighbour == -1 || tmp_g < g_best_neighbour)
				{
					//���ھ�Ϊ��ǰ̽��������õ��ھ�
					vector<double>().swap(best_neighbour);
					best_neighbour.push_back(1);
					best_neighbour.push_back(node_i->num);
					best_neighbour.push_back(node_j->num);
					best_neighbour.push_back(r);

					g_best_neighbour = tmp_g;
				}
			}

			n_s->restore2Exchange(n_s->routeSet[r], node_i, node_i_succ, node_j, node_j_succ);
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

			//cout << "------------------------------------------------------------" << endl;
			//cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����2Exchange..." << endl;
			//cout << "·��r��";
			//n_s->routeSet[r]->printRoute();
			//cout << "=====" << endl;

			n_s->do2Exchange(n_s->routeSet[r], node_j, node_j_succ, node_i, node_i_succ);

			//cout << "·��r��";
			//n_s->routeSet[r]->printRoute();

			//�жϽ���2Exchange֮��õ����ھӿɲ�����
			if (n_s->routeSet[r]->isFeasibleGC(node_j->num, node_j->d_t, node_i, data))
			{
				//�������
				//������������µĿ���
				double tmp_g;
				double tmp_allLength, tmp_allTime;
				double r_time;
				tmp_allLength = n_s->allLength -
					data->distance(node_j->num, node_j_succ->num) -
					data->distance(node_i->num, node_i_succ->num) +
					data->distance(node_j->num, node_i->num) +
					data->distance(node_j_succ->num, node_i_succ->num);
				r_time = n_s->routeSet[r]->computeNewAT(node_j->num, node_j->d_t, node_i, data);
				tmp_allTime = n_s->allTime + r_time - n_s->routeSet[r]->tail->a_t;
				tmp_g = G1 * tmp_allLength + G2 * tmp_allTime;

				if (g_best_neighbour == -1 || tmp_g < g_best_neighbour)
				{
					//���ھ�Ϊ��ǰ̽��������õ��ھ�
					vector<double>().swap(best_neighbour);
					best_neighbour.push_back(1);
					best_neighbour.push_back(node_j->num);
					best_neighbour.push_back(node_i->num);
					best_neighbour.push_back(r);

					g_best_neighbour = tmp_g;
				}
				
			}
			
			n_s->restore2Exchange(n_s->routeSet[r], node_j, node_j_succ, node_i, node_i_succ);
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

		//cout << "------------------------------------------------------------" << endl;
		//cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����OrExchange..." << endl;
		//cout << "·��r��";
		//n_s->routeSet[r]->printRoute();
		//cout << "=====" << endl;
		
		n_s->doOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

		//cout << "·��r��";
		//n_s->routeSet[r]->printRoute();

		double at_i, wt_i, dt_i;
		at_i = node_j->d_t + data->distance(node_j->num, node_i->num);
		wt_i = at_i > (*cus)[node_i->num - 1]->e_t ? 0 : (*cus)[node_i->num - 1]->e_t - at_i;
		dt_i = at_i + wt_i + (*cus)[node_i->num - 1]->s_t;

		//�ж�OrExchange�ھ��Ƿ����
		if (n_s->routeSet[r]->isFeasible(node_i->num, dt_i, node_j_succ, data))
		{
			//�������
			//������������µĿ���
			double tmp_g;
			double tmp_allLength, tmp_allTime;
			double  r_time;
			tmp_allLength = n_s->allLength
				+ data->distance(node_i_pred->num, node_i_succ->num)
				+ data->distance(node_j->num, node_i->num)
				+ data->distance(node_i->num, node_j_succ->num)
				- data->distance(node_i_pred->num, node_i->num)
				- data->distance(node_i->num, node_i_succ->num)
				- data->distance(node_j->num, node_j_succ->num);
			r_time = n_s->routeSet[r]->computeNewAT(node_j->num, node_j->d_t, node_i, data);
			tmp_allTime = n_s->allTime + r_time - n_s->routeSet[r]->tail->a_t;
			tmp_g = G1 * tmp_allLength + G2 * tmp_allTime;

			if (g_best_neighbour == -1 || tmp_g < g_best_neighbour)
			{
				//���ھ�Ϊ��ǰ̽��������õ��ھ�
				vector<double>().swap(best_neighbour);
				best_neighbour.push_back(2);
				best_neighbour.push_back(node_i->num);
				best_neighbour.push_back(node_j->num);
				best_neighbour.push_back(r);
				best_neighbour.push_back(1);   //Ϊ1��ʾj��iǰ��

				g_best_neighbour = tmp_g;
			}
			
		}

		n_s->restoreOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
		tmp = tmp->succ;
	}

	//j��i����
	tmp = node_i_succ;
	while (tmp != n_s->routeSet[r]->tail)
	{
		node_j = tmp;
		node_j_succ = node_j->succ;

		//cout << "------------------------------------------------------------" << endl;
		//cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����OrExchange..." << endl;
		//cout << "·��r��";
		//n_s->routeSet[r]->printRoute();
		//cout << "=====" << endl;

		n_s->doOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

		//cout << "·��r��";
		//n_s->routeSet[r]->printRoute();

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
				//������������µĿ���
				double tmp_g;
				double tmp_allLength, tmp_allTime;
				double  r_time;
				tmp_allLength = n_s->allLength
					+ data->distance(node_i_pred->num, node_i_succ->num)
					+ data->distance(node_j->num, node_i->num)
					+ data->distance(node_i->num, node_j_succ->num)
					- data->distance(node_i_pred->num, node_i->num)
					- data->distance(node_i->num, node_i_succ->num)
					- data->distance(node_j->num, node_j_succ->num);
				r_time = n_s->routeSet[r]->computeNewAT(node_i_pred->num, node_i_pred->d_t, node_i_succ, data);
				tmp_allTime = n_s->allTime + r_time - n_s->routeSet[r]->tail->a_t;
				tmp_g = G1 * tmp_allLength + G2 * tmp_allTime;

				if (g_best_neighbour == -1 || tmp_g < g_best_neighbour)
				{
					//���ھ�Ϊ��ǰ̽��������õ��ھ�
					vector<double>().swap(best_neighbour);
					best_neighbour.push_back(2);
					best_neighbour.push_back(node_i->num);
					best_neighbour.push_back(node_j->num);
					best_neighbour.push_back(r);
					best_neighbour.push_back(0);   //Ϊ1��ʾj��iǰ��

					g_best_neighbour = tmp_g;
				}

			}
		}

		n_s->restoreOrExchange(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
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

				//cout << "------------------------------------------------------------" << endl;
				//cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����Relocation..." << endl;
				//cout << "·��r��";
				//n_s->routeSet[r]->printRoute();
				//cout << "·��m��";
				//n_s->routeSet[m]->printRoute();
				//cout << "=====" << endl;

				n_s->doRelocation(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);

				//cout << "·��r��";
				//n_s->routeSet[r]->printRoute();
				//cout << "·��m��";
				//n_s->routeSet[m]->printRoute();


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
						
						//�������
						//������������µĿ���
						double tmp_g;
						double tmp_allLength, tmp_allTime;
						double  r_time, m_time;
						if (n_s->routeSet[r]->size == 1)
						{
							tmp_allLength = n_s->allLength
								+ data->distance(node_j->num, node_i->num)
								+ data->distance(node_i->num, node_j_succ->num)
								- data->distance(node_i_pred->num, node_i->num)
								- data->distance(node_i->num, node_i_succ->num)
								- data->distance(node_j->num, node_j_succ->num);
							r_time = n_s->routeSet[r]->head->d_t;
						}
						else
						{
							tmp_allLength = n_s->allLength
								+ data->distance(node_i_pred->num, node_i_succ->num)
								+ data->distance(node_j->num, node_i->num)
								+ data->distance(node_i->num, node_j_succ->num)
								- data->distance(node_i_pred->num, node_i->num)
								- data->distance(node_i->num, node_i_succ->num)
								- data->distance(node_j->num, node_j_succ->num);
							r_time = n_s->routeSet[r]->computeNewAT(node_i_pred->num, node_i_pred->d_t, node_i_succ, data);
						}
						m_time = n_s->routeSet[m]->computeNewAT(node_j->num, node_j->d_t, node_i, data);
						tmp_allTime = n_s->allTime + r_time + m_time - n_s->routeSet[r]->tail->a_t - n_s->routeSet[m]->tail->a_t;
						tmp_g = G1 * tmp_allLength + G2 * tmp_allTime;

						if (g_best_neighbour == -1 || tmp_g < g_best_neighbour)
						{
							//���ھ�Ϊ��ǰ̽��������õ��ھ�
							vector<double>().swap(best_neighbour);
							best_neighbour.push_back(3);
							best_neighbour.push_back(node_i->num);
							best_neighbour.push_back(node_j->num);
							best_neighbour.push_back(r);
							best_neighbour.push_back(m);

							g_best_neighbour = tmp_g;
						}

						
					}
				
				}
				n_s->restoreRelocation(node_i_pred, node_i, node_i_succ, node_j, node_j_succ);
				tmp = tmp->succ;
			}
		}
	}
	//cout << "here" << endl;
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

					
					//cout << "------------------------------------------------------------" << endl;
					//cout << "�Կͻ�" << node_i->num << "��" << node_j->num << "����Exchange..." << endl;
					//cout << "·��r��";
					//n_s->routeSet[r]->printRoute();
					//cout << "·��m��";
					//n_s->routeSet[m]->printRoute();
					//cout << "=====" << endl;

					n_s->doExchange(node_i_pred, node_i, node_i_succ, node_j_pred, node_j, node_j_succ);

					//cout << "·��r��";
					//n_s->routeSet[r]->printRoute();
					//cout << "·��m��";
					//n_s->routeSet[m]->printRoute();

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
								//������������µĿ���
								double tmp_g;
								double tmp_allLength, tmp_allTime;
								double  r_time, m_time;

								tmp_allLength = n_s->allLength
									+ data->distance(node_i_pred->num, node_j->num)
									+ data->distance(node_j->num, node_i_succ->num)
									+ data->distance(node_j_pred->num, node_i->num)
									+ data->distance(node_i->num, node_j_succ->num)
									- data->distance(node_i_pred->num, node_i->num)
									- data->distance(node_i->num, node_i_succ->num)
									- data->distance(node_j_pred->num, node_j->num)
									- data->distance(node_j->num, node_j_succ->num);
								r_time = n_s->routeSet[r]->computeNewAT(node_i_pred->num, node_i_pred->d_t, node_j, data);
								m_time = n_s->routeSet[m]->computeNewAT(node_j_pred->num, node_j_pred->d_t, node_i, data);
								tmp_allTime = n_s->allTime + r_time + m_time - n_s->routeSet[r]->tail->a_t - n_s->routeSet[m]->tail->a_t;
								tmp_g = G1 * tmp_allLength + G2 * tmp_allTime;

								if (g_best_neighbour == -1 || tmp_g < g_best_neighbour)
								{
									//���ھ�Ϊ��ǰ̽��������õ��ھ�
									vector<double>().swap(best_neighbour);
									best_neighbour.push_back(4);
									best_neighbour.push_back(node_i->num);
									best_neighbour.push_back(node_j->num);
									best_neighbour.push_back(r);
									best_neighbour.push_back(m);

									g_best_neighbour = tmp_g;
								}
								
							}
						}
					}	
				}
				n_s->restoreExchange(node_i_pred, node_i, node_i_succ, node_j_pred, node_j, node_j_succ);
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

					//cout << "------------------------------------------------------------" << endl;
					//cout << "��" << node_i->num << "��" << node_j->num << "��crossover..." << endl;
					//cout << "·��r��";
					//n_s->routeSet[r]->printRoute();
					//cout << "·��m��";
					//n_s->routeSet[m]->printRoute();
					//cout << "=====" << endl;

					n_s->doCrossover(node_i, node_i_succ, node_j, node_j_succ);
					Node* ptail = n_s->routeSet[r]->tail;
					n_s->routeSet[r]->tail = n_s->routeSet[m]->tail;
					n_s->routeSet[m]->tail = ptail;

					//cout << "·��r��";
					//n_s->routeSet[r]->printRoute();
					//cout << "·��m��";
					//n_s->routeSet[m]->printRoute();

					//�ж�Crossover�ھ��Ƿ����
					if (n_s->routeSet[m]->isFeasible(node_j->num, node_j->d_t, node_i_succ, data) &&
						n_s->routeSet[r]->isFeasible(node_i->num, node_i->d_t, node_j_succ, data))
					{	
						//�������
						//������������µĿ���
						double tmp_g;
						double tmp_allLength, tmp_allTime;
						double  r_time, m_time;

						if (node_j->num == 1 && node_i_succ->num == 1)
						{
							tmp_allLength = n_s->allLength
								+ data->distance(node_i->num, node_j_succ->num)
								- data->distance(node_i->num, node_i_succ->num)
								- data->distance(node_j->num, node_j_succ->num);
							m_time = n_s->routeSet[m]->head->d_t;
						}
						else
						{
							tmp_allLength = n_s->allLength
								+ data->distance(node_j->num, node_i_succ->num)
								+ data->distance(node_i->num, node_j_succ->num)
								- data->distance(node_i->num, node_i_succ->num)
								- data->distance(node_j->num, node_j_succ->num);
							m_time = n_s->routeSet[m]->computeNewAT(node_j->num, node_j->d_t, node_i_succ, data);
						}
						r_time = n_s->routeSet[r]->computeNewAT(node_i->num, node_i->d_t, node_j_succ, data);

						tmp_allTime = n_s->allTime + r_time + m_time - n_s->routeSet[r]->tail->a_t - n_s->routeSet[m]->tail->a_t;
						tmp_g = G1 * tmp_allLength + G2 * tmp_allTime;

						if (g_best_neighbour == -1 || tmp_g < g_best_neighbour)
						{
							//���ھ�Ϊ��ǰ̽��������õ��ھ�
							vector<double>().swap(best_neighbour);
							best_neighbour.push_back(5);
							best_neighbour.push_back(node_i->num);
							best_neighbour.push_back(node_j->num);
							best_neighbour.push_back(r);
							best_neighbour.push_back(m);

							g_best_neighbour = tmp_g;
						}
								
					}
					n_s->restoreCrossover(node_i, node_i_succ, node_j, node_j_succ);
					ptail = n_s->routeSet[r]->tail;
					n_s->routeSet[r]->tail = n_s->routeSet[m]->tail;
					n_s->routeSet[m]->tail = ptail;
				}
				tmp = tmp->succ;
			}
		}
	}
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

/*ģ���˻��㷨*/
void Solve::simulatedAnnealing()
{
	Solution* best_s = new Solution();
	best_s->doCopy(&s);

	Solution* n_s = new Solution();
	n_s->doCopy(&s);

	//��һ����Ϊ0ʱ��ʾ�������tmp_s���ڵ�ǰ��������������κα仯
    //1����һ��2-exchange 2����һ��or-exchange 3����һ��relocation 4����һ��exchange 5����һ��crossover		 
	vector<double>* flag = new vector<double>();
	flag->push_back(0);

	int maxIteration = 2500;
	double t;
	int o;  //������1.Two-exchange 2.Or-exchange 3.Relocation 4.Exchange 5.Crossover
	double c; //���������һ���ͻ����

	//��õ�ǰʱ��
	time_t iTime = time(0);
	time_t nTime = time(0);
	while (nTime - iTime < 0.1)
	{
		t = T;
		while (nTime - iTime < 0.1 && t > EPS)
		{
			for (int i = 1; i < maxIteration; i++)
			{
				//����n_s���ھӲ������ھӵ�����ֵ
				/*�����һ�β������ھӵ�����ֵ����Ϣ*/
				vector<double>().swap(best_neighbour);
				//0��ʾû���ҵ����е��ھ�
				best_neighbour.push_back(0);
				g_best_neighbour = -1;

				/*
				for (int l = 0; l < 50; ++l)
				{
					o = (rand() % 5) + 1;
					c = (double)(rand() % 99) + 2;
					//cout << "*****************************************************" << endl;
					//cout << "�Կͻ�" << c << "����" << o << "����" << endl;
					generateNeighbour(o, c, n_s);
				}
				*/

				for (c = 2; c <= 101; ++c)
				{
					for(int k = 1; k <=5;++k)
						generateNeighbour(k, c, n_s);
				}

				if (g_best_neighbour != -1)
				{
					double e = G1 * best_s->allLength + G2 * best_s->allTime;
					double n_e = G1 * n_s->allLength + G2 * n_s->allTime;

					if (e < g_best_neighbour)
					{
						double diff = n_e - g_best_neighbour;
						if (diff < 0)
						{
							if (rand() % 100 / (double)100 <= exp(diff / t))
							{
								changeToNeighbour(&best_neighbour, n_s);
							}
						}
						else
						{
							changeToNeighbour(&best_neighbour, n_s);
						}
					}
					else
					{
						changeToNeighbour(&best_neighbour, n_s);
						delete best_s;
						best_s = new Solution();
						best_s->doCopy(n_s);

						//best_s->printSolution();
						//best_s->isRegular(data);
					}
				}

			}
			t = R * t;
			nTime = time(0);
		}
	}
	//best_s->printSolution();
	//best_s->isRegular(data);

	record(best_s);
}