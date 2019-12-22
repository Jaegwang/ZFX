##----------##
## Makefile ##
##-------------------------------------------------------##
## author: Wanho Choi @ Dexter Studios                   ##
## last update: 2016.09.09                               ##
##-------------------------------------------------------##

include ../Makefile.base

PROJECT_NAME := ZelosBase

.SILENT:

all: lib$(PROJECT_NAME).so

# .o -> .so
lib$(PROJECT_NAME).so: $(OBJ_LIST)
	#@echo 'Building: $@'
	mkdir -p -m 755 $(TARGET_PATH)/lib
	$(CC) $(LDFLAGS) $(LINKS) -o $(TARGET_PATH)/lib/$@ $(OBJ_LIST)
	cp -rf ./header $(TARGET_PATH)/

# .cpp -> .o
$(OBJECT_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	#@echo 'Compiling: $(notdir $<) -> $(notdir $@)'
	$(CC) $(CCFLAGS) $(INCLUDES) $(SWITCHES) -c $< -o $@
    
#-include $(DEP_LIST)

clean:
	rm -f $(OBJECT_PATH)/*
	rm -f $(TARGET_PATH)/lib/*$(PROJECT_NAME)*
	rm -f documents/*
	rm -rf $(TARGET_PATH)/header

help:
	doxygen

