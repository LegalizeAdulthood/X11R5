;;; -*-Scheme-*-

(define-widget-type 'cascadebutton "CascadeB.h")

(define-widget-class 'cascade-button 'xmCascadeButtonWidgetClass)

(define-callback 'cascade-button 'activateCallback  #t)
(define-callback 'cascade-button 'cascadingCallback #t)
