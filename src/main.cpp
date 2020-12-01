#include <map>
#include <vector>
#include <string>
#include "disk.h"
#include "../include/filesystem/file.h"

using namespace std;

int main(int arc,char * argv[]){
    Disk filesystem = Disk(1000);
    int sentinel = 0;
    filesystem.memory_map();

    while(sentinel >= 0){
        
    }
}