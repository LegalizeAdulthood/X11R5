/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Noriyuki Hosoe, Sony Corporation
 */

#include <stdio.h>
#include <sys/types.h>
#include "sj_struct.h"
#include "../../include/Dict.h"

extern	HindoRec *askknj[];
extern	int	askknj_num;
extern	DouonRec *douon_ptr;
extern	FILE	*outfp;
extern	int	douon_num;

static	u_char	index[MainIndexLength];
static	int	idxpos = 0;
static	int	idxnum = 0;

OffsetRec *real_ofsrec();
u_char	*knjcvt();

static	u_char *set_ofsask(src, dst)
u_char	*src;
u_char	*dst;
{
	int	len;
	u_char	*ptr;
	OffsetRec *ofsrec;

	ofsrec = real_ofsrec(src);

	for (len = ofsrec->klen, ptr = ofsrec->kptr ; len > 0 ; ) {
		switch (*ptr & KanjiModeMask) {
		case NormalKanji0:
		case NormalKanji1:
		case NormalKanji2:
		case AiAttribute:	*dst++ = *ptr++;
					*dst++ = *ptr++;
					len -= 2;
					break;

		case ZenHiraAssyuku:
		case ZenKataAssyuku:
		case KanjiAssyuku:	*dst++ = *ptr++;
					len -= 1;
					break;

		case OffsetAssyuku:
			fprintf(stderr, "二重にオフセットが参照されている\n");
			exit(1);
					dst = set_ofsask(ptr, dst);
					ptr += 2;
					len -= 2;
					break;

		default:
			fprintf(stderr, "属性コード・エラー\n");
			exit(1);
		}
	}

	return dst;
}

make_knjstr(src, len, dst)
u_char	*src;
int	len;
u_char	*dst;
{
	u_char	*keep = dst;

	while (len > 0) {
		switch (*src & KanjiModeMask) {
		case NormalKanji0:
		case NormalKanji1:
		case NormalKanji2:
		case AiAttribute:	*dst++ = *src++;
					*dst++ = *src++;
					len -= 2;
					break;

		case ZenHiraAssyuku:
		case ZenKataAssyuku:
		case KanjiAssyuku:	*dst++ = *src++;
					len -= 1;
					break;

		case OffsetAssyuku:	dst = set_ofsask(src, dst);
					src += 2;
					len -= 2;
					break;

		default:
			fprintf(stderr, "属性コード・エラー\n");
			exit(1);
		}
	}

	return(dst - keep);
}

static	make_knjask()
{
	int	i, j;
	u_char	*p;
	u_char	buf[128];
	int	len;
	int	tlen = 0;

	for (i = 0 ; i < askknj_num ; i++) {

		len = make_knjstr(askknj[i] -> kptr, askknj[i] -> klen, buf);

		Free(askknj[i] -> kptr);

		p = (u_char *)Malloc(len);

		bcopy(buf, p, len);
		askknj[i] -> kptr = p;
		askknj[i] -> klen = len;

		tlen += len;
	}

	return tlen;
}

