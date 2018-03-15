# -*- coding: utf-8 -*-
"""
Created on Wed Sep 14 15:12:44 2016

@author: J.C. Vasquez-Correa
"""
import numpy as np
from sklearn import svm
from sklearn.preprocessing import StandardScaler
try:
    import cPickle as cp
except:
    import pickle as cp

from scipy.stats import spearmanr
import matplotlib.pyplot as plt

LabelsTrainAll=[]
LabelsTrainAll.append([31,32,47,38,27,29,33,29,15,13,21,39,35,37,27,31,33,17,13,41,24,31,31,23,38,28,29,39,45,27,35,41,35,29,27,27,37,17,13,17,21,25,39,40,21,37,25,19,19,23,1,2,2,9,13,0,1,0,25,0,15,0,13,0,4,15,10,11,1,13,0,0,1,19,27,17,0,0,0,0,9,11,23,29,0,4,4,13,9,15,0,0,0,1,0,0,0,0,0,0]) #G
LabelsTrainAll.append([31,32,47,38,27,29,33,29,15,13,23,32,35,39,25,29,21,13,13,41,25,29,27,25,33,15,25,39,41,23,37,39,29,27,25,29,32,23,13,21,25,29,34,33,25,33,25,17,23,25,11,11,21,12,12,9,16,7,21,19,17,17,19,16,15,20,17,5,14,19,12,13,19,15,21,27,11,13,16,13,23,13,23,16,8,9,17,21,17,17,14,11,10,17,17,12,7,1,3,13]) # I
medians=[np.median([LabelsTrainAll[0][im], LabelsTrainAll[1][im]]) for im in range(100)]        
labels=np.asarray(medians)
"""
labels=np.asarray([28,19,52, 32, 28, 30, 29, 41, 38, 43, 6, 61, 28, 44, 50, 30, 42, 20, 14, 92, 53, 21, 13, 19, 75, 40, 30, 19, 23, 40, 9, 67, 15, 54, 51, 71, 40, 40, 53, 28, 38, 57, 23, 33, 30, 53, 45, 65, 21, 29])
labelshc=np.zeros(50)
labels=np.hstack((labels, labelshc))    
"""

feat1file='C:/Users/elmon/Desktop/work/ICASSP_2017_GCCA/Features/PCGITA/art_ddk1.txt'
featGCCAfile='C:/Users/elmon/Desktop/work/ICASSP_2017_GCCA/Features/PCGITAGCCA/GCCA2art_ddk1.txt'


feat1=np.loadtxt(feat1file)
featGCCA=np.loadtxt(featGCCAfile)

Features=np.hstack((feat1, featGCCA))

st=StandardScaler()
TrainDev_setN=st.fit_transform(Features)



svrt=svm.SVR(C=0.01, kernel='linear', epsilon=10)
print('training SVR')
svrt.fit(Features, labels)
print('training done')

labelspred=svrt.predict(Features)
sp, p=spearmanr(labels, labelspred)
print('Spearman Correlation:'+str(sp))

fileSVR='C:/Users/elmon/Dropbox/Parkinson/PDTool/src/evaluation/SVRtrained.obj'
filest='C:/Users/elmon/Dropbox/Parkinson/PDTool/src/evaluation/scaler.obj'

fid=open(fileSVR, 'wb')
cp.dump(svrt, fid)

fid.close()

fid2=open(filest, 'wb')
cp.dump(st, fid2)

fid2.close()

fig=plt.figure()
ax=fig.add_subplot(211)
posupdrs=np.argsort(labels)
ax.scatter(np.asarray(range(len(posupdrs))), labels[posupdrs], alpha=0.5, s=50)
ax.scatter(np.asarray(range(len(posupdrs))), labelspred[posupdrs], color='g', alpha=0.5, s=50)
ax.set_xlim((0, len(labels)))
ax.legend(['real mFDA', 'predicted UPDRS'])
plt.text(0.05, 0.05, r'$\rho='+str(sp)+',\ P='+str(p)+'$')
            
ax=fig.add_subplot(212)
ax.scatter(labels, labelspred, alpha=0.5, s=50)
ax.set_xlim((0, 52))
ax.set_ylim((0, 52))
ax.set_xlabel('Real UPDRS')
ax.set_ylabel('Predicted UPDRS')
slo=np.polyfit(labels, labelspred, 1)
plt.plot(np.arange(52), np.arange(52)*slo[0]+slo[1])
plt.text(0.05, 0.05, r'$\rho='+str(sp)+',\ P='+str(p)+'$')
fig.savefig(fileSVR.replace('.obj', '.png'))
            


