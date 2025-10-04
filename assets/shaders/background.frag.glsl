#version 330 core
in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D tex;

uniform vec2 resolution;
uniform float time;



// Written by GLtracy
// Scattering - https://www.shadertoy.com/view/lslXDr
///////////////////////////////////////////////////////////////
// math const
const float PI = 3.14159265359;
const float MAX = 10000.0;

// ray intersects sphere
// e = -b +/- sqrt( b^2 - c )
vec2 ray_vs_sphere( vec3 p, vec3 dir, float r ) {
	float b = dot( p, dir );
	float c = dot( p, p ) - r * r;
	
	float d = b * b - c;
	if ( d < 0.0 ) {
		return vec2( MAX, -MAX );
	}
	d = sqrt( d );
	
	return vec2( -b - d, -b + d );
}

// Mie
// g : ( -0.75, -0.999 )
//      3 * ( 1 - g^2 )               1 + c^2
// F = ----------------- * -------------------------------
//      8pi * ( 2 + g^2 )     ( 1 + g^2 - 2 * g * c )^(3/2)
float phase_mie( float g, float c, float cc ) {
	float gg = g * g;
	
	float a = ( 1.0 - gg ) * ( 1.0 + cc );

	float b = 1.0 + gg - 2.0 * g * c;
	b *= sqrt( b );
	b *= 2.0 + gg;	
	
	return ( 3.0 / 8.0 / PI ) * a / b;
}

// Rayleigh
// g : 0
// F = 3/16PI * ( 1 + c^2 )
float phase_ray( float cc ) {
	return ( 3.0 / 16.0 / PI ) * ( 1.0 + cc );
}

// scatter const
const float R_INNER = 10.0;
const float R = R_INNER + 1.0;

const int NUM_OUT_SCATTER = 8;
const int NUM_IN_SCATTER = 80;

float density( vec3 p, float ph ) {
	return exp( -max( length( p ) - R_INNER, 0.0 ) / ph );
}

float optic( vec3 p, vec3 q, float ph ) {
	vec3 s = ( q - p ) / float( NUM_OUT_SCATTER );
	vec3 v = p + s * 0.5;
	
	float sum = 0.0;
	for ( int i = 0; i < NUM_OUT_SCATTER; i++ ) {
		sum += density( v, ph );
		v += s;
	}
	sum *= length( s );
	
	return sum;
}

vec3 in_scatter( vec3 o, vec3 dir, vec2 e, vec3 l ) {
	const float ph_ray = 0.05;
    const float ph_mie = 0.02;
    
    const vec3 k_ray = vec3( 3.8, 13.5, 33.1 );
    const vec3 k_mie = vec3( 21.0 );
    const float k_mie_ex = 1.1;
    
	vec3 sum_ray = vec3( 0.0 );
    vec3 sum_mie = vec3( 0.0 );
    
    float n_ray0 = 0.0;
    float n_mie0 = 0.0;
    
	float len = ( e.y - e.x ) / float( NUM_IN_SCATTER );
    vec3 s = dir * len;
	vec3 v = o + dir * ( e.x + len * 0.5 );
    
    for ( int i = 0; i < NUM_IN_SCATTER; i++, v += s ) {   
		float d_ray = density( v, ph_ray ) * len;
        float d_mie = density( v, ph_mie ) * len;
        
        n_ray0 += d_ray;
        n_mie0 += d_mie;
        
#if 0
        vec2 e = ray_vs_sphere( v, l, R_INNER );
        e.x = max( e.x, 0.0 );
        if ( e.x < e.y ) {
           continue;
        }
#endif
        
        vec2 f = ray_vs_sphere( v, l, R );
		vec3 u = v + l * f.y;
        
        float n_ray1 = optic( v, u, ph_ray );
        float n_mie1 = optic( v, u, ph_mie );
		
        vec3 att = exp( - ( n_ray0 + n_ray1 ) * k_ray - ( n_mie0 + n_mie1 ) * k_mie * k_mie_ex );
        
		sum_ray += d_ray * att;
        sum_mie += d_mie * att;
	}
	
	float c  = dot( dir, -l );
	float cc = c * c;
    vec3 scatter =
        sum_ray * k_ray * phase_ray( cc ) +
     	sum_mie * k_mie * phase_mie( -0.78, c, cc );
    
	
	return 10.0 * scatter;
}
///////////////////////////////////////////////////////////////




float map(vec3 position){
    return distance(position, vec3(0, 0, 0)) - 1.0;
}

vec3 normal(vec3 position){
    float eps = 0.00001;
    return normalize(vec3( 
        -map(position + vec3(eps, 0, 0)) + map(position - vec3(eps, 0, 0)),
        -map(position + vec3(0, eps, 0)) + map(position - vec3(0, eps, 0)),
        -map(position + vec3(0, 0, eps)) + map(position - vec3(0, 0, eps))
    ));
}

vec3 ray_dir( float fov, vec2 size, vec2 pos ) {
	vec2 xy = pos - size * 0.5;

	float cot_half_fov = tan( radians( 90.0 - fov * 0.5 ) );	
	float z = size.y * -0.5 * cot_half_fov;
	
	return normalize( vec3( xy, -z ) );
}



float rand(vec2 c){
	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43658.5453);
}

