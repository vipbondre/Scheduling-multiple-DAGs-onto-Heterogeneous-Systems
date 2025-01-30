#include <bits/stdc++.h> 
using namespace std;


int no_of_graphs = 2;
int no_of_procs = 5;
int ct;
vector<pair < double ,int > >slowdown;
vector<pair <int , int >> makespan;
vector <pair <int,int >> ready_queue[100];
vector<double> SD(2);


class DAG
{
public:
double ranks[100];
double avg[100];
int adj[100][100];
int task_time[100][50];
int proc;
int tasks;
int eft[100][50];
int est[100][50];
int aft[100];
vector <int> parent[100];
vector<int> child[100];
int mini_processor[100];
vector < pair <double, int> > rank_task;
};


class processor_slot
{
	public:
		int start_time;
		int end_time;
		int task_no;
		int d;

};

DAG Multidag[2];
vector <processor_slot> processor_scheduler[100];

bool sortbysec(const pair<int,int> &a,
              const pair<int,int> &b)
{
    return (a.second < b.second);
}


void add_edge(int dag, int x, int y , int d)
{
	Multidag[dag].adj[x][y]=d;

	Multidag[dag].parent[y].push_back(x);
	Multidag[dag].child[x].push_back(y);

}

void add_edges()
{
	//add edges
	add_edge(0, 0, 1, 2);
	add_edge(0, 0, 2, 2);
	add_edge(0, 0, 3, 2);
	add_edge(0, 1, 4, 2);
	add_edge(0, 2, 4, 2);
	add_edge(0, 3, 4, 2);

	add_edge(1, 0, 1, 2);
	add_edge(1, 0, 2, 2);
	add_edge(1, 0, 3, 2);
	add_edge(1, 1, 4, 2);
	add_edge(1, 2, 5, 2);
	add_edge(1, 3, 4, 2);
	add_edge(1, 3, 5, 2);
	add_edge(1, 4, 6, 2);
	add_edge(1, 5, 6, 2);
}

double rec_rank(int i, int dag)
{
    double maxi = 0;
	double temp;

    for (int j = 0; j < Multidag[dag].proc; j++) 
    {
        if(Multidag[dag].adj[i][j] != -1) 
        {
			temp = rec_rank(j,dag);
            if (maxi < Multidag[dag].adj[i][j] + temp)
                maxi = Multidag[dag].adj[i][j] + temp;
        }
    }
    Multidag[dag].ranks[i] = Multidag[dag].avg[i] + maxi;
    return Multidag[dag].ranks[i];
}


int est_cal(int i, int j, int dag)
{
	int comm;
	int par;
	int max_time;
	int temp = 0;;

	max_time = 0;

	for (int k = 0; k < Multidag[dag].parent[i].size(); k++) 
	{
		par = Multidag[dag].parent[i][k];
		if(Multidag[dag].mini_processor[par] == j) 
		{
			temp = Multidag[dag].aft[par];
		}
		else 
		{
			temp = Multidag[dag].adj[par][i] + Multidag[dag].aft[par];
		}
		max_time = max(max_time, temp);
	}
	return max_time;
}

int eft_cal(int i, int j, int dag)
{
	int start_time;

	if(processor_scheduler[j].empty()) {
		return Multidag[dag].est[i][j] + Multidag[dag].task_time[i][j];
	}
		/*
		for (int k = 0; k < processor_scheduler[j].length() -1 ; k++) {
			if( processor_scheduler[j][k].end_time > est[i][j] and processor_scheduler[j][k+1].start_time > est[i][j] + task_time[i][j]) {
				return est[i][j] + task_time[i][j];
			}

		}

		return processor_scheduler[

		*/
	int ok;
	int endtime;
	ok = 1;
	for (int timer = Multidag[dag].est[i][j]; 1; timer++) 
	{
		ok = 1;
		endtime = timer + Multidag[dag].task_time[i][j];
		for (int k = 0; k < processor_scheduler[j].size(); k++) 
		{
			//temp = processor_scheduler[j];

			if((timer < processor_scheduler[j][k].start_time and endtime > processor_scheduler[j][k].start_time) or (timer < processor_scheduler[j][k].end_time and endtime > processor_scheduler[j][k].end_time) ) 
			{
				ok = 0;
				break;
			}
		}

		if(ok == 1) 
		{
			return endtime;
		}
	}
}

