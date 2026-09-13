#include "light.h"
namespace eng {

    /**
     * @brief Imposta il colore diffuso (diffuse) della luce.
     * La componente diffusa rappresenta l'illuminazione principale di un oggetto:
     * i raggi luminosi colpiscono la superficie e si disperdono in tutte le direzioni,
     * dando colore e volume all'oggetto senza creare riflessi netti.
     * @param c Vettore 3D (RGB) che rappresenta il colore diffuso.
     */
    void ENG_API Light::set_diffuse_color(const glm::vec3& c) { diffuseColor = c; }

    /**
     * @brief Imposta il colore speculare (specular) della luce.
     * La componente speculare è responsabile dei classici "punti luce" (highlights)
     * sulle superfici lucide o metalliche. A differenza di quella diffusa, questa componente
     * dipende fortemente dall'angolo di visuale della telecamera.
     * @param c Vettore 3D (RGB) che rappresenta il colore speculare.
     */
    void ENG_API Light::set_specular_color(const glm::vec3& c) { specularColor = c; }

    /**
     * @brief Restituisce il colore diffuso attuale della luce.
     * @return glm::vec3 Il vettore RGB del colore diffuso.
     */
    glm::vec3 ENG_API Light::get_diffuse_color() const { return diffuseColor; }

    /**
     * @brief Restituisce il colore speculare attuale della luce.
     * @return glm::vec3 Il vettore RGB del colore speculare.
     */
    glm::vec3 ENG_API Light::get_specular_color() const { return specularColor; }


}