/*
 * $Id: SEP_README.j,v 1.1 1991/09/13 07:45:49 proj Exp $
 */
	SEP - SpotLocation��ĥ�ץ�ȥ���

SEP�ϡ����ݥåȥ��������ι�®��ư��¸�����ץ�ȥ���Ǥ���

�̾���ץꥱ�������ˤ����륫�������ư�ν����ϰʲ��褦�ˤʤ�ޤ���

	nested_list = XVaCreateNestedList(dummy, XNSpotLocation, spot NULL);
	XSetICValues(ic, XNPreeditAttributes, nested_list, NULL);
	XFree(dpy, nested_list);

�����ߥʥ륨�ߥ�졼���Τ褦�ʥ��ץꥱ�������ξ�硢���ˤ˥���������ư����
ɬ�פ�����ޤ�������椨�����Τ褦�ʽ����ϡ����ץꥱ����������ǽ�����������
���ޤ��ޤ���
SEP�ϡ�����������褷�ޤ������ץꥱ�������ϡ�SEP��Ȥ������ʲ��δؿ���Ƥ�
�����Ǥ���

	_XipChangeSpot(ic, spot_x, spot_y);

_XipChangeSpot()�ξܺ٤ϡ�xwnmo�Υޥ˥奢���SEP��ʬ�򻲾Ȳ�������

-------------------------------------------------------------------------
XIM�饤�֥���XWNMO�֤Υ����åȤ�Ȥä��̿��ǡ���

    �׵�
	�Х��ȿ�	��		����
    XIM -> XWNMO (ximChangeSpotReq)
	1	    XIM_ChangeSpot(20)	�ꥯ�����ȥ�����
	1				̤����
	2		12		�ǡ�����Ĺ��
	4		CARD32		xic
	2		INT16		spot location x
	2		INT16		spot location y

    ����
	�Х��ȿ�	��		����
    XIM <- XWNMO (sz_ximEventReply)
	2				����
			0		 ����
			-1		 �۾�
	6				̤����

