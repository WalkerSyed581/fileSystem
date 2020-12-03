#include <cctype>
#include <cstdio>
#include <map>
#include <string>
#include <string_view>
#include <iostream>
#include <vector>
#include <string>
#include "disk.h"
#include "../include/filesystem/file.h"

using namespace std;



int validate_file_name(string& fname){
    if(fname.find('|') != string::npos || 
       fname.find('&') != string::npos || 
       fname.find(',') != string::npos ||
       fname.find('\n') != string::npos || 
       fname.find('\t') != string::npos){
        return 0;
    } else {
        return 1;
    }
}

int main(int arc,char * argv[]){
    Disk filesystem = Disk(1000);
    string sentinel = "0";
    filesystem.memory_map();

    while(sentinel != "-1"){
        string fname;
        map<string,vector<int>> metadata = filesystem.get_file_metadata();


        cout << "Enter the number written corresponding to the action to move further"<<endl;
        cout << "1\t-> Create File"<<endl;
        cout << "2\t-> Delete File"<<endl;
        cout << "3\t-> Open File"<<endl;
        cout << "4\t-> View Map"<<endl;
        cout << "-1\t-> Quit"<<endl;
        cout << "Enter an action: ";
        cin.clear();
        getline(cin,sentinel,'\n');
        cin.clear();


        if(sentinel == "1"){
            cout << "Enter file name (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
            if(!validate_file_name(fname)){
                cout << "\nError: Invalid File Name"<<endl;
                continue;
            }
            auto file = metadata.find(fname);
            if(file == metadata.end()){
                int result = filesystem.create(fname);
                if(result == -2 || result == -1){
                    cout <<"\nError: Hard Disk is full\n";
                }
            } else {
                cout << "\nError: File name already exists\n";
                continue;
            }
            
        } else if (sentinel == "2"){
            cout << "Enter file name (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
            if(!validate_file_name(fname)){
                cout << "\nError: Invalid File Name"<<endl;
                continue;
            }
            auto file = metadata.find(fname);
            
            if(file != metadata.end()){
                int result = filesystem.del(fname);
                if(result == -1){
                    cout <<"\nError:  Hard Disk is full\n";
                }
            } else {
                cout << "\nError: File name not found\n";
                continue;
            }
        } else if (sentinel == "3"){
            cout << "Enter file name (Invalid Characters : |,& or a comma): ";
            cin.clear();
            fflush(stdin);
            getline(cin,fname,'\n');
            if(!validate_file_name(fname)){
                cout << "\nError: Invalid File Name"<<endl;
                continue;
            }

            auto file_data = metadata.find(fname);
            if(file_data != metadata.end()){
                File file = filesystem.open(fname);
                string nest_sentinel;
                while(nest_sentinel != "-1"){
                    if(!nest_sentinel.empty()){
                        string yn;
                        cout << "Do you want to continue? (Y/N): "<<endl;
                        cin.clear();
                        fflush(stdin);
                        getline(cin,yn,'\n');
                        cin.clear();
                        if(yn[0] == 'N'){
                            cout << "\n\nClosing File..."<<endl;
                            cout << "\n\n";
                            break;
                        }
                    }
                    cout << "\nEnter the number written corresponding to the action to move further"<<endl;
                    cout << "1 -> Write To File"<<endl;
                    cout << "2 -> Write To File at a specific position"<<endl;
                    cout << "3 -> Read File"<<endl;
                    cout << "4 -> Read File from a specific position"<<endl;
                    cout << "5 -> Truncate File"<<endl;
                    cout << "-1 -> Quit"<<endl;
                    cin.clear();
                    cout << "Enter an action: ";
                    fflush(stdin);
                    getline(cin,nest_sentinel,'\n');
                    cin.clear();
                    if(nest_sentinel == "1"){
                        string text;
                        cout << "Enter the text to write into the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,text,'\n');
                        cin.clear();
                        int result = file.write_to_file(filesystem,text);
                        if(result != 0){
                            cout << "\nError: Hard Disk is Full\n";
                            continue;
                        }
                        metadata = filesystem.get_file_metadata();
                    } else if(nest_sentinel == "2"){
                        string text,buffer;
                        int pos;
                        cout << "Enter the position to write the text into the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,buffer,'\n');
                        pos = stoi(buffer);
                        cin.clear();
                        if(pos > file.get_data().length()){
                            cout << "\nError: Invalid value\n";
                            continue;
                        }
                        cout << "Enter the text to write into the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,text,'\n');
                        int result = file.write_to_file(filesystem,pos,text);
                        if(result != 0){
                            cout << "\nError: Hard Disk is Full\n";
                            continue;
                        }
                        metadata = filesystem.get_file_metadata();
                    } else if(nest_sentinel == "3"){
                        string content = file.read_from_file();
                        if(content.empty()){
                            cout << "No data found"<<endl;
                        } else {
                            cout <<"\n\nFile Data: " + content + "\n\n"<< endl;
                        }
                    } else if(nest_sentinel == "4"){
                        int start,size;
                        string buffer;
                        string contents = file.get_data();
                        cout << "Enter the position to start reading data from the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,buffer,'\n');
                        start = stoi(buffer);
                        cin.clear();
                        if(start > contents.length()){
                            cout << "\nError: Invalid value\n";
                            continue;
                        }
                        cout << "Enter the size of data  to be read from the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,buffer,'\n');
                        size = stoi(buffer);
                        cin.clear();
                        if(size + start > contents.length()){
                            cout << "\nError: invalid values\n";
                            continue;
                        }

                        string content = file.read_from_file(start,size);
                        if(content.empty()){
                            cout << "No data found"<<endl;
                        } else {
                            cout << "\n\nThe contents are: "<<content<<"\n\n" << endl;
                        }

                    } else if(nest_sentinel == "5"){
                        string buffer;
                        int max_size;
                        cout << "Enter the max size of the file: ";
                        cin.clear();
                        fflush(stdin);
                        getline(cin,buffer,'\n');
                        max_size = stoi(buffer);
                        cin.clear();
                        if(max_size > file.get_data().length() || max_size == 0){
                            cout << "\nError: Invalid value\n";
                            continue;
                        }
                        file.truncate_file(filesystem,max_size);
                    } else if(nest_sentinel == "-1"){
                        cout << "\n\nClosing File..."<<endl;
                        cout << "\n\n";

                        break;
                    }
                }
            } else {
                cout << "\nError: File name not found\n";
                continue;
            }
        } else if (sentinel == "4"){
            cout << "\n";
            filesystem.memory_map();
        } else if(sentinel == "-1"){
            cout << "\nExiting...\n";
        }
    }
    return 0;
}