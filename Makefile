MAKE = make --no-print-directory

.PHONY: all clean clean_all build_engine build_tests run_tests build_client

# Default: Compila engine -> compila test -> esegue test -> (se ok) compila client
all: build_client

# 1. Compila l'Engine (Libreria condivisa)
build_engine:
	@echo "=== Building Engine ==="
	@$(MAKE) -C engine

# 2. Compila i Test (dipende dall'Engine)
build_tests: build_engine
	@echo "=== Building TestEngine2 ==="
	@$(MAKE) -C TestEngine2

# 3. Esegue i Test
# Se questo step fallisce, il make si interrompe e non passa al client
run_tests: build_tests
	@echo "=== Running Test Suite ==="
	@$(MAKE) -C TestEngine2 run

# 4. Compila il Client (dipende dal successo dei test)
build_client: run_tests
	@echo "=== Building Client ==="
	@$(MAKE) -C client

clean:
	@echo "=== Cleaning Engine ==="
	@$(MAKE) -C engine clean
	@echo "=== Cleaning Client ==="
	@$(MAKE) -C client clean
	@echo "=== Cleaning TestEngine2 ==="
	@$(MAKE) -C TestEngine2 clean