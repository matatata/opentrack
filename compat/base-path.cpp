#undef NDEBUG
#include <cassert>

#include "base-path.hpp"
#include <QCoreApplication>

const QString& application_base_path()
{
#ifndef __APPLE__
    assert(qApp && "logic error");
#endif
    static QString path = QCoreApplication::applicationDirPath();
    return path;
}
