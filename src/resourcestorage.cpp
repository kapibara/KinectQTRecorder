#include "resourcestorage.h"

#include <QFileInfo>
#include <QDir>

#include <iostream>

ResourceStorage::ResourceStorage(const QString &execLocation)
{
    QFileInfo finfo(execLocation);

    resourceLocation_ = (finfo.absolutePath() + "/");
}
