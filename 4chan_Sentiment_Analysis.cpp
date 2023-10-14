//4chan Sentiment Analysis
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iomanip>

std::map<std::string, double> analyze(std::vector<std::string> content, std::map<std::string, std::map<std::string, int>*>& lex){
    int size = content.size();
    double const MULTIPLIER = (20/3);
    std::map<std::string, int> valid_word_count;
    for (int i = 0; i < size; i++){
        if (lex.find(content[i])!=lex.end()){
            int hit = 0;
            std::map<std::string, int> working = *lex[content[i]];
            for (std::map<std::string, int>::const_iterator it = working.begin(); it != working.end(); it++){
                if (it->second == 1){
                    hit=1;
                    break;
                }
            }
            if (hit==1){
                valid_word_count[content[i]]++;
            }
        }
    }

    std::map<std::string, double> rating;
    for (std::map<std::string, int>::const_iterator it = valid_word_count.begin(); it != valid_word_count.end(); it++){
        double this_m = MULTIPLIER * (it->second);
        std::map<std::string, int> working = *lex[it->first];
        for (std::map<std::string, int>::const_iterator it2 = working.begin(); it2 != working.end(); it2++){
            if (it2->second == 1){
                rating[it2->first] += this_m;
            }
        }
    }

    for (std::map<std::string, double>::const_iterator it = rating.begin(); it != rating.end(); it++){
        rating[it->first] = (it->second)/double(size);
    }

    return rating;
}

int main(int argc, char* argv[]){
    if (argc!=3){
        std::cerr<<"Invalid Amount of Arguments"<<std::endl;
        return 1;
    }

    std::string link = argv[1];
    std::string output_file_name = argv[2];
	std::string word_file_name = "words.txt";

    std::ifstream word_file(word_file_name);
    if (!word_file.good()){
        std::cerr<<word_file_name<<" Cannot be Opened"<<std::endl;
        return 1;
    }

    std::map<std::string, std::map<std::string, int>*> all_words;
    std::string word_file_term;
    int count = 1;
    std::map<std::string, int>* working_map = new std::map<std::string, int>;
    std::string current_word;
    std::string previous_word = "";
    int first_word = 1;
    std::string current_feel;
    while (word_file >> word_file_term){
        if (count == 1){
            current_word=word_file_term;
            if (first_word == 1){
                previous_word = current_word;
                first_word = 0;
            }
            if (current_word!=previous_word){
                all_words[previous_word] = working_map;
                working_map = new std::map<std::string, int>;
                previous_word=current_word;
            }
            count++;
        }else if(count == 2){
            current_feel = word_file_term;
            (*working_map)[current_feel] = 0;
            count++;
        }else if(count == 3){
            int num = std::stoi(word_file_term);
            (*working_map)[current_feel] += num;
            count = 1;
        }
    }
    all_words[current_word] = working_map;

    word_file.close();

    std::system(("python 4chanThread.py "+link).c_str());

    std::ifstream thread_file("thread_info.txt"); 
    std::string thread_word;
    first_word = 1;
    int mode;
    int thread_no;
    std::vector<std::string> title;
    std::vector<std::string> body;
    while (thread_file >> thread_word){
        if (first_word==1){
            first_word=0;
            if (thread_word=="Error"){
                mode = 0;
                continue;
            }else if (thread_word == "Thread_Number:"){
                mode = 1;
                continue;
            }else{
                std::cerr<<"The File Produced by 4chanThread.py is Invalid"<<std::endl;
                return 1;
            }
        }

        if (mode==0){
            std::cout<<thread_word<<std::endl;
            return 1;
        }else if (mode==1){
            thread_no = std::stoi(thread_word);
            thread_file >> thread_word;
            if (thread_word == "Title:"){
                mode = 2;
                continue;
            }else{
                std::cerr<<"The File Produced by 4chanThread.py is Invalid"<<std::endl;
                return 1;
            }
        }else if (mode==2){
            if (thread_word=="Body:"){
                mode = 3;
                continue;
            }else{
                std::transform(thread_word.begin(), thread_word.end(), thread_word.begin(), [](unsigned char c){return std::tolower(c);});
                title.push_back(thread_word);
            }
        }else if (mode==3){
            std::transform(thread_word.begin(), thread_word.end(), thread_word.begin(), [](unsigned char c){return std::tolower(c);});
            body.push_back(thread_word);
        }
    }

    std::map<std::string, double> title_rating = analyze(title, all_words);
    std::map<std::string, double> body_rating = analyze(body, all_words);

    std::ofstream output_file(output_file_name);
    std::map<std::string, double>::iterator it;
    output_file<<"Thread Number: "<<thread_no<<std::endl;
    output_file<<"URL: "<<link<<std::endl;
    output_file<<"\nTitle:"<<std::endl;
    for (it = title_rating.begin(); it != title_rating.end(); it++) {
        output_file<< std::setw(15) << it->first << std::setw(15) << std::fixed << std::setprecision(2) << (it->second*100) << std::endl;
    }
	if (title_rating.size()==0){
		output_file<<"N/A"<<std::endl;
	}
    output_file<<"\nBody:"<<std::endl;
    for (it = body_rating.begin(); it != body_rating.end(); it++) {
        output_file<< std::setw(15) << it->first << std::setw(15) << std::fixed << std::setprecision(2) << (it->second*100) << "%" << std::endl;
    }

    return 0;
}