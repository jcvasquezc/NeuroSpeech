# -*- coding: utf-8 -*-
"""
Created on Wed Apr  6 09:19:49 2016

@author: J. C. Vasquez-Correa
"""

from scipy.io.wavfile import write, read
import matplotlib.pyplot as plt
import os, sys
import numpy as np
import math 
from radarArt import plot_radar

def artCont(audio, path_base):
    print(audio)
    fs, data_audio=read(audio)
    size_frameS=0.04*float(fs)
    size_stepS=0.02*float(fs)
    nB=25
    overlap=size_stepS/size_frameS
    time_stepF0=0 # 0 #value by default (0-->0.75/low_f0) Other values are in miliseconds
    low_f0=75 #75
    high_f0=600 #600
    maxPeriodVUV=0.02 #20 #milisecionds
    averagePeriodvuv=0.01#10 #milisecionds
    VUV(audio, path_base+'vuv.txt', path_base, time_stepF0, low_f0, high_f0, maxPeriodVUV, averagePeriodvuv)
    segmentsOn, fs=decode_Texgrid(path_base+'vuv.txt', audio, 'Onset')
    segmentsOff, fs=decode_Texgrid(path_base+'vuv.txt', audio, 'Offset')
    meOn, stOn=extractTrans(segmentsOn, fs, size_frameS, size_stepS, nB)
    meOff, stOff=extractTrans(segmentsOff, fs, size_frameS, size_stepS, nB)

    dataradar=np.hstack((meOn[0:22], meOff[0:22]))

    BBEonmeref = np.asarray([7.16, 7.92, 8, 7.54, 7.17, 6.86, 6.48, 6.09, 5.75, 5.54, 5.47, 5.48, 5.53, 5.44, 5.34, 5.14, 4.85, 4.78, 4.5, 4.32, 4.35, 3.21])
    BBEonstref =np.asarray([0.77, 1.02, 1.22, 1.38, 1.54, 1.66, 1.65, 1.61, 1.59, 1.57, 1.57, 1.64, 1.72, 1.71, 1.67, 1.62, 1.47, 1.45, 1.34, 1.3, 1.31, 1.3] ) 
    BBEoffmeref = np.asarray([7.46, 8.59, 8.71, 8.18, 7.78, 7.33, 6.85, 6.39, 5.94, 5.65, 5.54, 5.53, 5.47, 5.32, 5.22, 4.97, 4.66, 4.56, 4.32, 4.17, 4.18, 3.23])
    BBEoffstref= np.asarray([0.71, 0.73, 0.73, 0.85, 1.04, 1.15, 1.19, 1.18, 1.16, 1.14, 1.14, 1.21, 1.26, 1.26, 1.24, 1.22, 1.18, 1.19, 1.15, 1.16, 1.19, 1.09])
    
    
    refh=np.hstack((BBEonmeref, BBEoffmeref))+np.hstack((BBEonstref, BBEoffstref))
    refl=np.hstack((BBEonmeref, BBEoffmeref))-np.hstack((BBEonstref, BBEoffstref))
 
    names1=['BBE_on'+str(k) for k in range(1,23)]
    names2=['BBE_off'+str(k) for k in range(1,23)]
    names=np.hstack((names1, names2))

    plot_radar(np.power(10, 0.1*dataradar), np.power(10, 0.1*refh), np.power(10, 0.1*refl), names, '', path_base+'articulationfemale.png')    

    # for males
    BBEonmeref = np.asarray([8.1, 8.61, 8.32, 7.92, 7.65, 7.32, 6.93, 6.47, 6.13, 6.09, 6.08, 6.02, 5.96, 5.99, 5.88, 5.53, 5.4, 5.18, 4.88, 4.7, 4.57, 3.48])
    BBEonstref =np.asarray([0.97, 1.13, 1.28, 1.49, 1.66, 1.75, 1.76, 1.65, 1.62, 1.62, 1.65, 1.66, 1.68, 1.71, 1.71, 1.59, 1.57, 1.48, 1.37, 1.31, 1.24, 1.23] ) 
    BBEoffmeref = np.asarray([8.47, 9.15, 8.91, 8.46, 8.1, 7.62, 7.08, 6.49, 6.1, 5.99, 5.97, 5.87, 5.71, 5.71, 5.58, 5.21, 5.05, 4.82, 4.55, 4.4, 4.27, 3.39])
    BBEoffstref= np.asarray([0.75, 0.73, 0.8, 0.99, 1.16, 1.32, 1.36, 1.31, 1.29, 1.31, 1.37, 1.38, 1.38, 1.42, 1.43, 1.34, 1.35, 1.29, 1.19, 1.13, 1.08, 1.02])
    
    
    refh=np.hstack((BBEonmeref, BBEoffmeref))+np.hstack((BBEonstref, BBEoffstref))
    refl=np.hstack((BBEonmeref, BBEoffmeref))-np.hstack((BBEonstref, BBEoffstref))
 
    names1=['BBE_on'+str(k) for k in range(1,23)]
    names2=['BBE_off'+str(k) for k in range(1,23)]
    names=np.hstack((names1, names2))

    plot_radar(np.power(10, 0.1*dataradar), np.power(10, 0.1*refh), np.power(10, 0.1*refl), names, '', path_base+'articulationmale.png') 



