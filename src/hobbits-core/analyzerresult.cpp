#include "analyzerresult.h"

AnalyzerResult::AnalyzerResult()
{

}

AnalyzerResult* AnalyzerResult::setBitInfo(QSharedPointer<BitInfo> bitInfo)
{
    m_bitInfo = bitInfo;
    return this;
}

QSharedPointer<BitInfo> AnalyzerResult::bitInfo() const
{
    return m_bitInfo;
}

AnalyzerResult* AnalyzerResult::setParameters(const Parameters &parameters)
{
    m_parameters = parameters;
    return this;
}

const Parameters AnalyzerResult::parameters() const
{
    return m_parameters;
}

bool AnalyzerResult::hasNullParameters() const
{
    return m_parameters.isNull();
}

QString AnalyzerResult::errorString() const
{
    return m_errorString;
}

QSharedPointer<const AnalyzerResult> AnalyzerResult::result(QSharedPointer<BitInfo> bitInfo, Parameters parameters)
{
    return QSharedPointer<const AnalyzerResult>(
                (new AnalyzerResult())->setParameters(parameters)->setBitInfo(bitInfo)
            );
}

QSharedPointer<const AnalyzerResult> AnalyzerResult::error(QString errorString)
{
    auto result = new AnalyzerResult();
    result->m_errorString = errorString;
    return QSharedPointer<const AnalyzerResult>(result);
}
