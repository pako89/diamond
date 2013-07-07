#!/usr/bin/python
import sys
import os
import re
import ConfigParser
import datetime

def str2bool(v):
	  return v.lower() in ("yes", "true", "t", "y", "1")

class Video:
	def __init__(self, path, width, height, fmt):
		self.Width = width
		self.Height = height
		self.Format = fmt
		self.Path = path


class Config:
	def __init__(self):
		self.Videos = list()
		self.ConfigParser = ConfigParser.RawConfigParser()

	def parse_config(self, config_file):
		self.ConfigParser.read(config_file)
		# General configuration
		self.Encoder = self.ConfigParser.get('General', 'Encoder')
		self.Decoder = self.ConfigParser.get('General', 'Decoder')
		# Output configuration
		self.TarDecoded = str2bool(self.ConfigParser.get('Output', 'TarDecoded'))
		if self.TarDecoded:
			self.Tar = self.ConfigParser.get('Output', 'Tar')
		self.ResultsDir = self.ConfigParser.get('Output', 'ResultsDir')
		self.wildcard_results_dir()
		# Input Configuration
		self._videos_dir = self.ConfigParser.get('Input', 'VideosDir')
		self._videos_ext = self.ConfigParser.get('Input', 'VideosExt')
		self.parse_videos()
		
	def wildcard_results_dir(self):
		now = datetime.datetime.now()
		date = now.strftime("%Y%m%d")
		time = now.strftime("%H%M%S")
		self.ResultsDir = self.ResultsDir.replace("%d", date).replace("%t", time)

	def print_videos(self):
		print "Directory: {0}".format(self._videos_dir)
		for v in self.Videos:
			print "Video: [{0}x{1}] {2}".format(v.Width, v.Height, v.Path)

	def parse_videos(self):
		for d in os.listdir(self._videos_dir):
			#print "Parsing resolution: {0}".format(d)
			absdir = os.path.join(self._videos_dir, d)
			self.parse_resolution(absdir)

	def parse_resolution(self, dir):
		m = re.search('(?P<W>[0-9]+)x(?P<H>[0-9]+)', dir)
		width = m.group('W')
		height = m.group('H')
		for f in os.listdir(dir):
			#print "Parsing file: {0}".format(f)
			absf = os.path.join(dir, f)
			if os.path.isfile(absf) and f.endswith(self._videos_ext):
				v = Video(absf, width, height, "YUV420")
				self.Videos.append(v)

class Benchmark:
	def __init__(self, config):
		self.Config = config
	
	def run(self):
		print "Running benchmark"


class ResultItem:
	INVALID = "-"
	def __init__(self, desc, reg):
		self.Value = INVALID
		self.Description = desc
		self._reg = reg

	def parse(self, data):
		print "Parsing result item: {0} using regex: {1}".format(self.Description, self._reg)
		print "Parsing result item: {0} --> Value: {1}".format(self.Description, self.Value)

class Results:
	def __init__(self):
		self.Items = list()
		self.Items.append(ResultItem("Total", "(?P<v>)"))
		self.Items.append(ResultItem("DCT", "(?P<v>)"))
		self.Items.append(ResultItem("Quant", "(?P<v>)"))
		self.Items.append(ResultItem("Zig Zag", "(?P<v>)"))
		self.Items.append(ResultItem("RLC", "(?P<v>)"))
		self.Items.append(ResultItem("Copy to Device", "(?P<v>)"))
		self.Items.append(ResultItem("Copy to Host", "(?P<v>)"))
		self.Items.append(ResultItem("Prediction", "(?P<v>)"))
		self.Items.append(ResultItem("Prediction Transform", "(?P<v>)"))
		self.Items.append(ResultItem("Prediction Encoding", "(?P<v>)"))
		self.Items.append(ResultItem("Shift", "(?P<v>)"))

	def parse(self, data):
		for item in self.Items:
			item.parse(data)


def main():
	print "Test suite"
	argc = len(sys.argv)
	config_file = "benchmark.conf"
	if argc == 2:
		if os.path.exists(sys.argv[1]) and os.path.isfile(sys.argv[1]):
			config_file = sys.argv[1]
		else:
			print "{0}: directory not exists".format(sys.argv[1])
	config = Config()
	config.parse_config(config_file)
	config.print_videos()
	

if __name__ == '__main__':
	try:
		main()
	except:
		print "error: ", sys.exc_info()[1]

