def define():
	names = ['A4', 'AS4', 'B4', 'C4', 'D4', 'DS4', 'E4', 'F4', 'FS4', 'G4', 'GS4', 'A5', 'AS5', 'B5', 'C5', 'CS5', 'D5', 'DS5', 'E5', 'F5', 'FS5', 'G5', 'GS5', 'A6', 'AS6', 'B6', 'C6', 'CS6', 'D6', 'DS6']
	for i in range(len(names)):
		print("#define", names[i], i)