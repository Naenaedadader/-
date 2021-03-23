#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<string>
#include<algorithm>
#include<time.h>
using namespace std;
// ============================================================
struct information {
  string LON;
  string LAT;
  string time;
};
// 
struct dat{
  string TAG;
  vector<information> info;
  vector<string> rng;
  vector<int> rng_ind;
};
// ============================================================
void stotm(tm* time, string strtmp){
	// s -> string; to ; tm -> time
  stringstream ss(strtmp);
  string temp;
  
  getline(ss, temp, ' ');
  getline(ss, temp, ':');
  time->tm_hour = stoi(temp);
  getline(ss, temp, ':');
  time->tm_min = stoi(temp);
  getline(ss, temp, ':');
  time->tm_sec = stoi(temp);  
}
// ============================================================
/* 
void prttm(string str){ 
  // print -> prt; tm -> time 
  // for debug //
  struct tm *tm1;
  char buffer[80];

  tm1 = new struct tm();
  stotm(tm1, str);
  strftime(buffer, 80, "%H:%M:%S", tm1);
  cout << buffer << ", "; 
}
*/
// ============================================================
information ReadLine(string line, string* TAG){
  stringstream ss(line);
  information info_tmp;

  getline(ss, info_tmp.time, '\t');
  getline(ss, info_tmp.LON , '\t');
  getline(ss, info_tmp.LAT , '\t');
  getline(ss, *TAG, '\t');

  return info_tmp;
}
// ============================================================
/*
void prtInfo(vector<information>* info){
  // for debug //
  for (int i =0; i<info->size(); i++){
    cout << i << ", " 
	 << info->at(i).time << ", "
	 << info->at(i).LON  << ", "
	 << info->at(i).LAT  << endl;
	 
	 
	 
  }
}
*/
// ============================================================
/*
void prtInfo2(vector<information>::iterator it_begin,
	      vector<information>::iterator it_end){
  // for debug //
  int ind=0;
  for ( vector<information>::iterator it = it_begin;
	it < it_end; it++){
    
    cout << ind << ", "
	 << it-> time << ", "
	 << it-> LON  << ", "
	 << it-> LAT  << endl;
    ind++;
  }
}
*/
// ============================================================
bool tm_greater(const information &i, const information &j){
  return i.time < j.time;
}
bool tm_equal(const information &i, const information &j){
  return i.time == j.time;
}
bool LON_greater(const information &i, const information &j){
  return i.LON < j.LON;
}
bool LON_equal(const information &i, const information &j){
  return i.LON == j.LON;
}
bool LAT_greater(const information &i, const information &j){
  return i.LAT < j.LAT;
}
// ============================================================
void SortByLONLAT(vector<information>::iterator it_begin,
		  vector<information>::iterator it_end){
  vector<information>::iterator it;
  pair<vector<information>::iterator,
       vector<information>::iterator> bounds;

  sort(it_begin, it_end, LON_greater); // Sort by LON

  bounds.second = it_begin;
  it = adjacent_find(bounds.second, it_end, LON_equal);
  while(it != it_end){
    bounds=equal_range(bounds.second, it_end, *it, LON_greater); 
    sort(bounds.first, bounds.second, LAT_greater); // Sort by LAT
    // 更新搜尋範圍
    it = adjacent_find(--bounds.second, it_end, LON_equal);
  }
}
// ============================================================
void SortInfo(vector<information>::iterator it_begin,
	      vector<information>::iterator it_end){
  //
  vector<information>::iterator it;
  pair<vector<information>::iterator,
       vector<information>::iterator> bounds;
  //
  sort(it_begin, it_end, tm_greater);
	
  // 找下一個重複值
  bounds.second = it_begin;
  it = adjacent_find(bounds.second, it_end, tm_equal);
  while(it != it_end){
    // 計算重複值的索引範圍
    bounds=equal_range(bounds.second, it_end, *it, tm_greater); 
    SortByLONLAT(bounds.first, bounds.second); // sort 
    
    // 更新搜尋範圍
    it = adjacent_find(--bounds.second, it_end, tm_equal);
  }
}
// ============================================================
void pushDat(vector<dat>* D, dat* RDat){
	// R -> read; dat -> read data 
  // sort by time
  SortInfo(RDat->info.begin(), RDat->info.end());
  // Remove duplicates
  RDat->info.erase( unique(RDat->info.begin(), RDat->info.end(),
			   [](const information &i, const information &j)
			   { return ( (i.time == j.time) &&
				      (i.LON == j.LON) &&
				      (i.LAT == j.LAT) ); } ) ,
		    RDat->info.end() );

  D->push_back(*RDat) ;
}
// ============================================================
void ReadData(vector<dat>*D, string fname){
  fstream ifile;
  string line;
  string temp;
  information info_tmp;
  dat Read_dat;
  
  
  ifile.open(fname, ios::in);
  if(! ifile){
    cerr << "Can't open file!\n";
    exit(1);  
  }

  getline(ifile,line); // skip header
  getline(ifile,line); // first data
  info_tmp = ReadLine(line, &temp);
  Read_dat.info.push_back(info_tmp);  
  Read_dat.TAG = temp;

  while(getline(ifile,line)){
    info_tmp = ReadLine(line, &temp);
    
    if (temp != Read_dat.TAG){
      // push data to D1
      pushDat(D, &Read_dat);
      
      // get new data to Read_dat
      Read_dat.TAG = temp;
      Read_dat.info.clear();
    }
    Read_dat.info.push_back(info_tmp);
  }
  pushDat(D, &Read_dat);
}
// ============================================================
void CompRng(dat* cell_dat){
	// Compute range 
  time_t rawtime;
  time ( &rawtime );
  struct tm temp1, temp2;
  struct tm *tm1, *tm2;
 
  temp1 = *localtime(&rawtime);
  temp2 = *localtime(&rawtime);
  tm1 = &temp1;
  tm2 = &temp2;
  
   
  cell_dat->rng.push_back(cell_dat->info[0].time); //放進第一筆資料 
  cell_dat->rng_ind.push_back(0);  //時間範圍的index 
  stotm(tm1, cell_dat->info[0].time);
  
  for(int j =1; j < cell_dat->info.size(); j++){
    stotm(tm2, cell_dat->info[j].time);
 
	if(difftime(mktime(tm2), mktime(tm1)) <= 3600){
      swap(tm1,tm2);
    }
    else{
      cell_dat->rng.push_back(cell_dat->info[j-1].time);
      cell_dat->rng.push_back(cell_dat->info[ j ].time);
      
      cell_dat->rng_ind.push_back(j-1);
      cell_dat->rng_ind.push_back(j);
      stotm(tm1, cell_dat->info[j].time);
    }
  }
  if(cell_dat->rng.size()%2){
    cell_dat->rng.push_back(cell_dat->info.back().time);
    cell_dat->rng_ind.push_back(cell_dat->info.size()-1);
  }
  
}
// ============================================================
int main(){
  // part 1
  vector<dat>* Dat = new vector<dat>();
  ReadData(Dat, "data.csv");

  // part 2
  
  for(int i = 0; i < Dat->size(); i++)
    CompRng(&Dat->at(i)); // Compute range

  // sort by counter
  sort(Dat->begin(), Dat->end(),
       [](const dat &i, const dat &j) { return i.info.size() > j.info.size(); } );
  

  // ----------------------------------------
  cout << "TAG,Counter,Num_Range,Range" << endl;
  for (int i = 0; i < Dat->size(); i++){
    dat* cell;
    cell = &Dat->at(i);
    
    cout <<cell -> TAG << ","
	 << cell -> info.size() <<  ","
         << cell -> rng.size()/2 << ",[ ";
    for (int j=0; j < cell->rng.size(); j+=2){
    	cout << cell->rng[j] << " ~ " << cell->rng[j+1] << "; "; 
	}
	cout << " ]" <<endl;

  }
  // 存取 
  fstream ifile;
  ifile.open("task1.csv", ios::out | ios::trunc);
  if(! ifile){
     cerr << "Can't open file!\n";
     exit(1);  
  	}
  	ifile << "TAG,Counter,Num_Range,Range" << endl;
  for (int i = 0; i < Dat->size(); i++){
    dat* cell;
    cell = &Dat->at(i);
    
    ifile <<cell -> TAG << ","
	 << cell -> info.size() <<  ","
         << cell -> rng.size()/2 << ",[ ";
    for (int j=0; j < cell->rng.size(); j+=2){
    	ifile << cell->rng[j] << " ~ " << cell->rng[j+1] << "; "; 
	}
	ifile << " ]" <<endl;
  	} 
  	
  return 0;
}
