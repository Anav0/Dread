#define PI 3.14159265359

uniform vec2 resolution;

in vec4 ourColorA;
in vec4 ourColorB;
in vec4 ourColorC;

vec2 middle = vec2(0.3, 0.65);

void main() {

	vec4 colorA = ourColorA;
	vec4 colorB = ourColorB;
	vec4 colorC = ourColorC;

    vec2 st = gl_FragCoord.xy / resolution.xy;
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    
    float v = st.x;

    vec4 pct = vec4(v);

	float middle_l = middle.x;
	float middle_h = middle.y;
    
    if(v < middle_l) {
        color = mix(colorA, colorB, pct / middle_l);
	}
    
    if(v > middle_h) {
        color = mix(colorB, colorC, pct - middle_h);
	}
    
    if(v > middle_l && v < middle_h)
        color = colorB;
    
    gl_FragColor = color;
}

