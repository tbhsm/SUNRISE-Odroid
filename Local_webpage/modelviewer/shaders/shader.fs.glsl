precision mediump float;
varying vec2 fragTextureCoord;
varying vec3 fragNormal;
varying vec3 fragPosition;

uniform sampler2D sampler;
uniform float ambientReflCoef;
uniform float diffuseReflCoef;
uniform float specularReflCoef;
uniform float shininessCoef;
uniform vec3 viewerPosition;

//light properties:
uniform vec3 lightPosition;
uniform vec3 lightIntensity;

void main() {
//ambient
vec3 ambientIntensity = vec3(0.8,0.8,0.8);

//diffuse

//specular
//vec3 specularIntensity = vec3(0.8,0.8,0.8);

vec3 lightLocation = normalize(lightPosition-fragPosition);

vec4 texel = texture2D(sampler,fragTextureCoord);

//Specular lightning not working yet:

//vec3 directionVector = (2.0*dot(sunLocation,fragNormal)*fragNormal)-sunLocation;

//vec3 specular = specularReflCoef*pow((dot(directionVector,viewerPosition)),shininessCoef)*specularIntensity;

//phong shading:
vec3 lightIntensity = ambientIntensity*ambientReflCoef + max(diffuseReflCoef*(dot(fragNormal,lightLocation)*lightIntensity),0.0); //maxmum zero aka no negative values

//gl_FragColor = vec4(fragNormal,1.0);

//gl_FragColor = texel;

//discard the color white in the textures so transparent textures are possible
if (texel.rgb == vec3(25.0/255.0,255.0/255.0,0.0/255.0))
{
    discard;
}
else
{
    gl_FragColor = vec4(texel.rgb * lightIntensity,texel.a);
}

//vec3 removeColor = vec3(173, 173, 173);

//gl_FragColor = vec4(texel.rgb * lightIntensity, (0.9 - dot(removeColor, texel.rgb * lightIntensity) / (length(texel.rgb * lightIntensity))));
///gl_FragColor = vec4(specular,1.0);

}
