#include<iostream>
#include<bits/stdc++.h>

using namespace std;

int row,col,h_diff;

struct node{
	int r,c;
	double cost;
};

struct compare{
	bool operator()(node a,node b)
	{
		return a.cost>b.cost;
	}
};

double heuristic(int x_one,int y_one,int x_two,int y_two)
{
    double res,a,b,c,d;
    
    a = (double)x_one;
    b = (double)x_two;
    c = (double)y_one;
    d = (double)y_two;
    
    res = sqrt(pow(a-b,2)+pow(c-d,2));
    return res;
    
}




void bfs(vector<vector<int> > grid,pair<int,int> start_point,pair<int,int> target,vector<vector<bool> > visited,vector<vector<int> > &parent_row,vector<vector<int>> &parent_col)
{

	queue<pair<int,int>> q;
	q.push(start_point);
	visited[start_point.first][start_point.second] = true;
	int found = false;
	

	while(q.empty()==false)
	{
		pair<int,int> curr;
		curr = q.front();
		q.pop();
		int i,j,h,next_row,next_col,curr_row,curr_col;
		curr_row = curr.first;
		curr_col = curr.second;
		if(curr_row == target.first and curr_col==target.second) return;
		
		int count = 0;
		for(i=-1;i<=1;i++)
		{
			for(j=-1;j<=1;j++)
			{
				if(i==0 and j==0) continue;

				count++;
				next_row = curr_row-i;
				next_col = curr_col-j;
				//if(grid[curr_row][curr_col]==-11) cout<<i<<","<<j<<"leela";
				if(next_row<row and next_col<col and next_row>=0 and next_col>=0)   //  check for the boundaries 
				{
					if(visited[next_row][next_col]==false)  // check for the path discovery
					{	
						if(grid[curr_row][curr_col]>=0 and grid[next_row][next_col]>=0) // both cells do not contain the rock
						{
							q.push(make_pair(next_row,next_col)); //insert in the queue
							visited[next_row][next_col] = true;
							parent_row[next_row][next_col] = curr_row;
							parent_col[next_row][next_col] = curr_col;
							//cout<<grid[next_row][next_col]<<" ";
						}

						else if(grid[curr_row][curr_col]>=0 and grid[next_row][next_col]<0) // next cell contains but not current
						{
							if(abs(grid[next_row][next_col])<=h_diff)
							{
								
								q.push(make_pair(next_row,next_col)); //insert in the queue
								visited[next_row][next_col] = true;
								parent_row[next_row][next_col] = curr_row;
								parent_col[next_row][next_col] = curr_col;
								//cout<<grid[next_row][next_col]<<" ";
								
							}
						}

						else if(grid[curr_row][curr_col]<0 and grid[next_row][next_col]>=0) //curr cell contains rock but not next
						{
							if(abs(grid[curr_row][curr_col])<=h_diff)
							{
								q.push(make_pair(next_row,next_col)); //insert in the queue
								visited[next_row][next_col] = true;
								parent_row[next_row][next_col] = curr_row;
								parent_col[next_row][next_col] = curr_col;
								//cout<<grid[next_row][next_col]<<" ";
							}
						}

						else if(grid[curr_row][curr_col]<0 and grid[next_row][next_col]<0) //both cells contain the rock
						{
							//if(grid[curr_row][curr_col]==-11) cout<<"surya";
							int temp_h;

							if(abs(grid[curr_row][curr_col])>=abs(grid[next_row][next_col]))
							{
								temp_h = abs(grid[curr_row][curr_col])-abs(grid[next_row][next_col]);
								//cout<<"leela";
							}
							else
							{
								temp_h = abs(grid[next_row][next_col])-abs(grid[curr_row][curr_col]);
								//cout<<"surya";
							}

							//cout<<temp_h;

							if(temp_h<=h_diff)
							{
								//cout<<temp_h<<"leela";
								q.push(make_pair(next_row,next_col)); //insert in the queue
								visited[next_row][next_col] = true;
								parent_row[next_row][next_col] = curr_row;
								parent_col[next_row][next_col] = curr_col;
								//cout<<grid[next_row][next_col]<<" ";
							}

						}
					}	
					

				}



			}
		}
		//cout<<"count="<<count;	

	}



}



