/*
	 Copyright (c) 1991 by Simon Marshall, University of Hull, UK

		   If you still end up late, don't blame me!
				       
  Permission to use, copy, modify, distribute, and sell this software and its
       documentation for any purpose and without fee is hereby granted,
    provided that the above copyright notice appear in all copies and that
	both that copyright notice and this permission notice appear in
			   supporting documentation.
				       
  This software is provided AS IS with no warranties of any kind.  The author
    shall have no liability with respect to the infringement of copyrights,
     trade secrets or any patents by this file or any part thereof.  In no
      event will the author be liable for any lost revenue or profits or
	      other special, indirect and consequential damages.
*/

#include "xalarm.h"

#define		VALUE(ch)	((int) (ch) - (int) '0')
#define		INSTRING(ch, s)	(((ch) != '\0') and (index ((s), (ch))))
#define		DOWNCASE(ch)	(((ch) >= 'A') and ((ch) <= 'Z') ? \
				 ((ch) - 'A' + 'a') : (ch))


void		SetAlarmIfToday();
long		DateToMilliSeconds(), TimeToMilliSeconds();
Boolean		AppointmentIsToday();
static String	NextWord();
static Boolean	ParseTimeString();



extern AlarmData xalarm;



/* 
 * Is the date & time consistent with today?  This includes whether 
 * the time has passed or not.
 */

