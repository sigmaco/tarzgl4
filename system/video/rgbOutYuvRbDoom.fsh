#include <//./z/video/stdEnv.inc>

SAMPLING_UNIT(0, 0, sampler2D, samp0);
SAMPLING_UNIT(0, 1, sampler2D, samp1);
SAMPLING_UNIT(0, 2, sampler2D, samp2);

in block
{
    vec2 uv0;
} sgl_v;

OUT(0, vec4, sgl_rgba);

void main()
{
    const vec3 crc = vec3( 1.595794678, -0.813476563, 0 );
    const vec3 crb = vec3( 0, -0.391448975, 2.017822266 );
    const vec3 adj = vec3( -0.87065506, 0.529705048f, -1.081668854f );
    const vec3 YScalar = vec3( 1.164123535f, 1.164123535f, 1.164123535f );

    float Y = texture( samp0, sgl_v.uv0).r;
    float Cr = texture( samp1, sgl_v.uv0).r;
    float Cb = texture( samp2, sgl_v.uv0).r;

    vec3 p = ( YScalar * Y );
    p += ( crc * Cr ) + ( crb * Cb ) + adj;

    vec4 color;
    color.xyz = p;
    color.w = 1.0;
    //color *= rpColor;

    sgl_rgba = color;
}