LendingClub
===========

Lending Club Data Analysis and Algorithms

lcbt.py is a Genetic Algorithm which analyses the Lending Club data and searches for the best criteria and lending club filters to reduce default rates and maximize (NAR) Net Annual Return

The inspiration comes from David M. Patierno PHP implementation

[http://blog.dmpatierno.com/post/3161338411/lending-club-genetic-algorithm](http://blog.dmpatierno.com/post/3161338411/lending-club-genetic-algorithm)

Lending Club Data can be found here and is automatically downloaded if it cannot be found [https://www.lendingclub.com/info/download-data.action](https://www.lendingclub.com/info/download-data.action)

Lending Club Data field descriptions are here [http://www.lendingclub.com/kb/index.php?View=entry&EntryID=253](http://www.lendingclub.com/kb/index.php?View=entry&EntryID=253)

Here are the usage models

```
C:\pypy-2.0.2\pypy.exe lcbt.py

C:\Python27\python.exe -O lcbt.py

C:\Python32\python.exe -O lcbt.py
```
Best performance with linux + pypy + zmqpy

### Pypy2.0 beta version on linux with 8-threads
[iteration 190/4096 6.16 sec/iter] 1024 loans (48/mo.) test at 15.63% APY. 45 loans defaulted (4.00%, $21.30 avg loss) 11.9808% net APY

### Python27 version on linux with 8-threads
[iteration 53/4096 49.54 sec/iter] 1013 loans (50/mo.) test at 15.51% APY. 40 loans defaulted (3.00%, $21.15 avg loss) 12.2170% net APY

Some of the performance figures on my laptop

##New version using bit vectors##
### Pypy2 ~213-317MB of Memory### 
[iteration 1955/4096 24.13 sec/iter] 1010 loans (48/mo.) test at 15.90% APY. 43 loans defaulted (4.00%, $22.24 avg loss) 12.1523% net APY

### Python27 ~410MB of Memory###
[iteration 4/4096 114.57 sec/iter] 2203 loans (87/mo.) test at 12.58% APY. 119 loans defaulted (5.00%, $21.49 avg loss) 8.0941% net APY

##First version 
### Pypy2 ~735MB of Memory ###
[iteration 76/4096 43.89 sec/iter] 1037 loans test at 19.54% APY. 2 loans defaulted (0.00%, $22.67 avg loss) 20.0333% net APY

### Python27 ###
[iteration 1/4096 176 secs/iter] 1356 loans test at 15.56% APY.3 loans defaulted (0.00%, $20.62 avg loss) 15.3609% net APY

### Python32 ###
[iteration 3/4096 186.94 sec/iter] 1377 loans test at 17.07% APY. 2 loans defaulted (0.15%, $23.00 avg loss) 17.0613% net APY

### Future improvements  ###
- Verify based on www.lendstats.com
- Python Lending Club API http://python-lendingclub.readthedocs.org/en/latest/

Setting up Pypy
===============
```
$ curl -O http://python-distribute.org/distribute_setup.py
$ curl -O https://raw.githubusercontent.com/pypa/pip/master/contrib/get-pip.py
$ ./pypy-2.1/bin/pypy distribute_setup.py
$ ./pypy-2.1/bin/pypy get-pip.py
$ ./pypy-2.1/bin/pip install pygments
$ ./pypy-2.1/bin/pip install zmqpy
$ ./pypy-2.1/bin/pip install pyzmq

C:\\Users\gczajkow\boost\boost_1_53_0>.\b2 toolset=msvc-12.0 install --prefix=C:\\Users\gczajkow\boost\boost_1_53_0.release
```