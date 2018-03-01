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
import math
from statsmodels.tsa.tsatools import lagmat
from sklearn.metrics.pairwise import euclidean_distances as dist
from radarPlot import plot_radar


def phonationVowels(audio, path_base):

    fs, data_audio=read(audio)
    data_audio=data_audio-np.mean(data_audio)
    data_audio=data_audio/float(np.max(np.abs(data_audio)))
    size_frameS=0.04*float(fs)
    size_stepS=0.02*float(fs)
    overlap=size_stepS/size_frameS
    if os.name == 'nt':
        os.system(path_base+'../../Toolkits/praat '+path_base+'F0_Praat.praat '+audio+' '+path_base+'tempF0.txt 75 600 0') #f0 extraction Praat
    else:
        os.system('praat '+path_base+'F0_Praat.praat '+audio+' '+path_base+'tempF0.txt 75 600 0') #f0 extraction Praat
        print("Praat in ubuntu")
    F0=decodeF0(path_base+'tempF0.txt')
    Jitter=jitter_env(F0, len(F0))
    nF=int((len(data_audio)/size_frameS/overlap))-1
    Amp=[]
    logE=[]
    cd=[0]
    
    time_stepF0=0 # 0 #value by default (0-->0.75/low_f0) Other values are in miliseconds
    low_f0=75 #75
    high_f0=600 #600
    maxPeriodVUV=0.02 #20 #milisecionds
    averagePeriodvuv=0.01#10 #milisecionds
    VUV(audio, path_base+'vuv.txt', path_base, time_stepF0, low_f0, high_f0, maxPeriodVUV, averagePeriodvuv)
    segmentsV, fs=decode_Texgrid(path_base+'vuv.txt', audio, 'Voiced')
    segmentsU, fs=decode_Texgrid(path_base+'vuv.txt', audio, 'Unvoiced')

    durV=np.asarray([len(segmentsV[l]) for l in range(len(segmentsV))])    
    totaldurV=np.sum(durV)
    
    thresholdU=0.27*fs
    durU=np.asarray([len(segmentsU[l]) for l in range(len(segmentsU))])    
    totaldurU=np.sum(durU[np.where(durU<thresholdU)[0]])
        
    thresholdE=10*logEnergy(0.025)
    degreeU=100*float(totaldurU)/(totaldurU+totaldurV)
    print(totaldurV, durU, degreeU)
    for l in range(nF):
        data_frame=data_audio[int(l*size_stepS):int(l*size_stepS+size_frameS)]
        energy=10*logEnergy(data_frame)
        if energy>thresholdE:
            Amp.append(np.max(np.abs(data_frame)))
            logE.append(10*logEnergy(data_frame))
        #m=fnn(data_frame, 15)
        #tau=Tao(data_frame)
        #cd.append(Dim_Corr(data_frame, tau, m))
        #if l==nF/2:
            #atractor=PhaseSpace(data_frame, 2, tau)
            #atractor=atractor/float(np.max(atractor))
            #print atractor.shape
    Shimmer=shimmer_env(Amp, len(Amp))
    apq=APQ(Amp)
    F0nz=F0[F0!=0]
    ppq=PPQ(1/F0nz)
    logE=np.asarray(logE)
    cd=np.asarray(cd)
    mlogE=logE.mean(0)
    mjitter=Jitter.mean(0)
    mshimmer=Shimmer.mean(0)
    mcd=cd.mean(0)
    
    F0semi=np.asarray([Hz2semitones(F0nz[l]) for l in range(len(F0nz))])
    F0varsemi=np.std(F0semi)    
    
    
    #plt.plot(data_audio)
    #plt.show()



    # figure audio and f0
    plt.figure(1)
    plt.subplot(211)
    t=np.arange(0, float(len(data_audio))/fs, 1.0/fs)
    if len(t)>len(data_audio):
        t=t[:len(data_audio)]
    elif len(t)>len(data_audio):
        data_audio=data_audio[:len(t)]
    print(len(t), len(data_audio))
    plt.plot(t, data_audio, 'k')
    plt.ylabel('Amplitude', fontsize=14)
    plt.xlabel('Time (s)', fontsize=14)
    plt.xlim([0, t[-1]])
    plt.grid(True)
    plt.subplot(212)
    fsp=int(len(F0)/t[-1])
    print(fsp)
    t2=np.arange(0.0, t[-1], 1.0/fsp)
    if len(t2)>len(F0):
        t2=t2[:len(F0)]
    elif len(F0)>len(t2):
        F0=F0[:len(t2)]

    plt.plot(t2, F0, color='k', linewidth=2.0)
    plt.xlabel('Time (s)', fontsize=14)
    plt.ylabel('Frequency (Hz)', fontsize=14)
    plt.ylim([0,np.max(F0)+10])
    plt.xlim([0, t[-1]])    
    plt.grid(True)
    #plt.show()
    plt.savefig(path_base+'phonation1.png')
    plt.savefig(path_base+'phonation1.pdf')
    dataradar=np.asarray([mjitter, mshimmer, apq, ppq, degreeU, F0varsemi])
    dataradar[np.isnan(dataradar)]=1000
    plot_radar(dataradar, np.asarray([2.0, 6.50, 7.90, 0.50, 5.7, 1.7]), ['Jitter', 'Shimmer', 'APQ', 'PPQ', '%U', 'var. F0'], '', path_base+'phonation2male.png')    
    plot_radar(dataradar, np.asarray([1.3, 6.40, 8.60, 0.77, 3.6, 2.7]), ['Jitter', 'Shimmer', 'APQ', 'PPQ', '%U', 'var. F0'], '', path_base+'phonation2female.png')    

    return F0, F0semi, mjitter, mshimmer, apq, ppq, mlogE, mcd, degreeU, F0varsemi  

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
    if os.name=="nt":
        os.system(path_base+'../../Toolkits/praat '+ path_base+'vuv.praat '+audio+' '+ results+' '+str(lowf0)+' '+str(highf0)+' '+str(time_stepF0)+' '+str(maxVUVPeriod)+' '+str(averageVUVPeriod))
    else:
        os.system('praat '+ path_base+'vuv.praat '+audio+' '+ results+' '+str(lowf0)+' '+str(highf0)+' '+str(time_stepF0)+' '+str(maxVUVPeriod)+' '+str(averageVUVPeriod))

    
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



