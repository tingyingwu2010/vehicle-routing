#include "Problem.h"

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

//Russell(1995)�������
#define V1 0.5
#define V2 0.5

//Russell(2004)�������
#define T 3000 //��ʼ�¶�
#define EPS 1e-8 //��ֹ�¶�
#define R 0.98 //�¶Ƚ�������

#define maxIt 10 //����������

/*------------------------------------------------------------------------------*/
/*-------------------------------solomon(1987)----------------------------------*/
/*--------------------------��ʡ����ʽ·�������㷨-------------------------------*/
/*------------------------------------------------------------------------------*/
void Problem::buildSavings()
{
	allLength = 0;   //��ʼ����ܳ���
	vector<double> *saving;
	int size = data.size();
	//�����ʡ��ֵ
	for (int i = 1; i < size - 1; ++i)  
	{
		for (int j = i + 1; j < size; ++j)
		{
			//ֻ����ͬһ������ʱ�ż������
			if (data[i]->box_x == data[j]->box_x && data[i]->box_y == data[j]->box_y)
			{
				saving = new vector<double>();
				saving->push_back(dis[i - 1][0] + dis[j - 1][0] - dis[j - 1][i]);
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

void Problem::deleteSavings(int cus_i, int cus_j)
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

void Problem::downAdjust(int i)
{
	int size = savings.size();
	for (int j = 2 * i + 1; j < size;)
	{
		vector<double>* temp = savings[i];
		int maxSav = j;
		if (j + 1 < size)
		{
			if ((*savings[j])[0] < (*savings[j + 1])[0])
				maxSav = j + 1;
		}
		savings[i] = savings[maxSav];
		savings[maxSav] = temp;
		j = 2 * maxSav + 1;
	}
}

//ǰһ���ڵ�ı��;ǰһ���ڵ���뿪ʱ��;�����·��
bool Problem::judge(double pre_num, double pre_dTime, Route* r)
{
	double p_num = pre_num;
	double p_dTime= pre_dTime;
	double aTime;              //���ﵱǰ�ڵ��ʱ��
	double wTime;				//�ڵ�ǰ�ڵ�ȴ���ʱ��
	Node* tmp = r->head;        //��ǰ�ڵ�
	while (tmp)
	{
		double distance;
		if (p_num > tmp->num)
			distance = dis[p_num - 2][tmp->num - 1];
		else
			distance = dis[tmp->num - 2][p_num - 1];
		aTime = p_dTime + distance;
		if (aTime > tmp->l_t)
		{
			return false;
		}
		else
		{
			wTime = tmp->e_t > aTime ? tmp->e_t - aTime : 0;
			p_dTime = aTime + wTime + tmp->s_t;
			p_num = tmp->num;
			tmp = tmp->next;
		}
	}
	//�жϻص��ֿ��ʱ����û��Υ��ʱ�䴰
	if (p_dTime + dis[p_num - 2][0] > data[0]->l_t)
		return false;

	return true;
}

void Problem::updateRoute(Route* route, double num)
{
	
	double i = num;		//ǰһ���ڵ���
	double dt_i = data[i - 1]->d_t;   //��ǰһ���ڵ��뿪��ʱ��
	double dis_ij;    //�����ڵ�֮��ľ���
	double j;		//��ǰ�ڵ���
	double at_j;
	double wt_j;
	double dt_j;

	Node* tmp = data[num - 1]->next;
	while (tmp)
	{
		j = tmp->num;

		if (i > j)
			dis_ij = dis[i - 2][j - 1];
		else
			dis_ij = dis[j - 2][i - 1];

		at_j = dt_i + dis_ij;
		wt_j = at_j > data[j - 1]->e_t ? 0 : data[j - 1]->e_t - at_j;
		dt_j = at_j + wt_j + data[j - 1]->s_t;

		data[j - 1]->a_t = at_j;
		data[j - 1]->w_t = wt_j;
		data[j - 1]->d_t = dt_j;

		tmp = tmp->next;
		i = j;
		dt_i = dt_j;
	}

	route->at_depot = dt_i + dis[i - 2][0];
}

void Problem::addRoute(Route* r1, Route* r2)
{
	r1->tail->next = r2->head;
	r1->tail = r2->tail;
	r1->size += r2->size;
	r1->demand += r2->demand;
	
	updateRoute(r1, r1->tail->num);
}

void Problem::useSavings()
{
	Route *route;

	//Build original solution
	for (int i = 1; i < data.size(); ++i)
	{

		//�Ӳֿ�ڵ�data[0]�ĳ���ʱ��Ϊdata[0]->e_t
		data[i]->a_t = data[0]->e_t + dis[i - 1][0];
		data[i]->w_t = data[i]->e_t > data[i]->a_t ? data[i]->e_t - data[i]->a_t : 0;
		data[i]->d_t = data[i]->a_t + data[i]->w_t + data[i]->s_t;

		route = new Route(data[i], data[0]->e_t, data[i]->d_t + dis[data[i]->num - 2][0]);
		routeSet.push_back(route);
	}
	//Compute original length
	allLength = 0;
	for (int i = 0; i < dis.size(); ++i)
	{
		allLength += dis[i][0] * 2;
	}

	//Build better solution
	while (savings.size() != 0)
	{
		cout << "--------------------------------------------" << endl;
		cout <<"Savings����Ŀ��Ϊ�� " << savings.size() << endl;
		/* �����cus_i -> cus_j���ܹ���ʡ������ıߵ���Ϣ*/
		int cus_i = (*savings[0])[1];
		int cus_j = (*savings[0])[2];
		double sav = (*savings[0])[0];

		cout << "���ӽڵ㣺" << cus_i << "��" << cus_j << endl;

		/* �ҵ��������ϵ������ڵ�ֱ����ĸ�·���� */
		/* route_i:�����ڵ�i��·��   route_j:�����ڵ�j��·��
		   flag_i = 0:�ڵ�i��·��ͷ  flag_i = 1:�ڵ�i��·��β*/
		   /* del_i = 0:��ʾ�ϲ�����·���󣬽ڵ�i����·��ͷ��β����֮del_i = 1*/
		int route_i, route_j;
		int flag_i = 0, flag_j = 0;
		int del_i = 0, del_j = 0;

		//�ҵ�·�����а����ͻ�cus_i��cus_j������·��
		int flag = 0;  //���flag==2����ʾ�Ѿ��ҵ�������·��������ѭ��
		for (int i = 0; i < routeSet.size(); ++i)
		{
			Route* temp = routeSet[i];
			if ((temp->head)->num == cus_i)
			{
				route_i = i;
				++flag;
			}
			else if ((temp->tail)->num == cus_i)
			{
				route_i = i;
				flag_i = 1;
				++flag;
			}

			if ((temp->head)->num == cus_j)
			{
				route_j = i;
				++flag;
			}
			else if ((temp->tail)->num == cus_j)
			{
				route_j = i;
				flag_j = 1;
				++flag;
			}

			if (flag == 2)
				break;
		}
		
		if (routeSet[route_i]->size > 1)
		{
			del_i = 1;
			cout << "ɾ����" << cus_i << "��ص�saving" << endl;
		}
		if (routeSet[route_j]->size > 1)
		{
			del_j = 1;
			cout << "ɾ����" << cus_j << "��ص�saving" << endl;
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
		if (routeSet[route_i]->demand + routeSet[route_j]->demand > DEMAND)
		{
			//·���ϵĽڵ�����󳬹�������������
			deleteSavings();
			continue;
		}


		if (flag_i == 0 && flag_j == 1)
		{
			//�����Υ��ʱ�䴰�Ļ�
			if (judge(routeSet[route_j]->tail->num, routeSet[route_j]->tail->d_t, routeSet[route_i]))
			{
				//��route_i�ϵĽڵ���ӵ�route_j��
				addRoute(routeSet[route_j], routeSet[route_i]);

				//ɾ��·��route_i
				routeSet.erase(routeSet.begin() + route_i);
			}
			else
			{
				deleteSavings();
				continue;
			}
		}
		else if (flag_i == 1 && flag_j == 0)
		{
			//�����Υ��ʱ�䴰�Ļ�
			if (judge(routeSet[route_i]->tail->num, routeSet[route_i]->tail->d_t, routeSet[route_j]))
			{
				//��route_j�ϵĽڵ���ӵ�route_i��
				addRoute(routeSet[route_i], routeSet[route_j]);

				//ɾ��·��route_i
				routeSet.erase(routeSet.begin() + route_j);
			}
			else
			{
				deleteSavings();
				continue;
			}
		}
		else
		{
			if (flag_i == 0 && flag_j == 0)
			{
				//��route_i�ϵĽڵ㷴ת������route_j��ӵ�route_i֮��
				routeSet[route_i]->reverseRoute();
			}
			else if (flag_i == 1 && flag_j == 1)
			{
				//��route_j�ϵĽڵ㷴ת,������ӵ�route_i֮��
				routeSet[route_j]->reverseRoute();
			}

			//�����Υ��ʱ�䴰�Ļ�
			if (judge(routeSet[route_i]->tail->num, routeSet[route_i]->tail->d_t, routeSet[route_j]))
			{
				//��route_j�ϵĽڵ���ӵ�route_i��
				addRoute(routeSet[route_i], routeSet[route_j]);

				//ɾ��·��route_i
				routeSet.erase(routeSet.begin() + route_j);
			}
			else
			{
				//��֮ǰ�����ĸı仹ԭ
				if (flag_i == 0 && flag_j == 0)
				{
					//��route_i�ϵĽڵ㷴ת������route_j��ӵ�route_i֮��
					routeSet[route_i]->reverseRoute();
				}
				else if (flag_i == 1 && flag_j == 1)
				{
					//��route_j�ϵĽڵ㷴ת,������ӵ�route_i֮��
					routeSet[route_j]->reverseRoute();
				}

				deleteSavings();
				continue;
			}
		}

		/* ���㵱ǰ����·���ܳ��� */
		allLength -= sav;
		cout << "allLength = " << allLength << endl;

		/* ɾ��savings���Ѳ���·��ͷ��β�Ľڵ����ڵıߣ�������һ�μӱ�*/
		if (del_i == 1 && del_j == 1)
		{
			deleteSavings(cus_i, cus_j);
		}
		else if (del_i == 1 && del_j == 0)
		{
			deleteSavings(cus_i);
		}
		else if (del_i == 0 && del_j == 1)
		{
			deleteSavings(cus_j);
		}
		else
		{
			deleteSavings();
		}
		
	}
	cout << "------------------------------------------------------" << endl;
	cout << "---------�ɽ�ʡ����ʽ�㷨�õ���һ�����н�-------------" << endl;
	cout << "------------------------------------------------------" << endl;
	computeTime();
	printSolution();
}

/*------------------------------------------------------------------------------*/
/*-------------------------------solomon(1987)----------------------------------*/
/*--------------------------��������ʽ·�������㷨-------------------------------*/
/*------------------------------------------------------------------------------*/
void Problem::initInfor()
{
	vector<double>* tmp;
	for (int i = 1; i < data.size(); ++i)
	{
		tmp = new vector<double>();
		tmp->push_back(data[i]->num);
		tmp->push_back(-1);
		tmp->push_back(-1);    
		tmp->push_back(-1);
		infor.push_back(*tmp);
	}
}

bool Problem::judgeInsert(double num, double d_t, Node* node)
{
	
	Node* tmp = node;
	double m = num;  //ǰһ���ڵ�ı��
	double n;              //��ǰ�ڵ�ı��
	double m_dt = d_t;   //�뿪ǰһ���ڵ��ʱ��
	double n_at;           //���ﵱǰ�ڵ��ʱ��
	double n_wt;		//�ڵ�ǰ�ڵ�ȴ���ʱ��
	double n_dt;		//�뿪��ǰ�ڵ��ʱ��
	double dis_mn;  //ǰһ�ڵ㵽��ǰ�ڵ�ľ���
	double pf;
	while (tmp)
	{	
		n = tmp->num;
		if (m > n)
			dis_mn = dis[m - 2][n - 1];
		else
			dis_mn = dis[n - 2][m - 1];

		n_at = m_dt + dis_mn;
		n_wt = n_at > data[n - 1]->e_t ? 0 : data[n - 1]->e_t - n_at;
		n_dt = n_at + n_wt + data[n - 1]->s_t;

		pf = n_at - data[n - 1]->a_t;
		if (n_at > data[n - 1]->l_t)
		{
			return false;
		}
		else if (pf <= data[n - 1]->w_t)   //֮��Ľڵ㶼�ǿ��е�
		{
			return true;
		}
		else
		{
			m_dt = n_dt;
			m = n;
			tmp = tmp->next;
		}
	}
	//�жϻص��ֿ��ʱ����û�г����������ʱ��
	if (m_dt + dis[m - 2][0] > data[0]->l_t)
		return false;

	return true;
}

void Problem::updateInfor(Route* route)
{
	for (int i = 0; i < infor.size(); ++i)
	{

		double u = infor[i][0];  //Ҫ����Ľڵ�u
		Node* tmp = route->head;
		int m = 1, n = tmp->num; //���ڵ�u���뵽�ڵ�m��n֮��

		double insert_m = -1;    //��õĲ���λ��
		double insert_n = -1;

		double c_1 = -1;		//��Ҫ�����ֵ
		double c_11;
		double c_12;
		
		//����Υ������Լ��ʱ���Ž��п��ǲ���λ�õ�ѡ��
		if (data[u - 1]->d + route->demand < DEMAND)
		{
			//m�뵱ǰ�ڵ�u�ľ��룻m��n�ľ��룻u��n�ľ���
			double dis_mu, dis_mn, dis_un;

			/*at_u������ڵ�u��ʱ��*/
			double at_u;
			double wt_u;
			double dt_u;
			double at_n;

			while (tmp || n == 1)
			{
				if (m > u)
					dis_mu = dis[m - 2][u - 1];
				else
					dis_mu = dis[u - 2][m - 1];

				
				if (m > n)
					dis_mn = dis[m - 2][n - 1];
				else
					dis_mn = dis[n - 2][m - 1];

				if (n > u)
					dis_un = dis[n - 2][u - 1];
				else
					dis_un = dis[u - 2][n - 1];

				at_u = data[m - 1]->d_t + dis_mu;
				/*�ж��Ƿ�Υ���˽ڵ�u��ʱ�䴰Լ��
				���Υ���ˣ���ô����Ȼ�ڵ�ǰλ�ò��붼�������ˣ���ô֮���λ��Ҳ���ó�����*/
				if (at_u + data[u - 1]->s_t > data[u - 1]->l_t)
				{
					break;
				}

				//�����ǰ�ڵ�û��Υ��ʱ�䴰Լ��
				/*�������λ�ò���·��ĩβ*/
				c_11 = dis_mu + dis_un - U1 * dis_mn;

				//at_n:����ڵ�u�󵽴�n��ʱ�䣻wt_u���ڽڵ�u�ĵȴ�ʱ�䣻dt_u���ӽڵ�u�ĳ���ʱ��
				wt_u = at_u > data[u - 1]->e_t ? 0 : data[u - 1]->e_t - at_u;
				dt_u = at_u + wt_u + data[u - 1]->s_t;
				at_n = dt_u + dis_un;
				double wt_n = at_n > data[n - 1]->e_t ? 0 : data[n - 1]->e_t - at_n;
				c_12 = at_n + wt_n - (data[n - 1]->a_t + data[n - 1]->w_t);

				
				//ֻ�в���ڵ�u�󣬿���ʹ·������u֮��Ľڵ㶼���У����ܲ��������λ��
				if (judgeInsert(u, dt_u, tmp))
				{
					double tmp_c_1 = M1 * c_11 + M2 * c_12;
					if (c_1 == -1 || tmp_c_1 < c_1)
					{
						c_1 = tmp_c_1;
						insert_m = m;
						insert_n = n;
					}
				}
				
				if (n == 1)
					break;

				tmp = tmp->next;
				if (tmp)     
				{
					m = n;
					n = tmp->num;
				}
				else     //����ڲֿ�֮ǰ����ڵ�u
				{
					m = n;
					n = 1;
				}
			}	
		}
		
		infor[i][1] = insert_m;
		infor[i][2] = insert_n;
		infor[i][3] = c_1;
	}
}

void Problem::initRoute(int flag)
{
	//ѡ���������ʼ��·���Ľڵ���
	double c_num;
	//infor��Ҫɾ������Ŀ
	int del;

	/*����ѡ��1����ֿ���Զ�Ľڵ㣻2������ʱ�俪ʼ����Ľڵ�*/
	if (flag == 1)
	{
		c_num = infor[0][0];
		double maxDis = dis[c_num - 2][0];
		del = 0;

		//��û������·������ֿ���Զ�Ľڵ�
		for (int i = 1;i < infor.size(); ++i)
		{
			if (dis[infor[i][0] - 2][0] > maxDis)
			{
				c_num = infor[i][0];
				maxDis = dis[c_num - 2][0];
				del = i;
			}
		}
	}
	else if (flag == 2)
	{	
		c_num = infor[0][0];
		double eTime = data[c_num]->e_t;
		del = 0;
		
		
		//��û������·���з���ʱ�俪ʼ����Ľڵ�
		for (int j = 1 ; j < infor.size(); ++j)
		{
			if (data[infor[j][0]]->e_t < eTime)
			{
				c_num = infor[j][0];
				eTime = data[c_num]->e_t;
				del = j;
			}
		}
	}

	//ɾ��infor�е�del����Ŀ
	infor.erase(infor.begin() + del);

	//�Ӳֿ�ڵ�data[0]�ĳ���ʱ��Ϊdata[0]->e_t
	data[c_num - 1]->a_t = data[0]->e_t + dis[c_num - 2][0];
	data[c_num - 1]->w_t = data[c_num - 1]->e_t > data[c_num - 1]->a_t ? data[c_num - 1]->e_t - data[c_num - 1]->a_t : 0;
	data[c_num - 1]->d_t = data[c_num - 1]->a_t + data[c_num - 1]->w_t + data[c_num - 1]->s_t;
	
	Route* route = new Route(data[c_num - 1], data[0]->e_t, data[c_num]->d_t + dis[c_num - 1][0]);
	routeSet.push_back(route);

}

void Problem::useInsertion()
{
	int flag;
	cout << "------- --ѡ���ʼ��һ��·���ķ���----------------------" << endl;
	cout << "----------���ѡ��ʹ����ֿ���Զ�Ľڵ��ʼ��·�������룺1---------------" << endl;
	cout << "----------���ѡ��ʹ�÷���ʱ�俪ʼ����Ľڵ��ʼ��·�������룺2---------" << endl;
	cin >> flag;

	Route* nRoute;
	initInfor();
	while (infor.size() != 0)   //һֱ�����µ�·����ֱ�����еĽڵ㶼���뵽һ��·����
	{
		cout << "����һ���µ�·��..." << endl;
		//����һ���µ�·����ӵ�·������ĩβ
		initRoute(flag);         
		//nRouteΪ��ǰ��·��
		nRoute = routeSet.back();
		

		/*����ܹ��ڵ�ǰ·���п����ҵ����еĲ���*/
		while (true)
		{

			cout << "��ǰ·��Ϊ��";
			nRoute->print();

			//����infor
			updateInfor(nRoute);

			//�ҵ���õ��Ǹ��ڵ���뵽·����
			double c_2;
			double num = -1; //Ҫ����Ľڵ����ĿΪinfor[num]
			int i;
			for (i = 0; i < infor.size(); ++i)
			{
				if (infor[i][1] != -1) //infor[i][1]Ϊ-1ʱ��ʾ�ڵ�ǰ·����û�п��еĲ���ѡ��
				{
					c_2 = U2 * dis[infor[i][0] - 2][0] - infor[i][2];
					num = i;
					break;
				}
			}
			for (; i < infor.size(); ++i)
			{
				if (infor[i][1] != -1)
				{
					double tmp_c_2 = U2 * dis[infor[i][0] - 2][0] - infor[i][3];
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
				double insert_m, insert_n;  //�ڱ��Ϊinsert_m��insert_n�Ľڵ��м����ýڵ�
				//������Ϣ��infor[num]�д洢
				u = infor[num][0];
				insert_m = infor[num][1];
				insert_n = infor[num][2];
				//����Ŀinfor[num]ɾ������ʾ�ڵ�u�Ѿ���ӵ�·����
				infor.erase(infor.begin() + num);

				/*���ڵ�u���뵽insert_m��insert_n֮��*/
				cout << "��" << insert_m << "��" << insert_n << "֮�����ڵ㣺" << u << endl;
				if (insert_m == 1)
				{
					data[u - 1]->next = data[insert_n - 1];
					nRoute->head = data[u - 1];
				}
				else if (insert_n == 1)
				{
					data[insert_m - 1]->next = data[u - 1];
					nRoute->tail = data[insert_m - 1];
				}
				else
				{
					data[insert_m - 1]->next = data[u - 1];
					data[u - 1]->next = data[insert_n - 1];
				}
					

				/*���½ڵ�u�ĵ���ʱ�����Ϣ*/
				double dis_mu;
				if (insert_m > u)
					dis_mu = dis[insert_m - 2][u - 1];
				else
					dis_mu = dis[u - 2][insert_m - 1];

				data[u - 1]->a_t = data[insert_m - 1]->d_t + dis_mu;
				data[u - 1]->w_t = data[u - 1]->a_t > data[u - 1]->e_t ? 0 : data[u - 1]->e_t - data[u - 1]->a_t;
				data[u - 1]->d_t = data[u - 1]->a_t + data[u - 1]->w_t + data[u - 1]->s_t;


				/*���½ڵ�u֮��ڵ�ĵ���ʱ���*/
				updateRoute(nRoute, u);
			}
		}
	}
	

	cout << "------------------------------------------------------" << endl;
	cout << "---------�ɲ�������ʽ�㷨�õ���һ�����н�-------------" << endl;
	cout << "------------------------------------------------------" << endl;
	computeLength();
	computeTime();
	printSolution();
}

/*------------------------------------------------------------------------------*/
/*-------------------------------Russell(1995)----------------------------------*/
/*-------------------------------------------------------------------------------*/
void Problem::printU()
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

void Problem::computeU()
{
	//Ϊ���������У���ʼ��u��u[0]�Ǳ��Ϊ2�Ľڵ��U_i
	int size = data.size();
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
	for (int i = 0; i < routeSet.size(); ++i)
	{
		Route* tmp = routeSet[i];
		Node* temp = tmp->head;
		double m = 1, n; //��ǰ�ڵ��ǰһ���ڵ�ͺ�һ���ڵ�ı��
		double u_1, u_2;  //M1�еĽڵ�1�ͽڵ�2
		double v_1 = -1, v_2 = -1;  //M2�еĽڵ�1�ͽڵ�2
		while (temp)
		{
			//ȷ��u_1��u_2
			u_1 = temp->num;
			if (temp->next != NULL)
			{
				temp = temp->next;
				u_2 = temp->num;
				if (temp->next != NULL)
				{
					n = temp->next->num;
				}
				else
				{
					n = 1;
				}
			}
			else
			{
				temp = temp->next;
				//·����ֻ��һ���ڵ�
				u_2 = -1;
				n = 1;
			}

			//ȷ��v_1��v_2
			double d_1 = -1, d_2 = -1;
			double dis_mv, dis_vn;
			for (int j = 0; j < routeSet.size(); ++j)
			{
				if (j != i)
				{
					Node* v = (routeSet[j])->head;
					for (; v; v = v->next)
					{
						if (v->num > m)
							dis_mv = dis[v->num - 2][m - 1];
						else
							dis_mv = dis[m - 2][v->num - 1];

						if (v->num > n)
							dis_vn = dis[v->num - 2][n - 1];
						else
							dis_vn = dis[n - 2][v->num - 1];

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

void Problem::localSearch1()
{
	//ʹ��z�������Ƿ��µĽ��ԭʼ�����
	double z = V1 * allLength + V2 * allTime;
	//·��������
	int v = routeSet.size();

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
int Problem::searchRoute(double num)
{
	Node* tmp = data[num - 1];
	Node* tail = tmp;
	while (tmp->next)
	{
		tail = tmp->next;
		tmp = tmp->next;
	}
	int i;
	for (i = 0; i < routeSet.size(); ++i)
	{
		if (routeSet[i]->tail == tail)
			break;
	}
	return i;
}

double Problem::computeLatestArriveTime(double num)
{
	double t;
	if (num == 1)
	{
		t = data[0]->l_t;
		return t;
	}

	if (data[num - 1]->next == NULL)
	{
		t = data[0]->l_t - dis[num - 2][0] - data[num - 1]->s_t;
	}
	else
	{
		double d;
		double n_num = data[num - 1]->next->num;
		if (num > n_num)
			d = dis[num - 2][n_num - 1];
		else
			d = dis[n_num - 2][num - 1];

		t = computeLatestArriveTime(n_num) - d - data[num - 1]->s_t;
	}
	return data[num - 1]->l_t < t ? data[num - 1]->l_t : t;
}

void Problem::computeOrder()
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
				int temp = order[j - 1];
				order[j - 1] = order[j];
				order[j] = temp;
			}
			else
			{
				break;
			}
		}
	}
}

double Problem::computeMinimalDelay()
{
	double num = 0;

	//r��ʾ�ڵ������ٵ�·���������ҵ�·��r
	int r = 0;
	for (int i = 1; i < routeSet.size(); ++i)
	{
		if (routeSet[i]->size < routeSet[r]->size)
		{
			r = i;
		}
	}

	Node* tmp = routeSet[r]->head;
	while (tmp)
	{
		double t_num = -1;
		for (int i = 0; i < routeSet.size(); ++i)
		{
			//���ڲ��ǵ�ǰ·����·��
			if (i != r)
			{
				//�޷����뵱ǰ·��
				if (tmp->d + routeSet[i]->demand > DEMAND)
				{
					continue;
				}
				else
				{
					//���Խ��ڵ�tmp��·��routeSet[i]���ض�λ
					double u = tmp->num;
					int m = 1, n;
					Node* p = routeSet[i]->head;
					while (p)
					{
						if (p)
							n = p->num;
						else
							n = 1;

						//�����ж���nǰ�����᲻�����u�Լ�u����Ľڵ㲻����
						double dis_mu;
						if (m > u)
							dis_mu = dis[m - 2][u - 1];
						else
							dis_mu = dis[u - 2][m - 1];

						double dis_un;
						if (n > u)
							dis_un = dis[n - 2][u - 1];
						else
							dis_un = dis[u - 2][n - 1];

						double dt_m;
						if (m == 1)
							dt_m = data[0]->e_t;
						else
							dt_m = data[m - 1]->d_t;

						double at_u = dt_m + dis_mu;
						double wt_u = at_u > data[u - 1]->e_t ? 0 : data[u - 1]->e_t - at_u;
						double dt_u = at_u + wt_u + data[u - 1]->s_t;


						//�������λ�ÿ���
						if (at_u <= data[u - 1]->l_t && judgeInsert(u, dt_u, p))
						{
							t_num = 0;
							break;
						}
						//�������λ�ò�����
						else
						{
							double x = dt_m + dis_mu - data[u - 1]->l_t;
							double n1 = x > 0 ? x : 0;

							x = dt_u + dis_un - computeLatestArriveTime(n);
							double n2 = x > 0 ? x : 0;

							if (t_num == -1 || n1 + n2 < t_num)
								t_num = n1 + n2;
						}

						m = n;
						p = p->next;
					}
				}
			}

		}

		if (t_num = -1)
			num = -1;
		else
			num += t_num;
		tmp = tmp->next;
	}

	return num;
}

void Problem::reversePartialRoute(double i, double j)
{
	Node* pre = data[i - 1];
	Node* tmp = pre->next;
	while (tmp != data[j - 1])
	{
		Node* suc = tmp->next;
		tmp->next = pre;

		pre = tmp;
		tmp = suc;
	}
	tmp->next = pre;
}

void Problem::generate2Exchange(double num)
{
	double i = num;
	int r = searchRoute(i); //�ͻ�c���ڵ�·��

	//����i->i_succ,j->j_succ����i->j,i_succ->j_succ
	vector<double>* n;
	vector<double>* e;
	double i_succ;  //��ǰ�ͻ��ڵ�i�ĺ�һ���ڵ�
	if (data[i - 1]->next)
		i_succ = data[i - 1]->next->num;
	else
		i_succ = 1;

	//��j��i����ʱ
	double j, j_succ;   //j��j�ĺ�һ���ڵ�
	Node* tmp = data[i - 1];
	tmp = tmp->next;  //i����Ľڵ�i+1
	if (tmp) //����ڵ㲻Ϊ��ʱ�������ҵ����ʵ�j
	{
		tmp = tmp->next; //�ڵ�i+2
		while (tmp)
		{
			j = tmp->num;
			if (tmp->next)
				j_succ = tmp->next->num;
			else
				j_succ = 1;

			n = new vector<double>();
			n->push_back(1);
			n->push_back(i);
			n->push_back(i_succ);
			n->push_back(j);
			n->push_back(j_succ);

			n->push_back(r);

			//������������µĿ���
			e = new vector<double>();
			e->push_back(computeFirstEvaluation(n));
			e->push_back(computeSecondEvaluation(n));
			e->push_back(computeThirdEvaluation(n));

			neighbour.push_back(*n);
			evaluation.push_back(*e);

			tmp = tmp->next;
		}
	}

	//��j��iǰ��ʱ
	j = 1;
	tmp = routeSet[r]->head;
	while (tmp->num != i)
	{
		j_succ = tmp->num;
		n = new vector<double>();
		n->push_back(1);
		n->push_back(j);
		n->push_back(j_succ);
		n->push_back(i);
		n->push_back(i_succ);

		n->push_back(r);

		//������������µĿ���
		e = new vector<double>();
		e->push_back(computeFirstEvaluation(n));
		e->push_back(computeSecondEvaluation(n));
		e->push_back(computeThirdEvaluation(n));

		neighbour.push_back(*n);
		evaluation.push_back(*e);

		j = j_succ;
		tmp = tmp->next;
	}
}

void Problem::generateOrExchange(double num)
{
	double i = num;
	int r = searchRoute(i); //�ͻ�c���ڵ�·��

	//��i_pred->i,i->i_succ,j->j_succ����i_pred->i_succ,j->i,i->j_succ
	//i�̶���i_pred��i_succ���ǹ̶��ģ��ı����j
	double i_pred, i_succ;
	if (data[i - 1]->next)
		i_succ = data[i - 1]->next->num;
	else
		i_succ = 1;
	Node* pn = routeSet[r]->head;
	if (pn->num == i)
	{
		i_pred = 1;
	}
	else
	{
		while (pn)
		{
			i_pred = pn->num;
			pn = pn->next;
			if (pn->num == i)
				break;
		}
	}

	//j�ı仯,ֻҪj������i_pred,i������
	//j�����ǴӲֿ⵽�ص��ֿ�ǰ�����һ���ڵ�
	vector<double>* n;
	vector<double>* e;
	double j, j_succ;
	j = 1;
	Node* tmp = routeSet[r]->head;
	while (true)
	{
		if (tmp)
			j_succ = tmp->num;
		else
			j_succ = 1;

		if (j == i_pred)
		{
			if (i_succ == 1)
				break;
			j = i_succ;
			tmp = data[j - 1]->next;
			continue;
		}

		n = new vector<double>();
		n->push_back(2);
		n->push_back(i_pred);
		n->push_back(i);
		n->push_back(i_succ);
		n->push_back(j);
		n->push_back(j_succ);

		n->push_back(r);

		//������������µĿ���
		e = new vector<double>();
		e->push_back(computeFirstEvaluation(n));
		e->push_back(computeSecondEvaluation(n));
		e->push_back(computeThirdEvaluation(n));
	
		neighbour.push_back(*n);
		evaluation.push_back(*e);

		if (j_succ == 1)
			break;
		j = j_succ;
		tmp = tmp->next;
	}
}

void Problem::generateRelocation(double num)
{
	double i = num;
	int r = searchRoute(i); //�ͻ�c���ڵ�·��

	//��i_pred->i,i->i_succ,j->j_succ����i_pred->i_succ,j->i,i->j_succ
	//i�̶���i_pred��i_succ���ǹ̶��ģ��ı����j
	double i_pred, i_succ;
	if (data[i - 1]->next)
		i_succ = data[i - 1]->next->num;
	else
		i_succ = 1;
	Node* pn = routeSet[r]->head;
	if (pn->num == i)
	{
		i_pred = 1;
	}
	else
	{
		while (pn)
		{
			i_pred = pn->num;
			pn = pn->next;
			if (pn->num == i)
				break;
		}
	}

	//j����������·���ϵĽڵ㣬�Ӳֿ⵽�ֿ�ǰ�����һ���ڵ�
	vector<double>* n;
	vector<double>* e;
	double j, j_succ;
	for (int m = 0; m < routeSet.size(); ++m)
	{
		//����i�ڵ�·����
		if (m != r)
		{
			j = 1;
			Node* tmp = routeSet[m]->head;
			while(true)
			{
				if (tmp)
					j_succ = tmp->num;
				else
					j_succ = 1;

				n = new vector<double>();
				n->push_back(3);
				n->push_back(i_pred);
				n->push_back(i);
				n->push_back(i_succ);
				n->push_back(j);
				n->push_back(j_succ);

				n->push_back(r);
				n->push_back(m);

				//������������µĿ���
				e = new vector<double>();
				e->push_back(computeFirstEvaluation(n));
				e->push_back(computeSecondEvaluation(n));
				e->push_back(computeThirdEvaluation(n));

				neighbour.push_back(*n);
				evaluation.push_back(*e);

				if (j_succ == 1)
					break;
				j = j_succ;
				tmp = tmp->next;
			}
		}
	}
}

void Problem::generateExchange(double num)
{
	double i = num;
	int r = searchRoute(i); //�ͻ�c���ڵ�·��

	//��i_pred->i,i->i_succ,j->pred->j,j->j_succ����i_pred->j,j->i_succ,j_pred->i,i->j_succ
	//i�̶���i_pred��i_succ���ǹ̶��ģ��ı����j
	double i_pred, i_succ;
	if (data[i - 1]->next)
		i_succ = data[i - 1]->next->num;
	else
		i_succ = 1;
	Node* pn = routeSet[r]->head;
	if (pn->num == i)
	{
		i_pred = 1;
	}
	else
	{
		while (pn)
		{
			i_pred = pn->num;
			pn = pn->next;
			if (pn->num == i)
				break;
		}
	}

	//j����������·���ϵĽڵ㣬�Ӳֿ���һ���ڵ㵽�ֿ�ǰ�����һ���ڵ�
	vector<double>* n;
	vector<double>* e;
	double j_pred, j, j_succ;
	for (int m = 0; m < routeSet.size(); ++m)
	{
		//����i�ڵ�·����
		if (m != r)
		{

			j_pred = 1;
			Node* tmp = routeSet[m]->head;
			while (tmp)
			{
				j = tmp->num;
				if (tmp->next)
					j_succ = tmp->next->num;
				else
					j_succ = 1;

				n = new vector<double>();
				n->push_back(4);
				n->push_back(i_pred);
				n->push_back(i);
				n->push_back(i_succ);
				n->push_back(j_pred);
				n->push_back(j);
				n->push_back(j_succ);

				n->push_back(r);
				n->push_back(m);

				//������������µĿ���
				e = new vector<double>();
				e->push_back(computeFirstEvaluation(n));
				e->push_back(computeSecondEvaluation(n));
				e->push_back(computeThirdEvaluation(n));

				neighbour.push_back(*n);
				evaluation.push_back(*e);

				j_pred = j;
				tmp = tmp->next;
			}
		}
	}
}

void Problem::generateCrossover(double num)
{
	double i = num;
	int r = searchRoute(i); //�ͻ�c���ڵ�·��

	//��i->i_succ,j->j_succ����j->i_succ,i->j_succ
	//i�̶���i_succ���ǹ̶��ģ��ı����j
	double i_succ;
	if (data[i - 1]->next)
		i_succ = data[i - 1]->next->num;
	else
		i_succ = 1;

	//j����������·���ϵĽڵ㣬�Ӳֿ���һ���ڵ㵽�ֿ�ǰ�����һ���ڵ�
	vector<double>* n;
	vector<double>* e;
	double j, j_succ;
	for (int m = 0; m < routeSet.size(); ++m)
	{
		//����i�ڵ�·����
		if (m != r)
		{
			j = 1;
			Node* tmp = routeSet[m]->head;
			while (tmp)
			{
				if (tmp)
					j_succ = tmp->num;
				else
					j_succ = 1;

				//���i��j�������һ���ڵ㣬��ô��������µ��ھ��뵱ǰ�����ͬ
				//�����ھӲ�����������
				if (i_succ != 1 || j_succ != 1)
				{
					n = new vector<double>();
					n->push_back(5);
					n->push_back(i);
					n->push_back(i_succ);
					n->push_back(j);
					n->push_back(j_succ);

					n->push_back(r);
					n->push_back(m);

					//������������µĿ���
					e = new vector<double>();
					e->push_back(computeFirstEvaluation(n));
					e->push_back(computeSecondEvaluation(n));
					e->push_back(computeThirdEvaluation(n));

					neighbour.push_back(*n);
					evaluation.push_back(*e);
				}
				
				if (j_succ == 1)
					break;
				j = j_succ;
				tmp = tmp->next;
			}
		}
	}
}

void Problem::generateNeighbour(int operate, double num)
{
	int o = operate;

	/*�����һ�β������ھӵ���Ϣ*/
	vector<vector<double>>().swap(neighbour);
	vector<vector<double>>().swap(evaluation);
	vector<int>().swap(order);
	
	if (o == 1)  //����2-exchange  ��һ��·����
	{
		generate2Exchange(num);
	}
	else if (o == 2)  //����Or-exchange  ��һ��·����
	{
		generateOrExchange(num);
	}
	else if (o == 3) //����Relocation  ������·����
	{
		generateRelocation(num);
	}
	else if (o == 4) //����Exchange  ������·����
	{
		generateExchange(num);
	}
	else if (o == 5) //����Crossover  ������·����
	{
		generateCrossover(num);
	}
}

double Problem::computeFirstEvaluation(vector<double>* n)
{
	
	double x;
	x = routeSet.size();

	//���ֿ��ܼ���·����Ŀ�Ĳ���
	if ((*n)[0] == 3) //relocation�ھ�
	{
		double r1;
		r1 = (*n)[6];
		if (routeSet[r1]->size == 1)
			x = x - 1;
	}
	else if ((*n)[0] == 5) //crossover�ھ�
	{
		double i_succ = (*n)[2];
		double j = (*n)[3];
		if (j == 1 && i_succ == 1)
			x = x - 1;
	}

	return x;
}

double Problem::computeSecondEvaluation(vector<double>* n)
{
	double r1, r2;
	double sum = 0;
	if ((*n)[0] == 0 || (*n)[0] == 1 || (*n)[0] == 2 || (*n)[0] == 4)
	{
		//ÿ��·���ϵĽڵ���Ŀ��û�з����ı�	
		for (int m = 0; m < routeSet.size(); ++m)
		{
			sum += pow(routeSet[m]->size, 2);
		}
	}
	else if ((*n)[0] == 3) //relocation�ھ�
	{
		r1 = (*n)[6];
		r2 = (*n)[7];

		for (int n = 0; n < routeSet.size(); ++n)
		{
			if (n == r1)
				sum += pow(routeSet[n]->size - 1, 2);
			else if (n == r2)
				sum += pow(routeSet[n]->size + 1, 2);
			else
				sum += pow(routeSet[n]->size, 2);
		}
	}
	else if ((*n)[0] == 5) //crossover�ھ�
	{
		double i = (*n)[1];
		double i_succ = (*n)[2];
		double j = (*n)[3];
		double j_succ = (*n)[4];

		r1 = (*n)[5];
		r2 = (*n)[6];

		double r1_1, r1_2 = 0;
		double r2_1, r2_2 = 0;
		for (Node* tmp = data[i - 1]->next; tmp != NULL; tmp = tmp->next)
		{
			r1_2++;
		}
		r1_1 = routeSet[r1]->size - r1_2;

		if (j == 1)
		{
			r2_2 = routeSet[r2]->size;
		}
		else
		{
			for (Node* tmp = data[j - 1]->next; tmp != NULL; tmp = tmp->next)
			{
				r2_2++;
			}
		}
		r2_1 = routeSet[r2]->size - r2_2;

		for (int n = 0; n < routeSet.size(); ++n)
		{
			if (n == r1)
				sum += pow(r1_1 + r2_2, 2);
			else if (n == r2)
				sum += pow(r2_1 + r1_2, 2);
			else
				sum += pow(routeSet[n]->size, 2);
		}
	}

	return -sum;
}

double Problem::computeThirdEvaluation(vector<double>* n)
{
	double r, r1, r2;
	double i_pred, i, i_succ;
	double j_pred, j, j_succ;
	double x;
	if((*n)[0] == 0)
	{
		x = computeMinimalDelay();
	}
	else if ((*n)[0] == 1) //2-exchange�ھ�
	{
		//i�ڽڵ�jǰ��
		i = (*n)[1];
		i_succ = (*n)[2];
		j = (*n)[3];
		j_succ = (*n)[4];

		r = (*n)[5];

		//���н���
		if (i == 1)
			routeSet[r]->head = data[j - 1];
		else
			data[i - 1]->next = data[j - 1];

		if (j_succ == 1)
			data[i_succ - 1]->next = NULL;
		else
			data[i_succ - 1]->next = data[j_succ - 1];
		//��i_succ->...->j��Ϊj->...->i_succ
		reversePartialRoute(i_succ, j);

		//���������������׼
		x = computeMinimalDelay();

		//��ԭ����
		if (i == 1)
			routeSet[r]->head = data[i_succ - 1];
		else
			data[i - 1]->next = data[i_succ - 1];

		if (j_succ == 1)
			data[j - 1]->next = NULL;
		else
			data[j - 1]->next = data[j_succ - 1];
		//��j->...->i_succ��Ϊi_succ->...->j
		reversePartialRoute(j, i_succ);
	}
	else if((*n)[0] == 2) //or-exchange�ھ�
	{
		i_pred = (*n)[1];
		i = (*n)[2];
		i_succ = (*n)[3];
		j = (*n)[4];
		j_succ = (*n)[5];

		r = (*n)[6];

		//���н���
		//i_pred==1��i_succ������ͬʱ���֣���Ϊ�����Ļ���û��j��
		if (i_pred == 1)  //���i�ǲֿ���һ���ڵ�
			routeSet[r]->head = data[i_succ - 1];
		else if (i_succ == 1) //���i�ǲֿ�ǰ�����һ���ڵ�
			data[i_pred - 1]->next = NULL;
		else
			data[i_pred - 1]->next = data[i_succ - 1];

		if (j == 1)  //���j�ǲֿ�
			routeSet[r]->head = data[i - 1];
		else
			data[j - 1]->next = data[i - 1];

		if (j_succ == 1)  //���j_succ�ǲֿ�
			data[i - 1]->next = NULL;
		else
			data[i - 1]->next = data[j_succ - 1];

		x = computeMinimalDelay();

		//��ԭ����
		if (i_pred == 1)
			routeSet[r]->head = data[i - 1];
		else
			data[i_pred - 1]->next = data[i - 1];

		if (i_succ == 1)
			data[i - 1]->next = NULL;
		else
			data[i - 1]->next = data[i_succ - 1];

		if (j == 1)
			routeSet[r]->head = data[j_succ - 1];
		else if (j_succ == 1)
			data[j - 1]->next = NULL;
		else
			data[j - 1]->next = data[j_succ - 1];

	}
	else if ((*n)[0] == 3) //relocation�ھ�
	{
		i_pred = (*n)[1];
		i = (*n)[2];
		i_succ = (*n)[3];
		j = (*n)[4];
		j_succ = (*n)[5];

		r1 = (*n)[6];
		r2 = (*n)[7];

		//���н���
		if (i_pred == 1 && i_succ == 1)
			routeSet[r1]->head = NULL;
		else if (i_pred == 1 && i_succ != 1)
			routeSet[r1]->head = data[i_succ - 1];
		else if (i_succ == 1 && i_pred != 1)
			data[i_pred - 1]->next = NULL;
		else
			data[i_pred - 1]->next = data[i_succ - 1];

		if (j == 1)
			routeSet[r2]->head = data[i - 1];
		else
			data[j - 1]->next = data[i - 1];

		if (j_succ == 1)
			data[i - 1]->next = NULL;
		else
			data[i - 1]->next = data[j_succ - 1];

		routeSet[r1]->size--;
		routeSet[r2]->size++;

		x = computeMinimalDelay();

		//��ԭ����
		if (i_pred == 1)
			routeSet[r1]->head = data[i - 1];
		else
			data[i_pred - 1]->next = data[i - 1];

		if (i_succ == 1)
			data[i - 1]->next = NULL;
		else
			data[i - 1]->next = data[i_succ - 1];

		//j==1��j_succ==1������ͬʱ���֣���Ϊ·��r2������Ϊ��
		if (j == 1)
			routeSet[r2]->head = data[j_succ - 1];
		else if (j_succ == 1)
			data[j - 1]->next = NULL;
		else
			data[j - 1]->next = data[j_succ - 1];

		routeSet[r2]->size--;
		routeSet[r1]->size++;
	}
	else if ((*n)[0] == 4)  //exchange�ھ�
	{
		i_pred = (*n)[1];
		i = (*n)[2];
		i_succ = (*n)[3];
		j_pred = (*n)[4];
		j = (*n)[5];
		j_succ = (*n)[6];

		r1 = (*n)[7];
		r2 = (*n)[8];

		//���н���
		if (i_pred == 1)   //i�ǵ�һ���ͻ��ڵ�
			routeSet[r1]->head = data[j - 1];
		else
			data[i_pred - 1]->next = data[j - 1];

		if (i_succ == 1)  //i�����һ���ͻ��ڵ�
			data[j - 1]->next = NULL;
		else
			data[j - 1]->next = data[i_succ - 1];

		if (j_pred == 1)  //j�ǵ�һ���ͻ��ڵ�
			routeSet[r2]->head = data[i - 1];
		else
			data[j_pred - 1]->next = data[i - 1];

		if (j_succ == 1)   //j�����һ���ͻ��ڵ�
			data[i - 1]->next = NULL;
		else
			data[i - 1]->next = data[j_succ - 1];

		x = computeMinimalDelay();

		//��ԭ����
		if (i_pred == 1)   //i�ǵ�һ���ͻ��ڵ�
			routeSet[r1]->head = data[i - 1];
		else
			data[i_pred - 1]->next = data[i - 1];

		if (i_succ == 1)  //i�����һ���ͻ��ڵ�
			data[i - 1]->next = NULL;
		else
			data[i - 1]->next = data[i_succ - 1];

		if (j_pred == 1) //j�ǵ�һ���ͻ��ڵ�
			routeSet[r2]->head = data[j - 1];
		else
			data[j_pred - 1]->next = data[j - 1];

		if (j_succ == 1)  //j�����һ���ͻ��ڵ�
			data[j - 1]->next = NULL;
		else
			data[j - 1]->next = data[j_succ - 1];

	}
	else if ((*n)[0] == 5) //crossover�ھ�
	{
		i = (*n)[1];
		i_succ = (*n)[2];
		j = (*n)[3];
		j_succ = (*n)[4];

		r1 = (*n)[5];
		r2 = (*n)[6];

		//���н���
		//i_succ��j_succ����ͬʱ�ǲֿ⣬��Ϊ�����Ļ����ھӸ��Լ���һ����
		if (j_succ == 1)  //j�����һ���ͻ��ڵ�
			data[i - 1]->next = NULL;
		else
			data[i - 1]->next = data[j_succ - 1];

		if (i_succ == 1 && j == 1)
			routeSet[r2]->head = NULL;
		else if (i_succ == 1 && j != 1)  //i�����һ���ͻ��ڵ�
			data[j - 1]->next = NULL;
		else if (j == 1 && i_succ != 1)
			routeSet[r2]->head = data[i_succ - 1];
		else
			data[j - 1]->next = data[i_succ - 1];

		int r1_1, r1_2 = 0;
		int r2_1, r2_2 = 0;
		for (Node* tmp = data[i - 1]->next; tmp != NULL; tmp = tmp->next)
		{
			r1_2++;
		}
		r1_1 = routeSet[r1]->size - r1_2;

		if (j == 1)
		{
			r2_2 = routeSet[r2]->size;
		}
		else
		{
			for (Node* tmp = data[j - 1]->next; tmp != NULL; tmp = tmp->next)
			{
				r2_2++;
			}
		}
		r2_1 = routeSet[r2]->size - r2_2;

		int r1_size = routeSet[r1]->size;
		int r2_size = routeSet[r2]->size;
		routeSet[r1]->size = r1_1 + r2_2;
		routeSet[r2]->size = r2_1 + r1_2;

		x = computeMinimalDelay();

		//��ԭ����
		//j==1��j_succ==1������ͬʱ���֣���Ϊr2������Ϊ��
		if (j == 1)
			routeSet[r2]->head = data[j_succ - 1];
		else if(j_succ == 1)
			data[j - 1]->next = NULL;
		else
			data[j - 1]->next = data[j_succ - 1];

		if (i_succ == 1)   //i�����һ���ͻ��ڵ�
			data[i - 1]->next = NULL;
		else
			data[i - 1]->next = data[i_succ - 1];

		routeSet[r1]->size = r1_size;
		routeSet[r2]->size = r2_size;
	}

	return x;
}

void Problem::changeToNeighbour(int num)
{
	double r, r1, r2;
	double i_pred, i, i_succ;
	double j_pred, j, j_succ;
	if (neighbour[num][0] == 1)
	{
		i = neighbour[num][1];
		i_succ = neighbour[num][2];
		j = neighbour[num][3];
		j_succ = neighbour[num][4];

		r = neighbour[num][5];

		//���н���
		if (j == 1)
			routeSet[r]->head = data[i - 1];
		else
			data[j - 1]->next = data[i - 1];

		if (i_succ == 1)
			data[j_succ - 1]->next = NULL;
		else
			data[j_succ - 1]->next = data[i_succ - 1];

		reversePartialRoute(j_succ, i);
	}
	else if(neighbour[num][0] == 2)
	{
		i_pred = neighbour[num][1];
		i = neighbour[num][2];
		i_succ = neighbour[num][3];
		j = neighbour[num][4];
		j_succ = neighbour[num][5];

		r = neighbour[num][6];

		//���н���
		if (i_pred == 1)  //���i�ǲֿ���һ���ڵ�
			routeSet[r]->head = data[i_succ - 1];
		else if (i_succ == 1) //���i�ǲֿ�ǰ�����һ���ڵ�
			data[i_pred - 1]->next = NULL;
		else
			data[i_pred - 1]->next = data[i_succ - 1];

		if (j == 1)  //���j�ǲֿ�
			routeSet[r]->head = data[i - 1];
		else
			data[j - 1]->next = data[i - 1];

		if (j_succ == 1)  //���j_succ�ǲֿ�
			data[i - 1]->next = NULL;
		else
			data[i - 1]->next = data[j_succ - 1];
	}
	else if (neighbour[num][0] == 3)
	{
		i_pred = neighbour[num][1];
		i = neighbour[num][2];
		i_succ = neighbour[num][3];
		j = neighbour[num][4];
		j_succ = neighbour[num][5];

		r1 = neighbour[num][6];
		r2 = neighbour[num][7];

		//���н���
		if (i_pred == 1 && i_succ == 1)
			routeSet[r1]->head = NULL;
		else if (i_pred == 1)
			routeSet[r1]->head = data[i_succ - 1];
		else if (i_succ == 1)
			data[i_pred - 1]->next = NULL;
		else
			data[i_pred - 1]->next = data[i_succ - 1];

		data[j - 1]->next = data[i - 1];

		if (j_succ == 1)
			data[i - 1]->next = NULL;
		else
			data[i - 1]->next = data[j_succ - 1];
	}
	else if (neighbour[num][0] == 4)
	{
		i_pred = neighbour[num][1];
		i = neighbour[num][2];
		i_succ = neighbour[num][3];
		j_pred = neighbour[num][4];
		j = neighbour[num][5];
		j_succ = neighbour[num][6];

		r1 = neighbour[num][7];
		r2 = neighbour[num][8];

		//���н���
		if (i_pred == 1)   //i�ǵ�һ���ͻ��ڵ�
			routeSet[r1]->head = data[j - 1];
		else
			data[i_pred - 1]->next = data[j - 1];

		if (i_succ == 1)  //i�����һ���ͻ��ڵ�
			data[j - 1]->next = NULL;
		else
			data[j - 1]->next = data[i_succ - 1];

		if (j_pred == 1)  //j�ǵ�һ���ͻ��ڵ�
			routeSet[r2]->head = data[i - 1];
		else
			data[j_pred - 1]->next = data[i - 1];

		if (j_succ == 1)   //j�����һ���ͻ��ڵ�
			data[i - 1]->next = NULL;
		else
			data[i - 1]->next = data[j_succ - 1];
	}
	else if (neighbour[num][0] == 5)
	{
		i = neighbour[num][1];
		i_succ = neighbour[num][2];
		j = neighbour[num][3];
		j_succ = neighbour[num][4];

		//���н���
		if (j_succ == 1)  //j�����һ���ͻ��ڵ�
			data[i - 1]->next = NULL;
		else
			data[i - 1]->next = data[j_succ - 1];

		if (i_succ == 1)  //i�����һ���ͻ��ڵ�
			data[j - 1]->next = NULL;
		else
			data[j - 1]->next = data[i_succ - 1];
	}
}

void Problem::minimizeRoute()
{
	double t;
	int o;  //������1.Two-exchange 2.Or-exchange 3.Relocation 4.Exchange 5.Crossover
	double c; //���������һ���ͻ����

	//��һ����Ϊ0ʱ��ʾ�������tmp_s���ڵ�ǰ��������������κα仯
	//1����һ��2-exchange 2����һ��or-exchange 3����һ��relocation 4����һ��exchange 5����һ��crossover
	vector<double>* tmp_s = new vector<double>();
	tmp_s->push_back(0);

	//��õ�ǰʱ��
	time_t iTime = time(0);

	while (difftime(time(0), iTime) < 10)
	{
		t = T;
		while (difftime(time(0), iTime) < 10 && t > EPS)
		{
			for (int i = 1; i < maxIt; i++)
			{
				o = rand() % 6;
				c = (double)(rand() % 100) + 1;
				//�����ھӲ������ھӵ�����ֵ
				generateNeighbour(o, c);
				//�����ھӵ�����
				computeOrder();

				double v_1 = computeFirstEvaluation(tmp_s);
				double v_2 = computeSecondEvaluation(tmp_s);
				double v_3 = computeThirdEvaluation(tmp_s);
				if (evaluation[order[0]][0] > v_1 && evaluation[order[0]][1] > v_2 && evaluation[order[0]][2] > v_3)
				{
					int r = ceil((rand() / double(RAND_MAX)) * order.size());
					double diff = 0.5 * (evaluation[order[r]][0] - v_1) + 0.3 * (evaluation[order[r]][1] - v_2) + 0.2 * (evaluation[order[r]][2] - v_3);
					if (evaluation[order[r]][0] > v_1 && evaluation[order[r]][1] > v_2 && evaluation[order[r]][2] > v_3)
					{
						if (rand() % 100 / (double)100 <= exp(diff / t))
						{
							vector<double>().swap(*tmp_s);
							for (int m = 0; m < neighbour[order[r]].size(); ++m)
							{
								tmp_s->push_back(neighbour[order[r]][m]);
							}
						}
					}
					else
					{
						vector<double>().swap(*tmp_s);
						for (int m = 0; m < neighbour[order[r]].size(); ++m)
						{
							tmp_s->push_back(neighbour[order[r]][m]);
						}
					}
				}
				else
				{
					changeToNeighbour(order[0]);

					vector<double>().swap(*tmp_s);
					for (int m = 0; m < neighbour[order[0]].size(); ++m)
					{
						tmp_s->push_back(neighbour[order[0]][m]);
					}
				}
			}
			t = R * t;
		}
	}
}

