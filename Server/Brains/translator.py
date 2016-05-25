#import svm_predictor
import nn_predictor
def letter(data):
	return nn_predictor.predict(data['emg'])
