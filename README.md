# Project12
c++ neural network without 3rd party libraries

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
