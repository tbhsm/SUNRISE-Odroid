attribute vec3 position;
attribute vec2 textureCoord;
attribute vec3 normal;
varying vec2 fragTextureCoord;
varying vec3 fragNormal;
varying vec3 fragPosition;

//varying vec3 vertexPosition;
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main() 
{
    fragPosition = (worldMatrix * vec4(position,1.0)).xyz;
    
    //vertexPosition = 
    fragTextureCoord = textureCoord;
    //normal = normal of object. FragNormal = normal in worldspace
    fragNormal = (worldMatrix * vec4(normal,0.0)).xyz; //xyz -> grabbing only 3 components
    //fragNormal = normal;
    
    //cameraPosition = projMatrix.xyz;
    
    gl_Position = projMatrix * viewMatrix * worldMatrix * vec4(position,1.0);
}