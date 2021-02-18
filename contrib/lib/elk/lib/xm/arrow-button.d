;;; -*-Scheme-*-

(define-widget-type 'arrowbutton "ArrowB.h")

(define-widget-class 'arrow-button 'xmArrowButtonWidgetClass)

(define-callback 'arrow-button 'activateCallback #t)
(define-callback 'arrow-button 'armCallback      #t)
(define-callback 'arrow-button 'disarmCallback   #t)
