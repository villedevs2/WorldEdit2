#ifdef GL_ES_VERSION_2_0
precision mediump float;
#endif

varying vec2			v_texcoord;
varying float			v_depth;
uniform sampler2D		s_color_texture;

void main()
{
	gl_FragColor = vec4(0.15, 0.78, 0.95, 1.0);

	gl_FragDepth = v_depth;
}