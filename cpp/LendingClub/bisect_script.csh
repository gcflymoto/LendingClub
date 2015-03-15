#!/bin/csh -f
cd cpp/LendingClub
make opt || exit 125                     
./lcbt --iterations=1

