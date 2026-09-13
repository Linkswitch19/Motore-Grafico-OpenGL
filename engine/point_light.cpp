#include "point_light.h"
namespace eng {

    /**
     * @brief Imposta il raggio di influenza della luce puntiforme.
     * Il raggio determina la distanza massima alla quale la luce ha effetto
     * sull'ambiente circostante. Viene tipicamente utilizzato dal motore grafico
     * per calcolare l'attenuazione (decadimento) della luce man mano che i
     * frammenti si allontanano dall'origine della sorgente luminosa.
     * @param r Il valore del raggio (distanza massima di illuminazione).
     */
    void ENG_API PointLight::set_radius(float r) { radius = r; }

    /**
     * @brief Restituisce il raggio di influenza attuale della luce.
     * @return float Il raggio della luce puntiforme.
     */
    float ENG_API PointLight::get_radius() const { return radius; }

}