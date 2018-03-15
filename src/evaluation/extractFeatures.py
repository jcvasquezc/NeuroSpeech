# -*- coding: utf-8 -*-
"""
Created on Fri Apr 29 14:06:37 2016

@author: J. C. Vasquez-Correa
"""
#path_base='./'

import sys, os

from scipy.io.wavfile import read
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats
import math


def normalize(sig):
    sig=sig-np.mean(sig)
    m=float(np.abs(np.max(sig)))
    return sig/m

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
            F1.append(float(datam[int(end_line1[ji+1]+1):int(end_line1[ji+2])]))
            F2.append(float(datam[int(end_line1[ji+3]+1):int(end_line1[ji+4])]))
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




    
def VUV(audio, results, path_base, time_stepF0, lowf0, highf0, maxVUVPeriod, averageVUVPeriod):
    #print 'praat '+ path_base+'vuv.praat '+audio+' '+ results+' '+str(lowf0)+' '+str(highf0)+' '+str(time_stepF0)+' '+str(maxVUVPeriod)+' '+str(averageVUVPeriod)
    os.system(path_base+'../../Toolkits/praat '+ path_base+'vuv_praat.praat '+audio+' '+ results+' '+str(lowf0)+' '+str(highf0)+' '+str(time_stepF0)+' '+str(maxVUVPeriod)+' '+str(averageVUVPeriod))
    

def bark(f):
	x=(f*0.00076)
	x2=(f/7500)**2
	b=[]
	for i in range (0,len(f)):
		b.append(13*( math.atan(x[i]) )+3.5*( math.atan(x2[i]))) #Bark scale values
	return (b)

def barke(x,Fs, nfft=2048, nB=25):
		"""
		e: Energy in frequency bands according to the Bark scale
		x: signal 
		Fs: sampling frequency
            nfft: number of points for the Fourier transform
            nB: number of bands for energy computation
		"""
		y = fftsolp(x,nfft)
		f = (Fs/2)*(np.linspace(0,1,nfft/2+1))
		barkScale = bark(f)
		barkIndices = []
		for i in range (len(barkScale)):
			barkIndices.append(int(barkScale[i]))

		barkIndices = np.asarray(barkIndices)
		
		barkEnergy=[]
		for i in range (nB):
			brk = np.nonzero(barkIndices==i)
			brk = np.asarray(brk)[0]
			sizeb=len(brk)
			if (sizeb>0): 
				barkEnergy.append(sum(np.abs(y[brk]))/sizeb)
			else:
				barkEnergy.append(0)  

		e = np.asarray(barkEnergy)
		e = np.log(e)
		return e


def fftsolp(x,nfft):
     """
     STFT for compute the energy in Bark scale
     x: signal
     nffft: number of points of the Fourier transform
     """
     window = np.hamming(len(x)/4)
     noverlap = np.ceil(len(window)/2)
    
     nx = len(x)
     nwind = len(window)
    
     ncol = np.fix((nx-noverlap)/(nwind-noverlap))
     ncol = int(ncol)
     colindex = (np.arange(0,ncol))*(nwind-noverlap) 
     colindex = colindex.astype(int) 
    
     rowindex = np.arange(0,nwind) 
     rowindex = rowindex.astype(int) 
     rowindex = rowindex[np.newaxis] 
     rowindex = rowindex.T 
    
     y = np.zeros((nwind,ncol),dtype=np.int)
     d = np.ones((nwind,ncol),dtype=np.int)
    
    
     y = x[d*(rowindex+colindex)]
     window = window.astype(float) 
     window = window[np.newaxis]  
     window = window.T
     new = window*d
     y = new*y
     y = y[:,0] 
     
     y = np.fft.fft(y,nfft)
     y = (y[0:int(nfft/2+1)])
     return y

    
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

def extractTrans(segments, fs, size_frameS, size_stepS, nB, nfft=2048):
    frames=[]
    overlap=size_stepS/size_frameS
    for j in range(len(segments)):
        if (len(segments[j])>size_frameS):
            nF=int((len(segments[j])/size_frameS)/overlap)-1
            for iF in range(nF):
                frames.append(np.hamming(size_frameS)*segments[j][int(iF*size_stepS):int(iF*size_stepS+size_frameS)])
                    
    BarkEn=np.zeros((len(frames),nB)) 
    for j in range(len(frames)):    
        BarkEn[j,:]=(barke(frames[j],fs, nfft, nB))


    return BarkEn