float noise(vec2 p, float freq ){
	float unit = 1.0/freq;
	vec2 ij = floor(p/unit);
	vec2 xy = mod(p,unit)/unit;
	//xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = .5*(1.-cos(3.1415*xy));
	float a = rand((ij+vec2(0.,0.)));
	float b = rand((ij+vec2(1.,0.)));
	float c = rand((ij+vec2(0.,1.)));
	float d = rand((ij+vec2(1.,1.)));
	float x1 = mix(a, b, xy.x);
	float x2 = mix(c, d, xy.x);
	return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res){
	float persistance = .5;
	float n = 0.;
	float normK = 0.;
	float f = 4.;
	float amp = 1.;
	int iCount = 0;
	for (int i = 0; i<8; i++){
		n+=amp*noise(p, f);
		f*=2.;
		normK+=amp;
		amp*=persistance;
		if (iCount == res) break;
		iCount++;
	}
	float nf = n/normK;
	return nf*nf*nf*nf;
}

void main() {
    vec2 uv = gl_FragCoord.xy/resolution.y - vec2((resolution.x/resolution.y - 1.0)/2.0, 0);
    vec2 centered_uv = (uv - 0.5)*2;

    vec3 color = vec3(0);
    vec3 ray_origin = vec3(0, 0, -20.0);
    vec3 ray_direction = ray_dir( 90.0, resolution.xy, gl_FragCoord.xy );
    vec3 light_direction = normalize(vec3(sin(time/10.0), 0, cos(time/10.0)));




    // magic number to match the raymarched scatter with the justinvented:tm: UV-based sphere
    centered_uv *= 1.735; 

    float center_dist = length(centered_uv);
    float is_earth = step(center_dist, 1.0);

    float normal = sqrt(1.0-(center_dist*center_dist));

    if(isnan(normal)) normal = 0.0;
    vec3 normal_vec = normalize(vec3(centered_uv, normal));

    if(dot(ray_direction * vec3(-1.0, 1.0, 1.0), -light_direction) * (1.0 - is_earth) > 0.9995){
		FragColor = vec4(1.0);
        return;
    }

    //scattering
    
    vec2 e = ray_vs_sphere( ray_origin, ray_direction, R );
	if ( e.x > e.y ) {
		FragColor = vec4( color, 1.0 );
        return;
	}
	
	vec2 f = ray_vs_sphere( ray_origin, ray_direction, R_INNER );
	e.y = min( e.y, f.x );

    vec3 scatter = in_scatter( ray_origin, ray_direction, e, light_direction * vec3(1.0, 1.0, -1.0) );

    ////////////

    //Texture/noise mapping
    vec2 terrain_noise_pos = centered_uv * 0.4 * (2.0-normal);
    float height = pNoise(terrain_noise_pos, 10) * 4.0;

    //Terrain_normal
    float off = 0.05;
    float height_up    = pNoise(terrain_noise_pos + vec2(0.0,  off), 5);
    float height_down  = pNoise(terrain_noise_pos + vec2(0.0, -off), 5);
    float height_right = pNoise(terrain_noise_pos + vec2(off,  0.0), 5);
    float height_left  = pNoise(terrain_noise_pos + vec2(-off, 0.0), 5);

    vec3 terrain_normal = normalize(vec3(height_left - height_right, height_down - height_up, 0.5));
    vec3 tangent = cross(normal_vec, vec3(0, 1.0, 0));
    vec3 bitangent = cross(normal_vec, tangent);
   
    //water normal
    if(height < 0.15) terrain_normal = vec3(0, 0, 1.0);

    #define TERRAIN_INFLUENCE 1.0
    vec3 final_normal = normalize(
        (terrain_normal.x * -TERRAIN_INFLUENCE * tangent)
       +(terrain_normal.y * TERRAIN_INFLUENCE * bitangent)
       +(terrain_normal.z * normal_vec));


    //colors
    vec3 beach_color      = vec3(0.7,0.55,0.36);
    vec3 vegetation_color = vec3(0.121, 0.176, 0.05);
    vec3 mountain_color   = vec3(0.78,0.78,0.78);
    vec3 water_color      = vec3(0.007, 0.019, 0.078);

    float equatorness = 1.0 - abs(terrain_noise_pos.y*1.5);

    vec3 ground_color = mix(vegetation_color, beach_color, (equatorness-0.5)*2.0);
    ground_color = mix(ground_color, mountain_color, smoothstep(0.3, 0.5, max(0.0, 0.8-equatorness)));
    ground_color = mix(ground_color, mountain_color, smoothstep(0.4, 1.0, height));


    vec3 terrain_color = mix(water_color, ground_color, step(0.15, height));

    float diffuse_factor = max(0.0, dot(final_normal, light_direction));
    diffuse_factor = min(diffuse_factor, length(scatter)*1.0);


    // vec2 a = ray_vs_sphere( ray_origin, ray_direction, R_INNER);
    // color = vec3(is_earth - step(a.x, a.y) );

    color = vec3(diffuse_factor * terrain_color) * is_earth;
    color += scatter * vec3(1.0, 1.0, 1.5);
    // color = terrain_color * is_earth;
    

    color = pow(color, vec3(1.0/2.2));
    FragColor = vec4(color, 1.0);
};
