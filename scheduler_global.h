﻿#ifndef SCHEDULERLIB_GLOBAL_H
#define SCHEDULERLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SCHEDULER_LIBRARY)
#  define SCHEDULERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SCHEDULERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // SCHEDULERLIB_GLOBAL_H
