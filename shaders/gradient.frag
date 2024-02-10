#define PI 3.14159265359

uniform vec2 resolution;

in vec4 ourColor;
in vec2 ourSize;

vec3 colorA = vec3(1, 0.18, 0.18);
vec3 colorB = vec3(0.18, 0.26, 0.65);

void main() {
    vec2 st = gl_FragCoord.xy / resolution;

    float modifier = sin(st.x);

   	vec3 color = mix(colorA, colorB, modifier);

    gl_FragColor = vec4(color, 1.0);
}

