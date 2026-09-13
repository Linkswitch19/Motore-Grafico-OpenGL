#include "spot_light.h"
namespace eng {

    /**
    * @brief Imposta la direzione verso cui è orientata la luce spot.
    * * @param d Vettore 3D che rappresenta la nuova direzione della luce.
    */

    void ENG_API SpotLight::set_direction(const glm::vec3& d) { direction = d; }


    /**
     * @brief Imposta l'angolo di apertura (cutoff) del cono di luce.
     * * Valori tipici in OpenGL vanno da 0.0 a 90.0 gradi per un cono reale,
     * oppure 180.0 per indicare una luce che irradia in tutte le direzioni.
     * * @param c L'angolo di cutoff in gradi.
     */

    void ENG_API SpotLight::set_cutoff(float c) { cutoff = c; }

    /**
     * @brief Imposta il raggio di influenza massima della luce.
     * * Oltre questa distanza, l'intensità della luce decade a zero.
     * * @param r Il raggio di influenza della luce.
     */
    void ENG_API SpotLight::set_radius(float r) { radius = r; }

    /**
     * @brief Imposta l'esponente di attenuazione spaziale (focus) della luce.
     * * Valori più alti rendono il centro del cono di luce più intenso rispetto ai bordi,
     * creando un effetto di luce più "focalizzata" o "concentrata".
     * * @param e Il valore dell'esponente di attenuazione.
     */
    void ENG_API SpotLight::set_exponent(float e) { exponent = e; }
    /**
     * @brief Restituisce la direzione attuale verso cui punta la luce.
     * * @return glm::vec3 Il vettore direzione della luce.
     */
    glm::vec3 ENG_API SpotLight::get_direction() const { return direction; }

    /**
     * @brief Restituisce l'angolo di apertura (cutoff) del cono di luce.
     * * @return float L'angolo di cutoff in gradi.
     */
    float ENG_API SpotLight::get_cutoff() const { return cutoff; }

    /**
     * @brief Restituisce il raggio di influenza massima della luce.
     * * @return float Il raggio della luce.
     */
    float ENG_API SpotLight::get_radius() const { return radius; }

    /**
     * @brief Restituisce l'esponente di attenuazione (focus) della luce.
     * * @return float L'esponente di attenuazione.
     */
    float ENG_API SpotLight::get_exponent() const { return exponent; }
}