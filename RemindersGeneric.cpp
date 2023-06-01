struct reminder1 Reminder1[MAX_REMINDERS1]=
{
  // Reminder1[0]:
  {
     0,  0,  0,  1,  1, 2010, 1,   1, 0,   // StartPeriod   [Hours, Minutes, Seconds, DayOfMonth, Month, Year, DayOfWeek, DayOfYear, SummerTimeFlag]
    0ll,                                   // StartPeriodEpoch
    23, 59, 59, 31, 12, 2040, 1, 365, 0,   // EndPeriod
    0ll,                                   // EndPeriodEpoch
    16, 07, 45, 26, 03, 2023, 1,   0, 0,   // FirstRing
    0ll,                                   // FirstRingEpoch
    4 * 60 * 60,                           // RingDuration - intermittently ring for 4 hours
    15 * 60,                               // RingRepeatTime - ring every 15 minutes.
    2 * 7 * 24 * 60 * 60,                  // NextReminderDelay - next reminder in 2 weeks.
  },

  // Reminder1[1]:
  {
    //  0,  0,  0,  1,  1, 9999, 1,   1, 0,   // StartPeriod
    // 0ll,                                   // StartPeriodEpoch
    // 23, 59, 59, 31, 12, 9999, 1, 365, 0,   // EndPeriod
    // 0ll,                                   // EndPeriodEpoch
    // 16, 07, 45, 26, 03, 9999, 1,   0, 0,   // FirstRing
    // 0ll;                                   // FirstRingEpoch
    // 4 * 60 * 60;                           // RingDuration - intermittently ring for 4 hours
    // 15 * 60;                               // RingRepeatTime - ring every 15 minutes.
    // 2 * 7 * 24 * 60 * 60;                  // NextReminderDelay - next reminder in 2 weeks.
  },

  // Reminder1[2]:
  {
    //  0,  0,  0,  1,  1, 9999, 1,   1, 0,   // StartPeriod
    // 0ll,                                   // StartPeriodEpoch
    // 23, 59, 59, 31, 12, 9999, 1, 365, 0,   // EndPeriod
    // 0ll,                                   // EndPeriodEpoch
    // 16, 07, 45, 26, 03, 9999, 1,   0, 0,   // FirstRing
    // 0ll;                                   // FirstRingEpoch
    // 4 * 60 * 60;                           // RingDuration - intermittently ring for 4 hours
    // 15 * 60;                               // RingRepeatTime - ring every 15 minutes.
    // 2 * 7 * 24 * 60 * 60;                  // NextReminderDelay - next reminder in 2 weeks.
  },

  // Reminder1[3]:
  {
    //  0,  0,  0,  1,  1, 9999, 1,   1, 0,   // StartPeriod
    // 0ll,                                   // StartPeriodEpoch
    // 23, 59, 59, 31, 12, 9999, 1, 365, 0,   // EndPeriod
    // 0ll,                                   // EndPeriodEpoch
    // 16, 07, 45, 26, 03, 9999, 1,   0, 0,   // FirstRing
    // 0ll;                                   // FirstRingEpoch
    // 4 * 60 * 60;                           // RingDuration - intermittently ring for 4 hours
    // 15 * 60;                               // RingRepeatTime - ring every 15 minutes.
    // 2 * 7 * 24 * 60 * 60;                  // NextReminderDelay - next reminder in 2 weeks.
  },

  // Reminder1[4]:
  {
    //  0,  0,  0,  1,  1, 9999, 1,   1, 0,   // StartPeriod
    // 0ll,                                   // StartPeriodEpoch
    // 23, 59, 59, 31, 12, 9999, 1, 365, 0,   // EndPeriod
    // 0ll,                                   // EndPeriodEpoch
    // 16, 07, 45, 26, 03, 9999, 1,   0, 0,   // FirstRing
    // 0ll;                                   // FirstRingEpoch
    // 4 * 60 * 60;                           // RingDuration - intermittently ring for 4 hours
    // 15 * 60;                               // RingRepeatTime - ring every 15 minutes.
    // 2 * 7 * 24 * 60 * 60;                  // NextReminderDelay - next reminder in 2 weeks.
  },

  // Add other reminders of type 1 here... up to MAX_REMINDERS1.
};



