# -*- coding: utf-8 -*-
"""
Created on Tue Apr  5 10:25:24 2016

@author: J. C. Vasquez-Correa
"""


from scipy.io.wavfile import read
import os
import sys
import numpy as np
import matplotlib.pyplot as plt
from radarPros import plot_radar



def prosody(audio, path_base):

    fs, data_audio=read(audio)
    data_audio=data_audio-np.mean(data_audio)
    data_audio=data_audio/float(np.max(np.abs(data_audio)))
    size_frameS=0.02*float(fs)
    size_stepS=0.01*float(fs)
    thr_len_pause=0.14*float(fs)
    thr_en_pause=0.2
    overlap=size_stepS/size_frameS
    nF=int((len(data_audio)/size_frameS/overlap))-1
    os.system(path_base+'../../Toolkits/praat '+path_base+'F0_Praat.praat '+audio+' '+path_base+'tempF0.txt 75 500 0.02') #f0 extraction Praat
    F0=decodeF0(path_base+'tempF0.txt')
    logE=[]    
    for l in range(nF):
        data_frame=data_audio[int(l*size_stepS):int(l*size_stepS+size_frameS)]
        logE.append(10.0**logEnergy(data_frame))
    logE=np.asarray(logE)

    time_stepF0=0 # 0 #value by default (0-->0.75/low_f0) Other values are in miliseconds
    low_f0=75 #75
    high_f0=600 #600
    maxPeriodVUV=0.02 #20 #milisecionds
    averagePeriodvuv=0.01#10 #milisecionds
    VUV(audio, path_base+'vuv.txt', path_base, time_stepF0, low_f0, high_f0, maxPeriodVUV, averagePeriodvuv)
    segmentsV, fs=decode_Texgrid(path_base+'vuv.txt', audio, 'Voiced')
    segmentsU, fs=decode_Texgrid(path_base+'vuv.txt', audio, 'Unvoiced')
    
    Nvoiced=len(segmentsV)
    Nunvoiced=len(segmentsU)
        
    
    Vrate=fs*float(Nvoiced)/len(data_audio)
    
    avgdurv=1000*np.mean([len(segmentsV[k]) for k in range(Nvoiced)])/float(fs)
    stddurv=1000*np.std([len(segmentsV[k]) for k in range(Nvoiced)])/float(fs)
    
    silence=[]
    for k in range(Nunvoiced):
        eu=logEnergy(segmentsU[k])
        if (eu<thr_en_pause or len(segmentsU[k])>thr_len_pause):
            silence.append(segmentsU[k])

    Silrate=fs*float(len(silence))/len(data_audio)
    
    avgdurs=1000*np.mean([len(silence[k]) for k in range(len(silence))])/float(fs)
    stddurs=1000*np.std([len(silence[k]) for k in range(len(silence))])/float(fs)

    #plt.plot(logE)
    #plt.show()

    plt.figure(1)
    plt.subplot(311)
    t=np.arange(0, float(len(data_audio))/fs, 1.0/fs)
    if len(t)!=len(data_audio):
        t=np.arange(1.0/fs, float(len(data_audio))/fs, 1.0/fs)
    plt.plot(t, data_audio, 'k')
    plt.ylabel('Amplitude')
    plt.xlabel('Time (s)')
    plt.xlim([0, t[-1]])
    plt.grid(True)
    plt.subplot(312)
    fsp=len(F0)/t[-1]
    t2=np.arange(0.0, t[-1], 1.0/fsp)
    if len(t2)>len(F0):
        t2=t2[:len(F0)]
    elif len(F0)>len(t2):
        F0=F0[:len(t2)]
    plt.plot(t2, F0, color='k', linewidth=2.0)
    plt.xlabel('Time (s)')
    plt.ylabel('F0 (Hz)')
    plt.ylim([0,np.max(F0)+10])
    plt.xlim([0, t[-1]])   
    plt.grid(True)
    plt.subplot(313)
    fse=len(logE)/t[-1]
    t3=np.arange(0.0, t[-1], 1.0/fse)
    if len(t3)>len(logE):
        t3=t3[:len(logE)]
    elif len(logE)>len(t3):
        logE=logE[:len(t3)]
    plt.plot(t3, logE, color='k', linewidth=2.0)
    plt.xlabel('Time (s)')
    plt.ylabel('Energy')
    plt.ylim([0,np.max(logE)+5])
    plt.xlim([0, t[-1]])    
    plt.grid(True)
    #plt.show()
    plt.savefig(path_base+'prosody.png')
    plt.savefig(path_base+'prosody.pdf')
    
    F0std=np.std(F0[F0!=0])
    F0varsemi=Hz2semitones(F0std**2)    
    dataradar=np.asarray([F0varsemi, np.std(F0[F0!=0]), np.mean(logE), np.std(logE), np.max(logE), Vrate, avgdurv, stddurv, Silrate, avgdurs, stddurs])

    refl=np.asarray([74.5, 30.0, 10**(0.1*1.9), 10**(0.1*5.4), 10**(0.1*16.3), 1.3, 271.5, 200.1, 0.4, 360.5, 63.7])
    refh=np.asarray([91.7, 47.0, 10**(0.1*6.8), 10**(0.1*9.3), 10**(0.1*19.2), 2.1, 463.5, 377.7, 0.7, 834.9, 721.9])
    names=['varF0_semi', 'stdF0', 'avgE', 'stdE', 'maxE', 'Vrate', 'avgdurV', 'stddurV', 'Silrate', 'avgdurSil', 'stddurSil']
    plot_radar(dataradar, refh, refl, names, 'Prosody', path_base+'prosodyradarfemale.png')    
    
    
    refl=np.asarray([58.5, 19.0, 10**(0.1*2.3), 10**(0.1*5.6), 10**(0.1*15.8), 1.1, 241., 169.9, 0.42, 387.7,  236.3])
    refh=np.asarray([80.6, 35.6, 10**(0.1*7.0), 10**(0.1*9.4), 10**(0.1*19.2), 2.0, 488., 443.0, 0.80, 1098.4, 791.7])
    names=['varF0_semi', 'stdF0', 'avgE', 'stdE', 'maxE', 'Vrate', 'avgdurV', 'stddurV', 'Silrate', 'avgdurSil', 'stddurSil']
    plot_radar(dataradar, refh, refl, names, 'Prosody', path_base+'prosodyradarmale.png')    
    
    
    return F0, logE, np.mean(F0[F0!=0]), np.std(F0[F0!=0]), np.max(F0), 10*np.log10(np.mean(logE)), 10*np.log10(np.std(logE)), 10*np.log10(np.max(logE)), Vrate, avgdurv, stddurv, Silrate, avgdurs, stddurs, F0varsemi

