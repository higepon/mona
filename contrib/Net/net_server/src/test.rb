require "socket"
print("start\n");
s = TCPSocket.open("localhost", 5555)

(1..10).each {|i|
  s.write("Hello I'm packet #{i}")
  print("sent packet #{i}")
  print(s.gets)
}
s.close
