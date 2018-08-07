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
TARGET_ARTIFACT:=verasatIot.$(TARGET).update

# Clean command
CLEANCOMMAND:=find -maxdepth 1 -mindepth 1 ! -name Makefile ! -name .rsync-rules -exec rm -R {} \;

# MK command
MKCOMMAND:=cd ..;mkapp -t $(TARGET) -o /tmp/build/verasatIot/Target_Legato_Debug -w /tmp/build/verasatIot/Target_Legato_Debug -i "$(LEGATO_ROOT)/interfaces/portService" -i "$(LEGATO_ROOT)/interfaces/supervisor" -i "$(LEGATO_ROOT)/interfaces" -i "$(LEGATO_ROOT)/interfaces/airVantage/legacy" -i "$(LEGATO_ROOT)/interfaces/positioning" -i "$(LEGATO_ROOT)/interfaces/airVantage" -i "$(LEGATO_ROOT)/interfaces/logDaemon" -i "$(LEGATO_ROOT)/interfaces/atServices" -i "$(LEGATO_ROOT)/interfaces/watchdog" -i "$(LEGATO_ROOT)/interfaces/wifi" -i "$(LEGATO_ROOT)/interfaces/secureStorage" -i "$(LEGATO_ROOT)/interfaces/modemServices" -s "$(LEGATO_ROOT)/components/dataConnectionService/platformAdaptor/default" -s "$(LEGATO_ROOT)/apps/platformServices/airVantageConnector" -s "$(LEGATO_ROOT)/components/airVantage" -s "$(LEGATO_ROOT)/modules/WiFi/apps/tools/wifi" -s "$(LEGATO_ROOT)/components/positioning" -s "." -s "$(LEGATO_ROOT)/components/secStore" -s "$(LEGATO_ROOT)/components/uartMode/platformAdaptor/default" -s "$(LEGATO_ROOT)/components/portService" -s "$(LEGATO_ROOT)/components/modemServices/platformAdaptor/default" -s "$(LEGATO_ROOT)/components/audio/platformAdaptor/default" -s "$(LEGATO_ROOT)/components/3rdParty" -s "$(LEGATO_ROOT)/apps/tools" -s "$(LEGATO_ROOT)/components/fwupdate/platformAdaptor/default" -s "$(LEGATO_ROOT)/components/dataConnectionService" -s "$(LEGATO_ROOT)/modules/WiFi/service" -s "$(LEGATO_ROOT)/components/airVantage/platformAdaptor/default" -s "$(LEGATO_ROOT)/components/fwupdate" -s "$(LEGATO_ROOT)/components/positioning/platformAdaptor/default" -s "$(LEGATO_ROOT)/components/devMode" -s "$(LEGATO_ROOT)/components/secStore/platformAdaptor/default" -s "$(LEGATO_ROOT)/components/atServices" -s "$(LEGATO_ROOT)/components/modemServices" -s "$(LEGATO_ROOT)/components/uartMode" -s "$(LEGATO_ROOT)/components" -C -g -X -g -L -g verasatIot.adef

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
	rsync --copy-links --recursive --checksum --delete --delete-excluded --prune-empty-dirs --filter=". .rsync-rules" /tmp/build/verasatIot/Target_Legato_Debug .. 2>/dev/null || true

clean: syncClean
	$(CLEANCOMMAND)

syncClean:
	cd /tmp/build/verasatIot/Target_Legato_Debug; $(CLEANCOMMAND)

# Custom makefile include: targets
-include ../customTargets.mk