void heft(int dag)
{
	int mini;
	mini = INT_MAX;
	int t;
	int mini_eft;
	int length = -1;

	processor_slot temp;
	processor_slot temp2;
	t = Multidag[dag].rank_task[0].second;

	for (int i = 0; i < Multidag[dag].proc; i++)
	{
		Multidag[dag].est[Multidag[dag].rank_task[0].second][i] = 0;
		if(mini > Multidag[dag].task_time[t][i])
		{
			mini = Multidag[dag].task_time[t][i];
			Multidag[dag].mini_processor[t] = i;
		}
	}

	temp.start_time = 0;
	temp.end_time = mini;
	temp.task_no = t;
	temp.d=dag;

//	processor_scheduler[Multidag[dag].mini_processor[t]].push_back(temp);

	Multidag[dag].aft[Multidag[dag].rank_task[0].second] = mini;
	int i;
	for (int h = 0; h < Multidag[dag].tasks; h++) 
	{
		i = Multidag[dag].rank_task[h].second;

		for (int j = 0; j < Multidag[dag].proc; j++) 
		{
			Multidag[dag].est[i][j] = est_cal(i, j, dag);
		}
	
		mini_eft = INT_MAX;
		int processor_no = 0;

		for (int j = 0; j < Multidag[dag].proc; j++) 
		{
			Multidag[dag].eft[i][j] = eft_cal(i, j,dag);
			if(Multidag[dag].eft[i][j] < mini_eft) 
			{
				mini_eft = Multidag[dag].eft[i][j];
				processor_no = j;
			}
		}
		
		Multidag[dag].mini_processor[i] = processor_no;
		
		temp2.start_time = mini_eft - Multidag[dag].task_time[i][processor_no];
		temp2.end_time = mini_eft;
		Multidag[dag].aft[i] = mini_eft;
		temp2.task_no = i;
		length = max(mini_eft, length);
		temp2.d=dag;

		processor_scheduler[processor_no].push_back(temp2);
	}
}

void F1_finishtime()
{	
	cout<<endl<<"F1:"<<endl;
	int select_dag , select_task , select_machine , check=1 ,ft_multi=0 , ft_own=0 ,task[2];
	vector<int> v[2]; int n=1;
	double sd;
	vector <int> dag;

	for(int i=0;i<2;i++)
	{
		slowdown.push_back(make_pair(0.0,i));
		makespan.push_back(make_pair(i, Multidag[i].aft[(Multidag[i].tasks)-1]));
		ready_queue[i].push_back(make_pair(Multidag[i].aft[0],0));
		dag.push_back(1);
		task[i]=0;		
	}
			
	if(makespan[0].second >= makespan[1].second)
		select_dag = makespan[0].first;
	else
		select_dag = makespan[1].first;
			
	int c=0;
	int local_task;
	double local_ft;

	while(dag[0]||dag[1])
	{
		check=0;
		select_task = ready_queue[select_dag][0].second;
		//cout<<"select_task---------------"<<select_task<<"    graph------"<<select_dag<<endl;
		select_machine = Multidag[select_dag].mini_processor[select_task];

		if(ready_queue[select_dag].size())
			ready_queue[select_dag].erase(ready_queue[select_dag].begin() + 0);
							
		if(Multidag[select_dag].tasks > task[select_dag])
		{
			//cout<<"**************child***********";
			for(int a= 0; a < Multidag[select_dag].child[select_task].size() ; a++)
			{
				local_task = Multidag[select_dag].child[select_task][a];
				local_ft =  Multidag[select_dag].aft[local_task];

				for(int m =0 ; m < v[select_dag].size();m++)
				{
					if(v[select_dag][m]==local_task)
					{
				 		n=0;
					}	
				}
				if(n)
				{
					v[select_dag].push_back(local_task);
					ready_queue[select_dag].push_back(make_pair(local_ft ,	local_task ));
				}
				n=1;
			}				

			sort(ready_queue[select_dag].begin(), ready_queue[select_dag].end());
	  
			ft_own = Multidag[select_dag].aft[select_task];
			ft_multi = ct + Multidag[select_dag].task_time[select_task][select_machine];
			ct=ft_multi;

			sd = double(ft_own)/double(ft_multi);

			cout<<"DAG "<<select_dag <<" Task "<<select_task<<" machine "<<select_machine <<" completion time "<< ct <<endl;

			slowdown.erase(slowdown.begin() + 0);
			slowdown.push_back(make_pair(sd, select_dag));
			sort(slowdown.begin(), slowdown.end() );
			task[select_dag]++;
			select_dag = rand()%2;
		}
		else
		{
			//cout<<"OVERFLOW"<<endl;
			dag[select_dag]=0;
			if(select_dag)
			{
				if(dag[select_dag-1]!=0)
					select_dag=0;
			}
			else
			{
				if(dag[select_dag+1]!=0)
				select_dag=1;
			}				
		}		
	}
}

