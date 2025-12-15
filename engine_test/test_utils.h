#pragma once
#include <iostream>
#include <cassert>

// Macro per standardizzare l'output
#define TEST_START(name) std::cout << "[RUNNING] " << name << " ... ";
#define TEST_PASS() std::cout << "OK" << std::endl;