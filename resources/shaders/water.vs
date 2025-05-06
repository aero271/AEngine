#version 430

in vec3 vertexPosition;
in vec2 vertexTexCoord;

uniform mat4 mvp;
uniform float time;

#define WAVE_COUNT 6

uniform vec2 waveDir[WAVE_COUNT];
uniform float amplitude[WAVE_COUNT];
uniform float frequency[WAVE_COUNT];
uniform float phase[WAVE_COUNT];
uniform float steepness[WAVE_COUNT];

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec3 fragNormal;

float hash(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}

// 2D value noise
float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    // Four corners of a tile
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    // Smooth interpolation
    vec2 u = smoothstep(0.0, 1.0, f);
    
    // Mix between the four corners
    return mix(mix(a, b, u.x), 
               mix(c, d, u.x), u.y);
}

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x * 34.0) + 1.0) * x); }

float simplexNoise(vec2 v) {
    const vec4 C = vec4(0.211324865405187, 0.366025403784439, -0.577350269189626, 0.024390243902439);
    vec2 i = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);

    vec2 i1 = x0.x > x0.y ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
    vec4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    i = mod289(i);
    vec3 p = permute(permute(i.y + vec3(0.0, i1.y, 1.0)) + i.x + vec3(0.0, i1.x, 1.0));

    vec3 m = max(0.5 - vec3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0);
    m = m * m;
    m = m * m;

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    m *= 1.79284291400159 - 0.85373472095314 * (a0 * a0 + h * h);

    vec3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;

    return 130.0 * dot(m, g);
}


vec3 positionCalc(vec3 position)
{
    vec3 displacement = vec3(0.0);

    vec2 nwaveDir[WAVE_COUNT];
    for (int i = 0; i < WAVE_COUNT; i++)
    {
        nwaveDir[i] = normalize(waveDir[i]);
    }

    for(int i = 0; i < WAVE_COUNT; i++) {
        float d = position.x * nwaveDir[i].x + position.z * nwaveDir[i].y;
        float k = phase[i] / frequency[i];
        float omega = sqrt(9.81 * k);

        float sinPhase = sin(d * k - omega * time);
        float cosPhase = cos(d * k - omega * time);

        displacement.x += -steepness[i] * amplitude[i] * nwaveDir[i].x * cosPhase;
        displacement.z += -steepness[i] * amplitude[i] * nwaveDir[i].y * cosPhase;
        displacement.y += amplitude[i] * sinPhase;
    }
    position += displacement / 2;
    
    //apply noise
    
    float noiseFreq = 2.0;
    float noiseAmp = 0.125;
    float noiseY = noise(position.xz);
    position.y += noiseY * noiseAmp;
    
    

    return position;
}

void main()
{
    vec3 position = vertexPosition;
    
    // Calculate wave height
    float height = 0.0;

    position = positionCalc(position);

    vec3 posPlusDX = positionCalc(vertexPosition + vec3(0.01, 0.0, 0.0));
    vec3 posPlusDZ = positionCalc(vertexPosition + vec3(0.0, 0.0, 0.01));


    vec3 normal = normalize(cross(posPlusDX - position, posPlusDZ - position));
    if (normal.y < 0.0) normal.y *= -1;

    fragTexCoord = vertexTexCoord;
    fragNormal = normal;
    fragPosition = position;

    //fragNormal = vec3(0.0, 1.0, 0.0);

    gl_Position = mvp * vec4(position, 1.0);
    //gl_Position = mvp * vec4(vertexPosition, 1.0);
}