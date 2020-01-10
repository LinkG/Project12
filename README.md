# Project12
c++ neural network without 3rd party libraries

PREREQUISITE: gcc/g++ 9. It wont work with newer versions as they have removed freedom of memory management in a specific case which is used.

Make sure to build ```Network_Fast.cpp``` you will need to include the rest of the files while building:

```
 g++ FastTest.cpp FastSave.cpp Network_Fast.cpp MNISTRead.cpp NetworkPref.cpp -o <output-file-name>.o
 
 ##Example
 g++ FastTest.cpp FastSave.cpp Network_Fast.cpp MNISTRead.cpp NetworkPref.cpp -o COMPILE_PLS.o
```


While running, you must pass an argument to a config file:
```
./<built-executable>.o <conf-file name>
```

An example config is part of the repo, but in general here is the layout:
```
<save-file name>,<number of layers>,<size of layers seperated by ','>,<character set>

##Example:
save.dat,3,120,80,10,0123456789
```
