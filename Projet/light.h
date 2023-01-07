#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include "shader.h"
#include "actor.h"

class PointLight : public Actor {

public:

	PointLight(Shader lightShader,void*obj, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f)) :lightShader(lightShader)
	{

		lightShader.setInt("NRPointLights", nblights+1);
		isShape = true;
		objToDraw = obj;
		position = pos;
		lightIndex = nblights;
		updateLight();
		scale = glm::vec3(0.2f);
		nblights++;
	}

	void updateLight() {
		lightShader.use();
		std::string pointLight = std::string("pointLights[")+ std::to_string(lightIndex) + "]";
		lightShader.setVec3(pointLight+".position", position);
		lightShader.setVec3(pointLight+".ambient", ambiant);
		lightShader.setVec3(pointLight+".diffuse", diffuse);
		lightShader.setVec3(pointLight+".specular", specular);
		lightShader.setFloat(pointLight+".constant", constant);
		lightShader.setFloat(pointLight+".linear", linear);
		lightShader.setFloat(pointLight+".quadratic", quadratic);
	}

	void setAmbiant(glm::vec3 amb) {
		ambiant = amb;
	}
	void setDiffuse(glm::vec3 dif) {
		diffuse = dif;
	}
	void setSpecular(glm::vec3 spec) {
		specular = spec;
	}

private:
	inline static unsigned int nblights = 0;
	glm::vec3 ambiant = glm::vec3(0.01f, 0.01f, 0.01f);
	glm::vec3 diffuse = glm::vec3(1.0f, 0.8f, 0.6f);
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
	unsigned int lightIndex;
	float constant = 1;
	float linear = 0.09f;
	float quadratic = 0.032f;
	Shader lightShader;
};


class DirectionalLight
{
public:

	DirectionalLight(Shader lightShader, glm::vec3 dir) :lightShader(lightShader),direction(dir) {
		updateLight();
	};

	void updateLight() {
		lightShader.setVec3("dirLight.direction", direction);
		lightShader.setVec3("dirLight.ambient", ambiant);
		lightShader.setVec3("dirLight.diffuse", diffuse);
		lightShader.setVec3("dirLight.specular", specular);
	}
	void setAmbiant(glm::vec3 amb) {
		ambiant = amb;
		updateLight();
	}
	void setDiffuse(glm::vec3 dif) {
		diffuse = dif;
		updateLight();
	}
	void setSpecular(glm::vec3 spec) {
		specular = spec;
		updateLight();
	}
private:

	glm::vec3 direction;
	glm::vec3 ambiant = glm::vec3(0.05f, 0.05f, 0.05f);
	glm::vec3 diffuse = glm::vec3(.1f, .1f, .1f);
	glm::vec3 specular = glm::vec3(0.5f, 0.5f, 0.5f);
	Shader lightShader;
};

#endif