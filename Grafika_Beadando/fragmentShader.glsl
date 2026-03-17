#version 410 core
uniform vec2 u_cPos;
uniform vec2 u_res;
uniform bool u_hit;
uniform float u_padY;
out vec4 fCol;

void main() {
    vec2 scale = u_res / 600.0;
    vec2 p = gl_FragCoord.xy;
    float d = distance(p, u_cPos * scale);
    float radius = 50.0 * scale.x; // 50 pixel sugaru kor

    if (d < radius) {
        vec3 c1 = vec3(1,0,0); // piros
        vec3 c2 = vec3(0,1,0); // zold
        if (!u_hit) { vec3 t=c1; c1=c2; c2=t; } // szincsere
        fCol = vec4(mix(c1, c2, d/radius), 1.0); // atmenet
    } 
    else if (p.x >= 200.0 * scale.x && p.x <= 400.0 * scale.x && abs(p.y - u_padY * scale.y) < 1.5 * scale.y) { // szakasz x tartomanya, (abs) vastagsag 3 pixelre
        fCol = vec4(0, 0, 1, 1); // szakasz kek
    }
    else {
        fCol = vec4(1.0, 1.0, 0.4, 1.0);
    }
}