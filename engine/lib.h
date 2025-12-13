#pragma once

/////////////
// VERSION //
/////////////

#define DEBUG(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
#define WARN(fmt, ...)  printf("[WARN]  " fmt "\n", ##__VA_ARGS__)
#define ERROR(fmt, ...) printf("[ERROR] " fmt "\n", ##__VA_ARGS__)

   // Generic info:
#ifdef _DEBUG
#define LIB_NAME      "My Graphics Engine v0.1a (debug)"   ///< Library credits
#else
#define LIB_NAME      "My Graphics Engine v0.1a"   ///< Library credits
#endif
#define LIB_VERSION   10                           ///< Library version (divide by 10)

// Export API:
#ifdef _WINDOWS
   // Specifies i/o linkage (VC++ spec):
#ifdef ENGINE_EXPORTS
#define ENG_API __declspec(dllexport)
#else
#define ENG_API __declspec(dllimport)
extern "C"
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif      

// Get rid of annoying warnings:
#pragma warning(disable : 4251) 
#else // Under linux
#define ENG_API
#endif
