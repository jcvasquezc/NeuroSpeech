# -*- coding: utf-8 -*-
"""
Created on Tue Apr  5 10:35:26 2016

@author: J. C. Vasquez-Correa
"""

import os, sys
import numpy as np
import matplotlib.pyplot as plt
def articulationVowels(audioA, audioE, audioI, audioO, audioU, path_base):
    size_frameS=0.04
    size_stepS=0.02
    F1a, F2a=formants(audioA, path_base, size_frameS, size_stepS)
    F1e, F2e=formants(audioE, path_base, size_frameS, size_stepS)
    F1i, F2i=formants(audioI, path_base, size_frameS, size_stepS)
    F1o, F2o=formants(audioO, path_base, size_frameS, size_stepS)
    F1u, F2u=formants(audioU, path_base, size_frameS, size_stepS)
    mF1a=F1a.mean(0)
    mF1e=F1e.mean(0)
    mF1i=F1i.mean(0)
    mF1o=F1o.mean(0)
    mF1u=F1u.mean(0)
    mF2a=F2a.mean(0)
    mF2e=F2e.mean(0)
    mF2i=F2i.mean(0)
    mF2o=F2o.mean(0)
    mF2u=F2u.mean(0)
    VSA=(mF1i*np.abs(mF2a-mF2u)+mF1a*np.abs(mF2u-mF2i)+mF1u*np.abs(mF2i-mF2a))/2
    VAI=(mF2i+mF1a)/(mF2u+mF2a+mF1i+mF1u)
    ps1=mF1a*mF2o-mF1o*mF2a
    ps2=mF1o*mF2u-mF1u*mF2o
    ps3=mF1u*mF2i-mF1i*mF2u
    ps4=mF1i*mF2e-mF1e*mF2i
    ps5=mF1e*mF2a-mF1a*mF2e
    S=np.abs(ps1+ps2+ps3+ps4+ps5)/2.0
    VPA=np.sqrt(S*(S-ps1)*(S-ps2)*(S-ps3)*(S-ps4)*(S-ps5))
    
    fig = plt.figure(figsize=(12,5))
    plt.subplot(121)    
    plt.scatter(F1a, F2a, c='b', alpha=0.5, marker='o', s=100, label='/a/')
    plt.scatter(F1i, F2i, c='r', alpha=0.5, marker='o', s=100, label='/i/')
    plt.scatter(F1u, F2u, c='k', alpha=0.5, marker='o', s=100, label='/u/')
    plt.plot([mF1a, mF1i, mF1u, mF1a], [mF2a, mF2i, mF2u, mF2a], linewidth=3, c='g', label='Patient')

    plt.plot([775, 373, 350, 775], [1335, 2325, 500, 1335], linewidth=3, c='k', label='Reference')
    plt.fill([775, 373, 350, 775], [1335, 2325, 500, 1335], linewidth=3, color='#000000', alpha=0.5)
    plt.fill([mF1a, mF1i, mF1u, mF1a], [mF2a, mF2i, mF2u, mF2a], linewidth=3, color='#00FF00', alpha=0.5)

    plt.xlim([100, 900])
    plt.ylim([200,3000])
    plt.xlabel('Formant 1 (Hz)')
    plt.ylabel('Formant 2 (Hz)')    
    #plt.legend()
    #plt.show()
    plt.subplot(122)
    plt.scatter(F1a, F2a, c='b', alpha=0.5, marker='o', s=100, label='/a/')
    plt.scatter(F1e, F2e, c='c', alpha=0.5, marker='o', s=100, label='/e/')
    plt.scatter(F1i, F2i, c='r', alpha=0.5, marker='o', s=100, label='/i/')
    plt.scatter(F1o, F2o, c='g', alpha=0.5, marker='o', s=100, label='/o/')
    plt.scatter(F1u, F2u, c='k', alpha=0.5, marker='o', s=100, label='/u/')
    plt.plot([mF1a, mF1e, mF1i, mF1u, mF1o, mF1a], [mF2a, mF2e, mF2i, mF2u, mF2o, mF2a], linewidth=3, c='g', label='Patient')
    plt.plot([775, 490, 373, 350, 523, 775], [1335, 2074, 2325, 500, 1020, 1335], linewidth=3, c='k', label='Reference')
    plt.fill([775, 490, 373, 350, 523, 775], [1335, 2074, 2325, 500, 1020, 1335], linewidth=3, alpha=0.5, color='#000000')
    plt.fill([mF1a, mF1e, mF1i, mF1u, mF1o, mF1a], [mF2a, mF2e, mF2i, mF2u, mF2o, mF2a], linewidth=3, color='#00FF00', alpha=0.5)

    plt.legend(bbox_to_anchor=(1.3, 1))
    plt.xlim([100, 900])
    plt.ylim([200,3000])
    plt.yticks([])
    plt.xlabel('Formant 1 (Hz)')
    #plt.show()    
    plt.savefig(path_base+'articulation1.png')
    plt.savefig(path_base+'articulation1.pdf')
    return F1a, F1e, F1i, F1o, F1u, F2a, F2e, F2i, F2o, F2u, VSA, VAI, VPA

    
