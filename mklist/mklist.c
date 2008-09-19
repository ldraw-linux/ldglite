/*
 *  mklist.c, a replacement for James Jessiman's makelist
 *  Copyright (C) 1999  Lars C. Hassing
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
                                                                             */

/*****************************************************************************
  Please do not edit this file. In stead contact Lars C. Hassing (lch@cci.dk)
  to get your changes integrated in a future release.
******************************************************************************/

/*
990214 lch Release v1.0
990316 lch Release v1.1: added options + minor changes to sort algorithms
990518 lch Release v1.2: Skip "~Moved to xxx" by default
20000703 lch Release v1.3: Added -i and -o
20030625 dmh Release v1.4: Ported to gcc.
                                                                             */

/* Compile with Borland Turbo C 2.0: tcc -mc -d -f -k- -N -v- -y- -wrvl -wstv
-wucp -wnod -wpro -wuse -wsig -ncmdsrel -M src\mklist.c                      */


#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "alloc.h"
#include "dir.h"
#include "conio.h"

char                *ProgVer = "mklist  v1.4 20030625  (C) 1999-2003 Lars C. Hassing  lch@ccieurope.com";


/*****************************************************************/
int                  CmpNumber(const void *p1, const void *p2)
{
   char                *s1 = *((char **) p1);
   char                *s2 = *((char **) p2);
   long                 l1;
   long                 l2;

   if (isdigit(*s1) && isdigit(*s2))
   {
      l1 = atol(s1);
      l2 = atol(s2);
      if (l1 != l2)
         return (l1 < l2 ? -1 : 1);
      /* Numbers are equal. Be sure to make 3005.dat come before 3005-1.dat */
      do
      {
         l1 = (unsigned char) *s1++;
         if ('A' <= l1 && l1 <= 'Z')
            l1 -= ('A' - 'a');
         else if (l1 == '.')
            l1 = '\0';                    /* Sort dot very first             */
         l2 = (unsigned char) *s2++;
         if ('A' <= l2 && l2 <= 'Z')
            l2 -= ('A' - 'a');
         else if (l2 == '.')
            l2 = '\0';                    /* Sort dot very first             */
      } while (l1 && (l1 == l2));

      return ((int) (l1 - l2));
   }
   return (stricmp(s1, s2));
}

/*****************************************************************/
int                  CmpDescription(const void *p1, const void *p2)
{
   int                  Res;

   Res = stricmp(*((char **) p1) + 14, *((char **) p2) + 14);
   return (Res ? Res : CmpNumber(p1, p2));
}

/*****************************************************************/
void                 PressAnyKey(void)
{
   printf("  Press any key to continue");
   getch();
   printf("\n");
}

/*****************************************************************/
void                 PrintUsage(void)
{
  printf("Options:\n");
  printf("  -h        You already figured this one out :-)\n");
  printf("  -n        Sort by Number\n");
  printf("  -d        Sort by Description\n");
  printf("  -c        Check for duplicate descriptions. \"parts.lst\" unchanged.\n");
  printf("  -m        Don't skip parts with \"~Moved to xxx\" description\n");
  printf("  -~        Skip parts with ~ description, e.g. \"~Winch  2 x  4 x  2 Top\"\n");
  printf("  -i <dir>  input directory, default is \"PARTS\" in current directory\n");
  printf("  -o <file> output filename, default is \"parts.lst\" in current directory\n");
}

