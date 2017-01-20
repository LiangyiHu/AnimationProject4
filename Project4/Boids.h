#pragma once
// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
// GL Includes
#include <GL/glew.h> // Contains all the necessery OpenGL includes
// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stdlib.h>
#include "Mesh.h"
#include "MatrixTransformation.h"
#include "Model.h"
#include "Shader.h"  
#include "Background.h" //create and compile shader program



class Destination {
public:
	static GLfloat velocity, attractFactor;
	static glm::vec3 currentPosition;
	glm::mat4 currentTransformationMatrix;
	Model* body = new Model("Destination/Destination.dae", 0);
	Shader* objShader = new Shader("VertexShader.txt", "FragmentShader.txt");

	Destination(glm::vec3 initialPosition = glm::vec3(50.0f, 20.0f, 0.0f)) {

		this->currentPosition = initialPosition;
		this->velocity = 100.0f;
		this->currentTransformationMatrix = glm::translate(glm::mat4(), currentPosition);
		objShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(Camera::cameraMatrix));
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "transformMatrix"), 1, GL_FALSE, glm::value_ptr(currentTransformationMatrix));
	}

	~Destination() {
		delete body;
		delete objShader;
	}

	void Draw() {
		this->currentTransformationMatrix = glm::translate(glm::mat4(), currentPosition);
		objShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(Camera::cameraMatrix));
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "transformMatrix"), 1, GL_FALSE, glm::value_ptr(currentTransformationMatrix));
		body->Draw(objShader);
	}
};
GLfloat Destination::velocity;
GLfloat Destination::attractFactor = 5;
glm::vec3 Destination::currentPosition;

class Floor {
public:
	static GLfloat repulseFactor;
	glm::mat4 currentTransformationMatrix;
	Model* body = new Model("Floor/Floor.dae", 0);
	Shader* objShader = new Shader("VertexShader.txt", "FragmentShader.txt");

	Floor() {
		this->currentTransformationMatrix = glm::mat4();
		objShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(Camera::cameraMatrix));
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "transformMatrix"), 1, GL_FALSE, glm::value_ptr(currentTransformationMatrix));
	}
	~Floor() {
		delete body;
		delete objShader;
	}
	void Draw() {
		objShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(Camera::cameraMatrix));
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "transformMatrix"), 1, GL_FALSE, glm::value_ptr(currentTransformationMatrix));
		body->Draw(objShader);
	}
};
GLfloat Floor::repulseFactor = 100.0f;

class Obstacle {
public:
	static GLfloat repulseFactor;
	glm::mat4 currentTransformationMatrix;
	glm::vec3 location;
	GLfloat radius;
	Model* body = new Model("Obstacle/Obstacle.dae", 0);
	Shader* objShader = new Shader("VertexShader.txt", "FragmentShader.txt");

	Obstacle(glm::vec3 location) {
		this->radius = 10.0f;
		this->location = location;
		this->currentTransformationMatrix = glm::translate(glm::mat4(), this->location);
		objShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(Camera::cameraMatrix));
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "transformMatrix"), 1, GL_FALSE, glm::value_ptr(currentTransformationMatrix));
	}
	~Obstacle() {
		delete body;
		delete objShader;
	}
	void Draw() {
		objShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(Camera::cameraMatrix));
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "transformMatrix"), 1, GL_FALSE, glm::value_ptr(currentTransformationMatrix));
		body->Draw(objShader);
	}
};
GLfloat Obstacle::repulseFactor = 800.0f;


class Boid {
public:
	glm::mat4 currentTransformationMatrix, upperBeakMatrix, lowerBeakMatrix;
	glm::vec3 currentPosition, speed, acceleration, force,  deltaPosition;
	static GLfloat attractFactor, repulseFactor;
	GLfloat lastTimeUpdated, deltaTime, maxVelocity=10, mass = 1.0f, beakSpeed = 0.0f, beakAngle = 0.0f;
	bool openDirection = true;

	Model* body = new Model("boid/greenBirdBody.dae", 0);
	Model* upperBeak = new Model("boid/greenBirdUpperBeak.dae", 1);
	Model* lowerBeak = new Model("boid/greenBirdLowerBeak.dae", 2);
	Shader* objShader = new Shader("VertexShader.txt", "FragmentShader.txt");

