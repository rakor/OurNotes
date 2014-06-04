#ifndef GLOBAL_H
#define GLOBAL_H

#define PROGNAME    "OurNotes"

#ifdef Q_OS_WIN
#define USERENVVAR  "USERNAME"
#endif
#ifdef Q_OS_UNIX
#define USERENVVAR  "USER"
#endif

#endif // GLOBAL_H
