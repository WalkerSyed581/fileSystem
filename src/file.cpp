#include <iostream>
#include <string>
#include "../include/filesystem/file.h"


string File::get_data(){
    return this->data;
}

File::File(string data,string name){
    this->data = data;
    this->name = name;
}

File::write_to_file(string text){
    
}

// File::write_to_file(int write_at,string text){

// }

// File::Read_from_file(){

// }

// File::Read_from_file(int start,int size){

// }

