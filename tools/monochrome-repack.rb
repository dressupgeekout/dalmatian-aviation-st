#!/usr/bin/env ruby
#
# ATARI BITMAP CONVERTER
# Charlotte Koch <dressupgeekout@gmail.com>
#
# - The INPUT FORMAT is: each byte => 1 pixel.
# - The OUTPUT FORMAT is: each word (2 bytes) => 16 pixels.
#

fp = File.open(ARGV.shift, "rb")

until fp.eof?
  # Read 16 pixel's worth of input (=> 16 bytes in this case) 
  pixels = fp.read(16).unpack('C*')

  # Now we're going to squish them all together into a single 16-bit word.
  word = 0x0000

  pixels.each_with_index do |pixel, i|
    # We have "zero or 255" but really what we want is "zero or 1".
    real_pixel = (pixel == 0xFF ? 1 : 0)
    word |= (real_pixel << 15-i)
  end

  $stdout.write([word].pack("S>"))
end

fp.close
