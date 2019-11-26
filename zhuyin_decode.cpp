#include <limits.h>
#include <float.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <vector>
#include <stdio.h>

#include "Ngram.h"
using namespace std;

Vocab voc;
Ngram lm( voc, 3);

// Get P(W3 | W1, W2) -- trigram
double getTrigramProb(const char *w1, const char *w2, const char *w3)
{
    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);
    VocabIndex wid3 = voc.getIndex(w3);

    if(wid1 == Vocab_None)  //OOV
        {wid1 = voc.getIndex(Vocab_Unknown);}
    if(wid2 == Vocab_None)  //OOV
        {wid2 = voc.getIndex(Vocab_Unknown);}
    if(wid3 == Vocab_None)  //OOV
        {wid3 = voc.getIndex(Vocab_Unknown);}

    VocabIndex context[] = { wid2, wid1, Vocab_None };
    return lm.wordProb( wid3, context );
}

// Get P(W2 | W1) -- bigram
double getBigramProb(const char *w1, const char *w2)
{
	VocabIndex wid1 = voc.getIndex(w1);
	VocabIndex wid2 = voc.getIndex(w2);

	if(wid1 == Vocab_None)  //OOV
        {
		wid1 = voc.getIndex(Vocab_Unknown);
	}
    	if(wid2 == Vocab_None)  //OOV
        {
		wid2 = voc.getIndex(Vocab_Unknown);
	}
	
	VocabIndex context[] = { wid1, Vocab_None };

	return lm.wordProb( wid2, context);
}


double getOnegramProb(const char *w)
{

	
	VocabIndex wid = voc.getIndex(w);
	VocabIndex context[] = { Vocab_None };
	double one_gram;
	if(wid != Vocab_None)
	{
	 	one_gram = lm.wordProb( wid,context);
		return one_gram;
	}
	else{ return -10;}
	
}

