# -*- coding: utf-8 -*-
"""
Created on Mon Mar 28 09:51:19 2016

@author: J. C. Vasquez-Correa
"""

import os

import numpy as np
import sys
import matplotlib.pyplot as plt

from scipy.spatial.distance import euclidean
try:
    import cPickle
except:
    import pickle as cPickle  

try:
    import urllib2
except:
    import urllib.request as urllib2 

def intel_measures(path_base, audio, txt, lan):
    sys.path.insert(0, path_base)
    from fastdtw import fastdtw
    with open(path_base+'keys.pkl', 'rb') as fid:
        keys = cPickle.load(fid)
    f=open(txt)
    text_orig=f.read()
    f.close()
    if audio.find('.wav')!=-1:
        nfiles=1
        hf=['']
    else: 
        hf=os.listdir(audio)
        hf.sort()
        nfiles=len(hf)
    wa=np.zeros(nfiles)
    sim=np.zeros(nfiles)
    
    for k in range(nfiles):
        #print (audio)
        if os.path.isfile(path_base+'temp.flac'):
            os.remove(path_base+'temp.flac')
        print('ya0')
        wav2flac(path_base, audio+hf[k], path_base+'temp.flac')
        m=np.mod(k,4)
        print('ya1')
        speech2text(path_base+'temp.flac', path_base+'temp.txt', lan, keys[m])
        print('ya2')        
        fp=open(path_base+'temp.txt')
        text_pred=fp.read()
        fp.close()
        txt_orig, txt_pred=fill_txt(text_orig.upper(), text_pred.upper())
        sp_or=find_spaces(text_orig)
        words_orig=segm_words(text_orig, sp_or)
        sp_pr=find_spaces(text_pred)
        words_pr=segm_words(text_pred, sp_pr)
        error, wa[k]=compare_words(words_orig, words_pr)
        #print wa[k]

        txt_orig_arr=[float(ord(txt_orig[j]))/256.0 for j in range(len(txt_orig)-1)]
        txt_pred_arr=[float(ord(txt_pred[j]))/256.0 for j in range(len(txt_pred))]
        #print txt_orig_arr[:-1]
        #print txt_pred_arr
        dist, path = fastdtw(txt_orig_arr, txt_pred_arr, dist=euclidean)

#        plt.plot(np.asarray(path)[:,0], np.asarray(path)[:,1])
#        plt.xlabel('# of Graphemes real string', fontsize=18)
#        plt.ylabel('# of Graphemes predicted string', fontsize=18)
#        plt.show()
        sim[k]=1.0/(1.0+dist)
        #print audio+hf[k]+'\t'+str(wa[k])+'\t'+str(sim[k])
        if len(sp_or)>25: # if the length is too large, introduces some \n for visualization
            posq1=sp_pr[int(len(sp_pr)/4)]
            posq2=sp_pr[int(len(sp_pr)/2)]+2
            posq3=sp_pr[3*int(len(sp_pr)/4)]+4
            text_pred=text_pred[:posq1] + '\r\n' + text_pred[posq1:]
            text_pred=text_pred[:posq2] + '\r\n' + text_pred[posq2:]
            text_pred=text_pred[:posq3] + '\r\n' + text_pred[posq3:]
    return wa, sim, text_pred





def find_spaces(text):
    p=[0]
    for j in range(len(text)):
        if text.find(' ', j,j+1)!=-1:
            p.append(j+1)
    p.append(len(text)) 
    return p


def segm_words(text, sp):
    words=[]   
    for j in range(len(sp)-1):
        words.append(text[sp[j]:sp[j+1]])
    return words   
    
def compare_words(words1, words2):
    error_words=0
    error_chr=0
    if len(words1)==len(words2):
        for j in range(len(words1)):
            if (words1[j].find(words2[j])!=0): # word error
                error_words+=1
                minW=min([len(words1[j]), len(words2[j])])
                maxW=max([len(words1[j]), len(words2[j])])
                dif_char=maxW-minW
                for i in range(minW):
                    if words1[j][i]!=words2[j][i]:
                        error_chr+=1
                error_chr+=dif_char
    else:
        minWL=min([len(words1), len(words2)])
        maxWL=max([len(words1), len(words2)])
        print([len(words1), len(words2)])        

        for j in range(len(words1)):
            j2=0
            found=0
            while j2<len(words2):
                if (words1[j]==words2[j2]):
                    found=1
                j2+=1    
            if found==0:
                error_words+=1
        
    return [error_words, 1-float(error_words)/float(len(words1))]            



def fill_txt(txt1, txt2):
    minl=np.argmin([len(txt1), len(txt2)])
    if minl==0:
        txt1l=([chr(32) for j in range(len(txt2)-len(txt1))])
        txt1n=txt1+''.join(txt1l[2:len(txt1l)-2])
        return txt1n, txt2
    else:
        txt2l=([chr(32) for j in range(len(txt1)-len(txt2))])
        txt2n=txt2+''.join(txt2l[2:len(txt2l)-2])
        return txt1, txt2n
        
def wav2flac(path_base, file_name, file_name2):
    print(os.name)
    if os.name=='posix':
        cmd=path_base+'../../Toolkits/ffmpeg-git-20160326-64bit-static/ffmpeg -i ' + file_name +' -vn -ac 1 -ar 16000 -acodec flac '+file_name2
    elif os.name=='nt':            
        cmd=path_base+'../../Toolkits/ffmpeg-latest-win64-static/bin/ffmpeg -i ' + file_name +' -vn -ac 1 -ar 16000 -acodec flac '+file_name2
    #print cmd
    os.system(cmd)


def speech2text(file_name2, file_name3, lan, key):	
    #f = open(file_name2)
    #data = f.read()
    #f.close()
    url = 'https://www.google.com/speech-api/v2/recognize?output=json&lang='+lan+'&key='+key
    flac=open(file_name2,"rb").read()
    header = {'Content-Type' : 'audio/x-flac; rate=16000'}
    req = urllib2.Request(url, flac, header)
    if os.name=='posix':
        ret = urllib2.urlopen(req)    
        text=ret.read()
    elif os.name=='nt':
        #ret = urllib2.urlopen(req)    
        #text=ret.read().decode('utf-8')
        with urllib2.urlopen(req) as ret:
            text=ret.read().decode('utf-8')
    #print(text)
    p=text.find('"transcript":"')
    p2=text.find('"},{"transcript":"')
    text2 = text[p+14:p2]
    p3=text2.find('","confidence":')
    if p3!=-1:
        text2=text2[0:p3]
    
    if os.name=='nt':
        fid=open(file_name3, 'w')
        fid.write(text2)
    elif os.name=='posix':
        fid=open(file_name3, 'w')
        fid.write(text2)
    fid.close()
    
    
#print sys.argv
if len(sys.argv)<2:
    print ('python '+sys.argv[0]+' <file_audio or folder_audio> <filetxt.txt> <transliteration.txt> <fileresults.txt> <language>')
    sys.exit()
posint=sys.argv[0].find('intelligibility.py')
path_base=sys.argv[0][0:posint]
audio=sys.argv[1]
txt=sys.argv[2]
translit=sys.argv[3]
fileresults=sys.argv[4]
lan=sys.argv[5]
wa, sim, txt_pred=intel_measures(path_base, audio, txt, lan)
IntM=[[wa[k], sim[k]] for k in range(len(wa))]
np.savetxt(fileresults, np.asarray(IntM).T, fmt="%.2f")
fid=open(translit, 'w')
fid.write(txt_pred)
fid.close()