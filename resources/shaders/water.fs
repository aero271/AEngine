#version 430

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

uniform vec3 viewPos;
uniform samplerCube enviorment;
uniform samplerCube irradiance;
uniform int isSkybox;

out vec4 finalColor;

void main()
{
    vec3 waterColor = vec3(0.06,0.13,0.39);

    vec3 I = normalize(fragPosition - viewPos);
    vec3 R = reflect(I, normalize(fragNormal));

    vec4 reflectionColor = vec4(0.0);
    if (isSkybox == 1) reflectionColor = texture(enviorment, R);
    else reflectionColor = vec4(waterColor, 1.0);

    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(-I);

    //Fresnel-Schlick equation, calculates how much of color should be reflections and how much should be the base color
    float fresnel = 0.02 + 0.98 * pow(1.0 - max(dot(normal, viewDir), 0.0), 5.0);

    finalColor = vec4(mix(waterColor, reflectionColor.rgb, 1.0 * fresnel).rgb, 1.0);
}