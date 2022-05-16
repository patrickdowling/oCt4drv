ifdef VERBOSE
PIO=pio
PIO_OPTS+=-v
Q:=
else
PIO=@pio
PIO_OPTS+=-s
Q:=@
endif

.PHONY: all
all: dev

.PHONY: dev
dev:
	$(PIO) run $(PIO_OPTS) -e dev

.PHONY: prod
prod:
	$(PIO) run $(PIO_OPTS) -e prod

.PHONY: upload
upload:
	$(PIO) run $(PIO_OPTS) --target upload -e dev

.PHONY: upload_prod
upload_prod:
	$(PIO) run $(PIO_OPTS) --target upload -e prod

.PHONY: clean
clean:
	$(PIO) run $(PIO_OPTS) --target clean -e prod -e dev

# clang-format
CLANG_FORMAT_OPTS = -i --style=file
ifdef VERBOSE
	CLANG_FORMAT_OPTS += --verbose
endif
CLANG_FORMAT_DIRS = ./src
.PHONY: format
format:
	$(Q)clang-format $(CLANG_FORMAT_OPTS) \
		$(foreach dir,$(CLANG_FORMAT_DIRS),$(shell find $(dir) -name \*.cc -o -name \*.h))

# cppcheck
CPPSTD=c++14
CPPCHECK ?= cppcheck
CPPCHECK_FLAGS += --enable=all
CPPCHECK_FLAGS += -inconclusives
CPPCHECK_FLAGS += --std=$(CPPSTD)
CPPCHECK_FLAGS += --suppress=missingIncludeSystem
CPPCHECK_FLAGS += --error-exitcode=1
ifdef VERBOSE
CPPCHECK_FLAGS += -v
else
CPPCHECK_FLAGS += --quiet
endif
# CPPCHECK_FLAGS += --check-config

CPPCHECK_SRC_DIRS = ./src -i./src/extern
CPPCHECK_INCLUDES = ./src ./src/extern

.PHONY: check
check:
	$(Q)$(CPPCHECK) $(CPPCHECK_SRC_DIRS) $(addprefix -I,$(CPPCHECK_INCLUDES)) $(CPPCHECK_FLAGS)