def Dim_Corr(datas, Tao, m, graph=False): 
	"""
	Compute the correlation dimension of a time series with a time-lag Tao and an embedding dimension m
	datas--> time series to compute the correlation dimension
	Tao--> time lag computed using the first zero crossing of the auto-correlation function (see Tao func)   
	m--> embeding dimension of the time-series, computed using the false neighbors method (see fnn func)  
	graph (optional)--> plot the phase space (attractor) in 3D
	"""
	x=PhaseSpace(datas, m, Tao, graph)
	ED2=dist(x.T)
	posD=np.triu_indices_from(ED2, k=1)
	ED=ED2[posD]
	max_eps=np.max(ED)
	min_eps=np.min(ED[np.where(ED>0)])
	max_eps=np.exp(math.floor(np.log(max_eps)))
	n_div=int(math.floor(np.log(max_eps/min_eps)))
	n_eps=n_div+1
	eps_vec=range(n_eps)
	unos=np.ones([len(eps_vec)])*-1
	eps_vec1=max_eps*np.exp(unos*eps_vec-unos)
	Npairs=((len(x[1,:]))*((len(x[1,:])-1)))
	C_eps=np.zeros(n_eps)
 
	for i in eps_vec:
        	eps=eps_vec1[i]
        	N=np.where(((ED<eps) & (ED>0)))
        	S=len(N[0])
        	C_eps[i]=float(S)/Npairs

	omit_pts=1 
	k1=omit_pts
	k2=n_eps-omit_pts
	xd=np.log(eps_vec1)
	yd=np.log(C_eps)
	xp=xd[k1:k2]
	yp=yd[k1:k2]
	p = np.polyfit(xp, yp, 1)
	return p[0]


def PhaseSpace(data, m, Tao, graph=False):
  """
  Compute the phase space (attractor) a time series data with a time-lag Tao and an embedding dimension m
  data--> time series
  Tao--> time lag computed using the first zero crossing of the auto-correlation function (see Tao func)   
  m--> embeding dimension of the time-series, computed using the false neighbors method (see fnn func)  
  graph (optional)--> plot the phase space (attractor)
  """		
  ld=len(data)
  x = np.zeros([m, (ld-(m-1)*Tao)])
  for j in range(m):
      l1=(Tao*(j))
      l2=(Tao*(j)+len(x[1,:]))
      x[j,:]=data[l1:l2]
  if graph:
     fig = plt.figure()
     if m>2:
         ax = fig.add_subplot(111, projection='3d')
         ax.plot(x[0,:], x[1,:], x[2,:])
     else:
         ax = fig.add_subplot(111)
         ax.plot(x[0,:], x[1,:])
     plt.show()
             
  return x


def Tao(data):
    """
    Compute the time-lag of a time series data to build the phase space using the first zero crossing rate criterion
    data--> time series
    """    
    corr=np.correlate(data, data, mode="full")
    corr=corr[len(corr)/2:len(corr)]
    tau=0
    j=0
    while (corr[j]>0):
      j=j+1
    tau=j
    return tau


