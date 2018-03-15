from FourierSolap import fftsolp 


import numpy as np
import math

def bark(f):
	x=(f*0.00076)
	x2=(f/7500)**2
	b=[]
	for i in xrange (0,len(f)):
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
		for i in xrange (0,len(barkScale)):
			barkIndices.append(int(barkScale[i]))

		barkIndices = np.asarray(barkIndices)
		
		barkEnergy=[]
		for i in xrange (nB):
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
