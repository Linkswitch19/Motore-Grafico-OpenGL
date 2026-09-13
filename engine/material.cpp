#include "material.h"

namespace eng {

	/**
	 * @brief Imposta il nome identificativo del materiale.
	 * @param n Stringa contenente il nome.
	 */
	void ENG_API Material::set_name(const std::string& n) { name = n; }

	/**
	 * @brief Restituisce il nome del materiale.
	 * @return std::string Il nome del materiale.
	 */
	std::string ENG_API Material::get_name() const { return name; }

	/**
	 * @brief Imposta il colore di emissione del materiale.
	 * Il colore emissivo fa apparire l'oggetto come se brillasse di luce propria,
	 * indipendentemente dall'illuminazione esterna della scena. Non illumina
	 * gli altri oggetti, ma rende l'oggetto stesso luminoso al buio.
	 * @param c Vettore 3D (RGB) del colore emissivo.
	 */
	void ENG_API Material::set_emission_color(const glm::vec3& c) { emission = c; }

	/**
	 * @brief Imposta il colore ambientale del materiale.
	 * Indica come il materiale riflette la luce ambientale globale della scena.
	 * Di solito si imposta a un valore simile o identico al colore diffuso, ma più scuro.
	 * @param c Vettore 3D (RGB) del colore ambientale.
	 */
	void ENG_API Material::set_ambient_color(const glm::vec3& c) { ambient = c; }

	/**
	 * @brief Imposta il colore diffuso del materiale.
	 * Rappresenta il "colore vero" dell'oggetto quando viene colpito direttamente
	 * da una sorgente luminosa (riflessione diffusa o Lambertiana).
	 * @param c Vettore 3D (RGB) del colore diffuso.
	 */
	void ENG_API Material::set_diffuse_color(const glm::vec3& c) { diffuse = c; }

	/**
	 * @brief Imposta il colore speculare del materiale.
	 * Definisce il colore dei riflessi diretti (highlights) creati dalle luci sulla
	 * superficie. Per materiali plastici è solitamente bianco, per i metalli
	 * prende il colore del metallo stesso.
	 * @param c Vettore 3D (RGB) del colore speculare.
	 */
	void ENG_API Material::set_specular_color(const glm::vec3& c) { specular = c; }

	/**
	 * @brief Imposta il fattore di lucidità (shininess) del materiale.
	 * Regola la dimensione e la nitidezza del riflesso speculare. Valori più alti
	 * creano un riflesso più piccolo e concentrato (es. plastica liscia o vetro),
	 * valori bassi creano un riflesso ampio (es. superfici ruvide).
	 * @param s Valore float della lucidità (solitamente tra 0.0 e 128.0 in OpenGL).
	 */
	void ENG_API Material::set_shininess(float s) { shininess = s; }

	/**
	 * @brief Associa una texture 2D al materiale.
	 * Se presente, i colori campionati dalla texture verranno fusi (modulati)
	 * con i colori del materiale durante il rendering.
	 * @param tex Puntatore condiviso all'oggetto Texture.
	 */
	void ENG_API Material::set_texture(std::shared_ptr<Texture> tex) { texture = tex; }



	/**
	 * @brief Restituisce il colore emissivo del materiale.
	 * @return glm::vec3 Il vettore RGB emissivo.
	 */
	glm::vec3 Material::get_emission_color() const {
		return this->emission;
	}

	/**
	 * @brief Restituisce il colore ambientale del materiale.
	 * @return glm::vec3 Il vettore RGB ambientale.
	 */
	glm::vec3 Material::get_ambient_color() const {
		return this->ambient;
	}

	/**
	 * @brief Restituisce il colore diffuso del materiale.
	 * @return glm::vec3 Il vettore RGB diffuso.
	 */
	glm::vec3 Material::get_diffuse_color() const {
		return this->diffuse;
	}

	/**
	 * @brief Restituisce il colore speculare del materiale.
	 * @return glm::vec3 Il vettore RGB speculare.
	 */
	glm::vec3 Material::get_specular_color() const {
		return this->specular;
	}

	/**
	 * @brief Restituisce il fattore di lucidità (shininess) del materiale.
	 * @return float Il valore di shininess.
	 */
	float Material::get_shininess() const {
		return this->shininess;
	}

	/**
	 * @brief Restituisce la texture 2D correntemente associata al materiale.
	 * @return std::shared_ptr<Texture> Puntatore alla texture, oppure nullptr se non presente.
	 */
	std::shared_ptr<Texture> Material::get_texture() const {
		return this->texture;
	}

}