def fnn(data, maxm):
    """
    Compute the embedding dimension of a time series data to build the phase space using the false neighbors criterion
    data--> time series
    maxm--> maximmum embeding dimension
    """    
    RT=15.0
    AT=2
    sigmay=np.std(data, ddof=1)
    nyr=len(data)
    m=maxm
    EM=lagmat(data, maxlag=m-1)
    EEM=np.asarray([EM[j,:] for j in range(m-1, EM.shape[0])])
    embedm=maxm
    for k in range(AT,EEM.shape[1]+1):
        fnn1=[]
        fnn2=[]
        Ma=EEM[:,range(k)]
        D=dist(Ma)
        for i in range(1,EEM.shape[0]-m-k):
            #print D.shape            
            #print(D[i,range(i-1)])
            d=D[i,:]
            pdnz=np.where(d>0)
            dnz=d[pdnz]
            Rm=np.min(dnz)
            l=np.where(d==Rm)
            l=l[0]
            l=l[len(l)-1]
            if l+m+k-1<nyr:
                fnn1.append(np.abs(data[i+m+k-1]-data[l+m+k-1])/Rm)
                fnn2.append(np.abs(data[i+m+k-1]-data[l+m+k-1])/sigmay)
        Ind1=np.where(np.asarray(fnn1)>RT)
        Ind2=np.where(np.asarray(fnn2)>AT)
        if len(Ind1[0])/float(len(fnn1))<0.1 and len(Ind2[0])/float(len(fnn2))<0.1:
            embedm=k
            break
    return embedm

    

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
    
def jitter_env(vPPS, iNumPuntos):

    iLongSec=len(vPPS)

    if (iLongSec < 2):
        print( 'La secuencia de Periodos de Pitch es demasiado corta' ); 
        vJitta=np.zeros(iNumPuntos)
        return vJitta

    vJitta=np.zeros(iNumPuntos)
    iIndiceIni=0

    iDesplazamiento=iLongSec/iNumPuntos

# average f0 of signal
    rFoMed=np.mean(vPPS)

    for n in range(iNumPuntos-1):
        indice=int( iIndiceIni+n*iDesplazamiento )
        if ( n>0 ) and (indice==int( iIndiceIni+(n-1)*iDesplazamiento )):
            vJitta[ n ]=vJitta[ n-1 ]
        else:
            if (indice+1 < iLongSec):
                    vJitta[n]=np.abs(vPPS[indice+1]-vPPS[indice])
            else:
                vJitta[n]=0;
        vJitta[n]=100*vJitta[n]/rFoMed

    return vJitta
    
def logEnergy(sig):
    sig2=np.power(sig,2)
    sumsig2=np.sum(sig2)
    logE=np.log10(sumsig2)
    return logE


def Power(sig):
    sig2=np.square(sig)
    sumsig2=np.sum(sig2)/len(sig)
    return sumsig2


def shimmer_env(vPPS, iNumPuntos):

    iLongSec=len(vPPS)

    if (iLongSec < 2):
        print( 'La secuencia de Periodos de Pitch es demasiado corta' ); 
        vShimm=np.zeros(iNumPuntos)
        return vShimm

    vShimm=np.zeros(iNumPuntos)
    iIndiceIni=0

    iDesplazamiento=iLongSec/iNumPuntos

# average f0 of signal
    rFoMed=np.mean(vPPS)

    for n in range(iNumPuntos-1):
        indice=int( iIndiceIni+n*iDesplazamiento )
        if ( n>0 ) and (indice==int( iIndiceIni+(n-1)*iDesplazamiento )):
            vShimm[ n ]=vShimm[ n-1 ]
        else:
            if (indice+1 < iLongSec):
                    vShimm[n]=np.abs(vPPS[indice+1]-vPPS[indice])
            else:
                vShimm[n]=0;
        vShimm[n]=100*vShimm[n]/rFoMed

    return vShimm



def PQ(x,k):
    """
    Perturbation Quotient in percentage of the signal x
    input: x--> input sequence: F0 values or Amplitude values
    k--> average factor (must be an odd number)
    """
    N=len(x)
    if N<k or k%2==0:
        return 0
    m=int(0.5*(k-1))
    summ=0
    for n in range(N-k):
        dif=0
        for r in range(k):
            dif=dif+x[n+r]-x[n+m]
        dif=np.abs(dif/float(k))
        summ=summ+dif
    num=summ/(N-k)
    den=np.mean(np.abs(x))
    c=100*num/den
    if np.sum(np.isnan(c))>0:
        print(x)        
    return c
    
def APQ(PAS):
    """
    Amplitude perturbation quotient (APQ)
    input:-->PAS: secuence of peak amplitudes of a signal
    """
    return PQ(PAS,11)
    
def PPQ(PPS):
    """
    Period perturbation quotient (APQ)
    input:-->PAS: secuence of pitch periods of a signal
    """
    return PQ(PPS,5)











#print sys.argv
if len(sys.argv)<4:
    print('python '+sys.argv[0]+' <file_audio> <fileresultsf0.txt> <file_features.txt> <path_base>')
    sys.exit()
audio=sys.argv[1]
fileresultsf0=sys.argv[2]
file_features=sys.argv[3]
path_base=sys.argv[4]
F0, F0semi, mjitter, mshimmer, apq, ppq, mlogE, mcd, degreeU, varF0  = phonationVowels(audio, path_base)
feat=[mjitter, mshimmer, apq, ppq, mlogE, mcd, degreeU, varF0]

np.savetxt(fileresultsf0, F0)
np.savetxt(fileresultsf0.replace('.txt', 'semitones.txt'), F0semi)

np.savetxt(file_features, feat, fmt="%.2f")
#np.savetxt(path_base+'atractor.txt', atractor.T)


