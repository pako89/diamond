#!/usr/bin/python
import sys
import os
import re
import ConfigParser
import datetime
import commands
import string
import collections

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
		self.RemoveDecoded = str2bool(self.ConfigParser.get('Output', 'RemoveDecoded'))
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

class EncoderConfig:
	def __init__(self, video, variant, interpol, huffman, gop):
		self.Video = video
		self.Variant = variant
		self.Interpol = interpol
		self.Huffman = huffman
		self.GOP = gop
	
	def get_video_variant(self):
		return self.Video.get_name() + self.get_variant()

	def get_variant(self):
		s = "_"
		s += "V"+self.Variant
		s += "H"+self.Huffman
		s += "I"+self.Interpol
		s += "G"+self.GOP
		return s

class Benchmark:
	SEPARATOR_LEN = 40
	def __init__(self, config):
		self.Config = config
		if not os.path.exists(self.Config.ResultsDir):
			os.makedirs(self.Config.ResultsDir)
		self.flog = open(os.path.join(self.Config.ResultsDir, "benchmark.log"), 'w')
		self.Results = list()
	
	def run(self):
		print "Running benchmark"
		i = 1
		for v in self.Config.Videos:
			for I in self.Config.Interpolation:
				for H in self.Config.Huffman:
					for V in self.Config.EncoderVariant:
						for G in self.Config.GOP:
							cfg = EncoderConfig(v, V, I, H, G)
							self._run_item(cfg, i)
							i = i+1
	
	def save_results(self):
		resf = open(os.path.join(self.Config.ResultsDir, self.Config.ResultsName), 'w')
		self._write_csv_header(resf)
		for r in self.Results:
			r.create_dict()
			for i in r.Dict:
				self._write_csv(resf, r.Dict[i])
			resf.write('\n')
		resf.close()

	def _log(self, data):
		self.flog.write(data)
	
	def _log_gsep(self):
		self.flog.write('='*self.SEPARATOR_LEN+"\n")
	
	def _log_lsep(self):
		self.flog.write('-'*self.SEPARATOR_LEN+"\n")

	def _out(self, data):
		sys.stdout.write(data)
		sys.stdout.flush()

	def _run_encoder(self, cfg, bstr):
		self._out("Encoding...")
		cmd = Command(self.Config.Encoder)
		cmd.add_arg(self.Config.EncoderArgs)
		cmd.add_option("--interpolation", cfg.Interpol)
		cmd.add_option("--huffman", cfg.Huffman)
		cmd.add_option("--variant", cfg.Variant)
		cmd.add_option("--gop", cfg.GOP)
		self._append_video(cfg.Video, cmd, bstr)
		self._log_lsep()
		self._log(cmd.Command+'\n')
		cmd.run()
		stdout = cmd.get_stdout()
		self._log_lsep()
		self._log(stdout+'\n')
		self._out("Done\n")
		return stdout

	def _parse_results(self, cfg, data):
		self._out("Parsing results...")
		result = Result(cfg)
		result.parse(data)
		self.Results.append(result)
		self._out("Done\n")
		
	def _run_decoder(self, output, bstr):
		self._out("Decoding...")
		cmd = Command(self.Config.Decoder)
		cmd.add_arg(self.Config.DecoderArgs)
		cmd.add_option("--output", output)
		cmd.add_arg(bstr)
		self._log_lsep()
		self._log(cmd.Command+'\n')
		cmd.run()
		stdout = cmd.get_stdout()
		self._log_lsep()
		self._log(stdout+'\n')
		self._out("Done\n")

	def _run_tar(self, tar, output):
		self._out("Tarring...")
		self._log_lsep()
		tarcmd = self.Config.Tar.replace('%O', tar).replace('%I', output)
		cmd = Command(tarcmd)
		self._log(cmd.Command+'\n')
		cmd.run()
		self._out("Done\n")

	def _remove_output(self, output):
		self._out("Removing decoded video...")
		os.remove(output)
		self._out("Done\n")

	
	def _run_item(self, cfg, i):
		self._log_gsep()
		log = "Run {0}/{1}\n".format(i, self.Config.get_count())
		self._log(log)
		self._out(log)
		bstr = self._get_out_path(cfg.get_video_variant() + ".bstr")
		output = self._get_out_path(cfg.get_video_variant() + ".yuv")
		tar = self._get_out_path(cfg.get_video_variant())
		stdout = self._run_encoder(cfg, bstr) 
		self._parse_results(cfg, stdout)
		self._run_decoder(output, bstr)
		if self.Config.TarDecoded:
			self._run_tar(tar, output)
		if self.Config.RemoveDecoded:
			self._remove_output(output)

	def _get_out_path(self, name):
		return os.path.join(self.Config.ResultsDir, name)

	def _append_video(self, video, cmd, output):
		cmd.add_option("--type", self.Config.VideosFormat)
		cmd.add_option("--height", video.Height)
		cmd.add_option("--width", video.Width)
		cmd.add_option("--output", output)
		cmd.add_arg(video.Path)

	def _write_csv(self, resf, data):
		resf.write(data + ';')
	
	def _write_csv_header(self, resf):
		if len(self.Results) > 0:
			r = self.Results[0]
			r.create_dict()
			for i in r.Dict:
				self._write_csv(resf, i)



class ResultItem:
	INVALID = "-"
	def __init__(self, desc, reg):
		self.Value = self.INVALID
		self.Description = desc
		self._reg = reg

	def parse(self, data):
		if self.Value == self.INVALID:
			m = re.search(self._reg, data)
			if m != None:
				self.Value = m.group('v')

class Result:
	def __init__(self, cfg):
		self.EncoderConfig = cfg
		self.Items = list()
		self.Items.append(ResultItem("Total", self.create_regex("Timer total")))
		self.Items.append(ResultItem("DCT", self.create_regex("Timer DCT")))
		self.Items.append(ResultItem("Quant", self.create_regex("Timer Quant")))
		self.Items.append(ResultItem("Zig Zag", self.create_regex("Timer Zig Zag")))
		self.Items.append(ResultItem("RLC", self.create_regex("Timer RLC")))
		self.Dict = collections.OrderedDict()
		self.Order = list()

	def create_regex(self, text):
		return "{0}\s*:\s*(?P<v>[0-9]+\.[0-9]+)".format(text)
	
	def create_dict(self):
		self.Dict['Variant'] = self.EncoderConfig.Variant
		self.Dict['Interpolation'] = self.EncoderConfig.Interpol
		self.Dict['Huffman'] = self.EncoderConfig.Huffman
		self.Dict['GOP'] = self.EncoderConfig.GOP
		for i in self.Items:
			self.Dict[i.Description] = i.Value

	def parse(self, data):
		for item in self.Items:
			item.parse(data)


def main():
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
	benchmark.save_results()
	

if __name__ == '__main__':
	try:
		main()
	except:
		print "error: ", sys.exc_info()[1]