void ucs(vector<vector<int>> grid,pair<int,int> start_point,pair<int,int> target,vector<vector<bool>> visited,vector<vector<int>> &parent_row,vector<vector<int>> &parent_col,vector<vector<int>> &cost)
{

	priority_queue<node,vector<node>,compare> q;
	node start;
	start.r = start_point.first;
	start.c = start_point.second;
	start.cost = 0;
	q.push(start); // push the start node to the priority_queue
	visited[start_point.first][start_point.second] = true;
	vector<vector<bool>> closed(row,vector<bool> (col)); // closed queue

	int a,b;
	for(a=0;a<row;a++)
	{
		for(b=0;b<col;b++)
		{
			closed[a][b] = false;   // intialize clsed queue as false initially
		}
	}


	

	while(q.empty()==false)
	{
		node curr,temp;
		curr = q.top();
		q.pop();
		int i,j,h,next_row,next_col,curr_row,curr_col,temp_cost;
		curr_row = curr.r;
		curr_col = curr.c;
		closed[curr_row][curr_col] = true; // mark the poped element as true

		if(curr_row == target.first and curr_col==target.second) return; // if target is reached, exit from the funciton

		for(i=-1;i<=1;i++)
		{
			for(j=-1;j<=1;j++)
			{
				if(i==0 and j==0) continue;

				if(i==0 or j==0) temp_cost = 10;  // horizantal or vertical move
				else temp_cost = 14;  // diagonal move

				next_row = curr_row-i;
				next_col = curr_col-j;

				if(next_row<row and next_col<col and next_row>=0 and next_col>=0)   //  check for the boundaries 
				{
					if(closed[next_row][next_col]==false)  // check whether it is already in the closed queue or not
					{	
						if(grid[curr_row][curr_col]>=0 and grid[next_row][next_col]>=0) // both cells do not contain the rock
						{
							if(temp_cost+cost[curr_row][curr_col]<cost[next_row][next_col]) //insert in the q
							{
								temp.r = next_row;
								temp.c = next_col;
								temp.cost = temp_cost+cost[curr_row][curr_col];
								q.push(temp); //insert in the queue
								visited[next_row][next_col] = true;
								parent_row[next_row][next_col] = curr_row; //update the parent row
								parent_col[next_row][next_col] = curr_col; //update the parent col
								cost[next_row][next_col] = temp_cost+cost[curr_row][curr_col]; //update the cost matrix
							}
						}

						else if(grid[curr_row][curr_col]>=0 and grid[next_row][next_col]<0) // next cell contains but not current
						{
							if(abs(grid[next_row][next_col])<=h_diff)
							{
								if(temp_cost+cost[curr_row][curr_col]<cost[next_row][next_col]) //insert in the q
								{
									temp.r = next_row;
									temp.c = next_col;
									temp.cost = temp_cost+cost[curr_row][curr_col];
									q.push(temp); //insert in the queue
									visited[next_row][next_col] = true;
									parent_row[next_row][next_col] = curr_row; //update the parent row
									parent_col[next_row][next_col] = curr_col; //update the parent col
									cost[next_row][next_col] = temp_cost+cost[curr_row][curr_col]; //update the cost matrix
								}
							}
						}

						else if(grid[curr_row][curr_col]<0 and grid[next_row][next_col]>=0) //curr cell contains rock but not next
						{
							if(abs(grid[curr_row][curr_col])<=h_diff)
							{
								if(temp_cost+cost[curr_row][curr_col]<cost[next_row][next_col]) //insert in the q
								{
									temp.r = next_row;
									temp.c = next_col;
									temp.cost = temp_cost+cost[curr_row][curr_col];
									q.push(temp); //insert in the queue
									visited[next_row][next_col] = true;
									parent_row[next_row][next_col] = curr_row; //update the parent row
									parent_col[next_row][next_col] = curr_col; //update the parent col
									cost[next_row][next_col] = temp_cost+cost[curr_row][curr_col]; //update the cost matrix
								}
							}
						}

						else if(grid[curr_row][curr_col]<0 and grid[next_row][next_col]<0) //both cells contain the rock
						{
							int temp_h;

							if(abs(grid[curr_row][curr_col])>=abs(grid[next_row][next_col]))
							{
								temp_h = abs(grid[curr_row][curr_col])-abs(grid[next_row][next_col]);
								//cout<<"leela";
							}
							else
							{
								temp_h = abs(grid[next_row][next_col])-abs(grid[curr_row][curr_col]);
								//cout<<"surya";
							}

							//cout<<temp_h;

							if(temp_h<=h_diff) //insert in the q
							{
								if(temp_cost+cost[curr_row][curr_col]<cost[next_row][next_col]) 
								{
									temp.r = next_row;
									temp.c = next_col;
									temp.cost = temp_cost+cost[curr_row][curr_col];
									q.push(temp); //insert in the queue
									visited[next_row][next_col] = true;
									parent_row[next_row][next_col] = curr_row; //update the parent row
									parent_col[next_row][next_col] = curr_col; //update the parent col
									cost[next_row][next_col] = temp_cost+cost[curr_row][curr_col]; //update the cost matrix
								}
							}

						}

					}	
					

				}



			}
		}


	}



}


