# -*- coding: utf-8 -*-
"""
Created on Wed Sep 14 15:44:29 2016

@author: elmon
"""

import sys
import numpy as np
from sklearn import svm
from sklearn.preprocessing import StandardScaler
try:
    import cPickle as cp
except:
    import pickle as cp

import matplotlib.pyplot as plt
from extractFeatures import *


if len(sys.argv)!=3:
    print('python '+sys.argv[0]+'<folder_SVR> <file_audio>')
    sys.exit()
else:
    print (sys.argv)
    folderSVR=sys.argv[1]
    file_audio=sys.argv[2]


features=extractFeatures(file_audio, folderSVR)
    
posnan=np.isnan(features)
features[posnan]=0

filest=folderSVR+'scaler.obj'
fid2=open(filest, 'r')
st=cp.load(fid2)
fid2.close()



#features=st.transform(features)
    
fileSVR=folderSVR+'SVRtrained.obj'
fileSVRUPDRS=folderSVR+'SVRtrainedUPDRS.obj'

fileGCCA=folderSVR+'Wx2opt.txt'


WGCCA=np.loadtxt(fileGCCA)
mat1=np.matrix(WGCCA)
mat2=np.matrix(features)
print(mat1.shape, mat2.shape)
featuresGCCA=np.array(np.dot(mat1.T,mat2.T))
print(featuresGCCA.T.shape, features.shape)
Features=np.hstack((features, featuresGCCA.T[0]))
#Features=st.fit_transform(Features.reshape(1, -1))


intpred=np.zeros(8)
print(Features.shape)
fid=open(fileSVR, 'r')
svr=cp.load(fid)
fid.close()
predsvr=svr.predict(Features)
intpred[0]=np.round(predsvr)



fid=open(fileSVRUPDRS, 'r')
svr2=cp.load(fid)
fid.close()
predsvr2=svr2.predict(Features)
if predsvr2<0:
    predsvr2=0
if predsvr2>128:
    predsvr2=128
    
intpred[7]=np.round(predsvr2)


fileSVMresp=folderSVR+'SVMtrainedrespiration.obj'
fid=open(fileSVMresp, 'r')
cls=cp.load(fid)
fid.close()
pred=cls.predict(features)
intpred[1]=np.round(pred)

fileSVMlips=folderSVR+'SVMtrainedlips.obj'
fid=open(fileSVMlips, 'r')
cls=cp.load(fid)
fid.close()
pred=cls.predict(features)
intpred[2]=np.round(pred)

fileSVMpal=folderSVR+'SVMtrainedpalate.obj'
fid=open(fileSVMpal, 'r')
cls=cp.load(fid)
fid.close()
pred=cls.predict(features)
intpred[3]=np.round(pred)

fileSVMlx=folderSVR+'SVMtrainedlarinx.obj'
fid=open(fileSVMlx, 'r')
cls=cp.load(fid)
fid.close()
pred=cls.predict(features)
intpred[4]=np.round(pred)


fileSVMlx=folderSVR+'SVMtrainedtongue.obj'
fid=open(fileSVMlx, 'r')
cls=cp.load(fid)
fid.close()
pred=cls.predict(features)
intpred[5]=np.round(pred)

fileSVMlx=folderSVR+'SVMtrainedint.obj'
fid=open(fileSVMlx, 'r')
cls=cp.load(fid)
fid.close()
pred=cls.predict(features)
intpred[6]=np.round(pred)

np.savetxt(folderSVR+'predmFDA.txt', np.asarray(intpred), fmt='%s')



LabelsTrainAll=[]
LabelsTrainAll.append([31,32,47,38,27,29,33,29,15,13,21,39,35,37,27,31,33,17,13,41,24,31,31,23,38,28,29,39,45,27,35,41,35,29,27,27,37,17,13,17,21,25,39,40,21,37,25,19,19,23,1,2,2,9,13,0,1,0,25,0,15,0,13,0,4,15,10,11,1,13,0,0,1,19,27,17,0,0,0,0,9,11,23,29,0,4,4,13,9,15,0,0,0,1,0,0,0,0,0,0]) #G
LabelsTrainAll.append([31,32,47,38,27,29,33,29,15,13,23,32,35,39,25,29,21,13,13,41,25,29,27,25,33,15,25,39,41,23,37,39,29,27,25,29,32,23,13,21,25,29,34,33,25,33,25,17,23,25,11,11,21,12,12,9,16,7,21,19,17,17,19,16,15,20,17,5,14,19,12,13,19,15,21,27,11,13,16,13,23,13,23,16,8,9,17,21,17,17,14,11,10,17,17,12,7,1,3,13]) # I
medians=[np.median([LabelsTrainAll[0][im], LabelsTrainAll[1][im]]) for im in range(100)]        
labels=np.asarray(medians)


fig = plt.figure(figsize=(12,5))
plt.subplot(121)
plt.hist(labels[0:50], 20, color='g', alpha=0.5, label='Pacientes')
plt.hist(labels[50:-1], 20, color='c', alpha=0.5, label='Sanos')
plt.plot(np.asarray([predsvr, predsvr]), [0, 8], color='r', linewidth=2, label='Usuario')
plt.legend()
plt.xlabel('Nivel de disartria predicho')
#plt.show()






labels=np.asarray([28,19,52, 32, 28, 30, 29, 41, 38, 43, 6, 61, 28, 44, 50, 30, 42, 20, 14, 92, 53, 21, 13, 19, 75, 40, 30, 19, 23, 40, 9, 67, 15, 54, 51, 71, 40, 40, 53, 28, 38, 57, 23, 33, 30, 53, 45, 65, 21, 29])

plt.subplot(122)
plt.hist(labels[0:50], 20, color='g', alpha=0.5, label='Pacientes')
plt.plot(np.asarray([predsvr2, predsvr2]), [0, 11], color='r', linewidth=2, label='Usuario')
plt.legend()
plt.xlim([0,128])
plt.xlabel('UPRS -III predicho')
#plt.show()
plt.savefig(folderSVR+'prediction.png')