/*****************************************************************/
int                  main(int argc, char **argv)
{
   int                  CheckDuplicateDescriptions;
   int                  SortBy;
   int                  SkipTilde;
   int                  SkipMovedto;
   char                *arg;
   int                  c;
   int                  i;
   int                  j;
   FILE                *fp;
   struct ffblk         ffb;
   int                  done;
   int                  Len;
   char               **Lines;
   int                  maxLines;
   int                  nLines;
   int                  pathlen;
   char                 Line[200];
   char                 Dirname[200];
   char                 Filename[200];
   char                 OutFilename[200];
   char                *s;
   char                *Description;
   char                *FormattedLine;
   unsigned long        farcoreleftStart;
   unsigned long        farcoreleftEnd;
   long                 FileSize;
   struct stat		statbuf;

   printf("%s\n", ProgVer);
   printf("Replacement for James Jessiman's makelist\n");
   printf("Call with -h to see a list of options.\n\n");

   strcpy(Dirname, "PARTS"); /* Default input directory path */
   strcpy(OutFilename, "parts.lst"); /* Default output filename */

   CheckDuplicateDescriptions = 0;
   SortBy = 0;
   SkipTilde = 0;
   SkipMovedto = 1;
   while (--argc > 0)
   {
      arg = *++argv;
      if (arg[0] == '-')
      {
         switch (arg[1])
         {
            case '?':
            case 'h':
               PrintUsage();
               exit(1);
               break;
            case 'c':
               CheckDuplicateDescriptions = 1;
               break;
            case 'n':
            case 'd':
               SortBy = arg[1];
               break;
            case 'm':
               SkipMovedto = 0;
               break;
            case '~':
               SkipTilde = 1;
               break;
            case 'i':
	       if (--argc > 0)
		 strcpy(Dirname, *++argv);
	       else
	       {
		 PrintUsage();
		 printf("*** input directory expected as next argument after -i.\n");
		 exit(1);
	       }
               break;
            case 'o':
	       if (--argc > 0)
		 strcpy(OutFilename, *++argv);
	       else
	       {
		 PrintUsage();
		 printf("*** output filename expected as next argument after -o.\n");
		 exit(1);
	       }
               break;
            default:
               PrintUsage();
               printf("*** Unknown option '%s'.\n", arg);
               exit(1);
               break;
         }
      }
      else
      {
         PrintUsage();
         exit(1);
      }
   }

   // Do a stat to see if Dirname exists and is a directory.
   if (stat(Dirname, &statbuf) < 0)
   {
     printf("*** Could not stat input directory \"%s\".\n", Dirname);
     exit(1);
   }

   if ((statbuf.st_mode & S_IFDIR) == 0) 
   {
     printf("*** Input directory \"%s\" is not a directory.\n", Dirname);
     exit(1);
   }

   if (CheckDuplicateDescriptions)
      SortBy = 'd';
   if (!SortBy)
   {
      printf("Sort by [N]umber or [D]escription: ");
      c = getch();
      printf("%c\n", c);

      if (c == 'N' || c == 'n')
         SortBy = 'n';
      else if (c == 'D' || c == 'd')
         SortBy = 'd';
      else
      {
         printf("Nothing done.\n");
         exit(0);
      }
   }

   farcoreleftStart = farcoreleft();

   nLines = 0;
   maxLines = 1000;
   Lines = farmalloc(maxLines * sizeof(char *));
   if (!Lines)
   {
      printf("Out of memory after %d parts\n", nLines);
      printf("Memory available at beginning: %ld kBytes\n",
             (farcoreleftStart + 1023) / 1024);
      exit(1);
   }
   strcpy(Filename, Dirname);
   strcat(Filename, "\\");
   pathlen = strlen(Filename);
   strcat(Filename, "*.*");
   for (done = findfirst(Filename, &ffb, 0); !done; done = findnext(&ffb))
   {
      strcpy(Filename + pathlen, ffb.ff_name);
      fp = fopen(Filename, "rt");
      if (!fp)
      {
         printf("Cannot open \"%s\"", ffb.ff_name);
         PressAnyKey();
         continue;
      }
      fgets(Line, sizeof(Line), fp);
      fclose(fp);
      s = Line + strlen(Line) - 1;
      while (s >= Line && (*s == '\n' || *s == '\r' || *s == '\t' || *s == ' '))
         *s-- = '\0';                     /* clear newline and trailing tabs
                                             and spaces                      */
      s = Line;
      while (*s == '\t' || *s == ' ')
         *s++;
      if (*s++ != '0')
      {
         printf("Line type 0 expected in \"%s\", skipping...", ffb.ff_name);
         PressAnyKey();
         continue;
      }
      while (*s == '\t' || *s == ' ')
         *s++;
      Description = s;
      if (SkipTilde && Description[0] == '~')
         continue;
      if (SkipMovedto && strncmp(Description, "~Moved to", 9) == 0)
         continue;
      Len = strlen(Description);
      if (Len == 0)
      {
         printf("Empty description in \"%s\"", ffb.ff_name);
         PressAnyKey();
      }
      if (Len > 64)
      {
         /* Original makelist truncates to 64 characters. */
         printf("Description in \"%s\" will be truncated to 64 characters:\n",
                ffb.ff_name);
         printf("Before: \"%s\"\n", Description);
         printf("After:  \"%-64.64s\"\n", Description);
         PressAnyKey();
      }
      FormattedLine = farmalloc(79);
      if (!FormattedLine)
      {
         printf("Out of memory after %d parts\n", nLines);
         printf("Memory available at beginning: %ld kBytes\n",
                (farcoreleftStart + 1023) / 1024);
         exit(1);
      }
      sprintf(FormattedLine, "%-12s  %-64.64s", ffb.ff_name, Description);
      if (nLines >= maxLines)
      {
         /* Let's have another 1000 pointers */
         maxLines += 1000;
         Lines = farrealloc(Lines, maxLines * sizeof(char *));
         if (!Lines)
         {
            printf("Out of memory after %d parts\n", nLines);
            printf("Memory available at beginning: %ld kBytes\n",
                   (farcoreleftStart + 1023) / 1024);
            exit(1);
         }
      }
      Lines[nLines++] = FormattedLine;
      if (nLines % 100 == 0)
         printf("%d parts so far...\r", nLines);
   }
   printf("%d parts found in %s.\n", nLines, Dirname);
   if (nLines == 0)
   {
      printf("No parts found, nothing done.\n");
      exit(0);
   }

   printf("Sorting...\n");
   qsort(Lines, nLines, sizeof(Lines[0]),
         (SortBy == 'n') ? CmpNumber : CmpDescription);

   if (CheckDuplicateDescriptions)
   {
      printf("Checking for duplicate descriptions. \"%s\" unchanged.\n", OutFilename);
      for (i = 0; i < nLines; i += j)
      {
         for (j = 1; i + j < nLines; j++)
         {
            if (stricmp(Lines[i] + 14, Lines[i + j] + 14) != 0)
               break;                     /* OK to break, lines are sorted   */
            if (j == 1)                   /* First duplicate                 */
               printf("%s\n", Lines[i]);
            printf("%s\n", Lines[i + j]);
         }
         if (j > 1)                       /* Duplicates found                */
            PressAnyKey();
      }
   }
   else
   {
      fp = fopen(OutFilename, "wt");
      if (!fp)
      {
         printf("Cannot open \"%s\" for writing.\n", OutFilename);
         exit(1);
      }
      for (i = 0; i < nLines; i++)
         fprintf(fp, "%s\n", Lines[i]);
      FileSize = ftell(fp);
      fclose(fp);
      printf("\"%s\" successfully written, %ld kBytes\n", OutFilename,
             (FileSize + 1023) / 1024);
   }

#ifndef HAVE_GCC
   farcoreleftEnd = farcoreleft();

   printf("Maximum memory usage: %ld kBytes of %ld kBytes available\n",
          (farcoreleftStart - farcoreleftEnd + 1023) / 1024,
          (farcoreleftStart + 1023) / 1024);
#endif

   return (0);
}
