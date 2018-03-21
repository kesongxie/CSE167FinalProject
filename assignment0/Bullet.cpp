//
//  Bullet.cpp
//  assignment0
//
//  Created by Xie kesong on 3/17/18.
//  Copyright © 2018 ___CSE167___. All rights reserved.
//

#include "Bullet.hpp"
#include "shader.h"
#include <math.h>
#define OBJ_VERTEX_SHADER_PATH "./android.vert"
#define OBJ_FRAGMENT_SHADER_PATH "./android.frag"


Bullet::Bullet(OBJObject *bulletObj, Model *t){
    isActive = false;
    target = t;
    targetCenter = target->center;
    transform = glm::mat4(1.0f);
    bulletshaderProgram = LoadShaders(OBJ_VERTEX_SHADER_PATH, OBJ_FRAGMENT_SHADER_PATH);
    obj = bulletObj;
    initTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0, target->center.y, 0)) * glm::scale(glm::mat4(1.0f), glm::vec3(5.0, 5.0, 5.0));
    this->obj->transform = glm::scale(glm::mat4(1.0f), glm::vec3(10.0, 10.0, 10.0));
    this->obj->box->max_x *= 20;
    this->obj->box->min_x *= 20;
    this->obj->box->max_y *= 20;
    this->obj->box->min_y *= 20;
    this->obj->box->max_z *= 20;
    this->obj->box->min_z *= 20;
    
}


void Bullet::draw() {
    glUseProgram(bulletshaderProgram);
    float xDelta = target->center.x - targetCenter.x;
    float yDelta = target->center.y - targetCenter.y;
    this->obj->toWorld = transform * glm::translate(glm::mat4(1.0f), glm::vec3(xDelta, yDelta, 0)) * initTransform;
    this->obj->toWorld_noRot = this->obj->toWorld;
    this->obj->draw(bulletshaderProgram);
    
}

void Bullet::adjustPos() {
//    this->obj->toWorld =  glm::inverse(targetToWorld) * target->toWorld ;
//    targetToWorld = target->toWorld;
}

void Bullet::move(glm::vec3 direction) {
    shootingDistance += abs(direction.z);
    transform = glm::translate(glm::mat4(1.0f), direction) * transform;
    this->obj->toWorld = transform * this->obj->toWorld;
    this->obj->toWorld_noRot = this->obj->toWorld;
}

void Bullet::recycleIfNeeded() {
    if(shootingDistance > 800) {
        isActive = false;
        shootingDistance = 0;
        transform = glm::mat4(1.0f);
        this->obj->toWorld = glm::mat4(1.0f);
    }
}
