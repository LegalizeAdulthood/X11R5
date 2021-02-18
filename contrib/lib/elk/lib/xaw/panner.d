;;; -*-Scheme-*-

(define-widget-type 'panner "Panner.h")

(define-widget-class 'panner 'pannerWidgetClass)

(define-callback 'panner 'reportCallback #t)

(c->scheme 'panner-reportCallback
"   XawPannerReport *p = (XawPannerReport *)x;

    return Cons (Make_Fixnum (p->slider_x), Make_Fixnum (p->slider_y));")
