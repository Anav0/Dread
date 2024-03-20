#version 420 core

uniform vec2 u_resolution;

out vec4 finalColor;
in vec4 ourColor;
in vec2 ourSize;

void main(){
	vec2 st = gl_FragCoord.xy / u_resolution;

  float pct = distance(st, vec2(0.5)) * 1.5;

	finalColor = vec4(pct);
}

