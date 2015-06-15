

FILE_INCLUDE           = /home/hrd/analyze/include
       
INCLUDES :=  -I$(FILE_INCLUDE)

CXX     = g++
LINK    = g++

CXXFLAGS       += $(shell pkg-config --cflags opencv) -lavformat -lavcodec -lavfilter -lavutil -lswresample -lswscale -lz
LDLIBS         += $(shell pkg-config --libs  opencv)  -lpthread -lrt -lm -levent
CXXFLAGS       += -D__STDC_CONSTANT_MACROS

OBJ = main.o \
      CamThread.o \
      TcpClient.o\
      TcpServer.o\
      camera.o\
      algorithm.o\
      interface.o\
      SingleCamera.o\
      SmokeDetector.o\
      Wavelet.o\
	  rtptrans.o

TARGET  =  analyze_server

#all: $(TARGET)

$(TARGET): $(OBJ)
	$(LINK) $(INCLUDES)  -o $(TARGET) $(OBJ)   $(CXXFLAGS) $(LDLIBS)  
	rm $(OBJ)

%.o: %.cpp
	$(CXX)  -Wall -O2 $(INCLUDES) -o $@ -c $^  $(CXXFLAGS) 

.PHONY: clean
clean:
	rm  $(TARGET)


