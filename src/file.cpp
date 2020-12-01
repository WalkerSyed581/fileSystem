#include <cstdint>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
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

void File::write_to_file(Disk& disk,string text){
    vector<int> free_segments = disk.free_segments;
    map<string,vector<int>> metadata = disk.get_file_metadata();
    vector<string> splits;
    auto file_data = metadata.find(this->name);
    vector<int> file_segments;
    if(file_data != metadata.end()){

        if(!file_data->second.empty()){
            free_segments.insert( free_segments.end(), file_data->second.begin(), file_data->second.end() );
        }
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
        file_data->second = file_segments;

        string new_metadata = disk.set_file_metadata(metadata);

        fstream fout;
        fout.open("../file_system.txt");

        if(new_metadata != "-1"){
            fout << new_metadata;
        }



        for (auto i = file_segments.begin(); i != file_segments.end(); ++i){
            fout.seekg(1001 + ((*i) * 101));
            string curr_string  = splits[0];
            curr_string.resize(100);
            fout << (curr_string + '\n');
            splits.erase(splits.begin());
        }

        this->data = text;
    }
}

int File::write_to_file(Disk& disk,int write_at,string text){
    map<string,vector<int>> metadata = disk.get_file_metadata();
    auto file_data = metadata.find(this->name);
    vector<int> file_segments;
    if(file_data != metadata.end()){
        file_segments = file_data->second;
    }
    vector<int> free_segments = disk.free_segments;
    vector<string> splits;
    string curr_text = this->get_data();
    string new_text = this->get_data().insert(write_at,text);

    if(write_at >= curr_text.length()){
        return -1;
    }
    int segment_number = write_at/100;

    for(auto i = (file_segments.begin() + segment_number); i != file_segments.end();++i){
        free_segments.push_back(*i);
        file_segments.erase(i);
    }    
    
    sort(free_segments.begin(),free_segments.end());

    size_t file_length = new_text.length();
    if(file_length > 100){
        for (unsigned i = (segment_number * 100); i < file_length; i += 100) {
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
    file_data->second = file_segments;

    string new_metadata = disk.set_file_metadata(metadata);

    fstream fout;
    fout.open("../file_system.txt");

    if(new_metadata != "-1"){
        fout << new_metadata;
    }

    for (auto i = file_segments.begin(); i != file_segments.end(); ++i){
        fout.seekg(1001 + ((*i) * 101));
        string curr_string  = splits[0];
        curr_string.resize(100);
        fout << (curr_string + '\n');
        splits.erase(splits.begin());
    }
    return 0;
}

string File::read_from_file(){
    return this->data;
}

string File::read_from_file_at(int start,int size){
    return this->data.substr(start,size);
}

// File::move_within_file(int start,int size,int target){

// }

int File::truncate_file(Disk& disk,int max_size){
    map<string,vector<int>> metadata = disk.get_file_metadata();
    auto file_data = metadata.find(this->name);
    vector<int> file_segments;
    if(file_data != metadata.end()){
        file_segments = file_data->second;
    }
    vector<int> free_segments = disk.free_segments;
    vector<string> splits;

    if(max_size >= this->get_data().length()){
        return -1;
    }

    this->set_data(this->get_data().erase(max_size));
    string new_text = this->get_data();

    int segments_rem = (max_size/100) + 1;
    int segment_number = max_size/100;

    for(auto i = (file_segments.begin() + segments_rem); i != file_segments.end();++i){
        free_segments.push_back(*i);
        file_segments.erase(i);
    }    
    
    sort(free_segments.begin(),free_segments.end());

    size_t file_length = new_text.length();
    disk.free_segments = free_segments;
    file_data->second = file_segments;

    string new_metadata = disk.set_file_metadata(metadata);

    fstream fout;
    fout.open("../file_system.txt");

    if(new_metadata != "-1"){
        fout << new_metadata;
    }

    string last_segment_data = new_text.substr(segment_number,segment_number % 100);

    fout.seekg(1001 + ((segment_number) * 101));
    last_segment_data.resize(100);
    fout << (last_segment_data + '\n');
   
    return 0;
}


