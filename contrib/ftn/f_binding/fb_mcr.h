/*	@(#)fb_mcr.h 1.1 91/01/07 FJ	*/

/***********************************************************
Copyright 1990, 1991, by Fujitsu Limited.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Fujitsu not be used
in advertising or publicity pertaining to distribution of the software
without specific, written prior permission.  

FUJITSU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
******************************************************************/

/* Macro functions for PHIGS FORTRAN binding	*/

/* POINT3_DATA_TRANSLATE */

#define POINT3_DATA_TRANSLATE( _number, _point_x, _point_y, _point_z, _data) \
{ \
/*  Pint	_number;		IN  number of point \
    float  	_point_x[];		IN  point_x \
    float 	_point_y[];		IN  point_y \
    float	_point_z[];		IN  point_z \
    Ppoint3	*_data;			OUT point3 translate data \
*/ \
    register int	i ; \
    for ( i = 0 ; i < _number ; i++) { \
        (_data)[i].x = _point_x[i]; \
        (_data)[i].y = _point_y[i]; \
        (_data)[i].z = _point_z[i]; \
    } \
}

/* POINT_DATA_TRANSLATE */

#define POINT_DATA_TRANSLATE( _number, _point_x, _point_y, _data) \
{ \
/*  Pint	_number;		IN  number of point \
    float  	_point_x[];		IN  point_x \
    float 	_point_y[];		IN  point_y \
    Ppoint	*_data;			OUT point3 translate data \
*/ \
    register int	i ; \
    for ( i = 0 ; i < _number ; i++ ) { \
        (_data)[i].x = _point_x[i]; \
        (_data)[i].y = _point_y[i]; \
    } \
}

/* COLR_IND_DATA_TRANSLATE */

#define COLR_IND_DATA_TRANSLATE( _isc, _isr, _dx, _dy, _dimx, _dimy, _index, _data) \
{ \
/*  Pint	_isc;		IN  start column (ISC) \
    Pint	_isr;		IN  start row    (ISR) \
    Pint	_dx;		IN  number of column (DX) \
    Pint	_dy;		IN  number of row    (DY) \
    Pint	_dimx;		IN  dimension x of colour (DIMX) \
    Pint	_dimy;		IN  dimension y of colour (DIMY) \
    Pint	_index[];	IN  colour index data \
    Ppat_rep   	_data;		OUT colour index translate data \
*/ \
    int         i; \
    int         j; \
    int         k = 0; \
    int			ix; \
    (_data).dims.size_x = (_dx); \
    (_data).dims.size_y = (_dy); \
    for ( i = ((_isr) - 1); i < ((_isr) + (_dy) - 1); i++) { \
		ix = (_dimx) * i; \
        for ( j = ((_isc) - 1); j < ((_isc) + (_dx) - 1); j++) { \
            (_data).colr_array[k] = (_index)[ix + j]; \
            k++; \
        } \
    } \
}

/* COLR_VAL_DATA_TRANSLATE */