	Boid() {
		this->beakSpeed =(rand() % 10 + 5)*0.1;
		upperBeakMatrix = glm::rotate(glm::mat4(), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		lowerBeakMatrix = glm::rotate(glm::mat4(), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	}

	void initialization(glm::vec3 initialPosition, glm::vec3 initialSpeed) {
		this->currentPosition = initialPosition;
		this->speed = initialSpeed;
		this->currentTransformationMatrix = getRotationMatrixDirectionAndUp(currentPosition, speed);
		objShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(Camera::cameraMatrix));
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "transformMatrix"), 1, GL_FALSE, glm::value_ptr(currentTransformationMatrix));
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "upperBeak"), 1, GL_FALSE, glm::value_ptr(upperBeakMatrix));
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "lowerBeak"), 1, GL_FALSE, glm::value_ptr(lowerBeakMatrix));
	}
	~Boid() {
		delete body;
		delete upperBeak;
		delete lowerBeak;
		delete objShader;
	}

	void startUpdating() {
		lastTimeUpdated = glfwGetTime();
	}

	void updateMotion() {
		deltaTime = glfwGetTime() - lastTimeUpdated;
		deltaPosition = deltaTime*speed; //+ 0.5f*glm::pow(deltaTime, 2)*acceleration;
		currentPosition = currentPosition + deltaPosition;
		this->currentTransformationMatrix = getRotationMatrixDirectionAndUp(currentPosition, speed);
		lastTimeUpdated = glfwGetTime();
		acceleration = force / mass;
		speed += deltaTime*acceleration;
		if (glm::length(speed) > maxVelocity) {
			speed = maxVelocity*glm::normalize(speed);
		}
		beakAngle += openDirection?deltaTime*beakSpeed: -deltaTime*beakSpeed;
		openDirection = beakAngle > glm::radians(20.0f) ? false : (beakAngle < 0 ? true : openDirection);
		force = glm::vec3(0.0f,0.0f,0.0f);
	}

	void Draw() {
		objShader->Use();
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "cameraMatrix"), 1, GL_FALSE, glm::value_ptr(Camera::cameraMatrix));
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "transformMatrix"), 1, GL_FALSE, glm::value_ptr(currentTransformationMatrix));
		upperBeakMatrix = glm::rotate(glm::mat4(), beakAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		lowerBeakMatrix = glm::rotate(glm::mat4(), -beakAngle, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "upperBeak"), 1, GL_FALSE, glm::value_ptr(upperBeakMatrix));
		glUniformMatrix4fv(glGetUniformLocation(objShader->Program, "lowerBeak"), 1, GL_FALSE, glm::value_ptr(lowerBeakMatrix));
		body->Draw(objShader);
		upperBeak->Draw(objShader);
		lowerBeak->Draw(objShader);
	}

	void updateDynamicBoid(Boid* theOtherBoid) {
		glm::vec3 repulseForce = glm::normalize(this->currentPosition - theOtherBoid->currentPosition) / glm::pow(glm::length(this->currentPosition - theOtherBoid->currentPosition),2)*repulseFactor;
		glm::vec3 attractForce = glm::normalize(theOtherBoid->currentPosition - this->currentPosition)*glm::length(this->currentPosition - theOtherBoid->currentPosition)*attractFactor;
		this->force = this->force + repulseForce + attractForce;
		theOtherBoid->force = theOtherBoid->force - repulseForce - attractForce;
	}
	void updateDestination(Destination* destination) {
		GLfloat distance = glm::length(this->currentPosition - destination->currentPosition);
		if (distance > 30.0f) {
			this->force += glm::normalize(destination->currentPosition - this->currentPosition)*destination->attractFactor;
		}
	}
	void updateFloor() {
		if (currentPosition.y < 20.0f) {
			this->force += glm::vec3(0.0f, Floor::repulseFactor/glm::pow(currentPosition.y,2), 0.0f);
		}
	}
	void updateObstacles(Obstacle* obstacle) {
		GLfloat distance =glm::sqrt(glm::pow(currentPosition.x-obstacle->location.x,2)+glm::pow(currentPosition.z - obstacle->location.z, 2));
		glm::vec3 directionalForce = this->currentPosition - obstacle->location;
		directionalForce = glm::normalize(glm::vec3(directionalForce.x, 0.0f, directionalForce.z));
		glm::vec3 repulseForce = directionalForce / glm::pow(distance-obstacle->radius,2)*Obstacle::repulseFactor;
		this->force += repulseForce;
	}
	
};
GLfloat Boid::attractFactor = 0.02f;
GLfloat Boid::repulseFactor = 800.0f;


