#import svm_predictor
import nn_predictor
import nlp_predictor
import numpy as np

def letter(data):
	nn_pred =  nn_predictor.predict(data['emg'])
	if "txt" in data:
		nlp_pred = nlp_predictor.predict(data['txt'])
		total = [nn_pred[i] + nlp_pred[i] for i in range(5)]
		return chr(np.argmax(total) + ord('A'))
	return chr(np.argmax(nn_pred) + ord('A'))
