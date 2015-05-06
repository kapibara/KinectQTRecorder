#ifndef RESOURCESTORAGE_H
#define RESOURCESTORAGE_H

#include<QString>

class ResourceStorage
{
public:
    ResourceStorage(const QString &execLocation);

    QString getResourceLocation()
    {
        return resourceLocation_;
    }

private:

    QString resourceLocation_;
};

#endif // RESOURCESTORAGE_H
