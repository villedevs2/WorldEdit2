#ifdef GL_ES_VERSION_2_0
precision mediump float;
#endif

varying vec2			v_texcoord;
varying float			v_depth;
varying vec4			v_color;
uniform sampler2D		s_color_texture;

void main()
{
	gl_FragColor = texture2D(s_color_texture, v_texcoord) * v_color;
	gl_FragDepth = v_depth;
}