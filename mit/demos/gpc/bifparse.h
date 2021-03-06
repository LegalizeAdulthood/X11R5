/* $XConsortium: bifparse.h,v 5.1 91/02/16 10:07:01 rws Exp $ */


typedef union 	{
	long		l;
	double		f;
	StringType	str;
	} YYSTYPE;
extern YYSTYPE yylval;
#define LONG 257
#define REAL 258
#define QSTRING 259
#define BEGIN_STRUCTURE 260
#define END_STRUCTURE 261
#define LABEL 262
#define MARKER 263
#define MARKER3 264
#define LINE 265
#define LINE3 266
#define POLYGON 267
#define POLYGON3 268
#define FILL_AREA_SET 269
#define FILL_AREA_SET3 270
#define TRIANGLE3 271
#define QUAD_MESH3 272
#define INDEX_POLYGONS3 273
#define GEN_SPHERE3 274
#define GEN_CIRCLE 275
#define GEN_CIRCLE3 276
#define TEXT 277
#define TEXT3 278
#define ANNOTATION_TEXT3 279
#define PIXEL_MAP3 280
#define NON_UNIFORM_BSPLINE_CURVE 281
#define NON_UNIFORM_BSPLINE_SURFACE 282
#define CURVE_APPROXIMATION_CRITERIA 283
#define TRIMCURVE_APPROXIMATION_CRITERIA 284
#define SURFACE_APPROXIMATION_CRITERIA 285
#define MARKER_TYPE 286
#define MARKER_SIZE 287
#define MARKER_COLOR 288
#define MARKER_COLOR_INDEX 289
#define LINE_TYPE 290
#define LINE_WIDTH 291
#define LINE_COLOR 292
#define LINE_COLOR_INDEX 293
#define LINE_SHADING 294
#define INTERIOR_STYLE 295
#define INTERIOR_PATTERN_INDEX 296
#define INTERIOR_COLOR 297
#define INTERIOR_COLOR_INDEX 298
#define BACKFACE_INTERIOR_COLOR 299
#define BACKFACE_INTERIOR_COLOR_INDEX 300
#define INTERIOR_SHADING 301
#define INTERIOR_LIGHTING 302
#define SURFACE_PROPERTIES 303
#define BACKFACE_PROPERTIES 304
#define BACKFACE_PROCESSING 305
#define EDGE_FLAG 306
#define EDGE_TYPE 307
#define EDGE_WIDTH 308
#define EDGE_COLOR 309
#define EDGE_COLOR_INDEX 310
#define TEXT_FONT 311
#define TEXT_PREC 312
#define TEXT_COLOR 313
#define TEXT_COLOR_INDEX 314
#define TEXT_PATH 315
#define TEXT_ALIGN 316
#define CHAR_HEIGHT 317
#define CHAR_EXP 318
#define CHAR_SPACE 319
#define CHAR_UP_VECTOR 320
#define ANNO_TEXT_CHAR_HEIGHT 321
#define ANNO_TEXT_CHAR_UP_VECTOR 322
#define ANNO_TEXT_STYLE 323
#define LIGHT_STATE 324
#define DEPTHCUE_INDEX 325
#define HLHS_REMOVAL 326
#define IDENTITY3 327
#define CONCAT_MATRIX3 328
#define INVERT_MATRIX3 329
#define ROTATE3 330
#define ROTATE_XYZ3 331
#define TRANSLATE3 332
#define SCALE3 333
#define MATRIX3 334
#define GET_MATRIX3 335
#define PUSH_MATRIX3 336
#define POP_MATRIX3 337
#define GLOBAL_TRANSFORMATION3 338
#define LOCAL_TRANSFORMATION3 339
#define APPLY_TO_GLOBAL3 340
#define APPLY_TO_LOCAL3 341
#define VIEW_ORIENTATION3 342
#define VIEW_MAPPING3 343
#define ACTIVE_VIEW 344
#define EXECUTE_STRUCTURE 345
#define CALL_STRUCTURE 346
#define READ_GEOMETRY_FILE 347
#define CLEAR_GEOMETRY 348
#define BEGIN_TEST 349
#define END_TEST 350
#define PAUSE 351
#define SLEEP 352
#define INVOKE_AT_FRAME 353
#define DEFINE_COLOR 354
#define BACKGROUND_COLOR 355
#define BACKGROUND_COLOR_INDEX 356
#define DEFINE_VIEW_SPECIFICATION 357
#define DEFAULT_VIEW_SPECIFICATION 358
#define DEFINE_LIGHT 359
#define DEFINE_DEPTHCUE 360
#define CONFIGURATION 361
#define KNOTS 362
#define CTRL_POINTS 363
#define UKNOTS 364
#define VKNOTS 365
#define TRIMLOOP 366
#define TRIMCURVE 367
#define RATIONAL 368
#define NON_RATIONAL 369
#define IGNORE_GROUP 370
#define VERTEX_COLORS 371
#define VERTEX_NORMALS 372
#define VERTEX_COLOR_INDICES 373
#define FACET_COLORS 374
#define FACET_NORMALS 375
#define FACET_COLOR_INDICES 376
#define VERTEX_COORDINATES 377
#define EDGE_VISIBILITY 378
#define ENABLE 379
#define DISABLE 380
#define STRING 381
#define CHAR 382
#define STROKE 383
#define WORLD 384
#define MODELLING 385
#define AMBIENT_LIGHT 386
#define DIRECTIONAL_LIGHT 387
#define POSITIONAL_LIGHT 388
#define SPOT_LIGHT 389
#define LD_TRANSFORM 390
#define ACTIVATE_LIST 391
#define DEACTIVATE_LIST 392
#define HLHSRID 393
#define HLHS_DISABLE 394
#define HLHS_ENABLE 395
#define MAKE_RAMP 396
#define COLOR_LIST 397
#define RGB 398
#define CIE 399
#define HSV 400
#define HLS 401
#define PRECONCAT 402
#define POSTCONCAT 403
#define REPLACE 404
#define X_AXIS 405
#define Y_AXIS 406
#define Z_AXIS 407
#define PERSPECTIVE 408
#define PARALLEL 409
#define MATCH_VIEW_AREA 410
#define ADJUST_X 411
#define ADJUST_Y 412
#define GROW 413
#define SHRINK 414
#define XY_CLIP 415
#define NO_XY_CLIP 416
#define FRONT_CLIP 417
#define NO_FRONT_CLIP 418
#define BACK_CLIP 419
#define NO_BACK_CLIP 420
#define HOLLOW 421
#define SOLID 422
#define PATTERN 423
#define EMPTY 424
#define VIEW_MAPPING 425
#define VIEW_ORIENTATION 426
#define GLOBAL_MODELLING 427
#define LOCAL_MODELLING 428
#define COMPOSITE_MODELLING 429
#define SPECIFY_REPORT_FILE 430
#define TO 431
#define END 432
#define EXECUTE 433
#define CALL 434
#define WINDOW_SIZE 435
#define DOUBLE_BUFFER 436
#define SINGLE_BUFFER 437
#define TRUE_COLOR 438
#define PSEUDO_COLOR 439
#define FACET_CONNECTIVITY 440
#define EXACT 441
#define CENTER 442
#define RADIUS 443
#define SCALE_FACTORS 444
#define NORMAL 445
#define TEXT_DIRECTION 446
#define ADD 447
#define AND 448
#define CLEAR 449
#define INVERT 450
#define NAND 451
#define NOOP 452
#define NOR 453
#define OR 454
#define SET 455
#define SUBTRACT_DEST 456
#define SUBTRACT_SOURCE 457
#define XOR 458
#define PIXEL_VALUES 459
#define INTENSITY_VALUES 460
#define PIXEL_VALUE_SEGMENTS 461
#define INTENSITY_VALUE_SEGMENTS 462
#define FIRST_FILE 463
#define END_GEOM_FILE 464
#define UNRECOGNIZED_KEYWORD 465
