
#include "shader_code.h"

#define STRINGIFY(A)  #A

namespace QMedia {
namespace RenderEngine {


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
precision mediump float;
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
precision mediump float;
#ifdef GL_ES
varying highp vec2 v_texCoord;
#else
varying vec2 v_texCoord;
#endif

uniform sampler2D uTexture;
uniform vec4 uColor;

void main()
{
//    vec4 value = texture2D(uTexture, v_texCoord);
//    value = dot(value , uColor);
    gl_FragColor = texture2D(uTexture, v_texCoord) * uColor;
}
)";

}//End RenderEngine
}//End MMC
