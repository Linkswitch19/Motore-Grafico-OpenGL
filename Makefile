# Makefile Generale
MAKE = make --no-print-directory

.PHONY: all clean clean_all build_engine build_client

all: build_engine build_client

build_engine:
	@echo "=== Building Engine ==="
	@$(MAKE) -C engine

build_client: build_engine
	@echo "=== Building Client ==="
	@$(MAKE) -C client

clean:
	@echo "=== Cleaning Engine ==="
	@$(MAKE) -C engine clean
	@echo "=== Cleaning Client ==="
	@$(MAKE) -C client clean