def formants(audio, path_base, size_frame, time_shift):
    os.system(path_base+'../../Toolkits/praat '+ path_base+'FormantsPraat.praat '+audio+' '+path_base+'tempF12.txt 5 5500 '+' '+str((size_frame))+' '+str((time_shift))) #formant extraction praat
    [F1, F2]=decodeFormants(path_base+'tempF12.txt')
    return F1, F2    
    
    
def decodeFormants(fileTxt):
    fid=open(fileTxt)
    datam=fid.read()
    end_line1=multi_find(datam, '\n')
    F1=[]
    F2=[]
    ji=10
    while (ji<len(end_line1)-1):
        line1=datam[end_line1[ji]+1:end_line1[ji+1]]
        cond=(line1=='3' or line1=='4' or line1=='5')
        if (cond): 
            F1.append(float(datam[end_line1[ji+1]+1:end_line1[ji+2]]))
            F2.append(float(datam[end_line1[ji+3]+1:end_line1[ji+4]]))
        ji=ji+1
    F1=np.asarray(F1)
    F2=np.asarray(F2)
    return F1, F2    


def multi_find(s, r):
    s_len = len(s)
    r_len = len(r)
    _complete = []
    if s_len < r_len:
        n = -1
    else:
        for i in range(s_len):
            # search for r in s until not enough characters are left
            if s[i:i + r_len] == r:
                _complete.append(i)
            else:
                i = i + 1
    return(_complete)

print (sys.argv)
if len(sys.argv)<2:
    print ('python '+sys.argv[0]+' <file_audioA> <file_audioI> <file_audioU> <resultsA.txt> <resultsI.txt> <resultsU.txt> <resultsF.txt> <path_base>')
    sys.exit()
audioA=sys.argv[1]
audioE=sys.argv[2]
audioI=sys.argv[3]
audioO=sys.argv[4]
audioU=sys.argv[5]
FAres=sys.argv[6]
FEres=sys.argv[7]
FIres=sys.argv[8]
FOres=sys.argv[9]
FUres=sys.argv[10]
file_features=sys.argv[11]
path_base=sys.argv[12]

F1a, F1e, F1i, F1o, F1u, F2a, F2e, F2i, F2o, F2u, VSA, VAI, VPA=articulationVowels(audioA, audioE, audioI, audioO, audioU, path_base)

np.savetxt(FAres.replace('.txt', '1.txt'),F1a)
np.savetxt(FAres.replace('.txt', '2.txt'),F2a)

np.savetxt(FEres.replace('.txt', '1.txt'),F1e)
np.savetxt(FEres.replace('.txt', '2.txt'),F2e)

np.savetxt(FIres.replace('.txt', '1.txt'),F1i)
np.savetxt(FIres.replace('.txt', '2.txt'),F2i)

np.savetxt(FOres.replace('.txt', '1.txt'),F1o)
np.savetxt(FOres.replace('.txt', '2.txt'),F2o)

np.savetxt(FUres.replace('.txt', '1.txt'),F1u)
np.savetxt(FUres.replace('.txt', '2.txt'),F2u)
feat=[VSA, 1.0/VAI, VPA, np.mean(F1a), np.mean(F2a), np.mean(F1e), np.mean(F2e), np.mean(F1i), np.mean(F2i), np.mean(F1o), np.mean(F2o), np.mean(F1u), np.mean(F2u)]
np.savetxt(file_features, feat, fmt="%.2f")