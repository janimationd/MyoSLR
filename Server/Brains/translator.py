import svm_predictor

def letter(data):
	return svm_predictor.predict(data['emg'])
