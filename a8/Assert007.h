/*
 *  Adrienne Slaughter
 *  5007 Spr 2020
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  It is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  See <http://www.gnu.org/licenses/>.
 */

#ifndef _A6_ASSERT007_H_
#define _A6_ASSERT007_H_

// A wrapper for assert that permits side-effects within the
// Assert007() statement.  Borrowed from:
//
//   http://www.acm.uiuc.edu/sigops/roll_your_own/2.a.html
void AssertionFailure(const char *exp, const char *file,
                      const char *basefile, int line);

#define Assert007(exp) if (exp) ; \
  else AssertionFailure(#exp, __FILE__, __BASE_FILE__, __LINE__)

#endif  // _a6_ASSERT007_H_
