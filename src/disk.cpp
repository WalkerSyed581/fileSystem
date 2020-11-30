#include <iostream>
#include <string>
#include <map>
#include <fstream> 
#include <stringstream>
#include <vector>

#include "disk.h"
#include "../include/filesystem/file.h"

using namespace std;


/*
File Delimeters:

Metadata Entry: \n 
File Info Element: &
Segment Separator: ,
FileSystem Starts After: |
FileName: *
FileContent: &
FilesSeparator: |


*/



map<string,vector<int>> Disk::get_file_metadata(){
    return this->file_info;
}

void Disk::set_file_metadata(map<string,vector<int>> data){
    this->file_info = data;
}


int Disk::get_seg_sequence(){
    return this->seg_sequence;
}

void Disk::set_file_metadata(int seg_sequence){
    this->seg_sequence = seg_sequence;
}
 

//Done not tested
Disk::Disk(int meta_data_limit){
    this->meta_data_limit = meta_data_limit;
    map<string,vector<int>> data;
    fstream fin("file_system.txt",ios::in);
    fstream fout;
    string metadata,field,entry,segments;
    


    if (fin.fail()) {
        fout.open("file_system.txt"); 
        this->set_file_metadata(data);
        this->set_seg_sequence(0);
    } else {

        getline(fin,metdata,'|');

        stringstream buffer(metadata);

        while(getline(buffer, field, '\n')){
            getline(field,entry,'&');

            getline(field,segments,'&');

            size_t pos = 0;
            vector<int> segment_array;
            string token;
            string delimiter = ",";
            while ((pos = segments.find(delimiter)) != string::npos) {
                token = segments.substr(0, pos);
                segment_array.push_back(stoi(token));
                segments.erase(0, pos + delimiter.length());
            }
            segment_array.push_back(stoi(segments));



            data.insert(pair<string,vetor<int>>(entry,segment_array));
        }

        this->set_file_metadata(data);
        this->set_seg_sequence(*max_element(segment_array.begin(), segment_array.end()));


    }
}

int Disk::create(File new_file){
    map<string,vector<int>> metadata = this->get_file_metadata;
    int segment_number = this->get_seg_sequence();

 
    fstream fout;
    fout.open("file_system.txt");
    
    string text = new_file.get_data();

    vector<string> splits;

    size_t file_length = text.length();
    if(file_length > 100){
        for (unsigned i = 0; i < file_length; i += 100) {
            if(file_length % 100 != 0 && i > (file_length - 100)){
                splits.push_back(text.substr(i,file_length % 100));
            } else {
                splits.push_back(text.substr(i,100));
            }
        };
    } else {
        splits.push_back(text);
    }

    
    if(segment_number == 0){
        seekg(1000);
    } else {
        seekg(1000 + (segment_number * 101));
    }

    for (auto i = splits.begin(); i != splits.end(); ++i){
        splits[i].resize(100);
        fout << (splits[i] + '\n');
    }

    return 1;
}

Disk::del(string fname){

}

Disk::open(string fname,int mode){

}

Disk::close(string fname){

}

Disk::memMap(){
    
}