Boolean AppointmentIsToday (line, timestr, chpos)
  String   line, *timestr;
  int 	  *chpos;
{
    static int 	  daysin[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    static char   strings[][4] = {WEEKDAYS, MONTHS, "!!!", "!!!"};
    int 	  wday, month, mday, hrs, mins;
    struct tm 	 *now, *localtime();
    time_t 	  abstime, time();
    String 	  word;
    Boolean 	  today, notime, whocares;
    int 	  i;

    if (*line == '\n')
	return (False);

    (void) time (&abstime);
    now = localtime (&abstime);
    wday = now->tm_wday;
    month = now->tm_mon;
    mday = now->tm_mday;

    while (strcmp (word = NextWord (line, chpos), "-"))
	if (*word == '\0')
	    break;
	else if ((*word == '#') or (*word == '!'))
	    return (False);
	else {
	    i = 0;
	    while ((strncmp (word, strings[i], 3)) and (i < 20))
		i++;
	    if (i < 7)
		wday = i;
	    else if (i < 20)
		month = i-7;
	    else
		mday = atoi (word);
	    if ((mday > daysin[month]) or (mday <= 0)) {
		fprintf (stderr, "xalarm: Couldn't parse this entry (what is \"%s\"?):\n%s",
			 word, line);
		return (False);
	    }
	}

    if (not (notime = (*(*timestr = NextWord (line, chpos)) == '\0')))
	notime = (not ParseTimeString (*timestr, &hrs, &mins, &whocares));

    today = ((wday == now->tm_wday) and (month == now->tm_mon) and
	     (mday == now->tm_mday));

    if (today and notime) {
	if (*timestr[0] != '\0')
	    do
		(*chpos)--;
	    while (line[*chpos] != ' ');
	(void) strcpy (*timestr, "+0");
	return (True);
    }
    else
	return (today and
		 ((hrs > now->tm_hour) or
		  ((hrs == now->tm_hour) and (mins >= now->tm_min)) or
		  (*timestr[0] == '+')));
}



/* 
 * Return the next word in str, starting at position chpos in the 
 * string.  The returned word is in lower case.
 */

static String NextWord (str, chpos)
  String   str;
  int 	  *chpos;
{
    String   word;
    int      start, i;

    while (isspace (str[*chpos]))
	(*chpos)++;

    start = *chpos;
    while ((not isspace (str[*chpos])) and (str[*chpos] != '\0'))
	(*chpos)++;

    if ((str[*chpos] == '\0') and (start == *chpos))
	return ("");
    else {
	word = XtMalloc (*chpos - start + 1);
	for (i=0; i<(*chpos-start); i++)
	    word[i] = DOWNCASE (str[start+i]);
	word[*chpos-start] = '\0';
	
	return (word);
    }
}



/* 
 * Convert the string into milliseconds, or INVALID if 
 * not recognised.
 * Recognises quite a lot, really, but doesn't like non absolute times 
 * having "am" or "pm" etc.
 */	

long TimeToMilliSeconds (timestr)
  String   timestr;
{
    Boolean 	in24hrformat;
    int 	hours, minutes, seconds;
    struct tm  *now, *localtime();
    time_t 	abstime, time();

    if (not ParseTimeString (timestr, &hours, &minutes, &in24hrformat))
	return (INVALID);

    if (*timestr == '+')
	seconds = (hours * 60*60) + (minutes * 60);
    else {
	(void) time (&abstime);
	now = localtime (&abstime);
	seconds = ((hours - now->tm_hour) * 60*60) +
	    ((minutes - now->tm_min) * 60) - now->tm_sec;
	while (seconds < 0)
	    seconds += (in24hrformat ? 24*60*60 : 12*60*60);
    }
    
    return (seconds*1000);
}



/* 
 * Parse that string.  Robbed and hacked from xcal (3.2) by Peter 
 * Collinson et al.
 */

static Boolean ParseTimeString (str, hrs, mins, in24hrf)
  String    str;
  int 	   *hrs, *mins;
  Boolean  *in24hrf;
{
    enum  {Hhmm, hHmm, hh_mm, hhMm, hhmM, AmPm, aMpM, Finished}   state;
    Boolean   relative;
    int       numdigits = 0;
    char     *s = str;
		      
    if (relative = (*str == '+'))
	str++;

    while (*s)
	if (isdigit (*s++))
	    numdigits++;

    if (relative)
	switch (numdigits) {
	 case 1: state = hhmM; break;
	 case 2: state = hhMm; break;
	 case 3: state = hHmm; break;
	 case 4: state = Hhmm; break;
	 default: return (False);
	}
    else
	switch (numdigits) {
	 case 1:
	 case 2:
	 case 4: state = Hhmm; break;
	 case 3: state = hHmm; break;
	 default: return (False);
	}

    *hrs = *mins = 0;
    *in24hrf = False;

    while (state != Finished)
	switch (state) {
	 case Hhmm:
	    if (isdigit (*str)) {
		*hrs = VALUE (*str++);
		state = hHmm;
		continue;
	    }
	    else
		return (False);
	 case hHmm:
	    if (isdigit (*str)) {
		*hrs = *hrs*10 + VALUE (*str++);
		state = hh_mm;
		continue;
	    }
	 case hh_mm:
	    if (INSTRING (*str, ".:-")) {
		state = hhMm;
		str++;
		continue;
	    }
	 case hhMm:
	    if (isdigit (*str)) {
		*mins = VALUE (*str++);
		state = hhmM;
		continue;
	    }
	 case hhmM:
	    if (isdigit (*str)) {
		*mins = *mins*10 + VALUE (*str++);
		state = AmPm;
		continue;
	    }
	 case AmPm:
	    if (*str == '\0') {
		*in24hrf = (*hrs > 12);
		state = Finished;
		continue;
	    }
	    else
		if (relative)
		    return (False);

	    if (INSTRING (*str, "Aa")) {
		if (*hrs == 12)
		    *hrs = 0;
		state = aMpM;
		continue;
	    }
	    else
		if (INSTRING (*str, "Pp")) {
		    if (*hrs < 12)
			*hrs = *hrs + 12;
		    state = aMpM;
		    continue;
		}
	    return (False);
	 case aMpM:
	    str++;
	    if (INSTRING (*str, "Mm")) {
		*in24hrf = True;
		str++;
		state = Finished;
	    }
	    else
		return (False);
	}

    return ((*hrs < 24) and (*mins < 60) and (*str == '\0'));
}