#define COLR_VAL_DATA_TRANSLATE( _type, _isc, _isr, _dx, _dy, _dimx, _dimy, _index, _colour, _data) \
{ \
/*  Pint	_type;		IN  colour type \
    Pint	_isc;		IN  start column (ISC) \
    Pint	_isr;		IN  start row    (ISR) \
    Pint	_dx;		IN  number of column (DX)\
    Pint	_dy;		IN  number of row    (DY)\
    Pint	_dimx;		IN  dimention x of colour (DIMX) \
    Pint	_dimy;		IN  dimention y of colour (DIMY) \
    Pint	_index[];	IN  colour indices \
    Pfloat	_colour[];	IN  colour component \
    Pcoval	*_data;		OUT colour value translate data \
*/ \
    int		i; \
    int		j; \
    int		k = 0; \
    int		ix; \
    switch ( _type) { \
    case PINDCT: \
        for ( i = ((_isr) - 1); i < ((_isr) + (_dy) - 1); i++) { \
		    ix = (_dimx) * i; \
            for ( j = ((_isc) - 1); j < ((_isc) + (_dx) - 1); j++) { \
                (_data)[k].ind = (_index)[ix + j]; \
                k++; \
            } \
        } \
        break; \
    case PRGB: \
        for ( i = ((_isr) - 1); i < ((_isr) + (_dy) - 1); i++) { \
		    ix = (((_dimx) * i) + (_isc - 1)) * 3; \
            for ( j = ((_isc) - 1); j < ((_isc) + (_dx) - 1); j++) { \
	            (_data)[k].direct.rgb.red = (_colour)[ix]; \
	            (_data)[k].direct.rgb.green = (_colour)[ix + 1]; \
	            (_data)[k].direct.rgb.blue = (_colour)[ix + 2]; \
                ix += 3; \
	            k++; \
	        } \
    	} \
        break; \
    case PCIE: \
        for ( i = ((_isr) - 1); i < ((_isr) + (_dy) - 1); i++) { \
		    ix = (((_dimx) * i) + (_isc - 1)) * 3; \
            for ( j = ((_isc) - 1); j < ((_isc) + (_dx) - 1); j++) { \
	            (_data)[k].direct.cieluv.cieluv_x = (_colour)[ix]; \
                (_data)[k].direct.cieluv.cieluv_y = (_colour)[ix + 1]; \
                (_data)[k].direct.cieluv.cieluv_y_lum = (_colour)[ix + 2]; \
                ix += 3; \
	            k++; \
	        } \
	    } \
        break; \
    case PHSV: \
        for ( i = ((_isr) - 1); i < ((_isr) + (_dy) - 1); i++) { \
		    ix = (((_dimx) * i) + (_isc - 1)) * 3; \
            for ( j = ((_isc) - 1); j < ((_isc) + (_dx) - 1); j++) { \
                (_data)[k].direct.hsv.hue = (_colour)[ix]; \
                (_data)[k].direct.hsv.satur = (_colour)[ix + 1]; \
                (_data)[k].direct.hsv.value = (_colour)[ix + 2]; \
                ix += 3; \
	            k++; \
	        } \
	    } \
        break; \
    case PHLS: \
        for ( i = ((_isr) - 1); i < ((_isr) + (_dy) - 1); i++) { \
		    ix = (((_dimx) * i) + (_isc - 1)) * 3; \
            for ( j = ((_isc) - 1); j < ((_isc) + (_dx) - 1); j++) { \
                (_data)[k].direct.hls.hue = (_colour)[ix]; \
                (_data)[k].direct.hls.lightness = (_colour)[ix + 1]; \
                (_data)[k].direct.hls.satur = (_colour)[ix + 2]; \
                ix += 3; \
	            k++; \
	        } \
        } \
        break; \
    } \
}

/* COLR_IND_DATA_RE_TRANSLATE */

#define COLR_IND_DATA_RE_TRANSLATE( _rep, _dx, _dy, _dimx, _dimy, _colia) \
{ \
/*  Ppat_rep_plus   *_rep;		IN colour index data \
    Pint	_dx;		OUT number of column (DX) \
    Pint	_dy;		OUT number of row    (DY) \
    Pint	_dimx;		OUT dimension x of colour (DIMX) \
    Pint	_dimy;		OUT dimension y of colour (DIMY) \
    Pint	_colia[];	OUT colour index data \
*/ \
    int         i; \
    int         j; \
    int         k = 0; \
    Pint		*colia; \
    colia = (_colia); \
    for ( i = 0; i < (_dy); i++) { \
        for ( j = 0; j < (_dx); j++) { \
            colia[j] = (_rep)->colr_array[k].ind; \
            k++; \
        } \
        colia += (_dimx); \
    } \
}

/* COLR_VAL_DATA_RE_TRANSLATE */

