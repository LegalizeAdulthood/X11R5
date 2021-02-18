; -*-mode: emacs-lisp-*-

(defun iroha-version ()
  (interactive)
  (message "$Header: /private/usr/kon/emacs/RCS/iroha.el,v 1.24 91/02/20 17:40:00 kon Exp Locker: kon $"))

(provide 'iroha)

;;; ����Ϥ��ѿ�

(defvar iroha-save-undo-text-predicate nil)
(defvar iroha-undo-hook nil)

(defvar iroha-do-keybind-for-functionkeys t)
(defvar iroha-use-functional-numbers nil)
(defvar iroha-use-space-key-as-henkan-region t)

(defvar iroha-server nil)
(defvar iroha-file   nil)

(defvar iroha-underline nil)
(defvar iroha-with-fences (not iroha-underline))

(if iroha-underline
    (require 'attribute) )

;;; ����ϤΥ⡼�ɤ˴ؤ������

(defconst iroha:*alpha-mode*    0)
(defconst iroha:*henkan-mode*   1)
(defconst iroha:*hex-mode*      18)
(defconst iroha:*bushu-mode*    19)
(defconst iroha:*kigou-mode*    2)
(defconst iroha:*touroku*       20)

;;; ����Ϥδؿ��˴ؤ������

(defconst iroha:*henkan* 24)
(defconst iroha:*fn-extend-mode* 29)

(if iroha-use-functional-numbers
    (progn
      (defconst iroha:*fn-undefined*            0)
      (defconst iroha:*fn-selfInsert*           1)
      (defconst iroha:*fn-functionalInsert*     2)
      (defconst iroha:*fn-quotedInsert*         3)
      (defconst iroha:*fn-japaneseMode*         4)
      (defconst iroha:*fn-alphaMode*            5)
      (defconst iroha:*fn-henkanNyuryokuMode*   6)
      (defconst iroha:*fn-zenHiraKakuteiMode*   7)
      (defconst iroha:*fn-zenKataKakuteiMode*   8)
      (defconst iroha:*fn-hanKataKakuteiMode*   9)
      (defconst iroha:*fn-zenAlphaKakuteiMode* 10)
      (defconst iroha:*fn-hanAlphaKakuteiMode* 11)
      (defconst iroha:*fn-hexMode*             12)
      (defconst iroha:*fn-bushuMode*           13)
      (defconst iroha:*fn-kigouMode*           14)
      (defconst iroha:*fn-forward*             15)
      (defconst iroha:*fn-backward*            16)
      (defconst iroha:*fn-next*                17)
      (defconst iroha:*fn-prev*                18)
      (defconst iroha:*fn-beginningOfLine*     19)
      (defconst iroha:*fn-endOfLine*           20)
      (defconst iroha:*fn-deleteNext*          21)
      (defconst iroha:*fn-deletePrevious*      22)
      (defconst iroha:*fn-killToEndOfLine*     23)
      (defconst iroha:*fn-henkan*              24)
      (defconst iroha:*fn-kakutei*             25)
      (defconst iroha:*fn-extend*              26)
      (defconst iroha:*fn-shrink*              27)
      (defconst iroha:*fn-quit*                28)
      (defconst iroha:*fn-touroku*             29)
      (defconst iroha:*fn-convertAsHex*        30)
      (defconst iroha:*fn-convertAsBushu*      31)
      (defconst iroha:*fn-kouhoIchiran*        32)
      (defconst iroha:*fn-bubunMuhenkan*       33)
      (defconst iroha:*fn-zenkaku*             34)
      (defconst iroha:*fn-hankaku*             35)
      (defconst iroha:*fn-toUpper*             36)
      (defconst iroha:*fn-capitalize*          37)
      (defconst iroha:*fn-toLower*             38)
      ) )

;;;
;;; �⡼�ɥ饤��ν���
;;;

(defvar iroha:*kanji-mode-string* "����")

(defvar mode-line-iroha-mode "    ")

(make-variable-buffer-local 'mode-line-iroha-mode)

(defun mode-line-iroha-mode-update (str)
  (setq mode-line-iroha-mode str)
  (set-buffer-modified-p (buffer-modified-p))
  )

(if (not (fboundp 'member))
    (defun member (x l)
      (cond ((atom l) nil)
	    ((equal x (car l)) l)
	    (t (member x (cdr l))))) )

(defun iroha:create-mode-line ()
  (if (not (memq 'mode-line-iroha-mode mode-line-format))
      (if (memq 'mode-line-egg-mode mode-line-format)
	  (progn
;	    (rplaca (member " [" mode-line-format) (purecopy " -"))
	    (rplaca (member 'mode-line-egg-mode mode-line-format)
		    'mode-line-iroha-mode)
;	    (rplaca (member "]" mode-line-format) (purecopy "- "))
	    )
	(setq-default mode-line-format
		      (append (list (purecopy " [")
				    'mode-line-iroha-mode
				    (purecopy "]"))
			      mode-line-format))))
  (mode-line-iroha-mode-update mode-line-iroha-mode) )

(defun iroha:mode-line-display ()
  (mode-line-iroha-mode-update mode-line-iroha-mode))

(defconst iroha:*mode-assoc-table*
  '(("" . "    ") ("  [��]" . "  ��") ("  [����]" . "����")
    ("��[��]" . "�Τ�") ("��[��]" . "�Υ�")
    ("��[��]" . "�Σ�") ("��[a]" . "�� a")
    ("  [����]" . "����") ("  [����]" . "����") ("  [����]" . "����")
    ("  [����]" . "����")
    ("  [��]" . "  ��") ("  [��]" . "  ��")
    ("  [��]" . "  ��") ("  [a]" . "   a")
    ("  [16��]" . "16��") ("  [����]" . "����")
    ("  [��ĥ]" . "��ĥ")
    ("  [��]" . "  ��") ("  [��]" . "  ��") ("  [����]" . "����")
    ("  [�ѹ�]" . "�ѹ�") ("  [�Ѵ�]" . "�Ѵ�") ("  [���]" . "���")
    ("  [��Ͽ]" . "��Ͽ") ("  [�ʻ�]" . "�ʻ�") ("  [����]" . "����")
    ("  [��]" . "  ��") ("  [�Խ�]" . "�Խ�")
    ))

(defconst iroha:*base-mode-set*
  '("  ��" "�Τ�" "�Υ�" "�Σ�" "�� a" "  ��" "  ��" "  ��" "   a"))
;;;
;;; Iroha local variables
;;;

(defvar iroha:*japanese-mode* nil "T if iroha mode is ``japanese''.")
(make-variable-buffer-local 'iroha:*japanese-mode*)
(set-default 'iroha:*japanese-mode* nil)

(defvar iroha:*fence-mode* nil)
(make-variable-buffer-local 'iroha:*fence-mode*)
(setq-default iroha:*fence-mode* nil)

;;;
;;; global variables
;;;

(defvar iroha-sys:*global-map* (copy-keymap global-map))
(defvar iroha:*region-start* (make-marker))
(defvar iroha:*region-end*   (make-marker))
(defvar iroha:*spos-undo-text* (make-marker))
(defvar iroha:*epos-undo-text* (make-marker))
(defvar iroha:*undo-text-yomi* nil)
(defvar iroha:*local-map-backup*  nil)
(defvar iroha:*last-kouho* 0)
(defvar iroha:*initialized* nil)
(defvar iroha:*previous-window* nil)
(defvar iroha:*minibuffer-local-map-backup* nil)
(defvar iroha:*cursor-was-in-minibuffer* nil)
(defvar iroha:*total-time* 0)
(defvar iroha:*subtotal-time* 0)
(defvar iroha:*use-region-as-henkan-region* nil)

;;;
;;; �����ޥåץơ��֥�
;;;

;; �ե��󥹥⡼�ɤǤΥ�����ޥå�
(defvar iroha-mode-map (make-keymap))

(let ((ch 0))
  (while (<= ch 127)
    (aset iroha-mode-map ch 'iroha-functional-insert-command)
    (setq ch (1+ ch))))

;; �ߥ˥Хåե��˲�����ɽ�����Ƥ�����Υ�����ޥå�
(defvar iroha-minibuffer-mode-map (make-keymap))

(let ((ch 0))
  (while (<= ch 127)
    (aset iroha-minibuffer-mode-map ch 'iroha-minibuffer-insert-command)
    (setq ch (1+ ch))))


;;;
;;; �����Х�ؿ��ν��ؤ�
;;;


;; Keyboard quit

(if (not (fboundp 'iroha-sys:keyboard-quit))
    (fset 'iroha-sys:keyboard-quit (symbol-function 'keyboard-quit)) )

(defun iroha:keyboard-quit ()
  "See documents for iroha-sys:keyboard-quit"
  (interactive)
  (if iroha:*japanese-mode*
      (progn
;	(setq iroha:*japanese-mode* nil)
	(setq iroha:*fence-mode* nil)
	(if (boundp 'disable-undo)
	    (setq disable-undo iroha:*fence-mode*))
	(iroha:mode-line-display) ))
  (iroha-sys:keyboard-quit) )

;; Abort recursive edit

(if (not (fboundp 'iroha-sys:abort-recursive-edit))
    (fset 'iroha-sys:abort-recursive-edit 
	  (symbol-function 'abort-recursive-edit)) )

(defun iroha:abort-recursive-edit ()
  "see documents for iroha-sys:abort-recursive-edit"
  (interactive)
  (if iroha:*japanese-mode*
      (progn
	(setq iroha:*japanese-mode* nil)
	(setq iroha:*fence-mode* nil)
	(if (boundp 'disable-undo)
	    (setq disable-undo iroha:*fence-mode*))
	(iroha:mode-line-display) ))
  (iroha-sys:abort-recursive-edit) )


;; Exit-minibuffer

(if (not (fboundp 'iroha-sys:exit-minibuffer))
    (fset 'iroha-sys:exit-minibuffer (symbol-function 'exit-minibuffer)) )

(defun iroha:exit-minibuffer ()
  "See documents for iroha-sys:exit-minibuffer"
  (interactive)
;  (setq iroha:*japanese-mode* nil)
  (iroha-sys:exit-minibuffer) )

;; kill-emacs

(if (not (fboundp 'iroha-sys:kill-emacs))
    (fset 'iroha-sys:kill-emacs (symbol-function 'kill-emacs)))

(defun iroha:kill-emacs (&optional query)
  (interactive "P")
  (message "�ؤ���ϡ٤μ���򥻡��֤��ޤ���")
  (iroha:finalize)
  (iroha-sys:kill-emacs query))


;;;
;;; keyboard input for japanese language
;;;

(defun iroha-functional-insert-command (arg)
  "Use input character as a key of complex translation input such as\n\
kana-to-kanji translation."
  (interactive "p")
  (iroha:functional-insert-command2 last-command-char arg) )

(defun iroha:functional-insert-command2 (ch arg)
  "This function actualy isert a converted Japanese string."
  ;; ���δؿ���Ϳ����줿ʸ�������ܸ����ϤΤ���Υ������ϤȤ��Ƽ�갷
  ;; �������ܸ����Ϥ���ַ�̤�ޤ᤿������Emacs�ΥХåե���ȿ�Ǥ�����
  ;; �ؿ��Ǥ��롣
  (if (fboundp 'iroha-current-tick)
      (setq iroha:*total-time* (- iroha:*total-time* (iroha-current-tick))))
  (iroha:display-candidates (iroha-key-to-string ch)) 
  (if (fboundp 'iroha-current-tick)
      (setq iroha:*total-time* (+ iroha:*total-time* (iroha-current-tick))))
  )

(defun iroha:display-candidates (strs)
  (if (fboundp 'iroha-current-tick)
      (setq iroha:*subtotal-time* 
	    (- iroha:*subtotal-time* (iroha-current-tick))) )
  (let* ((nochange (null (car (cdr strs)))) 
	 ;; iroha-key-to-string�η�̤Υꥹ�Ȥ������ܤ����Ǥϸ���ɽ����
	 ;; �٤�ʸ�������ξ��󤬳�Ǽ����Ƥ��뤬�����Υե�����ɤ��̥�
	 ;; �Ǥ���Ȥ������Ȥϡ���ַ�̤�ɽ�����Ѳ������ʤ����ɤ��Ȥ�
	 ;; �����Ȥ�ɽ����(�Ǥ⤳�����̥�Ǥ��̤����äƤ��뤳�Ȥ�����
	 ;; �Τǡ����Υ����ǥ��󥰤ǤϤ��ᤫ�⤷��ʤ��ʤ�)
	 (kouho)
	 (len)
	 (echo-keystrokes 0)
	 (work) )
    (cond ((null (cdr strs)) ; ���顼�������ä����
	   (beep)
	   (message (car strs)) )
	  ((not nochange)
	   ;; �⤷����ɽ�������η�̤����Ѥ�äƤ��ʤ��ʤ��Ȥ���......
	   ;; (��Ǥ�Ҥ٤��������Τ褦�ˤ��Ƥ������Ϥ����ʤ��Τ��⤷��
	   ;; �ʤ������ȤǤ�äȤޤȤ�˽񤭴�����ɬ�פ�����ʤ�)

	   ;; �ߥ˥Хåե��˴ؤ������κ��
	   (setq iroha:*cursor-was-in-minibuffer* nil)

	   ;; ���礨���ǽ�����˽񤤤Ƥ�������ַ�̤�ä���
	   (cond ((not (zerop iroha:*last-kouho*))
		  (if iroha-underline
		      ; �ޤ���°����ä���
		      (progn
			(attribute-off-region 'inverse
					      iroha:*region-start*
					      iroha:*region-end*)
			(attribute-off-region 'underline
					      iroha:*region-start*
					      iroha:*region-end*)) )
		  (delete-region iroha:*region-start*
				 iroha:*region-end*) ))
	   ;; ��ַ�̤�ä����Τǡ��ѿ���ꥻ�åȤ��Ƥ���
	   (setq iroha:*last-kouho* 0)
	   ;; ���ꤷ��ʸ���󤬤���Ф�����������롣
	   (cond ((not (zerop (length (car strs))))
		  (setq work
			(car (cdr (cdr (cdr (cdr strs))))))
		  (cond ((and work
			      (or (null iroha-save-undo-text-predicate)
				  (funcall iroha-save-undo-text-predicate
					   work )))
			 (setq iroha:*undo-text-yomi* work)
			 (set-marker iroha:*spos-undo-text* (point))
			 (insert (car strs))
			 (iroha:do-auto-fill)
			 (set-marker iroha:*epos-undo-text* (point)) )
			(t
			 (insert (car strs))
			 (iroha:do-auto-fill) ))
		  ) )

	   ;; ��������ʸ����ꥹ�Ȥ��������롣
	   (setq strs (cdr strs))
	   (setq kouho (car strs))

	   ;; ���ϸ���ˤĤ��Ƥκ�ȤǤ��롣
	   (setq len (length (car kouho)))

	   ;; ������������롣����Ͻ������ܤˤƶ��ޤ�롣
	   (cond ((not (zerop len))
		  (set-marker iroha:*region-start* (point))
		  (if iroha-with-fences
		      (progn
			(insert "||")
			(set-marker iroha:*region-end* (point))
			(backward-char 1) ))
		  (insert (car kouho))
		  (if (not iroha-with-fences)
		      (set-marker iroha:*region-end* (point)) )
		  (if iroha-underline
		      (attribute-on-region 
		       'underline
		       iroha:*region-start* iroha:*region-end*) )
		  (setq iroha:*last-kouho* len)
		  ))

	   ;; �����ΰ�Ǥ϶�Ĵ������ʸ����¸�ߤ����Τȹͤ���
	   ;; ��롣��Ĵ������ʸ����Emacs�Ǥϥ�������ݥ������ˤ�ɽ��
	   ;; ���뤳�ȤȤ��롣��Ĵ������ʸ�����ʤ��ΤǤ���С���������
	   ;; �ϰ��ָ����ʬ(���Ϥ��Ԥ���ݥ����)���֤��Ƥ�����

	   ;; ����������ư���롣
           (if (not iroha-underline)
	       (backward-char 
		(- iroha:*last-kouho*
		   ;; ����������֤ϡ�ȿžɽ����ʬ��¸�ߤ��ʤ��ΤǤ���С�
		   ;; ����ʸ����κǸ����ʬ�Ȥ���ȿžɽ����ʬ��¸�ߤ����
		   ;; �Ǥ���С�������ʬ�λϤ�Ȥ��롣
		   (cond ((zerop (car (cdr (cdr kouho))))
			  iroha:*last-kouho*)
			 (t (car (cdr kouho))) )) )
	     (if (and (not (zerop (car (cdr (cdr kouho)))))
		      (not (zerop len)))
		 ; �����Ĺ����0�Ǥʤ���
		 ; ȿžɽ����Ĺ����0�Ǥʤ���С�
		 ; ������ʬ����žɽ�����롣
		 (attribute-on-region 'inverse
				      (+ iroha:*region-start*
					 (if iroha-with-fences 1 0)
					 (car (cdr kouho)))
				      (+ iroha:*region-start*
					 (if iroha-with-fences 1 0)
					 (car (cdr kouho))
					 (car (cdr (cdr kouho))))))
	     ) )
          (t ; ������Ʊ���٤Ȥ������
	   (setq strs (cdr strs))) )

    (setq strs (cdr strs)) ;; ���顼���ФƤ�����ˤϤ��λ���
			   ;; strs �� nil �ˤʤ롣
    
    ;; �⡼�ɤ�ɽ��ʸ����¸�ߤ���Ф����⡼�ɤȤ��Ƽ�갷����
    (if (stringp (car strs))
	(let ((mode-assoc (assoc (car strs) iroha:*mode-assoc-table*)))
	  (cond (mode-assoc
		 (mode-line-iroha-mode-update (cdr mode-assoc))
		 (cond ((member (cdr mode-assoc) iroha:*base-mode-set*)
			(setq iroha:*kanji-mode-string* (cdr mode-assoc)) ))
		 )
		(t (mode-line-iroha-mode-update (car strs))) )))

    (setq strs (cdr strs))
    ;; ����ɽ�����ʤ���Хե��󥹥⡼�ɤ���ȴ���롣
    (cond ((zerop iroha:*last-kouho*)
	   (iroha:quit-iroha-mode) ) )
    ;; �ߥ˥Хåե��˽񤯤��Ȥ�¸�ߤ���ΤǤ���С������ߥ˥Хåե�
    ;; ��ɽ�����롣
    (cond ((car strs)
	   (setq iroha:*cursor-was-in-minibuffer* t)
	   (iroha:minibuffer-input (car strs)) )
	  (t
	   (cond (iroha:*cursor-was-in-minibuffer*
		  (setq iroha:*previous-window* (selected-window))
		  (select-window (minibuffer-window))
		  ;; �ߥ˥Хåե��Υ����ޥåפ���¸���Ƥ�����
		  (setq iroha:*minibuffer-local-map-backup* 
			(current-local-map))
		  (use-local-map iroha-minibuffer-mode-map)
		  ) ) )
	  )
    ) 
  (if (fboundp 'iroha-current-tick)
      (setq iroha:*subtotal-time* 
	    (+ iroha:*subtotal-time* (iroha-current-tick))) )
  )

(defun iroha:minibuffer-input (strs)
  "Displaying misc informations for kana-to-kanji input."
  ;; ��Ȥ�ߥ˥Хåե��˰ܤ��Τ˺ݤ��ơ����ߤΥ�����ɥ��ξ������¸
  ;; ���Ƥ�����
  (setq iroha:*previous-window* (selected-window))
  (select-window (minibuffer-window))
  ;; �ߥ˥Хåե��򥯥ꥢ���롣
  (if iroha-underline
      (attribute-off-region 'inverse (point-min) (point-max)))
  (delete-region (point-min) (point-max))
  ;; �ߥ˥Хåե��Υ����ޥåפ���¸���Ƥ�����
  (setq iroha:*minibuffer-local-map-backup* (current-local-map))
  (use-local-map iroha-minibuffer-mode-map)
  (insert (car strs))
  ;; �ߥ˥Хåե���ȿžɽ������٤�ʸ���ΤȤ���˥���������ư���롣
  (cond ((not (zerop (car (cdr (cdr strs)))))
	 (backward-char (- (length (car strs)) (car (cdr strs)))) ))
  ;; �ߥ˥Хåե���ɽ������٤�ʸ���󤬥̥�ʸ����ʤΤǤ���С����Υ���
  ;; ��ɥ�����롣
  (cond ((zerop (length (car strs)))
	 (setq iroha:*cursor-was-in-minibuffer* nil)
	 (use-local-map iroha:*minibuffer-local-map-backup*)
	 (select-window iroha:*previous-window*) )
	((or (and (> (length (car strs)) 12)
		  (string= (substring (car strs) -12 nil)
			   "��Ͽ���ޤ���"))
	     (string= (car strs) "�桼�����񤬻��ꤵ��Ƥ��ޤ���")
	     (string= (car strs) "��������θ���Ϥ���ޤ���")
	     (string= (car strs) "ñ����Ͽ�Ǥ��ޤ���Ǥ���")
	     )
	 (delete-region (point-min) (point-max))
	 (setq iroha:*cursor-was-in-minibuffer* nil)
	 (use-local-map iroha:*minibuffer-local-map-backup*)
	 (select-window iroha:*previous-window*)
	 (message (car strs)) ))
  )

(defun iroha-minibuffer-insert-command (arg)
  "Use input character as a key of complex translation input such as\n\
kana-to-kanji translation, even if you are in the minibuffer."
  (interactive "p")
  (use-local-map iroha:*minibuffer-local-map-backup*)
  (select-window iroha:*previous-window*)
  (iroha:functional-insert-command2 last-command-char arg) )

;(defun iroha:minibuffer-input (strs)
;  (message (car strs)) )

;;;
;;; ����ϥ⡼�ɤμ���ϡ����� iroha-self-insert-command �Ǥ��롣����
;;; ���ޥ�ɤ����ƤΥ���ե��å������˥Х���ɤ���롣
;;;
;;; ���δؿ��Ǥϡ����ߤΥ⡼�ɤ����ܸ����ϥ⡼�ɤ��ɤ���������å����ơ�
;;; ���ܸ����ϥ⡼�ɤǤʤ��ΤǤ���С������ƥ�� self-insert-command 
;;; ��Ƥ֡����ܸ����ϥ⡼�ɤǤ���С��ե��󥹥⡼�ɤ����ꡢ
;;; iroha-functional-insert-command ��Ƥ֡�
;;;

(defun iroha-self-insert-command (arg)
  "Self insert pressed key and use it to assemble Romaji character."
  (interactive "p")
  (if (and iroha:*japanese-mode*
	   ;; �ե��󥹥⡼�ɤ��ä���⤦���٥ե��󥹥⡼�ɤ����ä��ꤷ
	   ;; �ʤ���
	   (not iroha:*fence-mode*) )
      (iroha:enter-iroha-mode-and-functional-insert)
    (self-insert-command arg) ))

(defun iroha-toggle-japanese-mode ()
  "Toggle iroha japanese mode."
  (interactive)
  (cond (iroha:*japanese-mode*
	 (setq iroha:*japanese-mode* nil) 
	 (iroha-abandon-undo-info)
	 (setq iroha:*use-region-as-henkan-region* nil)
	 (mode-line-iroha-mode-update "    ") )
	(t
	 (setq iroha:*japanese-mode* t)
	 (mode-line-iroha-mode-update iroha:*kanji-mode-string*) ) ))

(defun iroha:initialize ()
  (let ((init-val nil))
    (cond (iroha:*initialized*) ; initialize ����Ƥ����鲿�⤷�ʤ�
	  (t
	   (setq iroha:*initialized* t)
	   (setq init-val (iroha-initialize 
			   (if iroha-underline 0 1)
			   iroha-server iroha-file))
	   (cond ((car (cdr (cdr init-val)))
		  (iroha:output-warnings (car (cdr (cdr init-val)))) ))
	   (cond ((car (cdr init-val))
		  (error (car (cdr init-val))) ))
	   ) )
    (iroha-change-mode iroha:*henkan-mode*)
    init-val))

(defun iroha:finalize ()
  (cond ((null iroha:*initialized*)) ; initialize ����Ƥ��ʤ��ä��鲿�⤷�ʤ�
	(t
	 (setq iroha:*initialized* nil)
	 (let ((init-val (iroha-finalize)))
	   (cond ((car (cdr (cdr init-val)))
		  (iroha:output-warnings (car (cdr (cdr init-val)))) ))
	   (cond ((car (cdr init-val))
		  (error (car (cdr init-val))) ))
	   ) )) )

(defun iroha:enter-iroha-mode ()
  (if (not iroha:*initialized*)
      (progn 
	(message "�ؤ���ϡ٤ν������ԤäƤ��ޤ�....")
	(iroha:initialize)
	(message "�ؤ���ϡ٤ν������ԤäƤ��ޤ�....done")
	))
  (setq iroha:*local-map-backup*  (current-local-map))
  (setq iroha:*fence-mode* t)
  (if (boundp 'disable-undo)
      (setq disable-undo iroha:*fence-mode*))
  (use-local-map iroha-mode-map) )

(defun iroha:enter-iroha-mode-and-functional-insert ()
  (iroha:enter-iroha-mode)
  (setq iroha:*use-region-as-henkan-region* nil)
  (setq unread-command-char last-command-char))

(defun iroha:quit-iroha-mode ()
  (cond (iroha:*fence-mode*
	 (use-local-map iroha:*local-map-backup*)
	 (setq iroha:*fence-mode* nil)
	 (if (boundp 'disable-undo)
	     (setq disable-undo iroha:*fence-mode*))
	 ))
  (set-marker iroha:*region-start* nil)
  (set-marker iroha:*region-end* nil)
  )

(defun iroha-touroku ()
  "Register a word into a kana-to-kanji dictionary."
  (interactive)
  (if iroha:*japanese-mode*
      (progn
	(iroha:enter-iroha-mode)
	(iroha:display-candidates (iroha-touroku-string "")) )
    (beep)
  ))

(defun iroha-touroku-region (start end)
  "Register a word which is indicated by region into a kana-to-kanji\n\
dictionary."
  (interactive "r")
  (if iroha:*japanese-mode*
      (progn
	(iroha:enter-iroha-mode)
	(iroha:display-candidates
	 (if (or (zerop (- end start))
		 (> (- end start) 16)
		 (< (- end start) -16))
	     (if (fboundp 'iroha-do-function)
		 (iroha-do-function iroha:*fn-extend-mode*)
	       (iroha-key-to-string 138))
	   (iroha-touroku-string (buffer-substring start end))))) ))

(defun iroha-kigou-mode ()
  "Enter symbol choosing mode."
  (interactive)
  (if iroha:*japanese-mode*
      (progn
        (iroha:enter-iroha-mode)
	(iroha:display-candidates (iroha-change-mode iroha:*kigou-mode*)) )
    (beep)
  ))

(defun iroha-hex-mode ()
  "Enter hex code entering mode."
  (interactive)
  (if iroha:*japanese-mode*
      (progn
        (iroha:enter-iroha-mode)
	(iroha:display-candidates (iroha-change-mode iroha:*hex-mode*)) )
    (beep)
  ))

(defun iroha-bushu-mode ()
  "Enter special mode to convert by BUSHU name."
  (interactive)
  (if iroha:*japanese-mode*
      (progn
        (iroha:enter-iroha-mode)
	(iroha:display-candidates (iroha-change-mode iroha:*bushu-mode*)) )
    (beep)
  ))

(defun iroha-reset ()
  (interactive)
  (message "�ؤ���ϡ٤μ���򥻡��֤��ޤ���");
  (iroha:finalize)
  (message "�ؤ���ϡ٤κƽ������ԤäƤ��ޤ�....")
  (iroha:initialize)
  (message "�ؤ���ϡ٤κƽ������ԤäƤ��ޤ�....done")
  )
  

(defun iroha ()
  (interactive)
  (message "�ؤ���ϡ٤��������Ƥ��ޤ�....")
  (let (init-val)
    (cond ((and (fboundp 'iroha-initialize)
		(fboundp 'iroha-change-mode) )

	   ;; iroha ���Ȥ�����ϼ��ν����򤹤롣
	 
	   ;; �ؤ���ϡ٥����ƥ�ν����

	   (setq init-val (iroha:initialize))

	 ;; �����ΥХ���ǥ���

	   (let ((ch 32))
	     (while (< ch 127)
	       (aset global-map ch 'iroha-self-insert-command)
	       (setq ch (1+ ch)) ))

	   (cond ((let ((keys (car init-val)) (ok nil))
		    (while keys
		      (cond ((< (car keys) 128)
			     (global-set-key
			      (make-string 1 (car keys))
			      'iroha-toggle-japanese-mode)
			     (setq ok t) ))
		      (setq keys (cdr keys))
		      ) ok))
		 (t ; �ǥե���Ȥ�����
		  (global-set-key "\C-o" 'iroha-toggle-japanese-mode) ))

	   (if (not (keymapp (global-key-binding "\e[")))
	       (global-unset-key "\e[") )
	   (global-set-key "\e[210z" 'iroha-toggle-japanese-mode)
	   (if iroha-do-keybind-for-functionkeys
	       (progn
		 (global-set-key "\e[28~" 'iroha-touroku-region)
		 (global-set-key "\e[11~" 'iroha-kigou-mode)
		 (global-set-key "\e[12~" 'iroha-hex-mode)
		 (global-set-key "\e[13~" 'iroha-bushu-mode)
		 ))

	   (if iroha-use-space-key-as-henkan-region
	       (progn
		 (define-key ctl-x-map "\ " 'iroha-set-mark-command)
		 (global-set-key " " 'iroha-henkan-region-or-self-insert) ))

	 ;; �⡼�ɹԤκ���

	   (iroha:create-mode-line)
	   (mode-line-iroha-mode-update "    ")

	 ;; �����ƥ�ؿ��ν��ؤ�

	   (fset 'abort-recursive-edit 
		 (symbol-function 'iroha:abort-recursive-edit))
	   (fset 'keyboard-quit 
		 (symbol-function 'iroha:keyboard-quit))
	   (fset 'exit-minibuffer
		 (symbol-function 'iroha:exit-minibuffer))
	   (fset 'kill-emacs
		 (symbol-function 'iroha:kill-emacs)) )

	  ((fboundp 'iroha-initialize)
	   (beep)
	   (with-output-to-temp-buffer "*iroha-warning*"
	     (princ "���� Emacs �Ǥ� new-iroha ���Ȥ��ޤ���")
	     (terpri)
	     (print-help-return-message)) )

	  (t ; �ؤ���ϡ٥����ƥब�Ȥ��ʤ��ä����ν���
	   (beep)
	   (with-output-to-temp-buffer "*iroha-warning*"
	     (princ "���� Emacs �Ǥ� iroha ���Ȥ��ޤ���")
	     (terpri)
	     (print-help-return-message))
	   ))
    (message "�ؤ���ϡ٤��������Ƥ��ޤ�....done")
    ) )

;;;
;;; auto fill controll (from egg)
;;;

(defun iroha:do-auto-fill ()
  (if (and auto-fill-hook (not buffer-read-only)
	   (> (current-column) fill-column))
      (let ((ocolumn (current-column)))
	(run-hooks 'auto-fill-hook)
	(while (and (< fill-column (current-column))
		    (< (current-column) ocolumn))
  	  (setq ocolumn (current-column))
	  (run-hooks 'auto-fill-hook)))))

(defun iroha:output-warnings (mesg)
  (with-output-to-temp-buffer "*iroha-warning*"
    (while mesg
      (princ (car mesg))
      (terpri)
      (setq mesg (cdr mesg)) )
    (print-help-return-message)))

(defun iroha-undo (&optional arg)
  (interactive "*p")
  (if (and iroha:*undo-text-yomi*
	   (eq (current-buffer) (marker-buffer iroha:*spos-undo-text*)) )
      (progn
	(message "�ɤߤ��ᤷ�ޤ���")
	(switch-to-buffer (marker-buffer iroha:*spos-undo-text*))
	(goto-char iroha:*spos-undo-text*)
	(delete-region iroha:*spos-undo-text*
		       iroha:*epos-undo-text*)

	(if (null iroha:*japanese-mode*)
	    (iroha-toggle-japanese-mode) )
	(if (not iroha:*fence-mode*)
	    ;; �ե��󥹥⡼�ɤ��ä���⤦���٥ե��󥹥⡼�ɤ����ä��ꤷ
	    ;; �ʤ���
	    (iroha:enter-iroha-mode) )
	(iroha:display-candidates 
	 (let ((texts (iroha-store-yomi (car iroha:*undo-text-yomi*)
					(cdr iroha:*undo-text-yomi*) )) )
	   (cond (iroha-undo-hook
		  (funcall iroha-undo-hook))
		 (t texts) )))
	(iroha-abandon-undo-info)
	)
    (undo arg) ))

(defun iroha-abandon-undo-info ()
  (interactive)
  (setq iroha:*undo-text-yomi* nil)
  (set-marker iroha:*spos-undo-text* nil)
  (set-marker iroha:*epos-undo-text* nil) )

(defun iroha-henkan-region (start end)
  "Convert a text which is indicated by region into a kanji text."
  (interactive "r")
  (let ((res nil))
    (setq res (iroha-store-yomi (buffer-substring start end)))
    (delete-region start end)
    (iroha:enter-iroha-mode)
    (if (fboundp 'iroha-do-function)
	(setq res (iroha-do-function iroha:*henkan*)))
    (iroha:display-candidates res) ))

(defun iroha-reset-time ()
  (setq iroha:*total-time* 0 iroha:*subtotal-time* 0))

(defun iroha-time ()
  (list iroha:*total-time* iroha:*subtotal-time*))

;;;
;;; �ޡ������ޥ�ɡ�iroha-henkan-region-or-self-insert �ǻȤ�����
;;;

(defun iroha-set-mark-command (arg)
  "Beside setting mark, set mark as a HENKAN region if it is in\n\
the japanese mode."
  (interactive "P")
  (set-mark-command arg)
  (if iroha:*japanese-mode*
      (progn
	(setq iroha:*use-region-as-henkan-region* t)
	(message "Mark set(�Ѵ��ΰ賫��)") )))

(defun iroha-henkan-region-or-self-insert (arg)
  "Do kana-to-kanji convert region if HENKAN region is defined,\n\
self insert otherwise."
  (interactive "p")
  (if iroha:*use-region-as-henkan-region*
      (progn
	(setq iroha:*use-region-as-henkan-region* nil)
	(iroha-henkan-region (region-beginning) (region-end)))
    (iroha-self-insert-command arg) ))
