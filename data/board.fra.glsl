#version 100

precision mediump float;

uniform sampler2D uTexture;

varying vec2 vTex;
varying float vIsTex;
varying vec4 vColor;

void main() {
	if (vIsTex > 0.5) {
		gl_FragColor = texture2D(uTexture, vTex) * vColor;
	} else {
		gl_FragColor = vColor;
	}
}
