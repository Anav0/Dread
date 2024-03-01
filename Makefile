# Alternative GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug)
  Engine_config = debug
  Game_config = debug
  Window_config = debug

else ifeq ($(config),release)
  Engine_config = release
  Game_config = release
  Window_config = release

else
  $(error "invalid configuration $(config)")
endif

PROJECTS := Engine Game Window

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

Engine:
ifneq (,$(Engine_config))
	@echo "==== Building Engine ($(Engine_config)) ===="
	@${MAKE} --no-print-directory -C Engine -f Makefile config=$(Engine_config)
endif

Game: Engine
ifneq (,$(Game_config))
	@echo "==== Building Game ($(Game_config)) ===="
	@${MAKE} --no-print-directory -C Game -f Makefile config=$(Game_config)
endif

Window: Engine
ifneq (,$(Window_config))
	@echo "==== Building Window ($(Window_config)) ===="
	@${MAKE} --no-print-directory -C Window -f Makefile config=$(Window_config)
endif

clean:
	@${MAKE} --no-print-directory -C Engine -f Makefile clean
	@${MAKE} --no-print-directory -C Game -f Makefile clean
	@${MAKE} --no-print-directory -C Window -f Makefile clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug"
	@echo "  release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   Engine"
	@echo "   Game"
	@echo "   Window"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"