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
import sys

if len(sys.argv)!=5:
    print('python'+sys.argv[0]+' <file_matrix.txt> <file_labels.txt> <file_scaler.obj> <fileSVR.obj>')
    sys.exit()
else:


    feat1file=sys.grav[1]
    labels=np.loadtxt(sys.argv[2])
    fileSVR=sys.argv[3]#'C:/Users/elmon/Dropbox/Parkinson/PDTool/src/evaluation/SVRtrainedUPDRS.obj'
                        #'C:/Users/elmon/Dropbox/Parkinson/PDTool/src/evaluation/SVRtrainedmFDA.obj'
    filest=sys.argv[4]#'C:/Users/elmon/Dropbox/Parkinson/PDTool/src/evaluation/scalerUPDRS.obj'
                        #'C:/Users/elmon/Dropbox/Parkinson/PDTool/src/evaluation/scalermFDA.obj'

    Features=np.loadtxt(feat1file)
    
    st=StandardScaler()
    TrainDev_setN=st.fit_transform(Features)
    
    
    svrt=svm.SVR(C=0.01, kernel='linear', epsilon=10)
    print('training SVR')
    svrt.fit(Features, labels)
    print('training done')
    
    labelspred=svrt.predict(Features)
    sp, p=spearmanr(labels, labelspred)
    print('Spearman Correlation:'+str(sp))
    
    
    fid=open(fileSVR, 'w')
    cp.dump(svrt, fid)
    
    fid.close()
    
    fid2=open(filest, 'w')
    cp.dump(st, fid2)
    
    fid2.close()



