#ifdef GL_ES_VERSION_2_0
precision mediump float;
#endif

varying vec2			v_texcoord;
varying float			v_depth;
uniform sampler2D		s_color_texture;

void main()
{
	gl_FragColor = texture2D(s_color_texture, v_texcoord);
	gl_FragDepth = v_depth;
}