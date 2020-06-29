/****************************************************************************
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2012 		cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "ccShaders.h"

#define STRINGIFY(A)  #A

GRAPHICCORE_BEGIN
//

#include "ccShader_Position_uColor.frag"

#include "ccShader_Position_uColor.vert"

//
#include "ccShader_PositionColor.frag"
#include "ccShader_PositionColor.vert"

//
#include "ccShader_PositionColorTextureAsPointsize.vert"

//
#include "ccShader_PositionTexture.frag"
#include "ccShader_PositionTexture.vert"

//
#include "ccShader_PositionTextureA8Color.frag"
#include "ccShader_PositionTextureA8Color.vert"

//
#include "ccShader_PositionTextureColor.frag"
#include "ccShader_PositionTextureColor.vert"

//
#include "ccShader_PositionTextureColor_noMVP.frag"
#include "ccShader_PositionTextureColor_noMVP.vert"

//
#include "ccShader_PositionTextureColorAlphaTest.frag"

//
#include "ccShader_PositionTexture_uColor.frag"
#include "ccShader_PositionTexture_uColor.vert"

#include "ccShader_PositionColorLengthTexture.frag"
#include "ccShader_PositionColorLengthTexture.vert"

#include "ccShader_UI_Gray.frag"
//
#include "ccShader_Label.vert"
#include "ccShader_Label_df.frag"
#include "ccShader_Label_df_glow.frag"
#include "ccShader_Label_normal.frag"
#include "ccShader_Label_outline.frag"

//
#include "ccShader_3D_PositionTex.vert"
#include "ccShader_3D_Color.frag"
#include "ccShader_3D_ColorTex.frag"
#include "ccShader_3D_PositionNormalTex.vert"
#include "ccShader_3D_ColorNormal.frag"
#include "ccShader_3D_ColorNormalTex.frag"
#include "ccShader_3D_Particle.vert"
#include "ccShader_3D_Particle.frag"
#include "ccShader_3D_Skybox.vert"
#include "ccShader_3D_Skybox.frag"
#include "ccShader_3D_Terrain.vert"
#include "ccShader_3D_Terrain.frag"
#include "ccShader_CameraClear.vert"
#include "ccShader_CameraClear.frag"

// ETC1 ALPHA support
#include "ccShader_ETC1AS_PositionTextureColor.frag"
#include "ccShader_ETC1AS_PositionTextureGray.frag"

#include "ccShader_Position.vert"
#include "ccShader_LayerRadialGradient.frag"


const char* base2dTexture_vert = R"(
attribute vec4 a_position;
attribute vec4 a_texCoord;

#ifdef GL_ES
varying highp vec2 textureCoordinate;
#else
varying vec2 textureCoordinate;
#endif

void main()
{
    gl_Position = a_position;
    textureCoordinate = a_texCoord.xy;
}
)";

const char* defaultPositionTexture_vert = R"(
uniform mat4 uMVPMatrix;
uniform mat4 uTexMatrix;
attribute vec4 a_position;
attribute vec4 a_texCoord;

#ifdef GL_ES
varying highp vec2 v_texCoord;
#else
varying vec2 v_texCoord;
#endif

void main()
{
    gl_Position = uMVPMatrix * a_position;
    v_texCoord = (uTexMatrix * a_texCoord).xy;
}
)";

const char* defaultPositionTexture_frag = R"(
#ifdef GL_ES
  precision highp float;
#endif
varying vec2 v_texCoord;

uniform sampler2D uTexture;
uniform vec4 uColor;

void main()
{
//    vec4 value = texture2D(uTexture, v_texCoord);
//    value = dot(value , uColor);
    gl_FragColor = texture2D(uTexture, v_texCoord) * uColor;
}
)";

GRAPHICCORE_END