#define BBEonmeref QVector<double>  [7.86, 8.57, 8.47, 8.05, 7.69, 7.32, 6.95, 6.56, 6.20, 6.02, 5.93, 5.85, 5.79, 5.78, 5.71, 5.48, 5.24, 5.05, 4.71, 4.54, 4.48, 3.44]
#define BBEoffmeref QVector<double> [0.84, 1.04, 1.19, 1.42, 1.60, 1.69, 1.68, 1.60, 1.57, 1.55, 1.55, 1.55, 1.56, 1.56, 1.56, 1.51, 1.45, 1.41, 1.28, 1.22, 1.18, 1.19]
#define BBEonstref QVector<double>  [8.15, 9.07, 9.01, 8.58, 8.20, 7.73, 7.27, 6.80, 6.35, 6.07, 5.95, 5.83, 5.70, 5.66, 5.58, 5.31, 5.04, 4.83, 4.53, 4.37, 4.32, 3.46]
#define BBEoffstref QVector<double> [0.66, 0.68, 0.72, 0.89, 1.07, 1.20, 1.28, 1.26, 1.23, 1.20, 1.21, 1.20, 1.17, 1.18, 1.18, 1.14, 1.11, 1.07, 1.00, 0.96, 0.96, 0.95]    
    
    
#    fig, axs = plt.subplots(nrows=2, ncols=1, sharex=True)
#    ax = axs[0]
#    ax.errorbar(range(25), meOn, yerr=stOn)
#    ax.set_title('Energy of Onset')
#    
#    # With 4 subplots, reduce the number of axis ticks to avoid crowding.
#   
#    ax = axs[1]
#    ax.errorbar(range(25), meOff, yerr=stOff)
#    ax.set_title('Energy of Offset')
#    plt.show()
    return meOn, stOn, meOff, stOff

    
def VUV(audio, results, path_base, time_stepF0, lowf0, highf0, maxVUVPeriod, averageVUVPeriod):
    #print 'praat '+ path_base+'vuv.praat '+audio+' '+ results+' '+str(lowf0)+' '+str(highf0)+' '+str(time_stepF0)+' '+str(maxVUVPeriod)+' '+str(averageVUVPeriod)
    os.system(path_base+'../../Toolkits/praat '+ path_base+'vuv.praat '+audio+' '+ results+' '+str(lowf0)+' '+str(highf0)+' '+str(time_stepF0)+' '+str(maxVUVPeriod)+' '+str(averageVUVPeriod))
    

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
		for i in range (0,len(barkScale)):
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
    print(len(segments))
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

    me=BarkEn.mean(0)
    st=BarkEn.std(0,ddof=1)

    return me, st

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
    
print( sys.argv)
if len(sys.argv)<2:
    print ('python '+sys.argv[0]+' <audio> <features> <path_base>')
    sys.exit()
audio=sys.argv[1]
file_features=sys.argv[2]
path_base=sys.argv[3]

meOn, stOn, meOff, stOff=artCont(audio, path_base)

np.savetxt(file_features,np.hstack((meOn, stOn, meOff, stOff)))
