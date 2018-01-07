#version 100

precision mediump float;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform float uIsTex;

varying vec2 vTex;

void main() {
	if (uIsTex > 0.5) {
		gl_FragColor = texture2D(uTexture, vTex) * uColor;
	} else {
		gl_FragColor = uColor;
	}
}
