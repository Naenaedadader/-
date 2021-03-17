#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<string>
#include<algorithm>
#include<time.h>
using namespace std;

  
void stotm(tm* time, string strtmp){ //���Ʈ榡 
  
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

void prttm(string str){ //�ɶ�����X 
  time_t rawtime;
  struct tm *tm1;
  char buffer[80];

  tm1 = localtime ( &rawtime );
  stotm(tm1, str);
  strftime(buffer, 80, "%H:%M:%S", tm1);
  cout << buffer << ", "; 
   
}

int main(){
	//��l�ƫŧi 
  fstream ifile;
  string line;
  
  vector<string> time_str;
  vector<string> LON;
  vector<string> LAT;
  vector<string> TAG_tmp;
  vector<string> TAG;
  // Ū��csv�ä��� 
  ifile.open("data.csv", ios::in);
  if(! ifile){
     cerr << "Can't open file!\n";
     exit(1);  
  }

  while(getline(ifile,line)){
    stringstream ss(line);
    string temp;

    getline(ss, temp, '\t');
    time_str.push_back(temp);

    getline(ss, temp, '\t');
    LON.push_back(temp);

    getline(ss, temp, '\t');
    LAT.push_back(temp);

    getline(ss, temp, '\t');
    TAG_tmp.push_back(temp);
  }
	ifile.close();	

  vector< vector<string> > time_vec;
  vector<int> counter;
  {
    vector<string> tmp_time;
    int ind= 0;
    int i  = 1;

    TAG.push_back(TAG_tmp[1]);
    tmp_time.push_back(time_str[1]);
    while( i < TAG_tmp.size()){
      if( TAG_tmp[i] != TAG[ind]){
      	
	sort(tmp_time.begin(), tmp_time.end());
	tmp_time.erase( unique(tmp_time.begin(), tmp_time.end()),
			tmp_time.end() );
			
	counter.push_back(tmp_time.size());
      
	TAG.push_back(TAG_tmp[i]);

	time_vec.push_back(tmp_time);
	tmp_time.clear();
	ind += 1;
      }
      tmp_time.push_back(time_str[i]);
      i += 1;
    }

    sort(tmp_time.begin(), tmp_time.end());
    tmp_time.erase( unique(tmp_time.begin(), tmp_time.end()),
		    tmp_time.end() );
    counter.push_back(tmp_time.size());
    time_vec.push_back(tmp_time);
    tmp_time.clear();
  }
  
  //writefile
  fstream file;
 
  file.open("write.csv", ios::out | ios::trunc);
  for (int i=0; i<TAG.size(); ++i){
	 	file << TAG[i] << "	"<< counter[i] << endl;
	 }
  file.close();
  
  
  // ========================================
  for (int i =0; i<TAG.size();i++){
    cout << i << " ;" << TAG[i] << ": " << counter[i] << endl;
  }

  cout << "==============================" << endl;
  // ========================================
  // ========================================

  vector< vector<string> > time_rng;  // �d�� 
  vector< vector<int> > time_rng_ind; // 
  {

    vector<int> rng_ind;
    vector<string> rng_tmp;
    time_t rawtime;
    struct tm tm1,tm2;
    struct tm *tm_ptr1;
    struct tm *tm_ptr2;

    tm1 = *localtime ( &rawtime );
    tm2 = tm1;
    tm_ptr1 = &tm1;
    tm_ptr2 = &tm2;

    // ------------------------------
    // time_vec.size(): CELL �`�Ӽ� 
    for(int k =0; k<time_vec.size(); k++){
    	rng_tmp.push_back(time_vec[k][0]); // start 
      	rng_ind.push_back(0);

      	stotm(tm_ptr1, time_vec[k][0]); //�Ĥ@�Ӯɶ��নtm 

		// time_vec[k].size(): CELL_??? ���U�ɶ����Ӽ� 
  	    for(int j =1; j < time_vec[k].size(); j++){
			stotm(tm_ptr2, time_vec[k][j]); //�ĤG�Ӯɶ��নtm 
			if(difftime(mktime(tm_ptr2), mktime(tm_ptr1)) <= 3600){ //diff �۴�2�Ӯɶ� 
	  			swap(tm_ptr1,tm_ptr2);
			}
			else{
	  			rng_tmp.push_back(time_vec[k][j-1]); // �d�򵲧��ɶ� 
	  			rng_tmp.push_back(time_vec[k][j]);   // �U�@�ӽd�򪺶}�l�ɶ� 

	  			rng_ind.push_back(j-1);
	  			rng_ind.push_back(j);
	  			stotm(tm_ptr1, time_vec[k][j]);
			}
      	}
      	if(rng_tmp.size()%2){ // �ɶ��d�򪺭ӼƬ��_��    start1-end1, start2-end2, start3   
			rng_tmp.push_back(time_vec[k].back());  // �̫�@�ӽd�򪺮ɶ������ɶ�,���ɶ����̫�@�� 
			rng_ind.push_back(time_vec[k].size()-1);
      	}


      	time_rng.push_back(rng_tmp);
      	time_rng_ind.push_back(rng_ind);
      	rng_tmp.clear();

      	rng_ind.clear();
    }
  }


  for (int k =0; k<time_rng.size();k++){
    cout << TAG[k] << ", counter: " << counter[k]
	 << ", range: " << time_rng[k].size()/2 << endl;
    cout << "========================================" << endl;
    
    for (int i =0; i<time_rng[k].size();i+=2){
      cout << "<< " << time_rng[k][i] << " ~ "
	   << time_rng[k][i+1] << " >>" << endl;
      for(int j = time_rng_ind[k][i]; j<= time_rng_ind[k][i+1]; j++)
	prttm(time_vec[k][j]);
      cout << endl << endl;
      
    }
    
    cout << "========================================" << endl;
}
    
  ifile.open("task2.txt", ios::out | ios::trunc);
  if(! ifile){
     cerr << "Can't open file!\n";
     exit(1);  
  	}
  
	 for (int k = 0; k < time_rng.size();k++){
	 	ifile << TAG[k] << ": ";
			for(int i=0; i<time_rng[k].size(); i+=2){
				ifile << "[ " << time_rng[k][i] << " ~ " 
				<< time_rng[k][i+1] << " ], "; 
			}
			ifile << endl;

	 }
  	ifile.close();
  	////////////
  	ifile.open("cell1.csv", ios::out | ios::trunc);
  	
  if(! ifile){
     cerr << "Can't open file!\n";
     exit(1);  
  	}
  	ifile << "DATE_TIME,LON,LAT,TAG,marker-color" << endl;
	 for (int k = 0; k < TAG_tmp.size();k++){
	 	if( TAG_tmp[k] == TAG_tmp[2341] ){
	 		ifile << time_str[k] << ","
	 		<< LON[k] << "," << LAT[k] << "," 
	 		<< TAG_tmp[k] << ",\"#ff002b\"" << endl;
	 		continue;
		 }
		 
	 	if( TAG_tmp[k] == TAG_tmp[884] ){
	 		ifile << time_str[k] << ","
	 		<< LON[k] << "," << LAT[k] << "," 
	 		<< TAG_tmp[k] << ",\"#00ff99\"" << endl;
	 		continue;
		 }
	 	
	 	if( TAG_tmp[k] == TAG_tmp[1885] ){
	 		ifile << time_str[k] << ","
	 		<< LON[k] << "," << LAT[k] << "," 
	 		<< TAG_tmp[k] << ",\"#00ff00\"" << endl;
	 		continue;
		 }
		 
		if( TAG_tmp[k] == TAG_tmp[1112] ){
	 		ifile << time_str[k] << ","
	 		<< LON[k] << "," << LAT[k] << "," 
	 		<< TAG_tmp[k] << ",\"#0000ff\"" << endl;
	 		continue;
		 }
		
	 	if( TAG_tmp[k] == TAG_tmp.back() ){
	 		ifile << time_str[k] << ","
	 		<< LON[k] << "," << LAT[k] << "," 
	 		<< TAG_tmp[k] << ",\"#ff7800\"" << endl;
	 		continue;
		 }
	 	
			

	 }
  	ifile.close();
  	
  
  return 0;
}
