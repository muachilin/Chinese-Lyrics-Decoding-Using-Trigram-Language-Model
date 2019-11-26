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

        string lm_filename = argv[2];
        File lmFile( lm_filename.c_str(), "r" );
        lm.read(lmFile);
        lmFile.close();
	
	string corpus_filename = argv[1];
        ifstream corpus_file;
        corpus_file.open(corpus_filename.c_str());
	
	double H_val = 0.0;	
	string buffer;

	
	
	double total_line_val = 0.0;
	int total_sen = 0;
	int total_num = 0;

	int word_count = 0;
	while(getline(corpus_file,buffer))
	{
		double  single_line_val = 0.0;
		string first_word = buffer.substr(0,2);
		string second_word = buffer.substr(2,2);
		single_line_val += getBigramProb("<s>",first_word.c_str());
		
		
		
		single_line_val += \
		getTrigramProb\
		("<s>",first_word.c_str(),second_word.c_str());
			
		word_count += 1;
		
		
		for(int i=0;i<(buffer.size()/2)-2;++i)
		{
			single_line_val += getTrigramProb\
			(buffer.substr(2*i,2).c_str()\
			,buffer.substr((2*i)+2,2).c_str()\
			,buffer.substr((2*i)+4,2).c_str());
			
			word_count++;
		}
               
		single_line_val += getTrigramProb\
                        (buffer.substr(buffer.size()-4,2).c_str()\
                        ,buffer.substr(buffer.size()-2,2).c_str()\
                        ,"</s>");
		total_line_val += single_line_val;
		total_sen++;
		
		word_count ++;
	}
	

	
	H_val = (double)((-total_line_val)/(word_count));
	double perplexity_val = pow(10,H_val);

	cout<<"perplexity : "<<perplexity_val<<endl;	
	
}
