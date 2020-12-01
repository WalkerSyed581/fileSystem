# OS Filesystem Project

## Running the program

The project can be run by simply by typing the following commands in the command line:

> mkdir build
> cd build
> cmake 
> make
> ./binary

Once the program is run, it will first display a memory map. Then it will ask you what you want to do, that is either create a file, open a certain file, delete a file or move the contents of a file to another one. The file based operation other than the ones mentioned can be accessed after opening a file. 

Then you will be asked to enter the filename. After it is open the various operations will be available to either write to it, read or truncate, as required.

The following functions have been implemented:

1. Create(fname)
2. Delete(fname)
3. Move(source_fname, target_fname)
4. Open(fName,mode)
5. Close(Fname)
6. fileObj.Write_to_file(text), fileObj.write_to_file(write_at ,text)
7. fileObj.Read_from_file(),fileObj.Read_from_file(start,size)
8. fileObj.Move_within_file(start,size,target)
9. fileObj.Truncate_file(maxSize)
10. Show memory map

## Filesystem Structure
 For now, we have kept the directory structure to be simple and all the files are in the root directory. In this case we have not implemented any directory based structure. The file being created is a simple text file. The first 1000bytes have been specified to store all the metadata. The metadata contains the file name as a key and the coressponding segment numbers in which the content is being stored. 

 The segments in which the file contents are being stored are all of the same size ending with a delimeter namely '\n'. The size in this case is of 100bytes for the file content. 

