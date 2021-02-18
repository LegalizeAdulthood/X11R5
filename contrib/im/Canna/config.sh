/*
 * ���Υե�����Ǥϡ��ƥե�����򥤥󥹥ȡ��뤹��ǥ��쥯�ȥ�ʤɤ�
 * ���ꤹ�뤿��Υޥ�������ꤷ�ޤ���
 * 
 * ������ѥ����Ԥ�����ɬ�� config.sh �������ԤäƤ���������
 * 
 * �ʲ��˼����ޥ�������ꤷ�Ʋ�������
 *
 * irohaBinDir;
 *		���ޥ�ɤ򥤥󥹥ȡ��뤹��ǥ��쥯�ȥ�Ǥ���
 *		���Υǥ��쥯�ȥ�˥��ޥ�ɥ������ѥ���ĥ��褦��
 *		���Ƥ���������
 * irohaLibDir;
 *		�饤�֥�����򥤥󥹥ȡ��뤹��ǥ��쥯�ȥ�Ǥ���
 * DicDir;
 *		����򥤥󥹥ȡ��뤹��ǥ��쥯�ȥ�Ǥ���
 * ErrDir:
 *		���ե�����򥤥󥹥ȡ��뤹��ǥ��쥯�ȥ�Ǥ���
 *		���ե�����ϡ����ʴ����Ѵ������Ф����顼��å�������
 *		���Ϥ���ե�����Ǥ���
 * LockDir;
 *		��å��ե�����򥤥󥹥ȡ��뤹��ǥ��쥯�ȥ�Ǥ���    
 *		��å��ե�����ϡ����ʴ����Ѵ������Ф�
 *		ʣ���ĵ�ư���ʤ��褦�ˤ��뤿��ˡ����������ե�����Ǥ���
 * LockFile;
 *		��å��ե������̾���Ǥ���
 */

irohaBinDir = /usr/local/bin
irohaLibDir = /usr/local/lib/iroha
DicDir = $(irohaLibDir)/dic
ErrDir = /tmp
LockDir = /tmp
LockFile = .IROHALOCK

/*
 * �������鲼���ѹ����ʤ��褦�ˤ��Ƥ���������
 */

              BINDIR = $(irohaBinDir)
              LIBDIR = $(irohaLibDir)
 IROHASERVER_DEFINES = -DLOCKDIR=\"$(LockDir)\" \
			-DDICHOME=\"$(DicDir)\" \
			-DERRDIR=\"$(ErrDir)\" \
			-DLOCKFILE=\"$(LockFile)\"
         RKC_DEFINES = -DIROHAHOSTFILE=\"$(irohaLibDir)/irohahost\"
       UILIB_DEFINES = -DLIBDIR=\"$(LIBDIR)\"
     SCRIPTS_DEFINES = -DLOCKFILEPATH=\"$(LockDir)/$(LockFile)\"
