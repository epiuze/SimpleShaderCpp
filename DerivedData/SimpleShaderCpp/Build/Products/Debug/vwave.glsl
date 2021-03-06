    uniform float time;
void main()
{
    vec4 t = gl_Vertex;
    t.y = 0.1*sin(0.001*time+5.0*gl_Vertex.x)*sin(0.001*time+5.0*gl_Vertex.z);
    // t.y = 0.1*sin(0.01*time*gl_Vertex.x)*sin(0.01*time*gl_Vertex.z);
    gl_Position = gl_ModelViewProjectionMatrix * t;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec4 eyePosition = gl_ModelViewMatrix * gl_Vertex;
    vec4 eyeLightPos = gl_LightSource[0].position;
    vec3 N = normalize(gl_NormalMatrix * gl_Normal);
    vec3 L = normalize(eyeLightPos.xyz - eyePosition.xyz);
    vec3 E = -normalize(eyePosition.xyz);
    vec3 H = normalize(L + E);
    float Kd = max(dot(L, N), 0.0);
    float Ks = pow(max(dot(N, H), 0.0), gl_FrontMaterial.shininess);
    float Ka = 0.0;

    ambient = Ka*gl_FrontLightProduct[0].ambient;
    diffuse = Kd*gl_FrontLightProduct[0].diffuse;
    specular = Ks*gl_FrontLightProduct[0].specular;
   gl_FrontColor = ambient+diffuse+specular;
}