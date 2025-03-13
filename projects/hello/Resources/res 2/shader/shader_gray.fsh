varying vec2 v_texCoord;
uniform sampler2D u_texture;
uniform vec4 u_shadowColor;
uniform vec2 u_shadowOffset;

void main()
{
    vec4 color = texture2D(u_texture, v_texCoord);
    vec4 shadowColor = texture2D(u_texture, v_texCoord + vec2(0.05, -0.05));
    gl_FragColor = vec4(vec4(0.0,0.0,0.0,1.0).rgb * shadowColor.a, 0.5);
}
