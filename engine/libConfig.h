/**
 * @file libConfig.h
 * @brief Header di configurazione per la gestione delle macro di esportazione/importazione e compatibilità multipiattaforma.
 */
#pragma once

 // Export API:
#ifdef _WINDOWS
   // Specifies i/o linkage (VC++ spec):
#ifdef ENGINE_EXPORTS
/** * @def ENG_API
 * @brief Definisce la direttiva di esportazione simboli per la compilazione della DLL (Windows).
 */
#define ENG_API __declspec(dllexport)
#else
/** * @def ENG_API
 * @brief Definisce la direttiva di importazione simboli per l'utilizzo della DLL (Windows).
 */
#define ENG_API __declspec(dllimport)
#endif      

 // Get rid of annoying warnings:
 /** @brief Disabilita il warning C4251 relativo all'esportazione di classi contenenti membri STL (specifico MSVC). */
#pragma warning(disable : 4251) 
#else // Under linux
/** * @def ENG_API
 * @brief Definizione vuota della macro API per la compilazione su sistemi non Windows (Linux/Unix).
 */
#define ENG_API
#endif

