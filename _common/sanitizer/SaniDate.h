#ifndef SANIDATE_H
/* A set a function to check the validity of dates */

struct Time {
   int hour, minute, second;
   Time(int _hour, int _minute, int _second) {
      hour = _hour;
      minute = _minute;
      second = _second;
   }

   bool isValid() {
      bool valid = true;
      valid |= (0 <= hour && hour <= 23) || sani::error("Invalid hour %d", hour);
      valid |= (0 <= minute && minute <= 59) || sani::error("Invalid minute %d", minute);
      valid |= (0 <= second && second <= 59) || sani::error("Invalid second %d", second);
      return valid;
   }
};

struct Date {
   int year, month, day;
   Date(int _year, int _month, int _day) {
      year = _year;
      month = _month;
      day = _day;
   }

   bool isBisextile() {
      return (year % 4 == 0) && (year % 100 != 0 || year % 400 == 0);
   }
   bool isValid() {
      int nbDays[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
      if (isBisextile())
         nbDays[2] = 29;
      bool valid = true;
      valid |= (1600 <= year && year <= 10 * 1000) || sani::error("Invalid year %d", year);
      valid |= (1 <= month && month <= 12) || sani::error("Invalid month %d", month);
      valid |= (1 <= day && day <= 31) || sani::error("Invalid day %d", day);

      valid |= day <= nbDays[month] || sani::error("Invalid date %d %d %d", year, month, day);
      return valid;
   }

};


#define SANIDATE_H
#endif