#define COLR_VAL_DATA_RE_TRANSLATE( _rep, _dx, _dy, _dimx, _dimy, _colra) \
{ \
/*  Ppat_rep_plus   *_rep;	IN colour index data \
    Pint	_dx;		OUT number of column (DX) \
    Pint	_dy;		OUT number of row    (DY) \
    Pint	_dimx;		OUT dimension x of colour (DIMX) \
    Pint	_dimy;		OUT dimension y of colour (DIMY) \
    Pint	_colra[];	OUT colour index data \
*/ \
    int         i; \
    int         j; \
    int         k = 0; \
    int         ix = 0; \
    Pfloat		*colra; \
    colra = (_colra); \
    for ( i = 0; i < (_dy); i++) { \
        for ( j = 0, ix = 0; j < (_dx); j++, ix += 3) { \
            colra[ix] = (_rep)->colr_array[k].direct.rgb.red; \
            colra[ix + 1] = (_rep)->colr_array[k].direct.rgb.green; \
            colra[ix + 1] = (_rep)->colr_array[k].direct.rgb.blue; \
            k++; \
        } \
        colra += (_dimx); \
    } \
}


/* MATRIX_DATA_TRANSLATE */

#define MATRIX_DATA_TRANSLATE( _number, _matrix, _data) \
{ \
/*  int		_number;	IN  number of dimension \
    float	_matrix[][];	IN  fortran (c) matrix \
    float	_data[][];	OUT c (fortran) matrix \
*/ \
    register int	 i,j; \
    for ( i = 0; i <= _number; i++) { \
        for ( j = 0; j <= _number; j++) { \
            _data[i][j] = _matrix[j][i]; \
        } \
    } \
}

/* POINT3_DATA_RE_TRANSLATE */

#define POINT3_DATA_RE_TRANSLATE( _data, _number, _point_x, _point_y, _point_z) \
{ \
/*  Ppoint3	*_data;		IN  point3 translate data \
    Pint	_number;	IN  number of point \
    Pfloat	_point_x[];	OUT point_x \
    Pfloat	_point_y[];	OUT point_y \
    Pfloat	_point_z[];	OUT point_z \
*/ \
    register int	i ; \
    for ( i = 0; i < _number; i++) { \
        _point_x[i] = _data[i].x; \
        _point_y[i] = _data[i].y; \
        _point_z[i] = _data[i].z; \
    } \
}

/* POINT_DATA_RE_TRANSLATE */

#define POINT_DATA_RE_TRANSLATE( _data, _number, _point_x, _point_y) \
{ \
/*  Ppoint	*_data;		IN  point translate data \
    Pint	_number;	IN  number of point \
    Pfloat	_point_x[];	OUT point_x \
    Pfloat	_point_y[];	OUT point_y \
*/ \
    register int	i ; \
	for ( i = 0; i < _number; i++) { \
           _point_x[i] = _data[i].x; \
           _point_y[i] = _data[i].y; \
    } \
}

/* FACET_DATA_TRANSLATE */

