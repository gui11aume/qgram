#!/usr/bin/env python
# -*- coding:utf-8 -*-

import sys
from collections import defaultdict

q = 2
e = 2
n = 8
tau = 3

def main(txt):
   # Index the text.
   Index = defaultdict(list)
   for i in range(len(txt)-q):
      Index[txt[i:(i+q)]].append(i)
   Bins = [0] * (len(txt)-q+1)

   # Fill the bins.
   L = n-q+1
   for i in range(L):
      qgramin = txt[i:(i+q)]
      for j in Index[qgramin]:
         for k in range(e+1):
            if j-i-k <= 0: break
            Bins[j-i-k] += 1
   for b in range(len(Bins)):
      if Bins[b] > tau-1: print b,0,Bins[b]

   # Read the text and update the bins.
   for i in range(L,len(txt)-q+1):
      qgramout = txt[(i-L):(i-L+q)]
      for j in Index[qgramout]:
         if j-(i-L) <= 0: continue
         for k in range(e+1):
            if j-(i-L)-k <= 0: break
            Bins[j-(i-L)-k] -= 1
      qgramin = txt[i:(i+q)]
      for j in Index[qgramin]:
         if j-i <= 0: continue
         for k in range(e+1):
            if j-i-k <= 0: break
            Bins[j-i-k] += 1
      for b in range(len(Bins)):
         if Bins[b] > tau-1: print b+i-L,i-L,Bins[b]

if __name__ == '__main__':
   with open(sys.argv[1]) as f:
      main(f.read())

