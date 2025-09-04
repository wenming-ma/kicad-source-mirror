// QT_TRANSFORMATION_COMPLETED

#ifndef FILTER_READER_H_
#define FILTER_READER_H_

#include <richio.h>
#include <QString>
class FILTER_READER : public LINE_READER
{
public:
    FILTER_READER( LINE_READER& aReader );

    ~FILTER_READER();

    char* ReadLine() override;

    const QString& GetSource() const override
    {
        return reader.GetSource();
    }

    unsigned LineNumber() const override
    {
        return reader.LineNumber();
    }

private:
    LINE_READER& reader;
};


class WHITESPACE_FILTER_READER : public LINE_READER
{
public:
    WHITESPACE_FILTER_READER( LINE_READER& aReader );

    ~WHITESPACE_FILTER_READER();

    char* ReadLine() override;

    const QString& GetSource() const override
    {
        return reader.GetSource();
    }

    unsigned LineNumber() const override
    {
        return reader.LineNumber();
    }

private:
    LINE_READER& reader;
};

#endif // FILTER_READER_H_
