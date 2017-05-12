all:
	echo 'Probably want to run in arduino'
	echo 'Run "make vi" to edit all files at once.'

vi:
	vim Makefile wifi-loc.ino wifi_config.h circle_buffer.h
