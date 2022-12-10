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

# Get the initial color
pixel = infile_fp.read(1).unpack(UCHAR_STAR)[0]
this_color = pixel
infile_fp.rewind

# OK now we can start for real
counter = 0

until infile_fp.eof?
  while pixel == this_color
    byte = infile_fp.read(1)
    break if not byte # XXX skip
    pixel = byte.unpack(UCHAR_STAR)[0]

    if (pixel == this_color) && (counter < 128)
       # Run continues up to a maximum
      counter += 1
    else
      # The run is over, commit what we have learned
      #
      # The input says "zero or 255" but really what we want is "zero or 1"
      #
      # The output will be 1 byte: LSB contains the color (0 or 1) and the
      # rest of the bits encode the run-count
      real_color = (this_color == 0xFF ? 1 : 0)
      out = counter
      out |= (real_color << 7)
      outfile_fp.write([out].pack(UCHAR_STAR))
      $stderr.puts([counter, real_color].inspect)
      counter = 0
      this_color = pixel
    end
  end
end

infile_fp.close
outfile_fp.close
