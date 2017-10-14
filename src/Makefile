.PHONY : clean install uninstall

SRC_FILES = $(wildcard *.cpp)
OBJ_FILES = $(SRC_FILES:%.cpp=../obj/%.o)
INSTALL_PATH?=/usr
CFLAGS = -std=c++11
LFLAGS = `pkg-config gtkmm-3.0 --cflags --libs` `curl-config --cflags --libs` -ltinyxml -pthread
INC=-I./

all: dirs ../bin/file_downloader

dirs:
	@mkdir -p ../obj
	@mkdir -p ../bin

../bin/file_downloader: $(OBJ_FILES) 
	@echo Linking...
	@g++ $(OBJ_FILES) -o $@ $(LFLAGS)

$(OBJ_FILES): ../obj/%.o: %.cpp
	@echo Compiling...
	@g++ $< -c -o $@ $(INC) $(CFLAGS) $(LFLAGS)

clean:
	@echo Cleaning...
	@rm -rf ../obj
	@rm -rf ../bin

install: all
	@echo Installing...
	@cp ../bin/file_downloader $(INSTALL_PATH)/bin
	@cp fd.glade $(INSTALL_PATH)/share

uninstall: 
	@echo Uninstalling...
	@rm -f $(INSTALL_PATH)/bin/file_downloader
	@rm -f $(INSTALL_PATH)/share/fd.glade
