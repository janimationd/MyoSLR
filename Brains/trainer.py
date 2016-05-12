def letter(data):
	print data['emg']
	return 'Letter was "' + data['letter'] + '" and received ' + str(len(data['emg'])) + ' lines of emg data!'
	