def extractFeatures(file_audio, path_base):
    
    sys.path.insert(0, path_base)
    from voiceAnalysis import mfcc
    sizeframe=40

    Nmfcc=12
    fs, data_audio=read(file_audio)
    data_audio=normalize(data_audio)
    os.system(path_base+'../../Toolkits/praat '+ path_base+'FormantsPraat.praat '+file_audio+' '+ path_base+'tempForm.txt 5 5500 '+str(float(sizeframe)/1000)+' '+str(float(sizeframe)/2000)) #formant extraction praat
    [F1, F2]=decodeFormants(path_base+'tempForm.txt')
    DF1=np.diff(F1, n=1)
    DF2=np.diff(F2, n=1)
    DDF1=np.diff(F1, n=2)
    DDF2=np.diff(F2, n=2)
    size_frameS=sizeframe*fs/1000
    size_stepS=size_frameS/2.0
    nF=int((len(data_audio)/size_frameS)/0.5)-1
    MFCCcoef=np.zeros((nF,Nmfcc)) 

    for l in range(nF):
        data_frame=data_audio[int(l*size_stepS):int(l*size_stepS+size_frameS)]
        data_frame=data_frame*np.hamming(len(data_frame))
        MFCCcoef[l,:]=mfcc(data_frame,fs,float(sizeframe)/1000,float(sizeframe)/2000,Nmfcc)

    DMFCCcoef=np.asarray([np.diff(MFCCcoef[:,nf], n=1) for nf in range(Nmfcc)])
    DDMFCCcoef=np.asarray([np.diff(MFCCcoef[:,nf], n=2) for nf in range(Nmfcc)])

    nB=22
    overlap=size_stepS/size_frameS
    time_stepF0=0 # 0 #value by default (0-->0.75/low_f0) Other values are in miliseconds
    low_f0=75 #75
    high_f0=600 #600
    maxPeriodVUV=0.02 #20 #milisecionds
    averagePeriodvuv=0.01#10 #milisecionds
    VUV(file_audio, path_base+'vuv.txt', path_base, time_stepF0, low_f0, high_f0, maxPeriodVUV, averagePeriodvuv)
    segmentsOn, fs=decode_Texgrid(path_base+'vuv.txt', file_audio, 'Onset')
    segmentsOff, fs=decode_Texgrid(path_base+'vuv.txt', file_audio, 'Offset')
    BBE_on=extractTrans(segmentsOn, fs, size_frameS, size_stepS, nB)
    BBE_off=extractTrans(segmentsOff, fs, size_frameS, size_stepS, nB)

    featmean=[]
    featstd=[]
    featsk=[]
    featku=[]
        

    featmean.append(DDF1.mean(0))
    featstd.append(DDF1.std(0,ddof=1))
    featsk.append(stats.skew(DDF1))
    featku.append(stats.kurtosis(DDF1,fisher=False))

    featmean.append(DDF2.mean(0))
    featstd.append(DDF2.std(0,ddof=1))
    featsk.append(stats.skew(DDF2))
    featku.append(stats.kurtosis(DDF2,fisher=False))
        
        
    featmean.append(DDMFCCcoef.mean(1))
    featstd.append(DDMFCCcoef.std(1,ddof=1))
    featsk.append(stats.skew(DDMFCCcoef.T))
    featku.append(stats.kurtosis(DDMFCCcoef.T,fisher=False))        
        
    featmean.append(DF1.mean(0))
    featstd.append(DF1.std(0,ddof=1))
    featsk.append(stats.skew(DF1))
        
    featku.append(stats.kurtosis(DF1,fisher=False))

    featmean.append(DF2.mean(0))
    featstd.append(DF2.std(0,ddof=1))
    featsk.append(stats.skew(DF2))
    featku.append(stats.kurtosis(DF2,fisher=False))
        
    featmean.append(DMFCCcoef.mean(1))
    featstd.append(DMFCCcoef.std(1,ddof=1))
    featsk.append(stats.skew(DMFCCcoef.T))
    featku.append(stats.kurtosis(DMFCCcoef.T,fisher=False))
        
    featmean.append(F1.mean(0))
    featstd.append(F1.std(0,ddof=1))
    featsk.append(stats.skew(F1))
    featku.append(stats.kurtosis(F1,fisher=False))

    featmean.append(F2.mean(0))
    featstd.append(F2.std(0,ddof=1))
    featsk.append(stats.skew(F2))
    featku.append(stats.kurtosis(F2,fisher=False))

    featmean.append(MFCCcoef.mean(0))
    featstd.append(MFCCcoef.std(0,ddof=1))
    featsk.append(stats.skew(MFCCcoef))
    featku.append(stats.kurtosis(MFCCcoef,fisher=False))

    featmean.append(BBE_on.mean(0))
    featstd.append(BBE_on.std(0,ddof=1))
    featsk.append(stats.skew(BBE_on))
    featku.append(stats.kurtosis(BBE_on,fisher=False))

    featmean.append(BBE_off.mean(0))
    featstd.append(BBE_off.std(0,ddof=1))
    featsk.append(stats.skew(BBE_off))
    featku.append(stats.kurtosis(BBE_off,fisher=False))

    featmean=np.hstack(featmean)
    featstd=np.hstack(featstd)
    featsk=np.hstack(featsk)
    featku=np.hstack(featku)
    #print featmean.shape, featstd.shape, featsk.shape, featku.shape
    Featvec=np.hstack((featmean, featstd, featsk, featku))
    return Featvec