void F2_finishtime()
{
	cout<<endl<<"F2:"<<endl;
	int select_dag , select_task , select_machine , check=1 ,ft_multi=0 , ft_own=0 ,task[2];
	vector<int> v[2]; int n=1;
	double sd;
	vector <int> dag;

	slowdown.clear();
	makespan.clear();
	for(int x=0; x< no_of_graphs; x++)
	{
		ready_queue[x].clear();
	}

	for(int i=0;i<2;i++)
	{
		slowdown.push_back(make_pair(0.0,i));
		makespan.push_back(make_pair(i, Multidag[i].aft[(Multidag[i].tasks)-1]));
		ready_queue[i].push_back(make_pair(Multidag[i].aft[0],0));
		dag.push_back(1);
		task[i]=0;		
	}
			
	if(makespan[0].second >= makespan[1].second)
		select_dag = makespan[0].first;
	else
		select_dag = makespan[1].first;
			
	int c=0;
	int local_task;
	double local_ft;

	while(dag[0]||dag[1])
	{
		check=0;
		select_task = ready_queue[select_dag][0].second;
		//cout<<"select_task---------------"<<select_task<<"    graph------"<<select_dag<<endl;
		select_machine = Multidag[select_dag].mini_processor[select_task];

		if(ready_queue[select_dag].size())
			ready_queue[select_dag].erase(ready_queue[select_dag].begin() + 0);
							
		if(Multidag[select_dag].tasks > task[select_dag])
		{
			//cout<<"**************child***********";
			for(int a= 0; a < Multidag[select_dag].child[select_task].size() ; a++)
			{
				local_task = Multidag[select_dag].child[select_task][a];
				local_ft =  Multidag[select_dag].aft[local_task];

				for(int m =0 ; m < v[select_dag].size();m++)
				{
					if(v[select_dag][m]==local_task)
					{
				 		n=0;
					}	
				}
				if(n)
				{
					v[select_dag].push_back(local_task);
					ready_queue[select_dag].push_back(make_pair(local_ft ,	local_task ));
				}
				n=1;
			}				

			sort(ready_queue[select_dag].begin(), ready_queue[select_dag].end());
	  
			/*ft_own = Multidag[select_dag].aft[select_task];
			ft_multi = ct + Multidag[select_dag].task_time[select_task][select_machine];
			ct=ft_multi;

			sd = double(ft_own)/double(ft_multi);*/

			ct = ct + Multidag[select_dag].task_time[select_task][select_machine];
			for(int x=0; x<no_of_graphs; x++)
			{
				if(dag[x])
				{
					int t57 = ready_queue[x][0].second;
					int m57 = Multidag[x].mini_processor[t57];
					int ct_own = Multidag[x].task_time[t57][m57];
					double sd57 = double(ct_own) / double(ct);
					SD[x] = sd57;
				}
			}
			sort(SD.begin(), SD.end());
			sd = SD[0];

			cout<<"DAG "<<select_dag <<" Task "<<select_task<<" machine "<<select_machine <<" completion time "<< ct <<endl;

			slowdown.erase(slowdown.begin() + 0);
			slowdown.push_back(make_pair(sd, select_dag));
			sort(slowdown.begin(), slowdown.end() );
			task[select_dag]++;
			select_dag = rand()%2;
		}
		else
		{
			//cout<<"OVERFLOW"<<endl;
			dag[select_dag]=0;
			if(select_dag)
			{
				if(dag[select_dag-1]!=0)
					select_dag=0;
			}
			else
			{
				if(dag[select_dag+1]!=0)
				select_dag=1;
			}				
		}		
	}	
}

