import numpy as np
import pandas as pd
import numpy as np
from sklearn.cross_validation import KFold, ShuffleSplit
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA
from sklearn import svm
import re
import os

p = re.compile('(\w+)_(\w)_\w\d\.csv')
data = []
for path, subdirs, files in os.walk("../Data/"):
    for f in files:
        m = p.match(f)
        if m:
            emgdata = pd.read_csv(os.path.join(path, f), header=None)
            for y in range(0, len(emgdata)-10, 10):
                raw = np.apply_along_axis(np.fft.fft, 0, emgdata.iloc[y:(y+10), 0:8].as_matrix(), n =10)
                data.append(np.absolute(raw).flatten().tolist() + [m.group(2)])

df = pd.DataFrame(data)
X = df.iloc[:,0:80].as_matrix()
Y = df.iloc[:,80].as_matrix().ravel()

clf = svm.SVC(decision_function_shape='ovo', kernel='poly', degree=3, C=1.0)
clf.fit(X, Y)

def transform(emg):
    emg = np.array(emg)
    res = np.fft.fft(emg, axis = 0, n = 10)
    return np.absolute(res).flatten()

def predict(emg):
    global clf
    X = transform(emg)
    pred = clf.predict(X.reshape(1,-1))
    return pred[0]
