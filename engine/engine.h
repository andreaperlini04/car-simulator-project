/**
 * @file engine.h
 * @brief Header principale del motore grafico.
 */
#pragma once

#include <memory>
#include <functional>
#include <string>
#include "libConfig.h"
#include "camera.h"
#include "list.h"
#include <chrono>


namespace Eng {

   // Definizioni dei tipi di callback per il Client

   /** @brief Callback invocata per il rendering del frame. */
   using DisplayCallback = std::function<void()>;
   /** @brief Callback invocata al ridimensionamento della finestra. */
   using ReshapeCallback = std::function<void(int width, int height)>;
   /** @brief Callback invocata alla pressione di un tasto standard. */
   using KeyboardCallback = std::function<void(unsigned char key, int x, int y)>;
   /** @brief Callback invocata alla pressione di un tasto speciale. */
   using SpecialCallback = std::function<void(int key, int x, int y)>;

   /**
    * @class Base
    * @brief Classe Singleton che gestisce il ciclo di vita del motore grafico, la finestra e il loop di rendering.
    */
   class ENG_API Base final
   {
   public:
      // Singleton

      /**
       * @brief Restituisce l'istanza unica della classe Base (Singleton).
       * @return Riferimento all'istanza del motore.
       */
      static Base& getInstance();

      // Ciclo di vita
      /**
       * @brief Inizializza il sottosistema grafico e le librerie dipendenti.
       * @param argc Numero di argomenti da riga di comando.
       * @param argv Array degli argomenti da riga di comando.
       * @return True se l'inizializzazione ha successo, False altrimenti.
       */
      bool init(int argc, char* argv[]);

      /**
       * @brief Libera le risorse allocate dal motore grafico.
       * @return True se la liberazione delle risorse ha successo.
       */
      bool free();

      /**
       * @brief Imposta la lista degli oggetti da renderizzare nella scena principale.
       * @param list Puntatore alla lista di oggetti.
       */
      void setRenderList(List* list);

      /**
       * @brief Imposta la lista degli oggetti per il calcolo delle riflessioni.
       * @param list Puntatore alla lista di oggetti riflessi.
       */
      void setReflectionList(List* list);

      /**
       * @brief Assegna la telecamera principale utilizzata per il rendering.
       * @param camera Puntatore alla camera da utilizzare.
       */
      void setMainCamera(Camera* camera);

      // Gestione Finestra e Loop
      /**
       * @brief Crea la finestra di visualizzazione.
       * @param width Larghezza della finestra in pixel.
       * @param height Altezza della finestra in pixel.
       * @param x Posizione orizzontale iniziale dello schermo.
       * @param y Posizione verticale iniziale dello schermo.
       * @param title Titolo della finestra.
       */
      void createWindow(int width, int height, int x, int y, const char* title);
      
      /**
       * @brief Avvia il ciclo principale (main loop) del motore grafico.
       */
      void update(); // Avvia il main loop

      // Rendering
      /**
       * @brief Esegue la procedura di rendering della scena corrente.
       */
      void render();

      // Callback Registration (Il client usa queste per agganciare la logica)
     
      /**
       * @brief Registra la funzione di callback per il rendering definita dall'utente.
       * @param cb Funzione da invocare.
       */
      void setDisplayCallback(DisplayCallback cb);

      /**
       * @brief Registra la funzione di callback per il ridimensionamento della finestra.
       * @param cb Funzione da invocare.
       */
      void setReshapeCallback(ReshapeCallback cb);

      /**
       * @brief Registra la funzione di callback per l'input da tastiera standard.
       * @param cb Funzione da invocare.
       */
      void setKeyboardCallback(KeyboardCallback cb);

      /**
       * @brief Registra la funzione di callback per l'input di tasti speciali.
       * @param cb Funzione da invocare.
       */
      void setSpecialCallback(SpecialCallback cb);

      // Utility per il Client

