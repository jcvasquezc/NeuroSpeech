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
from sklearn import metrics
import sys

if len(sys.argv)!=6:
    print('python'+sys.argv[0]+' <file_matrix.txt> <file_labels.txt> <subscale> <file_scaler.obj> <fileSVR.obj>')
    sys.exit()
else:

    feat1file=sys.grav[1]
    labels=np.loadtxt(sys.argv[2])
    subtask=sys.grav[3]
    fileSVM=sys.argv[3]                        
    filest=sys.argv[4]                       

    Features=np.loadtxt(feat1file)

    if subtask=='r': # respiration
        labels=labels[0,:]
    elif subtask=='l': # lips
        labels=labels[1,:]       
    elif subtask=='p': # palate
        labels=labels[2,:]
    elif subtask=='x': # larinx
        labels=labels[3,:]
    elif subtask=='t': # tongue
        labels=labels[4,:]
    elif subtask=='i': # intelligibility
        labels=labels[5,:]
    

    Features=np.loadtxt(feat1file)
    st=StandardScaler()
    TrainDev_setN=st.fit_transform(Features)

    cls=svm.SVC(kernel='rbf', C=5, gamma=0.001, class_weight='balanced')

    print('training SVM')
    cls.fit(Features, labels)
    print('training done')

    labelspred=cls.predict(Features)
    CM=metrics.confusion_matrix(labels, labelspred)
    print('Confusion Matrix:'+str(CM))

    fid=open(fileSVM, 'w')
    cp.dump(cls, fid)
    
    fid.close()
    
    fid2=open(filest, 'w')
    cp.dump(st, fid2)
    
    fid2.close()
