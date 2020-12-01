#include <iostream>
#include <string>
#include <fstream>
#include "../include/filesystem/file.h"
#include "disk.h"



string File::get_data(){
    return this->data;
}

void File::set_data(string data){
    this->data = data;
}

File::File(string name){
    this->name = name;
}

void File::write_to_file(Disk disk,string text){
    vector<int> free_segments = disk.free_segments;
    vector<string> splits;
    vector<int> file_segments;

    size_t file_length = text.length();
    if(file_length > 100){
        for (unsigned i = 0; i < file_length; i += 100) {
            if(file_length % 100 != 0 && i > (file_length - 100)){
                splits.push_back(text.substr(i,file_length % 100));
            } else {
                splits.push_back(text.substr(i,100));
            }
            file_segments.push_back(free_segments[0]);
            free_segments.erase(free_segments.begin());
        };
    } else {
        splits.push_back(text);
        file_segments.push_back(free_segments[0]);
        free_segments.erase(free_segments.begin());
    }

    disk.free_segments = free_segments;

    fstream fout;
    fout.open("../file_system.txt");

    for (auto i = file_segments.begin(); i != file_segments.end(); ++i){
        fout.seekg(1001 + ((*i) * 101));
        string curr_string  = splits[0];
        curr_string.resize(100);
        fout << (curr_string + '\n');
        splits.erase(splits.begin());
    }

    this->data = text;
}

void File::write_to_file(Disk disk,int write_at,string text){
    map<string,vector<int>> metadata = disk.get_file_metadata();
    vector<int> free_segments = disk.free_segments;
    vector<string> splits;
    vector<int> file_segments = ;
    string new_text = this->get_data().insert(write_at,text);

    
    size_t file_length = new_text.length();
    if(file_length > 100){
        for (unsigned i = 0; i < file_length; i += 100) {
            if(file_length % 100 != 0 && i > (file_length - 100)){
                splits.push_back(new_text.substr(i,file_length % 100));
            } else {
                splits.push_back(new_text.substr(i,100));
            }
            file_segments.push_back(free_segments[0]);
            free_segments.erase(free_segments.begin());
        };
    } else {
        splits.push_back(new_text);
        file_segments.push_back(free_segments[0]);
        free_segments.erase(free_segments.begin());
    }

    disk.free_segments = free_segments;

    fstream fout;
    fout.open("../file_system.txt");

    for (auto i = file_segments.begin(); i != file_segments.end(); ++i){
        fout.seekg(1001 + ((*i) * 101));
        string curr_string  = splits[0];
        curr_string.resize(100);
        fout << (curr_string + '\n');
        splits.erase(splits.begin());
    }
}

string File::read_from_file(){
    return this->data;
}

string File::read_from_file_at(int start,int size){
    return this->data.substr(start,size);
}

// File::move_within_file(int start,int size,int target){

// }

void File::truncate_file(int max_size){
    this->data.resize(max_size);


}