def Hz2semitones(freq):
    A4=440.
    C0=A4*2**(-4.75)
    if freq>0:
        h=12*np.log2(freq/C0)
        octave=h//12.
        return h+octave
    else:
        return C0


def VUV(audio, results, path_base, time_stepF0, lowf0, highf0, maxVUVPeriod, averageVUVPeriod):
    #print 'praat '+ path_base+'vuv.praat '+audio+' '+ results+' '+str(lowf0)+' '+str(highf0)+' '+str(time_stepF0)+' '+str(maxVUVPeriod)+' '+str(averageVUVPeriod)
    os.system(path_base+'../../Toolkits/praat '+ path_base+'vuv.praat '+audio+' '+ results+' '+str(lowf0)+' '+str(highf0)+' '+str(time_stepF0)+' '+str(maxVUVPeriod)+' '+str(averageVUVPeriod))


def decode_Texgrid(file_textgrid, file_audio, type_segm, win_trans=0.04):
    fid=open(file_textgrid)
    data=fid.read()
    fs, data_audio=read(file_audio)
    if (type_segm=='Unvoiced' or type_segm=='Onset'):   
        pos=multi_find(data, '"U"')
    elif (type_segm=='Voiced' or type_segm=='Offset'):
        pos=multi_find(data, '"V"')
    segments=[]
    for j in range(len(pos)):
        pos2=multi_find(data[0:pos[j]], '\n')
        nP=len(pos2)
        inicio=data[pos2[nP-3]:pos2[nP-2]]
        fin=data[pos2[nP-2]:pos2[nP-1]]
        inicioVal=int(float(inicio)*fs)
        finVal=int(float(fin)*fs)-1
        if (type_segm=='Onset' or type_segm=='Offset'):
            segments.append(data_audio[int(finVal-win_trans*fs):int(finVal+win_trans*fs)])
        else:
            segments.append(data_audio[inicioVal:finVal])
    return segments, fs

    

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

def decodeF0(fileTxt):
    fid=open(fileTxt)
    datam=fid.read()
    end_line1=multi_find(datam, '\n')
    F0=[]
    ji=10
    while (ji<len(end_line1)-1):
        line1=datam[end_line1[ji]+1:end_line1[ji+1]]
        cond=(line1=='2' or line1=='3' or line1=='4' or line1=='5' or line1=='6' or line1=='7' or line1=='8' or line1=='9' or line1=='10' or line1=='11' or line1=='12' or line1=='13' or line1=='14' or line1=='15')
        if (cond): F0.append(float(datam[end_line1[ji+1]+1:end_line1[ji+2]]))
        ji=ji+1
    F0=np.asarray(F0)
    return F0
    
    
def logEnergy(sig):
    sig2=np.power(sig,2)
    sumsig2=np.sum(sig2)
    logE=np.log10(sumsig2)
    return logE


def Power(sig):
    sig2=np.square(sig)
    sumsig2=np.sum(sig2)/len(sig)
    return sumsig2



#print sys.argv
if len(sys.argv)<5:
    print('python '+sys.argv[0]+' <file_audio> <fileresultsf0.txt> <fileresultsEn.txt> <file_features.txt> <path_base>')
    sys.exit()
audios=sys.argv[1]
fileresultsf0=sys.argv[2]
fileresultsEn=sys.argv[3]

file_features=sys.argv[4]
path_base=sys.argv[5]

hf=os.listdir(audios)
hf.sort()
Features=[]
for j in range(len(hf)):
    audio=audios+hf[j]
    print(audio)
    if audio.find('.wav')>0:
        F0, logE, mF0, sF0, mmF0, mlogE, slogE, mmlogE, Vrate, avgdurv, stddurv, Silrate, avgdurs, stddurs, F0varsemi=prosody(audio, path_base)
        
        feat=[mF0, sF0, mmF0, mlogE, slogE, mmlogE, Vrate, avgdurv, stddurv, Silrate, avgdurs, stddurs, F0varsemi]
        Features.append(feat)
np.savetxt(fileresultsf0, F0)
np.savetxt(fileresultsEn, logE)

np.savetxt(file_features, Features, fmt="%.2f")
