varying vec3 N;
varying vec3 L;
varying vec3 E;

void main()
{
    const vec4 yellow = vec4(1.0, 1.0, 0.0, 1.0);
    const vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
    const vec4 black = vec4(0.0, 0.0, 0.0, 1.0);

    float Kd = max(dot(L, N), 0.0);

    // if(Kd > 0.6) gl_FragColor = yellow;
      // else gl_FragColor = red;
    gl_FragColor = mix(red, yellow, Kd);
    if(abs(dot(E,N))<0.25) gl_FragColor = black;
}
