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

subtask='t'

fileSVR='C:/Users/elmon/Dropbox/Parkinson/PDTool/src/evaluation/SVMtrainedtongue.obj'
filest='C:/Users/elmon/Dropbox/Parkinson/PDTool/src/evaluation/scalerSVMtongue.obj'

feat1file='C:/Users/elmon/Dropbox/Parkinson/PDTool/exp_predmFDA/Features/art_ddk1.txt'


if subtask=='r': # respiration
    labels=np.asarray([3, 3, 4, 3, 3, 3, 3, 3, 1, 1, 3, 3, 3, 3, 2, 3, 3, 1, 1, 4, 3, 3, 3, 3, 3, 1, 2, 3, 4, 3, 3, 3, 3, 2, 3, 3, 2, 1, 1, 1, 2, 3, 3, 4, 2, 3, 3, 1, 3, 1, 1, 1, 1, 2, 3, 0, 1, 1, 2, 0, 1, 1, 2, 1, 1, 3, 1, 2, 2, 3, 0, 0, 2, 2, 3, 1, 1, 0, 0, 1, 1, 1, 3, 3, 1, 1, 2, 2, 2, 2, 0, 0, 0, 1, 1, 2, 0, 0, 1, 1])
elif subtask=='l': # lips
    labels=np.asarray([1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 1, 3, 2, 3, 2, 1, 1, 1, 1, 3, 1, 2, 1, 1, 3, 2, 1, 3, 4, 1, 3, 3, 2, 1, 1, 2, 3, 1, 1, 1, 1, 1, 3, 3, 1, 3, 1, 1, 1, 1, 0, 0, 0, 2, 1, 0, 0, 0, 2, 0, 1, 1, 1, 1, 2, 2, 1, 0, 0, 1, 0, 0, 1, 1, 2, 3, 0, 0, 0, 1, 1, 1, 2, 1, 0, 0, 1, 1, 1, 2, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0])
elif subtask=='p': # palate
    labels=np.asarray([2, 3, 4, 4, 3, 3, 3, 3, 2, 1, 2, 3, 3, 3, 2, 3, 3, 1, 1, 3, 1, 3, 3, 3, 3, 3, 2, 3, 3, 2, 3, 3, 3, 3, 2, 3, 2, 3, 1, 1, 3, 3, 3, 3, 3, 3, 3, 2, 3, 3, 1, 0, 0, 2, 1, 0, 1, 0, 3, 1, 2, 1, 2, 1, 2, 2, 2, 2, 1, 2, 1, 1, 1, 2, 3, 2, 0, 0, 0, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1])
elif subtask=='x': # larinx
    labels=np.asarray([3, 3, 4, 4, 2, 2, 3, 2, 1, 1, 2, 3, 3, 3, 2, 2, 2, 1, 1, 3, 2, 2, 2, 2, 3, 2, 2, 3, 4, 2, 3, 3, 3, 2, 2, 2, 3, 2, 1, 2, 2, 2, 3, 3, 2, 2, 2, 1, 2, 2, 2, 0, 0, 1, 1, 0, 1, 1, 2, 1, 1, 0, 1, 0, 1, 2, 1, 1, 0, 2, 0, 0, 1, 1, 2, 2, 1, 0, 0, 0, 0, 1, 2, 3, 2, 1, 1, 2, 2, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1])
elif subtask=='t': # tongue
    labels=np.asarray([3, 4, 4, 3, 3, 3, 3, 3, 1, 1, 2, 3, 3, 3, 2, 3, 3, 1, 1, 3, 3, 3, 3, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 3, 3, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 2, 2, 3, 0, 0, 1, 1, 1, 0, 0, 0, 3, 0, 3, 1, 1, 0, 1, 2, 2, 3, 2, 1, 2, 1, 1, 3, 3, 3, 0, 0, 0, 0, 3, 2, 3, 3, 0, 1, 1, 2, 2, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1])
elif subtask=='i': # intelligibility
    labels=np.asarray([1, 1, 3, 2, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 3, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1] )       






feat1=np.loadtxt(feat1file)

#Features=np.hstack((feat1, featGCCA))
Features=feat1
st=StandardScaler()
TrainDev_setN=st.fit_transform(Features)


cls=svm.SVC(kernel='rbf', C=5, gamma=0.001, class_weight='balanced')

print('training SVM')
cls.fit(Features, labels)
print('training done')

labelspred=cls.predict(Features)
CM=metrics.confusion_matrix(labels, labelspred)
print('Confusion Matrix:'+str(CM))

fid=open(fileSVR, 'wb')
cp.dump(cls, fid)

fid.close()

fid2=open(filest, 'wb')
cp.dump(st, fid2)

fid2.close()
"""
fig=plt.figure()
ax=fig.add_subplot(211)
posupdrs=np.argsort(labels)
ax.scatter(np.asarray(range(len(posupdrs))), labels[posupdrs], alpha=0.5, s=50)
ax.scatter(np.asarray(range(len(posupdrs))), labelspred[posupdrs], color='g', alpha=0.5, s=50)
ax.set_xlim((0, len(labels)))
ax.legend(['real mFDA', 'predicted mFDA'])
plt.text(0.05, 0.05, r'$\rho='+str(sp)+',\ P='+str(p)+'$')
            
ax=fig.add_subplot(212)
ax.scatter(labels, labelspred, alpha=0.5, s=50)
ax.set_xlim((0, 52))
ax.set_ylim((0, 52))
ax.set_xlabel('Real mFDA')
ax.set_ylabel('Predicted mFDA')
slo=np.polyfit(labels, labelspred, 1)
plt.plot(np.arange(52), np.arange(52)*slo[0]+slo[1])
plt.text(0.05, 0.05, r'$\rho='+str(sp)+',\ P='+str(p)+'$')
fig.savefig(fileSVR.replace('.obj', '.png'))
            


"""