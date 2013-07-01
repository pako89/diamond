#!/usr/bin/python
import sys
import os
import re

class Video:
	def __init__(self, path, width, height):
		self.Width = width
		self.Height = height
		self.Path = path


class Config:
	def __init__(self):
		self.dir = ""
		self.Videos = list()
		self.ext = ".yuv"
	def verbose(self):
		print "Directory: {0}".format(self.dir)
		for v in self.Videos:
			print "Video: [{0}x{1}] {2}".format(v.Width, v.Height, v.Path)

	def parse_videos(self):
		for d in os.listdir(self.dir):
			absdir = os.path.join(self.dir, d)
			self.parse_resolution(absdir)

	def parse_resolution(self, dir):
		m = re.search('(?P<W>[0-9]+)x(?P<H>[0-9]+)', dir)
		width = m.group('W')
		height = m.group('H')
		for f in os.listdir(dir):
			absf = os.path.join(dir, f)
			if os.path.isfile(absf) and f.endswith(self.ext):
				v = Video(absf, width, height)
				self.Videos.append(v)

def main():
	print "Test suite"
	argc = len(sys.argv)
	config = Config()
	if argc == 2:
		if os.path.exists(sys.argv[1]) and os.path.isdir(sys.argv[1]):
			config.dir = sys.argv[1]
		else:
			print "{0}: directory not exists".format(sys.argv[1])
	else:
		print "Usage: {0}".format(sys.argv[0])
		exit(1)
	config.parse_videos()
	config.verbose()
	

if __name__ == '__main__':
	try:
		main()
	except:
		print "error: ", sys.exc_info()[1]