#define FACET_DATA_TRANSLATE( _fflag, _num, _type, _index, _colr, _x_val, _y_val, _z_val, _fdata) \
{ \
/*  Pint		_fflag;			IN  data per facet flag \
    Pint		_num;			IN  number of facet \
    Pint		_type;			IN  colour type \
    Pint		_index[];		IN  colour indices \
    Pfloat		_colr[];		IN  colour component data1 \
    Pfloat		_x_val[];		IN  vector_x \
    Pfloat		_y_val[];		IN  vector_y \
    Pfloat		_z_val[];		IN  vector_z \
    Pfacet_data_arr3	_fdata;		        OUT facet translate data \
*/ \
    register int		i,j; \
    switch ( _fflag) { \
    case PFCOLR: \
        switch ( _type) { \
        case PINDCT: \
            for ( i = 0; i < _num; i++) { \
                (_fdata).colrs[i].ind = _index[i]; \
            } \
            break; \
        case PRGB: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_fdata).colrs[i].direct.rgb.red = _colr[j]; \
                (_fdata).colrs[i].direct.rgb.green = _colr[j + 1]; \
                (_fdata).colrs[i].direct.rgb.blue = _colr[j + 2]; \
            } \
            break; \
        case PCIE: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_fdata).colrs[i].direct.cieluv.cieluv_x = _colr[j]; \
                (_fdata).colrs[i].direct.cieluv.cieluv_y = _colr[j + 1]; \
                (_fdata).colrs[i].direct.cieluv.cieluv_y_lum = _colr[j + 2]; \
            } \
            break; \
        case PHSV: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_fdata).colrs[i].direct.hsv.hue = _colr[j]; \
                (_fdata).colrs[i].direct.hsv.satur = _colr[j + 1]; \
                (_fdata).colrs[i].direct.hsv.value = _colr[j + 2]; \
            } \
            break; \
        case PHLS: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_fdata).colrs[i].direct.hls.hue = _colr[j]; \
                (_fdata).colrs[i].direct.hls.lightness = _colr[j + 1]; \
                (_fdata).colrs[i].direct.hls.satur = _colr[j + 2]; \
            } \
            break; \
        } \
        break; \
    case PFNORM: \
        for ( i = 0; i < _num; i++) { \
            (_fdata).norms[i].delta_x = _x_val[i]; \
            (_fdata).norms[i].delta_y = _y_val[i]; \
            (_fdata).norms[i].delta_z = _z_val[i]; \
        } \
        break; \
    case PFCONO: \
        for ( i = 0; i < _num; i++) { \
            (_fdata).conorms[i].norm.delta_x = _x_val[i]; \
            (_fdata).conorms[i].norm.delta_y = _y_val[i]; \
            (_fdata).conorms[i].norm.delta_z = _z_val[i]; \
        } \
        switch ( _type) { \
        case PINDCT: \
            for ( i = 0; i < _num; i++) { \
                (_fdata).conorms[i].colr.ind = _index[i]; \
            } \
            break; \
        case PRGB: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_fdata).conorms[i].colr.direct.rgb.red = _colr[j]; \
                (_fdata).conorms[i].colr.direct.rgb.green = _colr[j + 1]; \
                (_fdata).conorms[i].colr.direct.rgb.blue = _colr[j + 2]; \
            } \
            break; \
        case PCIE: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_fdata).conorms[i].colr.direct.cieluv.cieluv_x = _colr[j]; \
                (_fdata).conorms[i].colr.direct.cieluv.cieluv_y = \
                    _colr[j + 1]; \
                (_fdata).conorms[i].colr.direct.cieluv.cieluv_y_lum = \
                    _colr[j + 2]; \
            } \
            break; \
        case PHSV: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_fdata).conorms[i].colr.direct.hsv.hue = _colr[j]; \
                (_fdata).conorms[i].colr.direct.hsv.satur = _colr[j + 1]; \
                (_fdata).conorms[i].colr.direct.hsv.value = _colr[j + 2]; \
            } \
            break; \
        case PHLS: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_fdata).conorms[i].colr.direct.hls.hue = _colr[j]; \
                (_fdata).conorms[i].colr.direct.hls.lightness = _colr[j + 1]; \
                (_fdata).conorms[i].colr.direct.hls.satur = _colr[j + 2]; \
            } \
            break; \
        } \
        break; \
    } \
}

/* VERTEX_DATA_TRANSLATE */

