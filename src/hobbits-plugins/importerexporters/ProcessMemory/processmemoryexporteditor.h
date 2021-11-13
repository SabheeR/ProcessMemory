#ifndef PROCESSMEMORYEXPORTEDITOR_H
#define PROCESSMEMORYEXPORTEDITOR_H

#include "abstractparametereditor.h"
#include "parameterhelper.h"

namespace Ui
{
class ProcessMemoryExportEditor;
}

class ProcessMemoryExportEditor : public AbstractParameterEditor
{
    Q_OBJECT

public:
    ProcessMemoryExportEditor(QSharedPointer<ParameterDelegate> delegate);
    ~ProcessMemoryExportEditor() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;

private:
    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::ProcessMemoryExportEditor *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // PROCESSMEMORYEXPORTEDITOR_H
