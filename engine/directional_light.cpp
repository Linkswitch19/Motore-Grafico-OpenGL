#include "directional_light.h"
namespace eng {
    /**
     * @brief Imposta la direzione verso cui punta la luce direzionale.
     * * Trattandosi di una luce direzionale (es. la luce del sole), la sua origine
     * è considerata all'infinito e tutti i raggi luminosi viaggiano paralleli lungo
     * questo vettore.
     * * @param d Vettore 3D che rappresenta la direzione della luce.
     */
    void ENG_API DirectionalLight::set_direction(const glm::vec3& d) { direction = d; }
    /**
     * @brief Restituisce la direzione attuale della luce direzionale.
     * * @return glm::vec3 Il vettore direzione della luce.
     */

    glm::vec3 ENG_API DirectionalLight::get_direction() const { return direction; }

    
   
}