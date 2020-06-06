//
//  GcScene.cpp
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GcScene.h"

GRAPHICCORE_BEGIN

Scene::Scene():
_delta(0)
{
    initMatrix();
}
Scene::~Scene()
{
    
}

void Scene::initMatrix()
{
    while (!_modelViewMatrixStack.empty())
    {
        _modelViewMatrixStack.pop();
    }

    while (!_projectionMatrixStack.empty())
    {
         _projectionMatrixStack.pop();
    }

    while (!_textureMatrixStack.empty())
    {
        _textureMatrixStack.pop();
    }
    _modelViewMatrixStack.push(Mat4::IDENTITY);
    _projectionMatrixStack.push(Mat4::IDENTITY);
    _textureMatrixStack.push(Mat4::IDENTITY);
}

void Scene::loadMatrix(MATRIX_STACK_TYPE type, const Mat4& mat)
{
    if(MATRIX_STACK_MODELVIEW == type)
    {
        _modelViewMatrixStack.top() = mat;
    }
    else if(MATRIX_STACK_PROJECTION == type)
    {
        _projectionMatrixStack.top() = mat;
    }
    else if(MATRIX_STACK_TEXTURE == type)
    {
        _textureMatrixStack.top() = mat;
    }
    else
    {
//        CCASSERT(false, "unknown matrix stack type");
    }
}

void Scene::loadIdentityMatrix(MATRIX_STACK_TYPE type)
{
    if(MATRIX_STACK_MODELVIEW == type)
    {
        _modelViewMatrixStack.top() = Mat4::IDENTITY;
    }
    else if(MATRIX_STACK_PROJECTION == type)
    {
        _projectionMatrixStack.top() = Mat4::IDENTITY;
    }
    else if(MATRIX_STACK_TEXTURE == type)
    {
        _textureMatrixStack.top() = Mat4::IDENTITY;
    }
    else
    {
//        CCASSERT(false, "unknown matrix stack type");
    }
}

const Mat4& Scene::getMatrix(MATRIX_STACK_TYPE type) const
{
    if(type == MATRIX_STACK_MODELVIEW)
    {
        return _modelViewMatrixStack.top();
    }
    else if(type == MATRIX_STACK_PROJECTION)
    {
        return _projectionMatrixStack.top();
    }
    else if(type == MATRIX_STACK_TEXTURE)
    {
        return _textureMatrixStack.top();
    }
    
//    CCASSERT(false, "unknown matrix stack type, will return modelview matrix instead");
    return  _modelViewMatrixStack.top();
}
void Scene::pushMatrix(MATRIX_STACK_TYPE type)
{
    if(type == MATRIX_STACK_MODELVIEW)
    {
        _modelViewMatrixStack.push(_modelViewMatrixStack.top());
    }
    else if(type == MATRIX_STACK_PROJECTION)
    {
        _projectionMatrixStack.push(_projectionMatrixStack.top());
    }
    else if(type == MATRIX_STACK_TEXTURE)
    {
        _textureMatrixStack.push(_textureMatrixStack.top());
    }
    else
    {
//        CCASSERT(false, "unknown matrix stack type");
    }
}
void Scene::popMatrix(MATRIX_STACK_TYPE type)
{
    if(MATRIX_STACK_MODELVIEW == type)
    {
        _modelViewMatrixStack.pop();
    }
    else if(MATRIX_STACK_PROJECTION == type)
    {
        _projectionMatrixStack.pop();
    }
    else if(MATRIX_STACK_TEXTURE == type)
    {
        _textureMatrixStack.pop();
    }
    else
    {
//        CCASSERT(false, "unknown matrix stack type");
    }
}

void Scene::multiplyMatrix(MATRIX_STACK_TYPE type, const Mat4& mat)
{
    if(MATRIX_STACK_MODELVIEW == type)
    {
        _modelViewMatrixStack.top() *= mat;
    }
    else if(MATRIX_STACK_PROJECTION == type)
    {
        _projectionMatrixStack.top() *= mat;
    }
    else if(MATRIX_STACK_TEXTURE == type)
    {
        _textureMatrixStack.top() *= mat;
    }
    else
    {
//        CCASSERT(false, "unknown matrix stack type");
    }
}

void Scene::setProjection(Projection projection)
{
    Size size = _viewPort.size;
    
    if (size.width == 0 || size.height == 0)
    {
//        CCLOGERROR("cocos2d: warning, Director::setProjection() failed because size is 0");
        return;
    }
    
//    setViewport();
    
    switch (projection)
    {
        case Projection::_2D:
        {
            Mat4 orthoMatrix;
            Mat4::createOrthographicOffCenter(0, size.width, 0, size.height, -1024, 1024, &orthoMatrix);
            loadMatrix(MATRIX_STACK_PROJECTION, orthoMatrix);
            loadIdentityMatrix(MATRIX_STACK_MODELVIEW);
            break;
        }
            
        case Projection::_3D:
        {
            float zeye = this->getZEye();
            
            Mat4 matrixPerspective, matrixLookup;
            
            // issue #1334
            Mat4::createPerspective(60, size.width/size.height, 10, 1000/*zeye+size.height/2*/, &matrixPerspective);
            
            Vec3 eye(size.width/2, size.height/2, zeye), center(size.width/2, size.height/2, 0.0f), up(0.0f, 1.0f, 0.0f);
            Mat4::createLookAt(eye, center, up, &matrixLookup);
            Mat4 proj3d = matrixPerspective * matrixLookup;
            
            loadMatrix(MATRIX_STACK_PROJECTION, proj3d);
            loadIdentityMatrix(MATRIX_STACK_MODELVIEW);
            break;
        }
            
        case Projection::CUSTOM:
            // Projection Delegate is no longer needed
            // since the event "PROJECTION CHANGED" is emitted
            break;
            
        default:
//            CCLOG("cocos2d: Director: unrecognized projection");
            break;
    }
    
    _projection = projection;
}

float Scene::getZEye(void) const
{
    return (_viewPort.size.height / 1.154700538379252f);//(2 * tanf(M_PI/6))
}


GRAPHICCORE_END