#define VERTEX_DATA_TRANSLATE( _vflag, _num, _type, _index, _colr, _px, _py, _pz, _x_val, _y_val, _z_val, _vdata) \
{ \
/*  Pint		_vflag;			IN  data per vertex flag \
    Pint		_num;			IN  number of vertex \
    Pint		_type;			IN  colour type \
    Pint		_index[];		IN  colour indices \
    Pfloat		_colr[];		IN  colour component data \
    Pfloat		_px[];			IN  point_x \
    Pfloat		_py[];			IN  point_y \
    Pfloat		_pz[];			IN  point_z \
    Pfloat		_x_val[];		IN  vector_x \
    Pfloat		_y_val[];		IN  vector_y \
    Pfloat		_z_val[];		IN  vector_z \
    Pfacet_vdata_arr3	_vdata;		        OUT vertex translate data \
*/ \
    register int		i,j; \
    switch ( _vflag) { \
    case PCOORD: \
        for ( i = 0; i < _num; i++) { \
            (_vdata).points[i].x = _px[i]; \
            (_vdata).points[i].y = _py[i]; \
            (_vdata).points[i].z = _pz[i]; \
        } \
        break; \
    case PCCOLR: \
        for ( i = 0; i < _num; i++) { \
            (_vdata).ptcolrs[i].point.x = _px[i]; \
            (_vdata).ptcolrs[i].point.y = _py[i]; \
            (_vdata).ptcolrs[i].point.z = _pz[i]; \
        } \
        switch ( _type) { \
        case PINDCT: \
            for ( i = 0; i < _num; i++) { \
                (_vdata).ptcolrs[i].colr.ind = _index[i]; \
            } \
            break; \
        case PRGB: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_vdata).ptcolrs[i].colr.direct.rgb.red = _colr[j]; \
                (_vdata).ptcolrs[i].colr.direct.rgb.green = _colr[j + 1]; \
                (_vdata).ptcolrs[i].colr.direct.rgb.blue = _colr[j + 2]; \
            } \
            break; \
        case PCIE: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_vdata).ptcolrs[i].colr.direct.cieluv.cieluv_x = _colr[j]; \
                (_vdata).ptcolrs[i].colr.direct.cieluv.cieluv_y = \
                    _colr[j + 1]; \
                (_vdata).ptcolrs[i].colr.direct.cieluv.cieluv_y_lum = \
                    _colr[j + 2]; \
            } \
            break; \
        case PHSV: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_vdata).ptcolrs[i].colr.direct.hsv.hue = _colr[j]; \
                (_vdata).ptcolrs[i].colr.direct.hsv.satur = _colr[j + 1]; \
                (_vdata).ptcolrs[i].colr.direct.hsv.value = _colr[j + 2]; \
            } \
            break; \
        case PHLS: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_vdata).ptcolrs[i].colr.direct.hls.hue = _colr[j]; \
                (_vdata).ptcolrs[i].colr.direct.hls.lightness = _colr[j + 1]; \
                (_vdata).ptcolrs[i].colr.direct.hls.satur = _colr[j + 2]; \
            } \
            break; \
        } \
        break; \
    case PCNORM: \
        for ( i = 0; i < _num; i++) { \
            (_vdata).ptnorms[i].point.x = _px[i]; \
            (_vdata).ptnorms[i].point.y = _py[i]; \
            (_vdata).ptnorms[i].point.z = _pz[i]; \
            (_vdata).ptnorms[i].norm.delta_x = _x_val[i]; \
            (_vdata).ptnorms[i].norm.delta_y = _y_val[i]; \
            (_vdata).ptnorms[i].norm.delta_z = _z_val[i]; \
        } \
        break; \
    case PCCONO: \
        for ( i = 0; i < _num; i++) { \
            (_vdata).ptconorms[i].point.x = _px[i]; \
            (_vdata).ptconorms[i].point.y = _py[i]; \
            (_vdata).ptconorms[i].point.z = _pz[i]; \
            (_vdata).ptconorms[i].norm.delta_x = _x_val[i]; \
            (_vdata).ptconorms[i].norm.delta_y = _y_val[i]; \
            (_vdata).ptconorms[i].norm.delta_z = _z_val[i]; \
        } \
        switch ( _type) { \
        case PINDCT: \
            for ( i = 0; i < _num; i++) { \
                (_vdata).ptconorms[i].colr.ind = _index[i]; \
            } \
            break; \
        case PRGB: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_vdata).ptconorms[i].colr.direct.rgb.red = _colr[j]; \
                (_vdata).ptconorms[i].colr.direct.rgb.green = _colr[j + 1]; \
                (_vdata).ptconorms[i].colr.direct.rgb.blue = _colr[j + 2]; \
            } \
            break; \
        case PCIE: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_vdata).ptconorms[i].colr.direct.cieluv.cieluv_x = _colr[j]; \
                (_vdata).ptconorms[i].colr.direct.cieluv.cieluv_y = \
                   _colr[j + 1]; \
                (_vdata).ptconorms[i].colr.direct.cieluv.cieluv_y_lum = \
                   _colr[j + 2]; \
            } \
            break; \
        case PHSV: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_vdata).ptconorms[i].colr.direct.hsv.hue = _colr[j]; \
                (_vdata).ptconorms[i].colr.direct.hsv.satur = _colr[j + 1]; \
                (_vdata).ptconorms[i].colr.direct.hsv.value = _colr[j + 2]; \
            } \
            break; \
        case PHLS: \
            for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                (_vdata).ptconorms[i].colr.direct.hls.hue = _colr[j]; \
                (_vdata).ptconorms[i].colr.direct.hls.lightness = \
                    _colr[j + 1]; \
                (_vdata).ptconorms[i].colr.direct.hls.satur = _colr[j + 2]; \
            } \
            break; \
        } \
        break; \
    } \
}

