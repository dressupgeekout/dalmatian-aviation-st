#!/usr/bin/env ruby
#
# ATARI BITMAP CONVERTER
# Charlotte's Format
#
# - The INPUT FORMAT is: each byte => 1 pixel.
# - The OUTPUT FORMAT is: each word (2 bytes) => 16 pixels and extra
# metadata
#

require 'optparse'

MAGIC_NUMBER = 'YB'.freeze
UINT16_T_BE = "S>*".freeze
UCHAR_STAR = "C*".freeze

infile = nil
outfile = nil
width = nil
height = nil

# Parse command line options.
parser = OptionParser.new do |opts|
  opts.on("-i", "--input PATH", "file to convert") { |path| infile = File.expand_path(path) }
  opts.on("-o", "--output PATH", "(optional) defaults to stdout") { |path| outfile = File.expand_path(path) }
  opts.on("-w", "--width N") { |w| width = w.to_i }
  opts.on("-h", "--height N") { |h| height = h.to_i }
end
parser.parse!(ARGV)

[infile, width, height].each do |arg|
  if not arg
    $stderr.puts("FATAL: missing an arg")
    exit 1
  end
end

# Let's go:
infile_fp = File.open(infile, "rb")

if outfile
  outfile_fp = File.open(outfile, "wb")
else
  outfile_fp = $stdout
end

# 0x000 - 0x0002: Magic number
outfile_fp.write(MAGIC_NUMBER)

# 0x003 - 0x0006: The dimensions of the picture
outfile_fp.write([width, height].pack(UINT16_T_BE))

until infile_fp.eof?
  # Read 16 pixel's worth of input (which is 16 bytes) 
  pixels = infile_fp.read(16).unpack(UCHAR_STAR)

  # Now we're going to squish them all together into a single 16-bit word.
  word = 0x0000

  pixels.each_with_index do |pixel, i|
    # Input says "zero or 255" but really what we want is "zero or 1".
    real_pixel = (pixel == 0xFF ? 1 : 0)
    word |= (real_pixel << 15-i)
  end

  outfile_fp.write([word].pack(UINT16_T_BE))
end

infile_fp.close
outfile_fp.close
