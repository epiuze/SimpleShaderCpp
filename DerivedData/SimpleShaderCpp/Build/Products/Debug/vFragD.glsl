varying vec3 R;

void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    vec3 N = normalize(gl_NormalMatrix*gl_Normal);
    vec4 eyePos = gl_ModelViewMatrix*gl_Vertex;
    R = reflect(eyePos.xyz, N);
    
}
