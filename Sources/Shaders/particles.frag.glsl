#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D text;

varying vec2 texCoord;
varying vec4 tintCol;

void kore() {
	gl_FragColor = texture2D(text, texCoord) * tintCol;
}