import tensorflow as tf
import pandas as pd
import numpy as np
import re
import os
from sklearn.preprocessing import OneHotEncoder

sess = tf.InteractiveSession()


p = re.compile('(\w+)_(\w)_\w\d\.csv')
data = []

for path, subdirs, files in os.walk("../Data/"):
    for f in files:
        m = p.match(f)
        if m:
            emgdata = pd.read_csv(os.path.join(path, f), header=None)
            for y in range(0, len(emgdata)-10, 10):
                raw = np.apply_along_axis(np.fft.fft, 0, emgdata.iloc[y:(y+10), 0:8].as_matrix(), n =10)
                mns = np.apply_along_axis(np.mean, 0, emgdata.iloc[y:(y+10), 0:8].as_matrix())
                sds = np.apply_along_axis(np.std, 0, emgdata.iloc[y:(y+10), 0:8].as_matrix())
                data.append(np.absolute(raw).flatten().tolist() + mns.tolist() + sds.tolist() + [m.group(2)])
df = pd.DataFrame(data)
X = df.iloc[:,0:96].as_matrix()
Y = [[ord(y) - ord('A')] for y in df.iloc[:,96].as_matrix().ravel().tolist()]
Y = OneHotEncoder().fit_transform(Y).toarray()


ind = 0
def batch(size):
    global ind
    if ind + size >= X.shape[0]:
        ind = 0
    b = (X[ind: ind+size], Y[ind: ind+size])
    ind += size
    return b

features = 96

x = tf.placeholder(tf.float32, shape=[None, features])
y_ = tf.placeholder(tf.float32, shape=[None, 5])

W1 = tf.Variable(tf.zeros([features,features]))
b1 = tf.Variable(tf.zeros([features]))

h1 = tf.nn.softmax(tf.matmul(x,W1) + b1)

W2 = tf.Variable(tf.zeros([features,features]))
b2 = tf.Variable(tf.zeros([features]))

h2 = tf.nn.softmax(tf.matmul(h1,W2) + b2)

W3 = tf.Variable(tf.zeros([features,5]))
b3 = tf.Variable(tf.zeros([5]))

y = tf.nn.softmax(tf.matmul(h2,W3) + b3)

correct_prediction = tf.equal(tf.argmax(y,1), tf.argmax(y_,1))
accuracy = tf.reduce_mean(tf.cast(correct_prediction, tf.float32))

cross_entropy = tf.reduce_mean(-tf.reduce_sum(y_ * tf.log(y)))
train_step = tf.train.AdamOptimizer(1e-4).minimize(cross_entropy)

sess.run(tf.initialize_all_variables())
for i in range(400):
    b = batch(300)
    train_step.run(feed_dict={x: b[0], y_: b[1]})
    if i % 100 == 0:
        print(accuracy.eval(feed_dict={x: X, y_: Y}))
print accuracy.eval(feed_dict={x: X, y_: Y})

def transform(emg):
    emg = np.array(emg)
    fts = np.fft.fft(emg, axis = 0, n = 10)
    mns = np.mean(emg, axis=0)
    sds = np.std(emg, axis=0)
    return np.absolute(fts).flatten().tolist()  + mns.tolist() + sds.tolist()

def predict(emg):
    global y
    X = transform(emg)
    emg_y = y.eval(feed_dict={x:[X]})[0]
    return emg_y
