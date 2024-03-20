#version 420 core

#define PI 3.14159265359

uniform vec2 resolution;

in vec4 ourColors[4];
in vec4 ourMiddle;
in vec2 ourRadialPosition;
in vec2 ourSize;
in vec2 ourPosition;
in vec2 ourSmoothing;
in float ourRadialFactor;
flat in int ourGradientType;

out vec4 myOutputColor;

void radial(vec2 st, vec4 bg_color, vec4 radial_color, vec2 radial_pos, float factor = 1.0) {
	float d = distance(st, radial_pos) * factor;

	d = smoothstep(ourSmoothing.x, ourSmoothing.y, d);

	myOutputColor = mix(bg_color, radial_color, d);
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
    
    myOutputColor = color;
}

void main() {
  vec2 st = gl_FragCoord.xy / resolution.xy;

	vec2 relative = (ourPosition + ourSize * 0.5) / resolution;
	relative += ourRadialPosition / resolution;

	switch(ourGradientType) {
		case 0:
			radial(st, ourColors[0], ourColors[1], relative, ourRadialFactor);
			break;
		case 1:
			three_color(st);
			break;
	}
   
}