void print()
{
	for(int i=0;i<2;i++)
	{
		cout <<"Adjacent "<<endl;
		for(int j=0; j<Multidag[i].tasks;j++)
		{
			for(int k=0; k < Multidag[i].tasks; k++)
			{
				cout<< Multidag[i].adj[j][k] <<" ";
			}
			cout<<endl;
		}
	}

	for(int i=0;i<2;i++)
	{
		cout <<"task time "<<i<<endl;
		for(int j=0; j<Multidag[i].tasks;j++)
		{
			for(int k=0; k < Multidag[i].proc; k++)
			{
				cout<< Multidag[i].task_time[j][k] <<" ";
			}
			cout<<endl;
		}
	}

	for(int i=0;i<2;i++)
	{
		cout <<"parents "<<i<<endl;
		for(int j=0; j<Multidag[i].tasks;j++)
		{		
			for(int k=0;k<Multidag[i].parent[j].size();k++)
			{
				cout<< Multidag[i].parent[j][k] <<" ";
			}
			cout<<endl;
		}
	}

	for(int i=0;i<2;i++)
	{
		cout <<"rank "<<i<<endl;
		for(int j=0; j<Multidag[i].tasks;j++)
		{
			cout<< Multidag[i].ranks[j] <<" ";
			cout<<endl;
		}
	}

	for(int i=0;i<2;i++)
	{
		cout <<"rank_task "<<i<<endl;
		for(int j=0; j<Multidag[i].tasks;j++)
		{
			cout<< Multidag[i].rank_task[j].second <<"  "<<Multidag[i].rank_task[j].first ;
			cout<<endl;
		}
	}

	for(int i=0;i<2;i++)
	{
		cout <<"start time "<<i<<endl;
		for(int j=0; j<Multidag[i].tasks;j++)
		{
			for(int k=0; k < Multidag[i].proc; k++)
			{
				cout<< Multidag[i].est[j][k] <<" ";
			}
		cout<<endl;
		}
	}

	for(int i=0;i<2;i++)
	{
		cout <<"finish time "<<i<<endl;
		for(int j=0; j<Multidag[i].tasks;j++)
		{
			for(int k=0; k < Multidag[i].proc; k++)
			{
				cout<< Multidag[i].eft[j][k] <<" ";
			}
			cout<<endl;
		}
	}

	for(int i=0;i<2;i++)
	{
		cout <<"fav processor "<<i<<endl;
		for(int j=0; j<Multidag[i].tasks;j++)
		{
			cout<< Multidag[i].mini_processor[j] <<" ";
			cout<<endl;
		}
	}

	cout<<endl;
	for(int i=0;i<2;i++)
	{
		cout <<"actual finish time "<<i<<endl;
		for(int j=0; j<Multidag[i].tasks;j++)
		{
			cout<< Multidag[i].aft[j] <<" ";
			cout<<endl;
		}
	}

	/*for (int i = 0; i < no_of_procs; i++) 
	{		
		cout<<"processor "<<i<<endl;
		for (int j = 0; j < processor_scheduler[i].size(); j++) 
		{
			cout << "DAG "<< processor_scheduler[i][j].d<<"  task "<< processor_scheduler[i][j].task_no<<"  start time " << processor_scheduler[i][j].start_time<<"    finsih  time " << processor_scheduler[i][j].end_time<<endl;;
		}
		cout << endl;
	}*/
}
	
int main()
{
    int x;
    int y;
    int z;
    int n;
    float ans;
    int p;
	double temp;
	int edges_count;


	for(int dag=0;dag<2;dag++)
	{
		memset(Multidag[dag].adj, -1, sizeof(Multidag[dag].adj));
	}
    
    for(int dag=0;dag<2;dag++)
    {
		cout << "Enter number of tasks and pocessors for dag "<<dag <<endl;;

    	cin >> Multidag[dag].tasks >> Multidag[dag].proc;
    	no_of_procs = Multidag[dag].proc;
	}

	srand(time(0));
	for(int dag=0;dag<2;dag++)
	{
    	for (int i = 0; i < Multidag[dag].tasks; i++)
    	{
			temp = 0;
    		for (int j = 0; j < Multidag[dag].proc; j++)
    	 	{
				Multidag[dag].task_time[i][j] = rand()%10; 
				if(Multidag[dag].task_time[i][j]==0)
					Multidag[dag].task_time[i][j]++;
				temp += Multidag[dag].task_time[i][j];
			}
			Multidag[dag].avg[i] = (double) temp / (double) Multidag[dag].proc;
		}
	}

	//create graph
	add_edges();
	
	for(int dag=0; dag<2;dag++)
	{
		for (int i = 0; i < Multidag[dag].tasks; i++)
	 	{
        	rec_rank(i,dag);
    	}
	}

	for(int dag=0;dag<2;dag++)
	{
    	for (int i = 0; i < Multidag[dag].tasks; i++) 
    	{
			Multidag[dag].rank_task.push_back(make_pair(Multidag[dag].ranks[i], i));
    	}
	}

	for(int dag=0; dag<2;dag++)
	{
		sort(Multidag[dag].rank_task.rbegin(), Multidag[dag].rank_task.rend());
	}

    for(int dag=0;dag<2;dag++)
    {
		for (int i = 0; i < Multidag[dag].tasks; i++)
	 	{
			heft(dag);
    	}
	}

	cout<<endl<<endl;

	print();

	F1_finishtime();

	ct = 0;

	F2_finishtime();

    return 0;
}