makeseg()
{
	u_char	buf[MainSegmentLength];
	DouonRec *drec;
	HinsiRec *hrec;
	KanjiRec *krec;
	int	i, j, k;
	int	slen;
	u_char	*dst;
	u_char	*sdst;
	u_char	*p, *q;

#ifndef	HEHE
#define	datset(i)	{ if (dst >= buf+sizeof(buf)) goto err; *dst++ = i; }
#else
#define	datset(i)	{ *dst++; i; }
#endif

	dst = buf;

	i = make_knjask();
	if (i >= 0x100) {
		fprintf(stderr, "漢字圧縮文字列が長すぎる\n");
		exit(1);
	}

	datset(i);
	for (i = 0 ; i < askknj_num ; i++) {
		p = askknj[i] -> kptr;
		j = askknj[i] -> klen;
		while (j > 0) {
			k = codesize(*p);
			datset((j == k) ? (*p | KanjiStrEnd) : *p); p++;
			if (k > 1) {
				datset(*p++);
			}
			j -= k;
		}
	}

	clear_ofsrec();

	for (drec = douon_ptr ; drec ; drec = drec->dnext) {

		sdst = dst; dst += DouonBlkSizeNumber;

		p = drec -> yptr;
		slen = strlen(p) - drec -> dlen;
		p += slen;
		for (i = drec -> dlen ; i > 0 ; i--) {
			datset(*p++);
		}

		for (hrec = drec->hrec ; hrec ; hrec = hrec->hnext) {

			datset(hrec -> hinsi);

			for (krec = hrec->krec ; krec ; krec = krec->knext) {

				p = q = knjcvt(krec -> kptr, krec -> klen, &i);

				set_ofsrec(krec->kptr, krec->klen, dst - buf);

				while (i-- > 0) {
					datset(*p++);
				}

				Free(q);
			}

			datset(HinsiBlkTerm);
		}

		i = dst - sdst;
		*sdst = ((i >> 8) & 0x0f);
		*(sdst + 1) = (i & 0xff);

		if (drec -> dlen > 0x0f) *sdst |= 0x40;
		*(sdst + 2) = (drec -> dlen & 0x0f);

		if (slen > 0x0f) *sdst |= 0x80;
		*(sdst + 2) |= ((slen << 4) & 0xf0);
	}

	if (dst < buf + sizeof(buf)) {
		datset(DouonBlkTerm);
	}

	for (p = douon_ptr -> yptr ; ; ) {
		if (idxpos < MainIndexLength) {
			index[idxpos++] = *p;
		}
		else {
			fprintf(stderr, "インデックスがあふれました\n");
			exit(1);
		}
		if (!*p++) break;
	}
	if (idxpos < MainIndexLength) {
		index[idxpos] = 0;
	}
	else {
		fprintf(stderr, "インデックスがあふれました\n");
		exit(1);
	}

	i = dst - buf;

	while (dst < buf + sizeof(buf)) *dst++ = DouonBlkTerm;

	Fseek(outfp, (long)(HeaderLength + CommentLength +MainIndexLength
			+ idxnum * MainSegmentLength), 0);
	Fwrite(buf, sizeof(buf), 1, outfp);
	Fflush(outfp);

	printf("セグメント番号:%d:", idxnum++);
	output_yomi(stdout, douon_ptr -> yptr);
	printf("\tセグメント長:%d", i);
	printf("\t同音語数:%d\n", douon_num);
	printf("漢字圧縮文字列:");
	for (i = j = 0 ; i < askknj_num ; i++) {
		putchar('\t');
		if (i && j == 0) putchar('\t');
		output_knj(stdout, askknj[i] -> kptr, askknj[i] -> klen);
		printf(":%d", askknj[i] -> hindo + askknj[i] -> exist);
		if (++j >= 4) {
			putchar('\n');
			j = 0;
		}
	}
	if (j) putchar('\n');
	putchar('\n');
	fflush(stdout);

	return;

err:
	fprintf(stderr, "セグメント構成中にバッファがあふれた\n");
	fprintf(stderr, "セグメント先頭の読み:");
	output_yomi(stderr, douon_ptr -> yptr);
	fputc('\n', stderr);

	exit(1);
#undef	datset
}

static	void	put4byte(p, n)
u_char	*p;
long	n;
{
	p += 3;
	*p-- = n; n >>= 8;
	*p-- = n; n >>= 8;
	*p-- = n; n >>= 8;
	*p = n;
}
void	makehead()
{
	u_char	header[HeaderLength + CommentLength];
	long	i;
	u_char	*p;

#define	IndexPos	(HeaderLength + CommentLength)

	bzero(header, sizeof(header));
	put4byte(header + VersionPos, DictVersion);
	time(&i);
	p = header + CommentPos;
	sprintf(p, "sjmain.dic : %s", ctime(&i));
	while (*p) {
		if (*p == '\n') { *p = 0; break; }
		p++;
	}

	i = IndexPos;

	put4byte(header + DictIdxPos, i);
	put4byte(header + DictIdxLen, MainIndexLength);
	i += MainIndexLength;

	put4byte(header + DictSegPos, i);
	put4byte(header + DictSegLen, MainSegmentLength);
	put4byte(header + DictSegMax, 0);
	put4byte(header + DictSegNum, idxnum);

	put4byte(header + DictAIdxPos, 0);
	put4byte(header + DictAIdxLen, 0);

	put4byte(header + DictASegPos, 0);
	put4byte(header + DictASegLen, 0);

	Fseek(outfp, (long)HeaderPos, 0);
	Fwrite(header, sizeof(header), 1, outfp);
	Fseek(outfp, (long)IndexPos, 0);
	Fwrite(index, MainIndexLength, 1, outfp);

#undef	IndexPos
}
