require "socket"

s = TCPSocket.open("localhost", 5555)

while gets
  s.write($_)
  print(s.gets)
end
s.close
