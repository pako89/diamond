#!/usr/bin/python
import sys
import os
import re
import ConfigParser
import datetime
import commands
import string

def str2bool(v):
	  return v.lower() in ("yes", "true", "t", "y", "1")

class Video:
	def __init__(self, path, width, height, fmt):
		self.Width = width
		self.Height = height
		self.Format = fmt
		self.Path = path
	
	def get_name(self):
		base = os.path.basename(self.Path)
		return os.path.splitext(base)[0]


class Config:
	def __init__(self):
		self.Videos = list()
		self.ConfigParser = ConfigParser.RawConfigParser()

	def parse_config(self, config_file):
		self.ConfigParser.read(config_file)
		# General configuration
		self.Encoder = self.ConfigParser.get('General', 'Encoder')
		self.EncoderArgs = self.ConfigParser.get('General', 'EncoderArgs')
		self.Decoder = self.ConfigParser.get('General', 'Decoder')
		self.DecoderArgs = self.ConfigParser.get('General', 'DecoderArgs')
		# Output configuration
		self.TarDecoded = str2bool(self.ConfigParser.get('Output', 'TarDecoded'))
		if self.TarDecoded:
			self.Tar = self.ConfigParser.get('Output', 'Tar')
		self.ResultsDir = self.ConfigParser.get('Output', 'ResultsDir')
		self.wildcard_results_dir()
		self.ResultsName = self.ConfigParser.get('Output', 'ResultsName')
		# Input Configuration
		self._videos_dir = self.ConfigParser.get('Input', 'VideosDir')
		self._videos_ext = self.ConfigParser.get('Input', 'VideosExt')
		self.VideosFormat = self.ConfigParser.get('Input', 'VideosFormat')
		self.parse_videos()
		# Options configuration
		self.Interpolation = string.split(self.ConfigParser.get('Options', 'Interpolation'), ' ')
		self.Huffman = string.split(self.ConfigParser.get('Options', 'Huffman'), ' ')
		self.EncoderVariant = string.split(self.ConfigParser.get('Options', 'EncoderVariant'), ' ')
		self.GOP = string.split(self.ConfigParser.get('Options', 'GOP'), ' ')

	def get_count(self):
		r = 1
		r *= len(self.Videos)
		r *= len(self.Interpolation)
		r *= len(self.Huffman)
		r *= len(self.EncoderVariant)
		r *= len(self.GOP)
		return r
	
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

class Command:
	def __init__(self, cmd):
		self.Command = cmd
		self._exit = 0
		self._stdout = ""

	def add_arg(self, arg):
		self.Command += " " + arg
	
	def add_option(self, arg, val):
		self.Command += " " + arg + " " + val

	def get_exit(self):
		return self._exit

	def get_stdout(self):
		return self._stdout

	def run(self):
		[s, o] = commands.getstatusoutput(self.Command)
		self._exit = s
		self._stdout = o



class Benchmark:
	SEPARATOR_LEN = 40
	def __init__(self, config):
		self.Config = config
		self.flog = open(os.path.join(self.Config.ResultsDir, "benchmark.log"), 'w')

	def log(self, data):
		self.flog.write(data)
	
	def log_gsep(self):
		self.flog.write('='*self.SEPARATOR_LEN+"\n")
	
	def log_lsep(self):
		self.flog.write('-'*self.SEPARATOR_LEN+"\n")

	def run(self):
		print "Running benchmark"
		if not os.path.exists(self.Config.ResultsDir):
			os.makedirs(self.Config.ResultsDir)
		counter = 1
		for v in self.Config.Videos:
			for i in self.Config.Interpolation:
				for h in self.Config.Huffman:
					for ev in self.Config.EncoderVariant:
						for g in self.Config.GOP:
							self.run_encoder(v, i, h, ev, g, counter)
							counter = counter+1
	def run_encoder(self, v, I, H, V, G, i):
		cmd = Command(self.Config.Encoder)
		cmd.add_arg(self.Config.EncoderArgs)
		cmd.add_option("--interpolation", I)
		cmd.add_option("--huffman", H)
		cmd.add_option("--variant", V)
		cmd.add_option("--gop", G)
		self.append_video(v, cmd, self.get_variant(I, H, V, G))
		self.log_gsep()
		self.log("Running command {0}/{1}:\n{2}\n".format(i, self.Config.get_count(), cmd.Command))
		print "Running command {0}/{1}: {2}".format(i, self.Config.get_count(), cmd.Command)

	
	def get_variant(self, I, H, V, G):
		s = "_"
		s += "V"+V
		s += "H"+H
		s += "I"+I
		s += "G"+G
		return s

	def get_out_path(self, name):
		return os.path.join(self.Config.ResultsDir, name)

	def append_video(self, video, cmd, variant):
		cmd.add_option("--type", self.Config.VideosFormat)
		cmd.add_option("--height", video.Height)
		cmd.add_option("--width", video.Width)
		cmd.add_option("--output", self.get_out_path(video.get_name() + variant + ".bstr"))
		cmd.add_arg(video.Path)


class ResultItem:
	INVALID = "-"
	def __init__(self, desc, reg):
		self.Value = INVALID
		self.Description = desc
		self._reg = reg

	def parse(self, data):
		print "Parsing result item: {0} using regex: {1}".format(self.Description, self._reg)
		print "Parsing result item: {0} --> Value: {1}".format(self.Description, self.Value)

class Result:
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
	benchmark = Benchmark(config)
	benchmark.run()
	

if __name__ == '__main__':
	try:
		main()
	except:
		print "error: ", sys.exc_info()[1]

