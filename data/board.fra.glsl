#version 100

precision mediump float;

uniform sampler2D uTexture;

varying vec2 vTex;
varying float vIsTex;
varying vec4 vColor;

void main() {
	if (vIsTex > 0.5) {
		vec4 v = texture2D(uTexture, vTex);
		gl_FragColor = vec4(v.r * vColor.r, v.g * vColor.g, v.b * vColor.b, v.a * vColor.a);
	} else {
		gl_FragColor = vColor;
	}
}