/* GCOLR_DATA_TRANSLATE */

#define GCOLR_DATA_TRANSLATE( _ctype, _coli, _colr, _gcolr) \
{ \
/*  Pint	_ctype;		IN  colour type \
    Pint	_coli;		IN  colour index \
    Pfloat	_colr[];	IN  colour component \
    Pgcolr	_gcolr;		OUT colour \
*/ \
    (_gcolr).type = _ctype; \
    switch ( _ctype) { \
    case PINDCT: \
        (_gcolr).val.ind = (_coli); \
        break; \
    case PRGB: case PCIE: case PHSV: case PHLS: \
        (_gcolr).val.general.x = (_colr)[0]; \
        (_gcolr).val.general.y = (_colr)[1]; \
        (_gcolr).val.general.z = (_colr)[2]; \
        break; \
    } \
}

/* COVAL_DATA_TRANSLATE */

#define COVAL_DATA_TRANSLATE( _ctype, _coli, _colr, _coval) \
{ \
/*  Pint	_ctype;		IN  colour type \
    Pint	_coli;		IN  colour index \
    Pfloat	_colr[];	IN  colour component \
    Pcoval	_coval;		OUT colour \
*/ \
    switch ( _ctype) { \
    case PINDCT: \
        _coval.ind = _coli; \
        break; \
    case PRGB: \
        _coval.direct.rgb.red = _colr[0]; \
        _coval.direct.rgb.green = _colr[1]; \
        _coval.direct.rgb.blue = _colr[2]; \
        break; \
    case PCIE: \
        _coval.direct.cieluv.cieluv_x = _colr[0]; \
        _coval.direct.cieluv.cieluv_y = _colr[1]; \
        _coval.direct.cieluv.cieluv_y_lum = _colr[2]; \
        break; \
    case PHSV: \
        _coval.direct.hsv.hue = _colr[0]; \
        _coval.direct.hsv.satur = _colr[1]; \
        _coval.direct.hsv.value = _colr[2]; \
        break; \
    case PHLS: \
        _coval.direct.hls.hue = _colr[0]; \
        _coval.direct.hls.lightness = _colr[1]; \
        _coval.direct.hls.satur = _colr[2]; \
        break; \
    } \
}
/* GCOLR_DATA_RE_TRANSLATE */

#define GCOLR_DATA_RE_TRANSLATE( _gcolr, _il, _ia, _rl, _ra) \
{ \
/*  Pgcolr	_gcolr;		IN colour \
    Pint	_il;		OUT number of integer entries \
    Pint	_ia[];		OUT array containing integer entries \
    Pint	_rl;		OUT number of real entries \
    Pfloat	_ra[];		OUT array containing real entries \
*/ \
    (_ia)[0] = (_gcolr).type; \
    switch ( (_ia)[0]) { \
    case PINDCT: \
        _il += 2; \
        (_ia)[1] = (_gcolr).val.ind; \
        break; \
    case PRGB: case PCIE: case PHSV: case PHLS: \
        _il += 1; \
        _rl += 3; \
        (_ra)[0] = (_gcolr).val.general.x; \
        (_ra)[1] = (_gcolr).val.general.y; \
        (_ra)[2] = (_gcolr).val.general.z; \
        break; \
    } \
}
/* GCOLR_DATA_RE_TRANSLATE */

