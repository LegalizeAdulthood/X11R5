/*
 * Copyright (c) 1987, 1988, 1989 Stanford University
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
 * Interface to geometrical objects used to implement Graphic primitives.
 */

#ifndef geomobjs_h
#define geomobjs_h

#include <InterViews/Graphic/persistent.h>

class PointObj : public Persistent {
public:
    PointObj(Coord = 0, Coord = 0);
    PointObj(PointObj*);

    float Distance(PointObj&);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual boolean read(PFile*);
    virtual boolean write(PFile*);
public:
    Coord x, y;
};

class LineObj : public Persistent {
public:
    LineObj();
    LineObj(Coord, Coord, Coord, Coord);
    LineObj(LineObj*);

    boolean Contains(PointObj&);
    int Same(PointObj& p1, PointObj& p2);
    boolean Intersects(LineObj&);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual boolean read(PFile*);
    virtual boolean write(PFile*);
public:
    PointObj p1, p2;
};

class BoxObj : public Persistent {
public:
    BoxObj(Coord = 0, Coord = 0, Coord = 0, Coord = 0);
    BoxObj(BoxObj*);

    boolean Contains(PointObj&);
    boolean Intersects(BoxObj&);
    boolean Intersects(LineObj&);
    BoxObj operator-(BoxObj&);
    BoxObj operator+(BoxObj&);
    boolean Within(BoxObj&);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    virtual boolean read(PFile*);
    virtual boolean write(PFile*);
public:
    Coord left, right;
    Coord bottom, top;
};

class MultiLineObj : public Persistent {
public:
    MultiLineObj(Coord* = nil, Coord* = nil, int = 0);

    void GetBox(BoxObj& b);
    boolean Contains(PointObj&);
    boolean Intersects(LineObj&);
    boolean Intersects(BoxObj&);
    boolean Within(BoxObj&);
    void SplineToMultiLine(Coord* cpx, Coord* cpy, int cpcount);
    void ClosedSplineToPolygon(Coord* cpx, Coord* cpy, int cpcount);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    void GrowBuf();
    boolean CanApproxWithLine(
	double x0, double y0, double x2, double y2, double x3, double y3
    );
    void AddLine(double x0, double y0, double x1, double y1);
    void AddBezierArc(
        double x0, double y0, double x1, double y1,
        double x2, double y2, double x3, double y3
    );
    void CalcSection(
	Coord cminus1x, Coord cminus1y, Coord cx, Coord cy,
	Coord cplus1x, Coord cplus1y, Coord cplus2x, Coord cplus2y
    );
    void CreateMultiLine(Coord* cpx, Coord* cpy, int cpcount);

    virtual boolean read(PFile*);
    virtual boolean write(PFile*);
public:
    Coord* x, *y;
    int count;
};

class FillPolygonObj : public MultiLineObj {
public:
    FillPolygonObj(Coord* = nil, Coord* = nil, int = 0);
    ~FillPolygonObj();

    boolean Contains(PointObj&);
    boolean Intersects(LineObj&);
    boolean Intersects(BoxObj&);

    virtual ClassId GetClassId();
    virtual boolean IsA(ClassId);
protected:
    void Normalize();
protected:
    Coord* normx, *normy;
    int normCount;
};

class Extent {
public:
    Extent(float = 0, float = 0, float = 0, float = 0, float = 0);
    Extent(Extent&);

    boolean Undefined();
    boolean Within(Extent& e);
    void Merge(Extent&);
public:
    /* defines lower left and center of an object */
    float left, bottom, cx, cy, tol;
};

/*
 * inlines
 */

inline boolean Extent::Undefined () { return left == cx && bottom == cy; }

#endif
