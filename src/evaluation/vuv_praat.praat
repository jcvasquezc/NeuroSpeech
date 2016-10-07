form Directory to analyse and file
	text fileName 
	text fileNamet 
	integer low_f0
	integer high_f0
	positive time_step
	positive max_periodVUV
	positive average_periodVUV
endform



Read from file... 'fileName$'
To Pitch... 'time_step' 'low_f0' 'high_f0'
To PointProcess
To TextGrid (vuv)... 'max_periodVUV' 'average_periodVUV'
#Concatenate
fileName$ = replace$(fileName$,".wav",".txt",0)

Write to short text file... 'fileNamet$'

exit

