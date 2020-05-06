//
//  GcScene.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef GRAPHICCORE_SCENE_H
#define GRAPHICCORE_SCENE_H

#include <stack>
#include "base/ColorTypes.h"
#include "opengl/GLEngine.h"

GRAPHICCORE_BEGIN

//class Node;

/**
 * @brief Matrix stack type.
 */
enum MATRIX_STACK_TYPE
{
    /// Model view matrix stack
    MATRIX_STACK_MODELVIEW,
    
    /// projection matrix stack
    MATRIX_STACK_PROJECTION,
    
    /// texture matrix stack
    MATRIX_STACK_TEXTURE
};

enum class Projection
{
    /// Sets a 2D projection (orthogonal projection).
    _2D,
    /// Sets a 3D projection with a fovy=60, znear=0.5f and zfar=1500.
    _3D,
    /// It calls "updateProjection" on the projection delegate.
    CUSTOM,
    /// Default projection is 3D projection.
    DEFAULT = _3D,
};

class Scene {
public:

    Scene();
    ~Scene();
    
    void initMatrix();
    
    void loadIdentityMatrix(MATRIX_STACK_TYPE type);
    void loadMatrix(MATRIX_STACK_TYPE type, const Mat4& mat);
    const Mat4& getMatrix(MATRIX_STACK_TYPE type) const;
    void pushMatrix(MATRIX_STACK_TYPE type);
    void popMatrix(MATRIX_STACK_TYPE type);
    void multiplyMatrix(MATRIX_STACK_TYPE type, const Mat4& mat);
    
    void setViewPort(const Rect& viewPort) { _viewPort = viewPort; }
    const Rect& getViewPort() const { return _viewPort; }
//    void setRootNode(GhNode *rootNode) { _rootNode = rootNode; }
    
    void setProjection(Projection projection);
    
    //time stamp
    void setDelta(int64_t delta) { _delta = delta; }
    const int64_t getDelta() const { return _delta; }
    
    void setGLEngine(GLEngine* gle) { _gle = gle; }
    GLEngine* const getGLEngine() const { return _gle; }
    
private:
    
    inline float getZEye(void) const;
    
    std::stack<Mat4> _modelViewMatrixStack;
    std::stack<Mat4> _projectionMatrixStack;
    std::stack<Mat4> _textureMatrixStack;
    
    Rect _viewPort;
//    Node *_rootNode;
    int64_t _delta;
    
    GLEngine* _gle;
    
    Projection _projection;
};

GRAPHICCORE_END

#endif /* GRAPHICCORE_SCENE_H */
