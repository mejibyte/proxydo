require 'socket'   
require 'benchmark'

header = "GET / HTTP/1.1\r\n" +
"Host: www.google.com\r\n" +
"Connection: close\r\n" +
"\r\n"

a = ""
b = ""
Benchmark.bm(10) do |x|
  
  x.report("proxied") do
    100.times do 
      s = TCPSocket.open("localhost", 12345)
      s.puts(header)
      while line = s.gets
        unless line =~ /Set-Cookie/ || line =~ /Date/
          a += line 
        end
      end
      s.close
    end
  end
  
  x.report("unproxied") do
    100.times do 
      s = TCPSocket.open("www.google.com", 80)
      s.puts(header)
      while line = s.gets
        unless line =~ /Set-Cookie/ || line =~ /Date/
          b += line 
        end
      end
      s.close
    end    
  end
end


puts a == b