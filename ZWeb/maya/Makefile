##----------##
## Makefile ##
##-------------------------------------------------------##
## author: Wanho Choi @ Dexter Studios                   ##
##         Jaegwang Lim @ Dexter Studios                 ##
## last update: 2019.01.15                               ##
##-------------------------------------------------------##

ifndef MAYA_VER
	MAYA_VER := 2017
endif

ifndef RMAN_VER
	RMAN_VER := 21.4
endif

include ../../Makefile.base

PROJECT_NAME := ZWebForMaya
LINKS        += -lOpenMaya -lOpenMayaAnim -lFoundation -lOpenMayaUI -lOpenMayaFX -lopenvdb
LINKS        += -lZelosBase -lZelosMayaBase$(MAYA_VER)

.SILENT:

all: $(PROJECT_NAME).so

# .o -> .so
$(PROJECT_NAME).so: $(OBJ_LIST)
	#@echo 'Building: $@'
	mkdir -p -m 755 $(TARGET_PATH)/maya/$(MAYA_VER)/plug-ins
	$(CC) $(LDFLAGS) $(LINKS) -o $(TARGET_PATH)/maya/$(MAYA_VER)/plug-ins/$@ $(OBJ_LIST)
	mkdir -p -m 755 $(TARGET_PATH)/maya/$(MAYA_VER)/scripts
	rm -f $(addprefix $(TARGET_PATH)/maya/$(MAYA_VER)/scripts/, $(MEL_LIST))
	cp -f $(SCRIPTS) $(TARGET_PATH)/maya/$(MAYA_VER)/scripts
	cp -f ./icons/*.png $(TARGET_PATH)/maya/$(MAYA_VER)/icons

# .cpp -> .o
$(OBJECT_PATH)/%.o: $(SOURCE_PATH)/%.cpp
	#@echo 'Compiling: $(notdir $<) -> $(notdir $@)'
	$(CC) $(CCFLAGS) $(INCLUDES) $(SWITCHES) -c $< -o $@

maya:
	@./maya.dev $(MAYA_VER) $(RMAN_VER) $(TARGET_PATH)

clean:
	rm -f $(OBJECT_PATH)/*
	rm -f $(TARGET_PATH)/maya/$(MAYA_VER)/plug-ins/*$(PROJECT_NAME)*
	rm -f $(addprefix $(TARGET_PATH)/maya/$(MAYA_VER)/scripts/, $(MEL_LIST))

