form Directory to analyse and file
	text fileName 
	text fileResult 
	integer low_f0
	integer high_f0
	positive time_step
	positive max_periodVUV
	positive average_periodVUV
endform


Read from file... 'fileName$'
To Pitch... 'time_step' 'low_f0' 'high_f0'
#To Pitch (cc)... 0.01 60 15 off 0.0 0.8 0.03 0.5 0.25 400
To PointProcess
To TextGrid (vuv)... 'max_periodVUV' 'average_periodVUV'
#Concatenate

Write to short text file... 'fileResult$'