int main(int argc , char*argv[])
{
	// construct the map

	double top_sampling_rate = 1;

                string lm_filename = argv[3];
                File lmFile( lm_filename.c_str(), "r" );
                lm.read(lmFile);
                lmFile.close();
	
	

	map<string,map<string,vector<string> >> big_map;	
	map<string,vector<string> >word_map;
	
	string map_filename = argv[2];
	ifstream map_file;
	map_file.open(map_filename.c_str());
	string mbuffer;
	bool same_head = false;
	

	string pone_name = "phone.txt";
	ifstream phone_file;
	phone_file.open(pone_name.c_str());
	
	map<string,bool> initials_map;
	map<string,bool> finals_map;
	map<string,bool> middles_map;
	
	string buffer;
	getline(phone_file,buffer);
	
	for(int i=0;i<buffer.size()/2;++i)
	{
		string token = buffer.substr(2*i,2);
		initials_map[token] = true;
	}
	getline(phone_file,buffer);
	for(int i=0;i<buffer.size()/2;++i)
	{
		string token = buffer.substr(2*i,2);
		middles_map[token] = true;
	}
	getline(phone_file,buffer);
	for(int i=0;i<buffer.size()/2;++i)
	{
		string token = buffer.substr(2*i,2);
		finals_map[token] = true;
	}
		
	
	int same_head_number = 0;
	int map_index = 0;
	string tmp_token;
	
	cout<<"hello!!"<<endl;
		
	while(getline(map_file,mbuffer))
	{
		if(same_head == false)
		{
			string head = mbuffer.substr(0,2);
			map<string,vector<string> > small_map;
			big_map[head] = small_map;
			mbuffer.erase(0,3);
			same_head_number = stoi(mbuffer);
			same_head = true;
			map_index = 0;
		}
		
		else
		{
			
			if(map_index%2==0)
			{
				tmp_token = mbuffer;	
			}
			else
			{
				istringstream ss(mbuffer);
				string token;
				while(getline(ss, token, ' '))
				{
					VocabIndex wid = voc.getIndex(token.c_str());
					VocabIndex context[] = { Vocab_None };
					if(wid!=Vocab_None)
					{
						string head = tmp_token.substr(0,2);
						big_map[head][tmp_token].push_back(token);
					}
				}
				if(map_index == (same_head_number*2)-1)
				{
					same_head = false;
				}
			}
				
			map_index ++;
			
		}
		
		
	}
	
	map_file.close();	



	string ori_filename = "./finaldata/test_ori.txt";
        ifstream ori_file;
        ori_file.open(ori_filename.c_str());

        vector<vector<string>>ori_word;
        while(getline(ori_file,buffer))
        {
		if(buffer.empty()){continue;}
	
		buffer.erase(std::remove(buffer.begin()\
		,buffer.end(),' '),buffer.end());
			
		vector<string> ori_arr;
                for(int i=0;i<buffer.size()/2;++i)
                {
                        ori_arr.push_back(buffer.substr(2*i,2));
                }
                ori_word.push_back(ori_arr);

        }
	
		
	string input_filename = argv[1];
	ifstream input_file;
	input_file.open(input_filename.c_str()); 
	
			
	int sum = 0;
	int correct_point = 0;
	int all_point = 0;
	
	ofstream output_file_txt;
        output_file_txt.open(argv[4]);
	
	
	// segment the word list
	while(getline(input_file,buffer))
	{
		if(buffer.empty()){continue;}
		vector<string> word_list;
		
		string tmp_initials;
		int word_index = 0;
		int t_index = 0;
		int buffer_size = buffer.size();
		
		while(t_index <= buffer_size-1)
		{
			string token = buffer.substr(t_index,2);
			if(initials_map.count(token))
			{
				if(t_index <= buffer_size - 6)
				{
					string candidate_3 = buffer.substr(t_index,6);
					if(big_map[token].count(candidate_3))
					{
						word_list.push_back(candidate_3);
						t_index += 6;
						continue;
					}
				}
				if(t_index <= buffer_size - 4)
				{
					string candidate_2 = buffer.substr(t_index,4);	
					if(big_map[token].count(candidate_2))
					{
						word_list.push_back(candidate_2);
						t_index += 4;
						continue;
					}
				
				}
			}
			
			else if(middles_map.count(token))
			{
				if(t_index <= buffer_size - 4)
				{
					string candidate_2 = buffer.substr(t_index,4);
					if(big_map[token].count(candidate_2))
					{
						word_list.push_back(candidate_2);
						t_index += 4;
						continue;
					}
				}
				if(t_index <= buffer_size - 2)
				{
					string candidate_1 = buffer.substr(t_index,2);
					if(big_map[token].count(candidate_1))
                                        {
                                                word_list.push_back(candidate_1);
                                                t_index += 2;
                                                continue;
                                        }
				}
				
			}
			word_list.push_back(token);
                        t_index += 2;
				
			
		}
			
		

		
				
		vector<vector<double> > delta;
		vector<vector<string> > tree;
		vector<vector<int>> backtrack; // size should be tree.size()-1
		
		for(int i=0;i<word_list.size();++i)
		{
			
		
			//{
				vector<double> delta_arr;
				vector<string> tree_arr;
				vector<int> backtrack_arr;
				string head = word_list[i].substr(0,2);
				if(i==0)
				{
				   for(int j=0;j<big_map[head][word_list[i]].size();++j)
                                    {
                                        string candidate\
					 = big_map[head][word_list[i]][j];
                                        double bi_gram = 0.0;
                                        bi_gram = \
                                        getBigramProb("<s>"\
                                        ,candidate.c_str());

                                        delta_arr.push_back(bi_gram);
                                        tree_arr.push_back(candidate);
                                   }

                                   delta.push_back(delta_arr);
                                   tree.push_back(tree_arr);				
					
				}
				else if(i==1)
				{
				    for(int j=0;j<big_map[head][word_list[i]].size();++j)
                                    {
                                        double delta_max = DBL_MAX*(-1);
                                        int backtrack_index = 0;

                                        string candidate = \
                                        big_map[head][word_list[i]][j];
                                        int last_pos = 0;
					
                                        for(int k=0;\
                                        k<tree[last_pos].size();++k)
                                        {
                                           double score = getTrigramProb\
                                           ("<s>",\
                                           tree[last_pos][k].c_str(),\
                                                candidate.c_str());

                                           double delta_can;
                                           delta_can = score+delta[last_pos][k];

                                           if(delta_can > delta_max)
                                           {
						delta_max = delta_can;
                                                backtrack_index = k;
                                           }

                                        }
                                        delta_arr.push_back(delta_max);
                                        tree_arr.push_back(candidate);
                                        backtrack_arr.push_back(backtrack_index);
                                    }

                                    delta.push_back(delta_arr);
                                    tree.push_back(tree_arr);
                                    backtrack.push_back(backtrack_arr);
					
				}
				else{
					
				   for(int j=0;j<big_map[head][word_list[i]].size();++j)
                                    {
                                        double delta_max = DBL_MAX*(-1);
                                        int backtrack_index = 0;

                                        string candidate = \
                                        big_map[head][word_list[i]][j];
                                        int last_pos = tree.size()-1;
                                        for(int k=0;\
                                        k<tree[last_pos].size();++k)
                                        {
                                           for(int g=0;\
                                           g<tree[last_pos-1].size();++g)
                                           {

					     double score = getTrigramProb\
                                             (tree[last_pos-1][g].c_str(),\
                                             tree[last_pos][k].c_str(),\
                                             candidate.c_str());
					
                                           double delta_can;
                                           delta_can = score+delta[last_pos][k];
					   
    					   if(i==word_list.size()-1)
                                           {
						delta_can +=  (getBigramProb\
                                              (candidate.c_str(),"</s>")+\
                                                getTrigramProb(\
                                              tree[last_pos][k].c_str(),\
                                              candidate.c_str(),"</s>"\
                                                ));
                                           }

                                           if(delta_can > delta_max)
                                           {
                                                delta_max = delta_can;
                                                backtrack_index = k;
                                           }
                                          }// g
                                        }//k
                                        delta_arr.push_back(delta_max);
                                        tree_arr.push_back(candidate);
                                        backtrack_arr.push_back(backtrack_index);
                                    }

                                    delta.push_back(delta_arr);
                                    tree.push_back(tree_arr);
                                    backtrack.push_back(backtrack_arr);
					  
					
				}
				
				
			//}//if ZhuYin
			
		}
	
		///  start to backtrack result  ///
		
		int last_big_index;
		double delta_last_max = DBL_MAX*(-1);
			
		for(int i=0;i<delta[delta.size()-1].size();++i)
		{
			if(delta[delta.size()-1][i]> delta_last_max)
			{
				delta_last_max = delta[delta.size()-1][i]; 
				last_big_index = i;
			}
			
		}
		
		vector<int> backtrack_result;
		backtrack_result.push_back(last_big_index);
		
		for(int i=backtrack.size()-1;i>=0;--i)
		{
			int last_index\
			 = backtrack[i]\
			 [backtrack_result[backtrack_result.size()-1]];	

			backtrack_result.push_back(last_index);
			
		}
		
		reverse(backtrack_result.begin(),\
		backtrack_result.end());
		
		
		
		
		output_file_txt << "<s>" <<" ";	
		for(int i=0;i<backtrack_result.size();++i)
		{
			output_file_txt << tree[i][backtrack_result[i]] <<" ";
			if(!tree[i][backtrack_result[i]].compare(ori_word[sum][i]))
			{
				correct_point ++;
			}	
			all_point ++;
		}
		 output_file_txt << "</s>" <<endl;
		
			
		sum++;
	}

	cout<<"accuracy of decoding : "<<(double)correct_point/all_point<<endl;
	//cout<<all_point<<endl;

}



