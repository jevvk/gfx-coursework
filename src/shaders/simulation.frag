#version 330

out vec4 outColor;
in vec4 velocity;

uniform sampler2D tex;

void main(void) {
	outColor = texture(tex, gl_PointCoord) * vec4(1.0f, 1.0f, 1.0f, 1.0f);
	// outColor = vec4(1.0, 1.0, 1.0, 1.0);
}
