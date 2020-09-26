#! /usr/bin/python3 
import sys 
import re
a = sys.argv[1]

list_num = []
def outpint(x):
    n=0
    while x!=0 :
        print(x%2,end='')
        list_num.append(x%2)
        n+=1
        x//=2
    while n<4:
        print("0",end='')
        list_num.append(0)
        n+=1
    print(" ",end='')

a = a.replace('0x','')
l = len(a)
for i in range(l):
    outpint(int(a[i],16))
    if i%4== 3:
        print("")