#define GCOLR_DATA_RE_TRANSLATE_PLUS( _gcolr, _ccsbsz, _errind, _ctype, _coli, _ol, _colr) \
{ \
/*  Pgcolr	_gcolr;		IN colour \
    Pint	_ccsbsz;	IN colour component buffer size \
    Pint	_errind;	OUT error indicator \
    Pint	_ctype;		OUT colour type \
    Pint	_coli;		OUT colour index \
    Pint	_ol;		OUT number of colour components \
    Pfloat	_colr[];	OUT colour specification \
*/ \
    (_ctype) = (_gcolr).type; \
    switch ( (_ctype)) { \
    case PINDCT: \
        (_coli) = (_gcolr).val.ind; \
        break; \
    case PRGB: case PCIE: case PHSV: case PHLS: \
        (_ol) = 3; \
        if ( (_ol) > (_ccsbsz) ) { \
            (_errind) = ERR2001; \
        } else { \
            (_colr)[0] = (_gcolr).val.general.x; \
            (_colr)[1] = (_gcolr).val.general.y; \
            (_colr)[2] = (_gcolr).val.general.z; \
        } \
        break; \
    } \
}
/* GDP_FDATA_TRAN */

#define GDP_FDATA_TRAN( _flag, _num, _type, _index, _colr, _vec, _err, _data) \
{ \
/*  Pint		_flag;			IN  facet flag \
    Pint		_num;			IN  number of facet \
    Pint		_type;			IN  colour type \
    Pint		_index[];		IN  colour indices \
    Pfloat		_colr[];		IN  colour component data1 \
    Pfloat		_vec[];			IN  vector \
    Pint		_err;			OUT error indicater \
    Pfacet_data_arr3	_data;		        OUT translate data \
*/ \
    register int		i,j; \
    register int		num2 = (_num) * 2; \
    switch ( _flag) { \
    case PFCOLR: \
        (_data).colrs = NULL; \
        PMALLOC( _err, Pcoval, _num, (_data).colrs); \
        if ( !err) { \
            switch ( _type) { \
            case PINDCT: \
                for ( i = 0; i < _num; i++) { \
                    (_data).colrs[i].ind = (_index)[i]; \
                } \
                break; \
            case PRGB: case PCIE: case PHSV: case PHLS: \
                for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                    (_data).colrs[i].direct.rgb.red = (_colr)[j]; \
                    (_data).colrs[i].direct.rgb.green = (_colr)[j + 1]; \
                    (_data).colrs[i].direct.rgb.blue = (_colr)[j + 2]; \
	            } \
            } \
        } \
        break; \
    case PFNORM: \
        (_data).norms = NULL; \
        PMALLOC( _err, Pvec3, _num, (_data).norms); \
        if ( !err) { \
            for ( i = 0; i < _num; i++) { \
                (_data).norms[i].delta_x = (_vec)[i]; \
                (_data).norms[i].delta_y = (_vec)[_num + i]; \
                (_data).norms[i].delta_z = (_vec)[num2 + i]; \
            } \
        } \
        break; \
    case PFCONO: \
        (_data).conorms = NULL; \
        PMALLOC( _err, Pconorm3, _num, (_data).conorms); \
        if ( !err) { \
            for ( i = 0; i < _num; i++) { \
                (_data).conorms[i].norm.delta_x = (_vec)[i]; \
                (_data).conorms[i].norm.delta_y = (_vec)[_num + i]; \
                (_data).conorms[i].norm.delta_z = (_vec)[num2 + i]; \
            } \
            switch ( _type) { \
            case PINDCT: \
                for ( i = 0; i < _num; i++) { \
	                (_data).conorms[i].colr.ind = (_index)[i]; \
                } \
                break; \
       	    case PRGB: case PCIE: case PHSV: case PHLS: \
                for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                    (_data).conorms[i].colr.direct.rgb.red = (_colr)[i]; \
                    (_data).conorms[i].colr.direct.rgb.green = (_colr)[j + 1]; \
                    (_data).conorms[i].colr.direct.rgb.blue = (_colr)[j + 2]; \
                } \
            } \
            break; \
        } \
    } \
}

/* GDP_VDATA_TRAN */

