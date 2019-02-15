#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>

#include <jpeglib.h>
#include <quirc.h>

struct my_jpeg_error {
	struct jpeg_error_mgr   base;
	jmp_buf                 env;
};

static void my_output_message(struct jpeg_common_struct *com)
{
	struct my_jpeg_error *err = (struct my_jpeg_error *)com->err;
	char buf[JMSG_LENGTH_MAX];

	err->base.format_message(com, buf);
	fprintf(stderr, "[jpg] error: %s\n", buf);
}

static void my_error_exit(struct jpeg_common_struct *com)
{
	struct my_jpeg_error *err = (struct my_jpeg_error *)com->err;

	my_output_message(com);
	longjmp(err->env, 0);
}

static struct jpeg_error_mgr *my_error_mgr(struct my_jpeg_error *err)
{
	jpeg_std_error(&err->base);

	err->base.error_exit = my_error_exit;
	err->base.output_message = my_output_message;

	return &err->base;
}


static int read_jpeg_file( struct quirc* pObj, const char* pFilePath )
{
	FILE *infile = fopen( pFilePath, "rb" );
	struct jpeg_decompress_struct dinfo;
	struct my_jpeg_error err;
	uint8_t *image;
	int y;

	if (!infile) {
		perror( "[jpg] can't open input file" );
		return -1;
	}

	memset(&dinfo, 0, sizeof(dinfo));
	dinfo.err = my_error_mgr(&err);

	if (setjmp(err.env))
		goto fail;

	jpeg_create_decompress(&dinfo);
	jpeg_stdio_src(&dinfo, infile);

	jpeg_read_header(&dinfo, TRUE);
	dinfo.output_components = 1;
	dinfo.out_color_space = JCS_GRAYSCALE;
	jpeg_start_decompress(&dinfo);

	if (dinfo.output_components != 1) {
		fprintf(stderr, "[jpg] Unexpected number of output components: %d",
			 dinfo.output_components);
		goto fail;
	}

	if (quirc_resize(pObj, dinfo.output_width, dinfo.output_height) < 0)
		goto fail;

	image = quirc_begin(pObj, NULL, NULL);

	for (y = 0; y < dinfo.output_height; y++) {
		JSAMPROW row_pointer = image + y * dinfo.output_width;

		jpeg_read_scanlines(&dinfo, &row_pointer, 1);
	}

	jpeg_finish_decompress(&dinfo);
	fclose(infile);
	jpeg_destroy_decompress(&dinfo);
	return 0;

fail:
	fclose(infile);
	jpeg_destroy_decompress(&dinfo);
	return -1;
}

static int decode_via_obj( struct quirc* pObj, const char* pFilePath )
{
    // the content of the jpeg was filled into the quirc object
    if ( 0 != read_jpeg_file( pObj, pFilePath ) ) {
        return -1;
    }

    // start to decode
    quirc_end( pObj );

    // show decoding result one-by-one
    int nCodes = quirc_count( pObj );
    for ( int i = 0; i < nCodes; ++i ) {
        struct quirc_code       code;
        struct quirc_data       data;
        quirc_decode_error_t    err;

		quirc_extract( pObj, i, &code );

		if ( 0 != (err = quirc_decode( &code, &data )) ) {
            fprintf( stderr, "[decode] failed to decode: %d\n", err );
        }
        else {
            printf( "[decode] data: %s\n", data.payload );
        }
    }

    return 0;
}

static int decode_file( const char* pFilePath )
{
    if ( NULL == pFilePath || '\0' == pFilePath[ 0 ] ) {
        return -1;
    }

    struct quirc* pDecoderObj = quirc_new();
    if ( NULL == pDecoderObj ) {
        perror( "[decode] insufficient memory" );
        return -1;
    }

    int ret_code = decode_via_obj( pDecoderObj, pFilePath );

    quirc_destroy( pDecoderObj );

    return ret_code;
}

int main( int argc, char* argv[] )
{
    if ( 2 > argc ) {
        puts( "[usage] decoder jpg-file-to-be-decoded" );
        return -1;
    }

    return decode_file( argv[ 1 ] );
}
