/* Events to scroll on clock display at specific dates of the year. */
/* Must be customized by user and software rebuilt. */
struct event CalendarEvent[MAX_EVENTS] =
{
  /*           "----------------- MAX 50 characters --------------" */
  { 0, JAN, 0, "> > > Debug"},  // if a valid date is put here, it will allow displaying useful debugging variables in "process_scroll_queue()" function.
  /// { 1, JAN, JINGLE_FETE, "> > > Calendar event January 1st"},  // JINGLE_FETE is available when "PASSIVE_PIEZO_SUPPORT" is defined.
  { 1, JAN, 0, "> > > Calendar event January 1st"},
  { 1, FEB, 0, "> > > Calendar event February 1st"},
  { 1, MAR, 0, "> > > Calendar event March 1st"},
  { 1, APR, 0, "> > > Calendar event April 1st"},
  { 1, MAY, 0, "> > > Calendar event May 1st"},
  { 1, JUN, 0, "> > > Calendar event June 1st"},
  { 1, JUL, 0, "> > > Calendar event July 1st"},
  { 1, AUG, 0, "> > > Calendar event August 1st"},
  { 1, SEP, 0, "> > > Calendar event September 1st"},
  { 1, OCT, 0, "> > > Calendar event October 1st"},
  { 1, NOV, 0, "> > > Calendar event November 1st"},
  { 1, DEC, 0, "> > > Calendar event December 1st"},

  { 1, JAN, 0, "> > > Happy New Year !!"},
  { 1, MAR, 0, "> > > John's Birthday !!!"},
  /// { 1, MAR, JINGLE_BIRTHDAY, "> > > John's Birthday !!!"},  // JINGLE_BIRTHDAY is available when "PASSIVE_PIEZO_SUPPORT" is defined.
  {14, FEB, 0, "> > > Valentine's Day."},
  {25, DEC, 0, "> > > Merry Christmas !!"},
};