void a_star(vector<vector<int>> grid,pair<int,int> start_point,pair<int,int> target,vector<vector<bool>> visited,vector<vector<int>> &parent_row,vector<vector<int>> &parent_col,vector<vector<double>> &cost,vector<vector<double>> &fun_cost)
{

	priority_queue<node,vector<node>,compare> q;
	node start;
	start.r = start_point.first;
	start.c = start_point.second;
	start.cost = fun_cost[start_point.first][start_point.second];
	q.push(start); // push the start node to the priority_queue
	visited[start_point.first][start_point.second] = true;
	vector<vector<bool>> closed(row,vector<bool> (col)); // closed queue

	int a,b;
	for(a=0;a<row;a++)
	{
		for(b=0;b<col;b++)
		{
			closed[a][b] = false;   // intialize clsed queue as false initially
		}
	}


	

	while(q.empty()==false)
	{
		node curr,temp;
		curr = q.top();
		q.pop();
		int i,j,h,next_row,next_col,curr_row,curr_col;
		double total_cost,mud_cost,move_cost,height_cost;
		curr_row = curr.r;
		curr_col = curr.c;
		closed[curr_row][curr_col] = true; // mark the poped element as true

		if(curr_row == target.first and curr_col==target.second) return; // if target is reached exit from the funciton

		for(i=-1;i<=1;i++)
		{
			for(j=-1;j<=1;j++)
			{
				if(i==0 and j==0) continue;

				if(i==0 or j==0) move_cost = 10;  // horizantal or vertical move
				else move_cost = 14;  // diagonal move

				next_row = curr_row-i;
				next_col = curr_col-j;

				if(next_row<row and next_col<col and next_row>=0 and next_col>=0)   //  check for the boundaries 
				{
					if(closed[next_row][next_col]==false) // check whether it is already in the closed queue or not
					{	
						if(grid[curr_row][curr_col]>=0 and grid[next_row][next_col]>=0) // both cells do not contain the rock
						{
							height_cost = 0;
							mud_cost = grid[next_row][next_col];
							total_cost = move_cost + mud_cost + height_cost;

							if(total_cost+cost[curr_row][curr_col]+heuristic(next_row,next_col,target.first,target.second)<fun_cost[next_row][next_col]) //insert in the q
							{
								temp.r = next_row;
								temp.c = next_col;
								temp.cost = total_cost+cost[curr_row][curr_col]+heuristic(next_row,next_col,target.first,target.second);//
								q.push(temp); //insert in the queue
								visited[next_row][next_col] = true;
								parent_row[next_row][next_col] = curr_row; //update the parent row
								parent_col[next_row][next_col] = curr_col; //update the parent col
								cost[next_row][next_col] = total_cost+cost[curr_row][curr_col]; //update the cost matrix
								fun_cost[next_row][next_col] = total_cost+cost[curr_row][curr_col]+heuristic(next_row,next_col,target.first,target.second);
							}

						}

						else if(grid[curr_row][curr_col]>=0 and grid[next_row][next_col]<0) // next cell contains but not current
						{
							if(abs(grid[next_row][next_col])<=h_diff)
							{
								mud_cost = 0;
								height_cost = abs(grid[next_row][next_col]);
								total_cost = move_cost + mud_cost + height_cost;
								if(total_cost+cost[curr_row][curr_col]+heuristic(next_row,next_col,target.first,target.second)<fun_cost[next_row][next_col]) //insert in the q
								{
									temp.r = next_row;
									temp.c = next_col;
									temp.cost = total_cost+cost[curr_row][curr_col]+heuristic(next_row,next_col,target.first,target.second);//
									q.push(temp); //insert in the queue
									visited[next_row][next_col] = true;
									parent_row[next_row][next_col] = curr_row; //update the parent row
									parent_col[next_row][next_col] = curr_col; //update the parent col
									cost[next_row][next_col] = total_cost+cost[curr_row][curr_col]; //update the cost matrix
									fun_cost[next_row][next_col] = total_cost+cost[curr_row][curr_col]+heuristic(next_row,next_col,target.first,target.second);
								}
								
							}
						}

						else if(grid[curr_row][curr_col]<0 and grid[next_row][next_col]>=0) //curr cell contains rock but not next
						{
							if(abs(grid[curr_row][curr_col])<=h_diff)
							{
								mud_cost = grid[next_row][next_col];
								height_cost = abs(grid[curr_row][curr_col]);
								total_cost = mud_cost + height_cost + move_cost;
								if(total_cost+cost[curr_row][curr_col]+heuristic(next_row,next_col,target.first,target.second)<fun_cost[next_row][next_col]) //insert in the q
								{
									temp.r = next_row;
									temp.c = next_col;
									temp.cost = total_cost+cost[curr_row][curr_col]+heuristic(next_row,next_col,target.first,target.second);//
									q.push(temp); //insert in the queue
									visited[next_row][next_col] = true;
									parent_row[next_row][next_col] = curr_row; //update the parent row
									parent_col[next_row][next_col] = curr_col; //update the parent col
									cost[next_row][next_col] = total_cost+cost[curr_row][curr_col]; //update the cost matrix
									fun_cost[next_row][next_col] = total_cost+cost[curr_row][curr_col]+heuristic(next_row,next_col,target.first,target.second);
								}
							}
						}

						else if(grid[curr_row][curr_col]<0 and grid[next_row][next_col]<0) //both cells contain the rock
						{
							int temp_h;

							if(abs(grid[curr_row][curr_col])>=abs(grid[next_row][next_col]))
							{
								temp_h = abs(grid[curr_row][curr_col])-abs(grid[next_row][next_col]);
								//cout<<"leela";
							}
							else
							{
								temp_h = abs(grid[next_row][next_col])-abs(grid[curr_row][curr_col]);
								//cout<<"surya";
							}

							//cout<<temp_h;

							if(temp_h<=h_diff) //insert in the q
							{
								mud_cost = 0;
								height_cost = temp_h;
								total_cost = move_cost + mud_cost + height_cost;

								if(total_cost+cost[curr_row][curr_col]+heuristic(next_row,next_col,target.first,target.second)<fun_cost[next_row][next_col]) //insert in the q
								{
									temp.r = next_row;
									temp.c = next_col;
									temp.cost = total_cost+cost[curr_row][curr_col]+heuristic(next_row,next_col,target.first,target.second);//
									q.push(temp); //insert in the queue
									visited[next_row][next_col] = true;
									parent_row[next_row][next_col] = curr_row; //update the parent row
									parent_col[next_row][next_col] = curr_col; //update the parent col
									cost[next_row][next_col] = total_cost+cost[curr_row][curr_col]; //update the cost matrix
									fun_cost[next_row][next_col] = total_cost+cost[curr_row][curr_col]+heuristic(next_row,next_col,target.first,target.second);
								}
							}

						}

					}	
					

				}



			}
		}


	}


}









