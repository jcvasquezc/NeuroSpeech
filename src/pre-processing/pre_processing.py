# -*- coding: utf-8 -*-
"""
Created on Wed Nov  4 16:17:10 2015

@author: b.jvc
"""
import numpy as np
import scipy.signal as sig
import wave
import struct
import os
from scipy.io.wavfile import write, read

class pre_processing():
        
    def normalize(data_audio): # normalize speech frame
    	speech1=np.asarray(data_audio)
    	speechn=speech1-speech1.mean()
    	m=float(np.max(np.abs(speechn)))
    	speechn=speechn/m
    	return speechn	
     
    def resample(data_audio, fs1, fs2): 
        """
        data_audio: signal vector
        fs1: sampling frequency to resample the signal
        fs2: current sampling frequency of the signal
        """
        return sig.resample(data_audio, int(len(data_audio)*fs1/fs2))
        
    def normalizeDB(path_audios_in, path_audios_out):
        nReg=os.listdir(path_audios_in)
        nReg.sort()
        for j in range(len(nReg)):
            print ('Normalize audio'+nReg[j])
            file_audio=path_audios_in+nReg[j]
            (fs, data_audio)=read(file_audio)
            data_audio=normalize(data_audio)
            write(path_audios_out+nReg[j], fs, data_audio)
            
    def resampleDB(path_audios):
        nReg=os.listdir(path_audios)
        nReg.sort()
        for j in range(len(nReg)):
            file_audio=path_audios+nReg[j]
            wav = wave.open (file_audio, "r")
            (nchannels, sampwidth, fs, nframes, comptype, compname) = wav.getparams ()
            frames = wav.readframes (nframes * nchannels)
            data_audio = struct.unpack_from ("%dh" % nframes * nchannels, frames)
            data_audio=resample(data_audio)
            write(path_audios+nReg[j], fs, data_audio)    