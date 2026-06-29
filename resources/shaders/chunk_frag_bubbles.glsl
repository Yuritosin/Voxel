#version 330 core

layout (location = 0) out vec4 color;

uniform float iTime;

float opSmoothUnion( float d1, float d2, float k )
{
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

float sdSphere( vec3 p, float s )
{
  return length(p)-s;
} 

float map(vec3 p)
{
	float d = 2.0;
	for (int i = 0; i < 16; i++) {
		float fi = float(i);
		float time = iTime * (fract(fi * 412.531 + 0.513) - 0.5) * 2.0;
		d = opSmoothUnion(
            sdSphere(p + sin(time + fi * vec3(52.5126, 64.62744, 632.25)) * vec3(2.0, 2.0, 0.8), mix(0.5, 1.0, fract(fi * 412.531 + 0.5124))),
			d,
			0.4
		);
	}
	return d;
}

vec3 calcNormal( in vec3 p )
{
    const float h = 1e-5; // or some other value
    const vec2 k = vec2(1,-1);
    return normalize( k.xyy*map( p + k.xyy*h ) + 
                      k.yyx*map( p + k.yyx*h ) + 
                      k.yxy*map( p + k.yxy*h ) + 
                      k.xxx*map( p + k.xxx*h ) );
}

in vec2 vUv;
in float vBright;

uniform sampler2D tex;

uniform bool debug;

void main() {
	if (!debug) {
		vec2 iResolution = vec2(1024, 1024 / 16 * 9);
		vec2 uv = gl_FragCoord.xy/iResolution.xy;

		// screen size is 6m x 6m
		vec3 rayOri = vec3((uv - 0.5) * vec2(iResolution.x/iResolution.y, 1.0) * 6.0, 3.0);
		vec3 rayDir = vec3(0.0, 0.0, -1.0);

		float depth = 0.0;
		vec3 p;

		for(int i = 0; i < 64; i++) {
			p = rayOri + rayDir * depth;
			float dist = map(p);
			depth += dist;
			if (dist < 1e-6) {
				break;
			}
		}

		depth = min(6.0, depth);
		vec3 n = calcNormal(p);
		float b = max(0.0, dot(n, vec3(0.577)));
		vec3 col = (0.5 + 0.5 * cos((b + iTime * 3.0) + uv.xyx * 2.0 + vec3(0,2,4))) * (0.85 + b * 0.35);
		col *= exp( -depth * 0.15 );

		// maximum thickness is 2m in alpha channel
// 		color = vec4(col, 1.0 - (depth - 0.5) / 2.0);
        color = texture(tex, vUv) * vBright * vec4(col, 1.0 - (depth - 0.5) / 2.0);
	} else {
		color = vec4(0.0, 1.0, 0.0, 1.0);
	}
}

/** SHADERDATA
{
	"title": "My Shader 0",
	"description": "Lorem ipsum dolor",
	"model": "person"
}
*/
