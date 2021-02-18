;;; -*-Scheme-*-

(define-widget-type 'pushbutton "PushB.h")

(define-widget-class 'push-button 'xmPushButtonWidgetClass)

(define-callback 'push-button 'activateCallback #t)
(define-callback 'push-button 'armCallback      #t)
(define-callback 'push-button 'disarmCallback   #t)
