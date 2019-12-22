##----------##
## Makefile ##
##-------------------------------------------------------##
## author: Wanho Choi @ Dexter Studios                   ##
## last update: 2016.09.09                               ##
##-------------------------------------------------------##

include ../Makefile.base

PROJECT_NAME := ZelosMayaBase$(MAYA_VER)

LINKS        += -lOpenMaya -lOpenMayaAnim -lFoundation -lOpenMayaUI -lOpenMayaFX
LINKS        += -lZelosBase

.SILENT:

all: lib$(PROJECT_NAME).so

# .o -> .so
lib$(PROJECT_NAME).so: $(OBJ_LIST)
	#@echo 'Building: $@'
	mkdir -p -m 755 $(TARGET_PATH)/lib
	$(CC) $(LDFLAGS) $(LINKS) -o $(TARGET_PATH)/lib/$@ $(OBJ_LIST)
	mkdir -p -m 755 $(TARGET_PATH)/maya/$(MAYA_VER)/scripts
	rm -f $(addprefix $(TARGET_PATH)/maya/$(MAYA_VER)/scripts/, $(MEL_LIST))
	cp -f $(SCRIPTS) $(TARGET_PATH)/maya/$(MAYA_VER)/scripts
	rm -rf $(TARGET_PATH)/maya/$(MAYA_VER)/icons
	mkdir -p -m 755 $(TARGET_PATH)/maya/$(MAYA_VER)/icons

# .cpp -> .o
$(OBJECT_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	#@echo 'Compiling: $(notdir $<) -> $(notdir $@)'
	$(CC) $(CCFLAGS) $(INCLUDES) $(SWITCHES) -c $< -o $@

clean:
	rm -f $(OBJECT_PATH)/*
	rm -f $(TARGET_PATH)/lib/*$(PROJECT_NAME)*
	rm -f $(addprefix $(TARGET_PATH)/maya/$(MAYA_VER)/scripts/, $(MEL_LIST))

