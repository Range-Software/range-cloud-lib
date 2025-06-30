#ifndef RCL_FILE_TOOLS_H
#define RCL_FILE_TOOLS_H

#include <QByteArray>
#include <QString>

namespace RFileTools
{

    //! Read file.
    bool readBinaryFile(const QString &fileName, QByteArray &byteArray);

    //! Read file.
    bool writeBinaryFile(const QString &fileName, const QByteArray &byteArray);

};

#endif // RCL_FILE_TOOLS_H
