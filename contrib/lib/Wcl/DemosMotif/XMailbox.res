/*
 * XMailbox.res
 *
 * Mri resource file for xmailbox.
 *
 * This interface MUST include a XmText widget.
 * It must be registered w/ the main application using RegisterText().
 * See below.
 *
 * There are a number of functions that can be bound to various events
 * within the interface.  Commonly these are attached to something like
 * an activateCallback on a PushButton:
 *
 * WcExitCB	
 *	Will exit the application.
 *
 * ClearText
 *	This clears the messages in the mailbox window.  The next 
 *	incoming mail will become the first message in the window.
 *	This action happens automatically when the mailbox is emptied
 *	from a mailer like xmh or elm.
 *
 * HideMail
 *	Causes the mailbox window to disappear until the next incoming mail
 *	arrives.  Keeps the desktop clean.  When the hideNoMail resource
 *	is set to true, the mailbox window is hidden whenever there are
 * 	no unread mail messages left.
 *
 * ReScan
 *	Causes the mailbox to be rescanned for all unread messages and
 *	the mailbox window to be updated.  This action might be performed
 *	after a ClearText, when you wish to see some past message.
 *
 * None, some, or all of these actions may be used freely within this
 * interface.  There are a number of Mri internal actions as well, 
 * (WcExitCB is one of them) see Mri documentation for details.
 *
 */


/*
 * toplevel declaration.
 */
XMailbox*xmailbox.wcChildren:		table

/*
 * Uncomment this for a trace of the widget hierarchy.  Useful for building
 * resource files.
 */
!*wcTrace:				True

/*
 * The Text window.  Some kind of XmText construct MUST be declared and
 * used for displaying mail messages.
 */
XMailbox*text.wcConstructor:            XmCreateScrolledText
XMailbox*text.wcCallback:               RegisterText ( this )
XMailbox*text.editable:                 False
XMailbox*text.editMode:                 MULTI_LINE_EDIT
XMailbox*text.columns:                  40
XMailbox*text.rows:                     5

/*
 * I use a Table widget to hold it all together
 */
XMailbox*table.wcClass:			TableWidgetClass
XMailbox*table.wcChildren:		menuBar, text 

!                               	c r cs rs opts
XMailbox*table.layout:	menuBar		0 0  1  1  hH ;\
			textSW		0 1  1  1     ;

XMailbox*table.columnSpacing:         	5
XMailbox*table.rowSpacing:            	5
XMailbox*table.borderWidth:           	0
XMailbox*table.internalHeight:        	5
XMailbox*table.internalWidth:         	5

/*
 * Buttons attahced to a pulldown menu for invoking built in actions
 */
XMailbox*clear.wcConstructor:		XmCreateCascadeButton
XMailbox*clear.labelString:		Rubout
XMailbox*clear.mnemonic:		R
XMailbox*clear.activateCallback:	ClearTextCB

XMailbox*quit.wcConstructor:		XmCreateCascadeButton
XMailbox*quit.labelString:		Quit
XMailbox*quit.mnemonic:			Q
XMailbox*quit.activateCallback:		WcExitCB

XMailbox*hide.wcConstructor:		XmCreateCascadeButton
XMailbox*hide.labelString:		Hide
XMailbox*hide.mnemonic:			H
XMailbox*hide.activateCallback:		HideMailCB

XMailbox*rescan.wcConstructor:		XmCreateCascadeButton
XMailbox*rescan.labelString:		Rescan
XMailbox*rescan.mnemonic:		R
XMailbox*rescan.activateCallback:	ReScanCB

XMailbox*sep1.wcConstructor:            XmCreateSeparator
XMailbox*sep1.separatorType:            Double_Line

/*
 * the menubar
 */
XMailbox*menuBar.wcConstructor:         XmCreateMenuBar
XMailbox*menuBar.wcChildren:            fileMenu, file

XMailbox*file.wcConstructor:            XmCreateCascadeButton
XMailbox*file.labelString:              Command
XMailbox*file.mnemonic:                 C
XMailbox*file.subMenuId:                *fileMenu

XMailbox*fileMenu.wcConstructor:        XmCreatePulldownMenu
XMailbox*fileMenu.wcManaged:            False
XMailbox*fileMenu.wcChildren:           clear, hide, rescan, sep1, quit


