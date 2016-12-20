#version 100

precision mediump float;

uniform vec4 uColor;
uniform sampler2D uTexture;

varying vec2 vTex;

void main() {
	gl_FragColor = texture2D(uTexture, vTex) * uColor;
}