      /**
       * @brief Imposta il colore di pulizia dello sfondo (background).
       * @param r Componente rossa (0.0 - 1.0).
       * @param g Componente verde (0.0 - 1.0).
       * @param b Componente blu (0.0 - 1.0).
       * @param a Componente alpha (0.0 - 1.0).
       */
      void setClearColor(float r, float g, float b, float a);

      /**
       * @brief Abilita o disabilita il calcolo dell'illuminazione globale.
       * @param enable True per abilitare, False per disabilitare.
       */
      void setLighting(bool enable);

      /**
       * @brief Abilita o disabilita l'uso delle texture 2D.
       * @param enable True per abilitare, False per disabilitare.
       */
      void setTexture(bool enable);

      // --- METODI PER LA GESTIONE EVENTI (Chiamati dai wrapper statici) ---

      /**
       * @brief Gestisce internamente la richiesta di ridisegno della scena.
       */
      void handleDisplayRequest();

      /**
       * @brief Gestisce internamente l'evento di ridimensionamento della finestra.
       * @param width Nuova larghezza della finestra.
       * @param height Nuova altezza della finestra.
       */
      void handleReshapeRequest(int width, int height);

      /**
       * @brief Gestisce internamente l'evento di pressione di un tasto standard.
       * @param key Codice ASCII del tasto premuto.
       * @param x Coordinata X del mouse al momento della pressione.
       * @param y Coordinata Y del mouse al momento della pressione.
       */
      void handleKeyboardRequest(unsigned char key, int x, int y);

      /**
       * @brief Gestisce internamente l'evento di pressione di un tasto speciale.
       * @param key Codice del tasto speciale premuto.
       * @param x Coordinata X del mouse al momento della pressione.
       * @param y Coordinata Y del mouse al momento della pressione.
       */
      void handleSpecialRequest(int key, int x, int y);

      // No copy
      Base(Base const&) = delete;
      void operator=(Base const&) = delete;

      /**
       * @brief Abilita la visualizzazione dei frame per secondo (FPS) a schermo.
       */
      void enableFPS();

      /**
       * @brief Disabilita la visualizzazione dei frame per secondo.
       */
      void disableFPS();

      /**
       * @brief Aggiunge un testo alla lista interna degli elementi da visualizzare.
       * @param text La stringa di testo da aggiungere.
       */
      void calculateFPS();

      /**
       * @brief Accoda una stringa al buffer di testo visualizzato a schermo (overlay).
       * @param text La stringa da visualizzare.
       */
      void addToScreenText(std::string text);

      /**
       * @brief Rimuove tutto il testo presente nel buffer di visualizzazione a schermo.
       */
      void clearScreenText();

      /**
       * @brief Segnala al sistema che la finestra necessita di essere ridisegnata.
       */
      void postRedisplay();
      
      /**
       * @brief Aggiunge una stringa colorata in una posizione specifica dello schermo.
       * @param x Coordinata X.
       * @param y Coordinata Y.
       * @param text Testo da visualizzare.
       * @param r Componente rossa del colore.
       * @param g Componente verde del colore.
       * @param b Componente blu del colore.
       */
      void addString(float x, float y, std::string text, float r, float g, float b);

      /**
       * @brief Restituisce la larghezza corrente della finestra.
       * @return Larghezza in pixel.
       */
      int getWindowWidth();

      /**
       * @brief Restituisce l'altezza corrente della finestra.
       * @return Altezza in pixel.
       */
      int getWindowHeight();

      /**
       * @brief Calcola la larghezza in pixel di una stringa di testo renderizzata.
       * @param text La stringa di cui calcolare la lunghezza.
       * @return Larghezza in pixel.
       */
      int getTextWidth(const std::string& text);

   private:
      Base();
      ~Base();

      // PIMPL: Nasconde i dettagli privati (es. header GLUT)
      struct Reserved;
      std::unique_ptr<Reserved> reserved;
    
      ////FPS:
      bool show_fps;
      int frameCounter = 0;
      float fps = 0.0f;


      /*
      * @brief Timestamp dell'ultimo aggiornamento del frame per il calcolo del delta time.
      */
      std::chrono::time_point<std::chrono::steady_clock> lastTime;
   
   };

} // namespace Eng