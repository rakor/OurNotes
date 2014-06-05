#ifndef GLOBAL_H
#define GLOBAL_H

#define PROGNAME            "OurNotes"
#define PROGVERSION         "0.0.1"
#define DATENBANKVERSION    "1"


#ifdef Q_OS_WIN
#define USERENVVAR  "USERNAME"
#endif
#ifdef Q_OS_UNIX
#define USERENVVAR  "USER"
#endif

#endif // GLOBAL_H
