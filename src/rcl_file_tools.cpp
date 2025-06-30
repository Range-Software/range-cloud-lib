#include <QDir>
#include <QFile>

#include "rcl_file_tools.h"
#include <rbl_logger.h>
#include <rbl_error.h>

bool RFileTools::readBinaryFile(const QString &fileName, QByteArray &byteArray)
{
    R_LOG_TRACE_IN;
    RLogger::info("FileService: Reading file \"%s\".\n",fileName.toUtf8().constData());

    try
    {
        QFile inFile(fileName);
        if (!inFile.exists())
        {
            R_LOG_TRACE_OUT;
            throw RError(RError::Type::InvalidFileName,R_ERROR_REF,"File \"%s\" does not exist.",fileName.toUtf8().constData());
        }

        if(!inFile.open(QIODevice::ReadOnly))
        {
            R_LOG_TRACE_OUT;
            throw RError(RError::Type::OpenFile,R_ERROR_REF,
                         "Failed to open file \"%s\" for reading. %s.",
                         inFile.fileName().toUtf8().constData(),
                         inFile.errorString().toUtf8().constData());
        }

        byteArray = inFile.readAll();
        RLogger::info("FileService: Successfuly read \"%ld\" bytes from \"%s\".\n",byteArray.size(),inFile.fileName().toUtf8().constData());

        inFile.close();
    }
    catch (const std::exception &e)
    {
        RLogger::error("FileService: Failed to read file \"%s\". %s\n", fileName.toUtf8().constData(),e.what());
        R_LOG_TRACE_RETURN(false);
    }
    catch (const RError &e)
    {
        RLogger::error("FileService: Failed to read file \"%s\". %s\n", fileName.toUtf8().constData(),e.getMessage().toUtf8().constData());
        R_LOG_TRACE_RETURN(false);
    }
    catch (...)
    {
        RLogger::error("FileService: Failed to read file \"%s\".\n", fileName.toUtf8().constData());
        R_LOG_TRACE_RETURN(false);
    }
    R_LOG_TRACE_RETURN(true);
}

bool RFileTools::writeBinaryFile(const QString &fileName, const QByteArray &byteArray)
{
    R_LOG_TRACE_IN;
    RLogger::info("FileService: Writing file \"%s\".\n",fileName.toUtf8().constData());

    try
    {
        QString dirPath(QFileInfo(fileName).absolutePath());
        QDir outDir;
        if (!outDir.exists(dirPath))
        {
            if (!outDir.mkpath(dirPath))
            {
                R_LOG_TRACE_OUT;
                throw RError(RError::Type::WriteFile,R_ERROR_REF,
                             "Failed to create directory \"%s\".",
                             dirPath.toUtf8().constData());
            }
        }

        QFile outFile(fileName);

        if(!outFile.open(QIODevice::WriteOnly))
        {
            R_LOG_TRACE_OUT;
            throw RError(RError::Type::OpenFile,R_ERROR_REF,
                         "Failed to open file \"%s\" for writing. %s.",
                         outFile.fileName().toUtf8().constData(),
                         outFile.errorString().toUtf8().constData());
        }

        qint64 bytesOut = outFile.write(byteArray,byteArray.size());

        RLogger::info("FileService: Successfuly wrote \"%ld\" bytes to \"%s\".\n",bytesOut,outFile.fileName().toUtf8().constData());

        outFile.close();
    }
    catch (const std::exception &e)
    {
        RLogger::error("FileService: Failed to write file \"%s\". %s\n", fileName.toUtf8().constData(),e.what());
        R_LOG_TRACE_RETURN(false);
    }
    catch (const RError &e)
    {
        RLogger::error("FileService: Failed to write file \"%s\". %s\n", fileName.toUtf8().constData(),e.getMessage().toUtf8().constData());
        R_LOG_TRACE_RETURN(false);
    }
    catch (...)
    {
        RLogger::error("FileService: Failed to write file \"%s\".\n", fileName.toUtf8().constData());
        R_LOG_TRACE_RETURN(false);
    }
    R_LOG_TRACE_RETURN(true);
}