#define GDP_VDATA_TRAN( _flag, _num, _type, _index, _colr, _vec, _err, _data) \
{ \
/*  Pint		_flag;				IN  vertex flag \
    Pint		_num;				IN  number of facet \
    Pint		_type;				IN  colour type \
    Pint		_index[];			IN  colour indices \
    Pfloat		_colr[];			IN  colour component data1 \
    Pfloat		_vec[];				IN  vector \
    Pint		_err;				OUT error indicater \
    Pgdp_facet_vdata_arr3 _data;	OUT translate data \
*/ \
    register int		i,j; \
    register int        num2 = (_num) * 2; \
    switch ( _flag) { \
    case PCCOLR: \
        (_data).colrs = NULL; \
        PMALLOC( _err, Pcoval, _num, (_data).colrs); \
        if ( !err) { \
            switch ( _type) { \
            case PINDCT: \
                for ( i = 0; i < _num; i++) { \
                    (_data).colrs[i].ind = (_index)[i]; \
                } \
                break; \
            case PRGB: case PCIE: case PHSV: case PHLS: \
                for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                    (_data).colrs[i].direct.rgb.red = (_colr)[j]; \
                    (_data).colrs[i].direct.rgb.green = (_colr)[j + 1]; \
                    (_data).colrs[i].direct.rgb.blue = (_colr)[j + 2]; \
	            } \
            } \
        } \
        break; \
    case PCNORM: \
        (_data).normals = NULL; \
        PMALLOC( _err, Pvec3, _num, (_data).normals); \
        if ( !err) { \
            for ( i = 0; i < _num; i++) { \
                (_data).normals[i].delta_x = (_vec)[i]; \
                (_data).normals[i].delta_y = (_vec)[_num + i]; \
                (_data).normals[i].delta_z = (_vec)[num2 + i]; \
            } \
        } \
        break; \
    case PCCONO: \
        (_data).conorms = NULL; \
        PMALLOC( _err, Pconorm3, _num, (_data).conorms); \
        if ( !err) { \
            for ( i = 0; i < _num; i++) { \
                (_data).conorms[i].norm.delta_x = (_vec)[i]; \
                (_data).conorms[i].norm.delta_y = (_vec)[_num + i]; \
                (_data).conorms[i].norm.delta_z = (_vec)[num2 + i]; \
            } \
            switch ( _type) { \
            case PINDCT: \
                for ( i = 0; i < _num; i++) { \
	                (_data).conorms[i].colr.ind = (_index)[i]; \
                } \
                break; \
       	    case PRGB: case PCIE: case PHSV: case PHLS: \
                for ( i = 0, j = 0; i < _num; i++, j += 3) { \
                    (_data).conorms[i].colr.direct.rgb.red = (_colr)[j]; \
                    (_data).conorms[i].colr.direct.rgb.green = (_colr)[j + 1]; \
                    (_data).conorms[i].colr.direct.rgb.blue = (_colr)[j + 2]; \
                } \
            } \
            break; \
        } \
    } \
}

#define IN_ST_CONVERT_C_TO_F( _c_st, _f_st) \
    if ( (_c_st) == PIN_STATUS_OK) \
        (_f_st) = POK; \
    else if ( (_c_st) == PIN_STATUS_NONE) \
        (_f_st) = 2; /* PNPICK or PNCHOI */ \
    else if ( (_c_st) == PIN_STATUS_NO_IN) \
        (_f_st) = PNONE;
    
#define IN_ST_CONVERT_F_TO_C( _f_st, _c_st) \
    if ( (_f_st) == PNONE) \
        (_c_st) = PIN_STATUS_NO_IN; \
    else if ( (_f_st) == POK) \
        (_c_st) = PIN_STATUS_OK; \
    else if ( (_f_st) == 2) /* PNPICK or PNCHOI */ \
        (_c_st) = PIN_STATUS_NONE;

#define GETENV_UNIT_NO( _file_num, _unit_no, _file_name) \
{ \
    unit_no[0] = '\0'; \
    strcpy( unit_no, "FU00"); \
    if ( _file_num < 0 || _file_num > 99) { \
        _file_name = (char *)NULL; \
    } else { \
        if ( _file_num < 10) { \
            sprintf( &_unit_no[3], "%d", _file_num); \
        } else { \
            sprintf( &_unit_no[2], "%d", _file_num); \
        } \
        _file_name = (char *)getenv( _unit_no); \
    } \
}
