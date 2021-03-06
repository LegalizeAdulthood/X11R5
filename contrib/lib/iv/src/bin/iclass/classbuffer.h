/*
 * Copyright (c) 1989 Stanford University
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Stanford not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Stanford makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * STANFORD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL STANFORD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * ClassBuffer - caches information about class hierarchies from header files.
 */

#ifndef classbuffer_h
#define classbuffer_h

#include <InterViews/defs.h>

class ClassInfo;

class ClassBuffer {
public:
    ClassBuffer(
	boolean recursive = false, boolean verbose = false,
	boolean CPlusPlusFiles = false
    );
    virtual ~ClassBuffer();

    void Search(const char* path);

    const char* Class(int index);
    const char* Parent(const char* classname, int index = 0);
    const char* Child(const char*, int index = 0);
    const char* Path(const char*);
    int LineNumber(const char*);
    
    ClassInfo* Info(const char*);
private:
    void SearchDir(const char* path);
    void SearchDirs(const char* path);
    void SearchFile(const char* path, struct stat&);
    void SearchTextBuffer(class TextBuffer*, const char* path);

    char* FindClassDecl(TextBuffer*, int&);
    char* ParentName(TextBuffer*, int&);
    char* Identifier(TextBuffer*, int&);
private:
    class Classes* _classes;
    boolean _recursive, _verbose, _CPlusPlusFiles;
};

#endif
