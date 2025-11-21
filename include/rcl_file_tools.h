#ifndef RCL_FILE_TOOLS_H
#define RCL_FILE_TOOLS_H

#include <QByteArray>
#include <QFileInfoList>
#include <QString>

namespace RFileTools
{

    //! Read file.
    bool readBinaryFile(const QString &fileName, QByteArray &byteArray);

    //! Read file.
    bool writeBinaryFile(const QString &fileName, const QByteArray &byteArray);

    //! List files in directory.
    QFileInfoList listFiles(const QString &directoryPath);

};

#endif // RCL_FILE_TOOLS_H
