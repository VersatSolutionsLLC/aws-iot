#
# AUTO-GENERATED Makefile; do not edit, changes will be lost
#

# Define target type
TARGET:=wp76xx

# Custom makefile include: initialization
-include ../customInit.mk

# Only when not cleaning
ifneq ($(MAKECMDGOALS),clean)
endif

# Target artifact.*
TARGET_ARTIFACT:=versConfigService.$(TARGET).update

# Clean command
CLEANCOMMAND:=find -maxdepth 1 -mindepth 1 ! -name Makefile ! -name .rsync-rules -exec rm -R {} \;

# MK command
MKCOMMAND:=cd ..;mkapp -t ar7 -o Target_Legato_Debug -w Target_Legato_Debug -C -g -X -g -L -g versConfigService.adef

# Custom makefile include: definitions
-include ../customDefs.mk

.PHONY: all target clean codegen mkCodegen  airvantage sync syncClean

all: target sync

codegen: mkCodegen sync

mkCodegen:
	$(MKCOMMAND) --generate-code

target:
	$(MKCOMMAND)


sync:
	rsync --copy-links --recursive --checksum --delete --delete-excluded --prune-empty-dirs --filter=". .rsync-rules" /tmp/build/versConfigService/Target_Legato_Debug .. 2>/dev/null || true

clean: syncClean
	$(CLEANCOMMAND)

syncClean:
	cd /tmp/build/versConfigService/Target_Legato_Debug; $(CLEANCOMMAND)

# Custom makefile include: targets
-include ../customTargets.mk
