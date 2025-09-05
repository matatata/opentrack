
all: tool.exe tool64.exe

SRCS = main.cpp registryhelper.cpp utils.cpp
HDRS = registryhelper.h utils.h

tool.exe: $(SRCS) $(HDRS)
	i686-w64-mingw32-g++ -static -municode -mconsole  $(SRCS) -o tool

tool64.exe: $(SRCS) $(HDRS)
	x86_64-w64-mingw32-g++ -static -municode -mconsole $(SRCS) -o tool64


clean:
	rm -f tool.exe tool64.exe


