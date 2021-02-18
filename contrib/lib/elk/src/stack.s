/* int stksize();
 * int saveenv(char* envbuf);
 * dead jmpenv(const char* envbuf, int retcode);
 */
	.globl	_stksize
	.globl	_Special
_stksize:
	movl	_stkbase,d0
	subl	sp,d0
	addl	#120,d0
	rts

	.globl	_saveenv
_saveenv:
	movl	sp@(4),a0
	movl	a6,a0@(12)		/* save frame pointer of caller */
	movl	sp@+,a1			/* pop return address */
	movl	a1,a0@(4)		/* save pc of caller */
	movl	sp,a0@(8)
	moveml	#0xBCFC,a0@(40)		/* XXX (shouldn't need this) XXX */
	movl	sp,a2
	movl	_stkbase,a3
	movl	a0,a4
	addl	#110,a4
rep1:	movl	a2@+,a4@+
	cmpl	a2,a3
	jcc	rep1
	movl	a4,d0			/* New pointer */
	subl	a2,d0			/* Minus old pointer */
	movl	d0,a0@    		/* is the relocation offset */
	moveml	a0@(40),#0xBCFC		/* XXX (shouldn't need this) XXX */
	movl	_Special,d0
	jmp	a1@

	.globl	_jmpenv
_jmpenv:
	movl	sp@(8),d0		/* return value */
	movl	sp@(4),a0		/* fetch buffer */

	movl	a0@(8),sp
	movl	sp,a2
	movl	_stkbase,a3
	movl	a0,a4
	addl	#110,a4
rep2:	movl	a4@+,a2@+
	cmpl	a2,a3
	jcc	rep2
	moveml	a0@(40),#0xBCFC		/* XXX (shouldn't need this) XXX */
	movl	a0@(12),a6		/* restore frame pointer */
	movl	a0@(4),a1		/* pc */
	jmp	a1@