int main(){

	
	
	

	ifstream fin;
	fin.open("input.txt");
	string s;
	int start_row,start_col,sites,i,j,k;
	fin>>s; // line one
	fin>>col>>row;//line two
	vector<vector<int>> grid(row,vector<int> (col));
	pair<int,int> start_point;
	fin>>start_col>>start_row;//line three
	start_point.first = start_row;
	start_point.second = start_col;
	fin>>h_diff; //line four
	fin>>sites; //line five
	vector<pair<int,int> > target(sites);
	vector<pair<int,int> >::iterator it;
	int target_row,target_col;


	for(i=0;i<sites;i++)   //take the no of sites as input and store it in the target
	{

		
		fin>>target_col>>target_row;
		target[i].first = target_row;
		target[i].second = target_col;
		
	}

	for(j=0;j<row;j++) // SCAN THE GRID
	{
		int temp;
		for(k=0;k<col;k++)
		{
			fin>>temp;
			
			grid[j][k] = temp;
		}
		
	}

	fin.close(); // close the file
//------------------------ common code for the every file ----------------------------------------------------
	vector<vector<bool>> visited(row,vector<bool> (col));
	vector<vector<int>> cost(row,vector<int> (col));
	vector<vector<double>> a_cost(row,vector<double> (col));
	vector<vector<double>> fun_cost(row,vector<double> (col)); // cost for the g(n)+h(n)
	ofstream fout;
	fout.open("output.txt");

	//"C:\\Users\\LEELA SURYA TEJA\\Desktop\\output.txt"
	if(s=="BFS")
	{
		for(i=0;i<sites;i++)
		{

				vector<vector<int>> parent_row(row,vector<int> (col));
				vector<vector<int>> parent_col(row,vector<int> (col));
				vector<pair<int,int>> path;
				for(j=0;j<row;j++)
				{
					for(k=0;k<col;k++)
					{
						visited[j][k] = false;
						parent_row[j][k] = -1; //intialize row no of the parent
						parent_col[j][k] = -1; //intialize col no of the parent
					}
				}

				
				
				bfs(grid,start_point,target[i],visited,parent_row,parent_col); // call bfs
				//cout<<"leela";
				
				if(parent_row[target[i].first][target[i].second]==-1) fout<<"FAIL"; // cannot find path
				else
				{
					
					path.emplace_back(make_pair(target[i].first,target[i].second));
					//cout<<path.size();
					int res_row,res_col;
					res_row = parent_row[target[i].first][target[i].second];
					res_col = parent_col[target[i].first][target[i].second];
					while(res_row!=-1)
					{
						
						int temp_row;
						int temp_col;
						path.emplace_back(make_pair(res_row,res_col));
						

						temp_row = parent_row[res_row][res_col];
						temp_col = parent_col[res_row][res_col];
						res_row = temp_row;
						res_col = temp_col;
						
						//cout<<res_col<<","<<res_row;

					}

				}
				
				reverse(path.begin(),path.end()); //reverse the path 
				for(it=path.begin();it!=path.end();it++)
				{
					fout<<it->second<<","<<it->first;
					if(it!=path.end()-1)
					{
						fout<<" ";
					}	
					

				}
				// for(auto it:path)
				// {
				// 	cout<<grid[it.first][it.second];
				// 	cout<<" ";
				// }
			//cout<<path.size()-1<<" ";
			if(i!=sites-1)
			{
					fout<<'\n';
			} 
			path.clear(); // move to next line for next site path
		}

	}

	else if (s == "UCS")
	{
		//-----
		for(i=0;i<sites;i++)
		{

				vector<vector<int>> parent_row(row,vector<int> (col));
				vector<vector<int>> parent_col(row,vector<int> (col));
				vector<pair<int,int>> path;
				for(j=0;j<row;j++)
				{
					for(k=0;k<col;k++)
					{
						visited[j][k] = false;
						parent_row[j][k] = -1; //intialize row no of the parent
						parent_col[j][k] = -1; //intialize col no of the parent
					}
				}

				for(j=0;j<row;j++)
				{
					for(k=0;k<col;k++)
					{
						cost[j][k] = INT_MAX; // intialize cost as infinte for all the nodes
						
					}
				}
				cost[start_point.first][start_point.second] = 0; //intialize the cost of start point as zero

				
				
				ucs(grid,start_point,target[i],visited,parent_row,parent_col,cost); // call ucs
				//cout<<"leela";
				
				if(parent_row[target[i].first][target[i].second]==-1) fout<<"FAIL"; // cannot find path
				else
				{
					
					path.emplace_back(make_pair(target[i].first,target[i].second));
					//cout<<path.size();
					int res_row,res_col;
					res_row = parent_row[target[i].first][target[i].second];
					res_col = parent_col[target[i].first][target[i].second];
					while(res_row!=-1)
					{
						
						int temp_row;
						int temp_col;
						path.emplace_back(make_pair(res_row,res_col));
						

						temp_row = parent_row[res_row][res_col];
						temp_col = parent_col[res_row][res_col];
						res_row = temp_row;
						res_col = temp_col;
						
						//cout<<res_col<<","<<res_row;

					}

				}
				//cout<<path.size();
				reverse(path.begin(),path.end()); //reverse the path 
				for(it=path.begin();it!=path.end();it++)
				{
					fout<<it->second<<","<<it->first;
					if(it!=path.end()-1)
					{
						fout<<" ";
					}	
					

				}
			//cout<<cost[target[i].first][target[i].second]<<" ";	
			
			if(i!=sites-1)
			{
					fout<<'\n';
			} 
			path.clear(); // move to next line for next site path
			
		}

		
	}

	else
	{
		//------A*--------
		for(i=0;i<sites;i++)
		{

				vector<vector<int>> parent_row(row,vector<int> (col));
				vector<vector<int>> parent_col(row,vector<int> (col));
				vector<pair<int,int>> path;
				for(j=0;j<row;j++)
				{
					for(k=0;k<col;k++)
					{
						visited[j][k] = false;
						parent_row[j][k] = -1; //intialize row no of the parent
						parent_col[j][k] = -1; //intialize col no of the parent
					}
				}

				for(j=0;j<row;j++)
				{
					for(k=0;k<col;k++)
					{
						a_cost[j][k] = DBL_MAX; // intialize cost as infinte for all the nodes
						
					}
				}

				for(j=0;j<row;j++)
				{
					for(k=0;k<col;k++)
					{
						fun_cost[j][k] = DBL_MAX; // intialize cost as infinte for all the nodes
						
					}
				}




				a_cost[start_point.first][start_point.second] = 0; //initialize the cost of start point
				fun_cost[start_point.first][start_point.second] = heuristic(start_point.first,start_point.second,target[i].first,target[i].second);		
				a_star(grid,start_point,target[i],visited,parent_row,parent_col,a_cost,fun_cost); // call A*
				//cout<<grid[52][10];
				
				if(parent_row[target[i].first][target[i].second]==-1) fout<<"FAIL"; // cannot find path
				else
				{
					
					path.emplace_back(make_pair(target[i].first,target[i].second));
					//cout<<path.size();
					int res_row,res_col;
					res_row = parent_row[target[i].first][target[i].second];
					res_col = parent_col[target[i].first][target[i].second];
					while(res_row!=-1)
					{
						
						int temp_row;
						int temp_col;
						path.emplace_back(make_pair(res_row,res_col));
						

						temp_row = parent_row[res_row][res_col];
						temp_col = parent_col[res_row][res_col];
						res_row = temp_row;
						res_col = temp_col;
						
						//cout<<res_col<<","<<res_row;

					}

				}
				//cout<<path.size();
				reverse(path.begin(),path.end()); //reverse the path 
				for(it=path.begin();it!=path.end();it++)
				{
					fout<<it->second<<","<<it->first;
					if(it!=path.end()-1)
					{
						fout<<" ";
					}	
					
					
				}
				
				//cout<<a_cost[target[i].first][target[i].second]<<" ";
				if(i!=sites-1)
				{
					fout<<'\n';
				} 
				
			
			path.clear(); // move to next line for next site path
		}

	}
	//cout<<s;

	// for(j=0;j<row;j++)
	// {
	// 	for(k=0;k<col;k++)
	// 	{
	// 		cout<<grid[j][k]<<" ";
	// 	}

	// 		cout<<endl;
	// }

	//cout<<h_diff;
	//cout<<sites;
	//cout<<start_point.first<<" "<<start_point.second;
	//cout<<target[0].first<<" "<<target[0].second;


	
	fin.close();
	fout.close();
	return 0;
}