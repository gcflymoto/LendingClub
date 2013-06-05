LendingClub
===========

Lending Club Data Analysis and Algorithms

lcbt.py is a Genetic Algorithm which analyzes the Lending Club data and searches for the best criteria and lending club filters to reduce default rates and maximize (NAR) Net Annual Return

The inspiration comes from David M. Patierno PHP implementation

[http://blog.dmpatierno.com/post/3161338411/lending-club-genetic-algorithm](http://blog.dmpatierno.com/post/3161338411/lending-club-genetic-algorithm)

Lending Club Data can be found here [https://www.lendingclub.com/info/download-data.action](https://www.lendingclub.com/info/download-data.action)

Lending Club Data field descriptions are here [http://www.lendingclub.com/kb/index.php?View=entry&EntryID=253](http://www.lendingclub.com/kb/index.php?View=entry&EntryID=253)

Here are the usage models

```
C:\pypy-2.0.2\pypy.exe lcbt.py

C:\Python27\python.exe -O lcbt.py

C:\Python32\python.exe -O lcbt.py
```

Some of the performance figures on my laptop

##New version using bit vectors##
### Pypy2 ~317MB of Memory### 
[iteration 25/4096 23.77 sec/iter] 1066 loans (57/mo.) test at 15.89% APY. 50 loans defaulted (4.00%, $21.96 avg loss) 11.7945% net APY

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
- Parallelize the function which calculates fitness across N-cores
- Verify based on www.lendstats.com
