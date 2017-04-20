# -*- coding: utf-8 -*-
"""
Created on Fri Aug 12 16:21:11 2016

@author: elmon
"""

import sys, os
from scipy.io.wavfile import read, write



if len(sys.argv)!=5:
    print('python '+sys.argv[0]+'<audio_orig> <audio_trimed> <init_time> <end_time>')    
    sys.exit()
else:
    print(sys.argv)
    audio_orig=sys.argv[1]
    audio_trimed=sys.argv[2]
    init_time=sys.argv[3]
    end_time=sys.argv[4]
    fs, x=read(audio_orig)
    print(fs)
    init_times=int(float(init_time)*fs)
    end_times=int(float(end_time)*fs)
    xout=x[init_times:end_times]
    write(audio_trimed, fs, xout)