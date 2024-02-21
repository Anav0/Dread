#define PI 3.14159265359

uniform vec2 resolution;

in vec4 ourColors[4];
in vec4 ourMiddle;
in int ourGradientType;

void radial(vec2 st, vec4 bg, vec4 radial, vec2 pos, float factor = 1.0) {
	float pct = distance(st, pos) * factor;

	gl_FragColor = mix(bg, radial, pct);
}

void three_color(vec2 st) {
	float v = st.x;

    vec4 pct = vec4(v);

	float middle_l = ourMiddle.x;
	float middle_h = ourMiddle.y;
	
	vec4 color;

    if(v < middle_l) {
		color = mix(ourColors[0], ourColors[1], pct / middle_l);
	}
    
    if(v > middle_h) {
        color = mix(ourColors[1], ourColors[2], pct - middle_h);
	}
    
    if(v > middle_l && v < middle_h)
        color = ourColors[1];
    
    gl_FragColor = color;
}

void main() {
    vec2 st = gl_FragCoord.xy / resolution.xy;

	if (ourGradientType == 0) {
		radial(st, ourColors[0], ourColors[1], vec2(0.5, 0.5), 1.2);
	}

	if (ourGradientType == 1) {
		three_color(st);
	}
   
}

